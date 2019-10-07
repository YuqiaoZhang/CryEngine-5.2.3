import AmbleCommon

import time
import sys
import os
import re
import socket

import crashdump

pathToScript = os.path.dirname(os.path.abspath(__file__))
sys.path.append(pathToScript)
from replayshell import ReplayShell

def countingsleep(impl, t):
	step = 10 if t <= 120 else 60
	while t >= step:
		if impl.HasCachedException:
			raise AmbleCommon.TargetCrashedException()

		print "%i..." % (t)
		time.sleep(step)
		t -= step
	time.sleep(t)

def fetchPDBs(impl, targetFolder):
	print "Syncing PDBs..."
	pdbFiles = [f for f in impl.ListDirectory(impl.ExecutableWorkingDirectory) if re.match(r'.*\.pdb$', f)]
	for pdb in pdbFiles:
		print "'%s'..." % (pdb)
		impl.ReceiveFile(pdb, os.path.join(targetFolder, pdb))

def writeUserCfg(impl, params):
	groupedParams = dict()
	for k, v in params:
		groupedParams[k] = v
	user = open(os.path.join(impl.WorkingFolder, 'user.cfg'), 'w')
	for k, v in groupedParams.iteritems():
		user.write("%s=%s\n" % (k, str(v)))
	path = os.path.abspath(user.name)
	user.close()
	impl.SendFile(path, "user.cfg")

def getLocalIpAddress():
	return [ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0]

def startReplayShell(impl):
	workingDirectory = impl.WorkingFolder
	symArgs = ""

	lwrPlatform = impl.Platform.lower()
	if lwrPlatform == "ps3":
		self = impl.ExecutableFilename
		symArgs = "self=%s;ps3bin=%s;ps3binMaxCC=2" % (self, os.path.join(pathToScript, "ps3bin.exe"))
	elif lwrPlatform == "x360":
		fetchPDBs(impl, workingDirectory)
		symArgs = "pdbPath=%s" % (workingDirectory)
	else:
		print "Unknown platform"
		exit(1)

	targetLog = os.path.join(workingDirectory, "memlog.zmrl")
	fileCacheSize = 128 * 1024 * 1024
	rsCmd = "capture=%s;fcSize=%i;%s" % (targetLog, fileCacheSize, symArgs)

	print "Starting ReplayShell: %s" % (rsCmd)
	rs = ReplayShell(rsCmd)
	countingsleep(impl, 5)
	return rs

def registerArtefacts(impl):
	for filename in os.listdir(impl.WorkingFolder):
		if re.search(r'\.zmrl', filename):
			print "Registering %s" % (filename)
			impl.RegisterArtefact(filename, "MemReplay Log")

def capture(impl, test):
	cvars = test['cvars']
	testFunc = test['handler']

	impl.BeginSession()

	# Gather information to identify the build
	#impl.ReceiveArtefact("AssetChanges.txt", "AssetChanges.txt", "Build info", True)
	#impl.ReceiveArtefact("BuildInfo.txt", "BuildInfo.txt", "Build info", True)
	#impl.ReceiveArtefact("BuildName.txt", "BuildName.txt", "Build info", True)
	#impl.ReceiveArtefact("CodeChanges.txt", "CodeChanges.txt", "Build info", True)
	#impl.ReceiveArtefact("system.cfg", "system.cfg", "Build info", True)

	rs = startReplayShell(impl)

	print "Running %s from %s" % (impl.Platform, pathToScript)

	crashhandler = crashdump.DumpProcessor(impl)

	try:
		localIpAddress = getLocalIpAddress()
		print "Found local ip address as %s" % (localIpAddress)
		
		listenPort = rs.getListenPort()
		print "ReplayShell is listening on port %i" % (listenPort)

		memReplayCmd = "-memReplay socket:%s:%i" % (localIpAddress, listenPort)
		impl.ExecutableArguments = "%s -ps3_watchdog_timeout 3 +i_forcefeedback 0" % (memReplayCmd)

		writeUserCfg(impl, cvars)

		print "Resetting console..."
		impl.Reset(False)
		countingsleep(impl, 30)

		testFunc(impl)

		print "Stopping MemReplay in the game..."
		impl.Execute("memReplayStop")

		print "Stopping ReplayShell to finish indexing..."
		rs.quit()

		print "Registering all the zmrl bits as artefacts..."
		registerArtefacts(impl)

		impl.Shutdown()

		return True

	except (AmbleCommon.TargetCrashedException, AmbleCommon.AmbleCommunicationException) as ex:
		if impl.HasCachedException:
			print "Gathering crash dump"
			crashhandler.processCrash("memtest")

		# If anything bad happened check if the console crashed and get a dump
		print "Caught exception " + str(ex)

		# Submit a dummy request which will wait for the indexing to complete
		print "Waiting for indexing to complete..."
		rs.sampleLocationEstimate(0)

		print "Terminating ReplayShell..."
		rs.quit()

		return False

	except AmbleCommon.BuildSuiteException as ex:
		print "Caught exception " + str(ex)

	rs.kill()
	return False

