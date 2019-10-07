import json
import sys
import os

# IronPython subprocess is broken
if sys.platform == 'cli':
	import subprocess_ipy as subprocess
else:
	import subprocess

class ReplayShell:
	def __init__(self, connString):
		# Find where we are, as ReplayShell.exe will be in the folder above. Or rather, it should be.
		pathToScript = os.path.dirname(os.path.abspath(__file__))
		self.process = subprocess.Popen(
				[os.path.join(os.path.dirname(pathToScript), 'ReplayShell.exe'), connString],
				stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=False)

	def __del__(self):
		self.kill()

	def kill(self):
		if self.process != None:
			self.process.kill()
			self.process = None

	def quit(self):
		if self.process != None:
			self.process.stdin.write('Quit\n')
			self.process.stdin.flush()
			self.process.wait()
			self.process = None

	def getListenPort(self):
		return self._runCmd('GetListenPort\n')

	def getLabels(self):
		return self._runCmd('GetLabels\n')

	def getAddressRanges(self):
		return self._runCmd('GetAddressRanges\n')

	def expandAddressDescription(self, desc):
		return self._runCmd('ExpandAddressDesc "%s"\n' % (desc))

	def resolveAddresses(self, addresses):
		return self._runCmd('ResolveAddresses "%s"\n' % (','.join([str(a) for a in addresses])))

	def findCallstacks(self, addresses):
		return self._runCmd('FindCallstacks "%s"\n' % (','.join([str(a) for a in addresses])))

	def getAllocEv(self, locationDesc):
		return int(self._runCmd('GetAllocEv "%s"\n' % (str(locationDesc))))

	def getFrame(self, locationDesc):
		return int(self._runCmd('GetFrame "%s"\n' % (str(locationDesc))))

	def sampleLocationEstimate(self, locationDesc):
		return int(self._runCmd('SampleLocationEstimate "%s"\n' % (str(locationDesc))))

	def plotTotal(self, property):
		return self._runCmd('PlotTotal "%s"\n' % (property))

	def plotCallstacks(self, csIds):
		return self._runCmd("PlotCallstacks \"%s\"\n" % (','.join([str(c) for c in csIds])))

	def allocDiffSet(self, fromDesc, toDesc, **kwargs):
		cmd = 'AllocDiffSet %s %s' % (fromDesc, toDesc)
		cmd += ' '.join(" \"%s=%s\"" % (k, v) for (k, v) in kwargs.iteritems())
		return self._runCmd(cmd + '\n')
	
	def allocDiffTree(self, fromDesc, toDesc):
		return self._runCmd('AllocDiffTree %s %s\n' % (str(fromDesc), str(toDesc)))

	def resolveAllocs(self, allocIds):
		return self._runCmd('ResolveAllocs %s\n' % (','.join([str(a) for a in allocIds])))

	def resolveCallstacks(self, callstackIds):
		if len(callstackIds) > 0:
			jscallstacks = self._runCmd('ResolveCallstacks %s\n' % (','.join([str(a) for a in callstackIds])))
			return dict([(int(k), v) for (k, v) in jscallstacks.iteritems()])
		else:
			return {}

	def resolveContextStacks(self, contextStackIds):
		if len(contextStackIds) > 0:
			jsconstacks = self._runCmd('ResolveContextStacks %s\n' % (','.join([str(a) for a in contextStackIds])))
			return dict([(int(k), v) for (k, v) in jsconstacks.iteritems()])
		else:
			return {}

	def allocCallstackSet(self, callstackIds):
		return self._runCmd('AllocCallstackSet "%s"\n' % (','.join([str(a) for a in callstackIds])))

	def validate(self):
		return self._runCmd('Validate\n')

	def _runCmd(self, cmd):
		self.process.stdin.write(cmd)
		self.process.stdin.flush()

		soutput = self.process.stdout.readline()
		return json.loads(soutput)

