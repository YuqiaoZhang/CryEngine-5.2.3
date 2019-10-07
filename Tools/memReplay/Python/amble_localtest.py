import amble
import amblelib

import amble_capture
import analysis

import autotest_tests

from replayshell import ReplayShell

def getBundleTargetFolder():
	return "D:\\ReplayAutoTestLogs"

tests = [
		{'test': 'round-cycle-10', 'level': 'c3mp_airport', 'timelimit': 4*60*60},
		]
	
def submitBundle(impl):
	bundleTargetFolder = getBundleTargetFolder()

	print "Bundling to %s..." % (bundleTargetFolder)
	bundlePath = impl.CopyReportBundle(bundleTargetFolder, True)

	print "Bundled to %s" % (bundlePath)

	return bundlePath

impl = amble.GetConsoleConnection()
impl.ShowTTY = 0

wasSuccess = False

amblelib.waitForInstall(impl)

for testArgs in tests:
	testType = testArgs['test']
	if testType == 'load':
		wasSuccess = amble_capture.capture(impl, autotest_tests.makeLevelLoadTest(testArgs['level']))
	elif testType == 'soak':
		wasSuccess = amble_capture.capture(impl, autotest_tests.makeSoakTest(testArgs['level']))
	elif testType == 'round-cycle':
		wasSuccess = amble_capture.capture(impl, autotest_tests.makeMpRoundCycleTest(testArgs['level'], testArgs['timelimit']))
	elif testType == 'round-cycle-10':
		wasSuccess = amble_capture.capture(impl, autotest_tests.makeMpRoundCycleTest10(testArgs['level'], testArgs['timelimit']))
	elif testType == 'chainload':
		wasSuccess = amble_capture.capture(impl, autotest_tests.makeChainLoadTest())

	if wasSuccess:
		bundleName = submitBundle(impl)

#		print "Analysing log:"
#		rc = ReplayShell('file=%s' % (bundleName))
#		hlstats = analysis.getHighLevelStats(rc)
#		for s in hlstats:
#			print "%s: %i" % (s[0], s[1])

