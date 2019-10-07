import xmlrpclib
from operator import itemgetter
import sys

from replayshell import ReplayShell
import replayshell_shortcuts
import callstackutil
import analysis
import stats

class TelemetryIssueType:
	LevelLeak, Peak, SoakPerFrame, SoakPerLevel, CrossModule, SubClassMismatch = range(1, 7)

class TelemetryLevelStats:
	def __init__(self, name):
		self.name = name
		self.startMem = 0
		self.loadMem = 0
		self.peakMem = 0
		self.endMem = 0
		self.levelLeaksCount = 0
		self.levelLeaksSize = 0

		self.loadStartMemGlobal = 0
		self.playStartMemGlobal = 0
		self.changePerFrameGlobal = 0

def issueTypeFromValidationErrorType(vt):
	if vt == analysis.ValidationErrorType.SubClassMismatch:
		return TelemetryIssueType.SubClassMismatch
	elif vt == analysis.ValidationErrorType.CrossModuleAlloc:
		return TelemetryIssueType.CrossModule
	else:
		return None

def analyse(logName):
	rc = None
	levelStats = []
	logIssues = []
	logStats = {}

	#try:
	if True:
		rc = ReplayShell("file=%s" % (logName))

		levels = replayshell_shortcuts.findLevels(rc)
		skipAddressSet = set()
		for al in [rc.expandAddressDescription(line.strip()) for line in skipList]:
			skipAddressSet.update(al)

		totalPlotConsumed = rc.plotTotal('consumed')
		logStats['peak'] = reduce(max, totalPlotConsumed)
		logStats['smoothed_peak'] = reduce(max, stats.convolve(totalPlotConsumed['byAllocEv'], stats.gaussian_kernel(50)))

		totalPlotGlobal = rc.plotTotal('global')

		if len(levels) > 5:
			totalPlotGlobalByRound = analysis.projectAEvPlot(totalPlotGlobal['byAllocEv'], totalPlotGlobal['allocEvSplit'], [l.loadStartEv for l in levels[1:]])

			logStats['changePerRoundGlobal'] = int(stats.lls(totalPlotGlobalByRound)[0])

			print "Log is losing approximately %i bytes per round" % (logStats['changePerRoundGlobal'])

		for level in levels:
			print "Found level \"%s\" between %i-%i" % (level.name, level.loadStartEv, level.unloadEndEv)

		for issue in analysis.findSoakIssuesForLevels(rc, skipAddressSet, levels):
			logIssues.append({
				'type': TelemetryIssueType.SoakPerLevel,
				'callstack': callstackutil.makeXmlRpcSafe(issue.cs),
				'session': None,
				'size': issue.size,
				'numOccurrences': 1,
				'confidence': issue.confidence
				})

		for issue in analysis.findValidationIssues(rc, skipAddressSet):
			logIssues.append({
				'type': issueTypeFromValidationErrorType(issue.type),
				'callstack': callstackutil.makeXmlRpcSafe(issue.cs),
				'session': None,
				'size': 1,
				'numOccurrences': issue.numOccurrences,
				'confidence': 1.0
				})

		for level in levels:
			levelIssues = []

			startFrame = int(rc.getFrame(level.precacheEndEv))
			endFrame = int(rc.getFrame(level.unloadEndEv))

			levelStartEvs = level.loadStartEv / totalPlotConsumed['allocEvSplit']
			levelEndEvs = level.unloadEndEv / totalPlotConsumed['allocEvSplit']
			levelTotalPlotConsumed = totalPlotConsumed['byAllocEv'][levelStartEvs:levelEndEvs]
			levelTotalPlotGlobal = totalPlotGlobal['byFrame'][startFrame:endFrame]

			levStats = TelemetryLevelStats(level.name)

			# Get memory use before loading level
			startMem = totalPlotGlobal['byAllocEv'][levelStartEvs]

			# Get memory use a little way into the level (after initial ramp up)
			playStartMem = levelTotalPlotGlobal[1000] if len(levelTotalPlotGlobal) > 1000 else 0

			# Get LLS of remaining frames in level to determine approximate growth over time
			playLLS = stats.lls(levelTotalPlotGlobal[1000:])

			levStats.startMem = rc.sampleLocationEstimate(level.loadStartEv)
			levStats.loadMem = rc.sampleLocationEstimate(level.loadEndEv)
			levStats.peakMem = reduce(max, levelTotalPlotConsumed)
			levStats.endMem = rc.sampleLocationEstimate(level.unloadBeginEv)

			levStats.loadStartMemGlobal = startMem
			levStats.playStartMemGlobal = playStartMem
			levStats.changePerFrameGlobal = playLLS[0]

			print "Level \"%s\" with session id \"%s\" started at %i, loaded at %i, increased per frame by %i bytes" % (level.name, level.sessionId, levStats.loadStartMemGlobal, playStartMem, levStats.changePerFrameGlobal)

			levelLeakIssues = analysis.findLevelLeaksForAEvs(rc, skipAddressSet, level.loadStartEv, level.unloadEndEv)
			for issue in levelLeakIssues:
				logIssues.append({
					'type': TelemetryIssueType.LevelLeak,
					'callstack': callstackutil.makeXmlRpcSafe(issue.cs),
					'session': level.sessionId,
					'size': issue.size,
					'numOccurrences': issue.numOccurrences,
					'confidence': 1.0
					})

			levStats.levelLeaksCount = reduce(lambda x, y: x + y.numOccurrences, levelLeakIssues, 0)
			levStats.levelLeaksSize = reduce(lambda x, y: x + y.size, levelLeakIssues, 0)

			print "Finding soak issues in level \"%s\"" % (level.name)
			for issue in analysis.findSoakIssuesForFrames(rc, skipAddressSet, startFrame + 50, endFrame - 10):
				logIssues.append({
					'type': TelemetryIssueType.SoakPerFrame,
					'callstack': callstackutil.makeXmlRpcSafe(issue.cs),
					'session': level.sessionId,
					'size': issue.size,
					'numOccurrences': 1,
					'confidence': issue.confidence
					})

#			if startFrame + 500 < endFrame:
#				print "Finding peak issues in level \"%s\" evs %i-%i" % (level.name, level.loadEndEv, level.unloadBeginEv)
#				levelIssues.extend(findPeakIssues(rc, skipAddressSet, level.loadEndEv, level.unloadBeginEv))
#			else:
#				print "Ignoring peak issues in level \"%s\"" % (level.name)

			levelStats.append(levStats)

		rc.quit()

	#except:
	#	if rc != None:
	#		rc.kill()

	return (logStats, levelStats, logIssues)

def processLog(db, id, name):
	print "Found log %s (id: %s) to process." % (name, str(id))

	logStats, levelStats, logIssues = analyse(name)
	db.end_processing_log(id, levelStats, logIssues)#, logStats)

regressionSrv = sys.argv[1] if len(sys.argv) > 1 else "uktelemetry01:8003"
srvUrl = "http://%s/xml_rpc_srv/" % (regressionSrv)

regressionDb = xmlrpclib.ServerProxy(srvUrl, allow_none=True)

skipList = []
with open('skiplist.txt', 'r') as skipListFile:
	skipList = skipListFile.readlines()

if len(sys.argv) < 3:
	pendingLog = regressionDb.begin_processing_log()

	while pendingLog != False:
		logId = pendingLog[0]
		logName = pendingLog[1]

		processLog(regressionDb, logId, logName)

		pendingLog = regressionDb.begin_processing_log()
else:
	processLog(regressionDb, sys.argv[2], sys.argv[3])

