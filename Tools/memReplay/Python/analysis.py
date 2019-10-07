from operator import itemgetter

import stats
import replayshell_shortcuts as rssc
import callstackutil

soakSizeThreshold = 80 * 1024

class LevelLeak:
	def __init__(self, cs):
		self.cs = cs
		self.size = 0
		self.numOccurrences = 0
		self.csIds = set()

class SoakLeak:
	def __init__(self, cs, size, confidence):
		self.cs = cs
		self.size = size
		self.confidence = confidence

class ValidationErrorType:
	SubClassMismatch, CrossModuleAlloc = range(1, 3)

class ValidationError:
	def __init__(self, type, cs):
		self.type = type
		self.cs = cs
		self.numOccurrences = 1

class CallstackGroup:
	size = 0

	def __init__(self, scs):
		self.csIdSet = set()
		self.simplifiedCs = scs

def getPlottableSoakCallstacks(rc, skipSet, allocs, thresholdSize, csLen):
	diffAllocCallstacks = rc.resolveCallstacks([a['csId'] for a in allocs])

	reducedCallstacks = dict()
	reducedCallstackLen = 3

	for alloc in allocs:
		csId = alloc['csId']
		callstack = diffAllocCallstacks[csId]
		scs = callstackutil.applySkipSet(callstack, skipSet)[-csLen:]
		csKey = callstackutil.callstackToKey(scs)

		if csKey not in reducedCallstacks:
			reducedCallstacks[csKey] = CallstackGroup(scs)
		csinst = reducedCallstacks[csKey]
		csinst.csIdSet.add(csId)
		csinst.size += alloc['size']

	print "Number of reduced callstacks found: %i" % (len(reducedCallstacks))

	plottableCallstacks = dict()

	for (cskey, csGroup) in reducedCallstacks.iteritems():
		if csGroup.size >= soakSizeThreshold:
			plottableCallstacks[cskey] = csGroup

	return plottableCallstacks

def projectAEvPlot(plot, evSplit, sampleAtList):
	return [plot[x / evSplit] for x in sampleAtList if (x / evSplit) < len(plot)]

def findLevelLeaksForAEvs(rc, skipSet, startAEv, endAEv):
	lvlAddressRanges = rssc.findLevelHeap(rc)

	lvlLeakAllocIds = []
	for lvlar in lvlAddressRanges:
		diffSet = rc.allocDiffSet(startAEv, endAEv, addressRange=(lvlar['name']))
		lvlLeakAllocIds.extend(diffSet['allocs'])
	lvlLeaks = rc.resolveAllocs(lvlLeakAllocIds)

	print "Found %i level leak allocations" % (len(lvlLeaks))

	csIds = set([ int(a['csId']) for a in lvlLeaks ])
	callstacks = rc.resolveCallstacks(csIds)

	issues = dict()
	reducedCallstackLen = 5

	for alloc in lvlLeaks:
		csId = alloc['csId']
		callstack = callstacks[csId]
		scs = callstackutil.applySkipSet(callstack, skipSet)[-reducedCallstackLen:]
		issueKey = callstackutil.callstackToKey(scs)

		if issueKey not in issues:
			issues[issueKey] = LevelLeak(scs)

		issue = issues[issueKey]
		issue.csIds.add(csId)
		issue.size += alloc['size']
		issue.numOccurrences += 1

	print "Number of level leak issues found: %i" % (len(issues))
	return [i for (k, i) in issues.iteritems()]

def findSoakIssuesForLevels(rc, skipSet, levels):
	print "Scanning for level to level leaks"

	issues = []

	if len(levels) >= 5:
		deltaAllocIds = rc.allocDiffSet(levels[1].loadStartEv, levels[-1].loadStartEv)['allocs']
		print "Delta contains %i allocs" % (len(deltaAllocIds))

		deltaAllocs = rc.resolveAllocs(deltaAllocIds)

		reducedCallstackLen = 3
		plottableCallstacks = getPlottableSoakCallstacks(rc, skipSet, deltaAllocs, soakSizeThreshold, reducedCallstackLen)
		print "Number of callstacks above %iKB: %i" % (soakSizeThreshold / 1024, len(plottableCallstacks))

		for (cskey, csGroup) in plottableCallstacks.iteritems():
			print "Plotting: %s..." % (cskey)
			plot = rc.plotCallstacks(csGroup.csIdSet)
			byRound = projectAEvPlot(plot['byAllocEv'], plot['allocEvSplit'], [l.loadStartEv for l in levels[1:]])
			
			if (len(byRound)):
				print "Analysing..."
				llsres = stats.lls(byRound)
				plotMax = reduce(lambda x, y: max(x, y), byRound)

				if plotMax > 0 and llsres[1] > 0.75:
					issue = SoakLeak(csGroup.simplifiedCs, llsres[0], llsres[1])
					issues.append(issue)
			else:
				print "No interesting rounds?"
	else:
		print "Not enough levels"

	return issues

def findSoakIssuesForFrames(rc, skipSet, startFrame, endFrame):
	issues = []

	if startFrame + 15000 <= endFrame:
		print "Scanning frames %i to %i for soak leaks" % (startFrame, endFrame)

		startDesc = "frame:%i" % (startFrame)
		endDesc = "frame:%i" % (endFrame)
		diff = rc.allocDiffSet(startDesc, endDesc)
		print "Delta: %i allocs, %i frees" % (len(diff['allocs']), len(diff['frees']))

		diffAllocIds = diff['allocs']
		diffAllocs = rc.resolveAllocs(diffAllocIds)

		reducedCallstackLen = 3
		plottableCallstacks = getPlottableSoakCallstacks(rc, skipSet, diffAllocs, soakSizeThreshold, reducedCallstackLen)
		print "Number of callstacks above %iKB: %i" % (soakSizeThreshold / 1024, len(plottableCallstacks))

		for (cskey, csGroup) in plottableCallstacks.iteritems():
			print "Plotting: %s, callstacks (%s)..." % (cskey, ', '.join(str(i) for i in csGroup.csIdSet))
			byFrame = rc.plotCallstacks(csGroup.csIdSet)['byFrame']
			interestingFrames = byFrame[startFrame:endFrame]
			
			if (len(interestingFrames)):
				print "Analysing..."
				llsres = stats.lls(interestingFrames)
				plotMax = reduce(lambda x, y: max(x, y), interestingFrames)

				print "Max was %i" % (plotMax)

				if plotMax > 0 and llsres[1] > 0.75:
					issue = SoakLeak(csGroup.simplifiedCs, llsres[0], llsres[1])
					issues.append(issue)
			else:
				print "No interesting frames?"
	else:
		print "Not enough frames for soak issues"
	
	return issues

def findValidationIssues(rc, skipSet):
	print "Running validation query..."
	errs = rc.validate()

	subClassCallstacks = rc.resolveCallstacks([e['firstCs'] for e in errs if e['type'] == 'subclassmismatch'])
	crossModuleCallstacks = rc.resolveCallstacks([e['firstCs'] for e in errs if e['type'] == 'crossmodulealloc'])

	issues = {}
	for err in errs:
		issue = None

		if err['type'] == 'subclassmismatch':
			csId = int(err['firstCs'])
			cs = subClassCallstacks[csId]

			reducedCallstackLen = 5
			scs = callstackutil.applySkipSet(cs, skipSet)[-reducedCallstackLen:]
			issueKey = callstackutil.callstackToKey(scs)
			issue = ValidationError(ValidationErrorType.SubClassMismatch, scs)

		elif err['type'] == 'crossmodulealloc':
			csId = int(err['firstCs'])
			cs = crossModuleCallstacks[csId]

			reducedCallstackLen = 5
			scs = callstackutil.applySkipSet(cs, skipSet)[-reducedCallstackLen:]
			issueKey = callstackutil.callstackToKey(scs)
			issue = ValidationError(ValidationErrorType.CrossModuleAlloc, scs)

		if issue != None:
			if issueKey in issues:
				issues[issueKey].numOccurrences += 1
			else:
				issues[issueKey] = issue

	print "Found %i validation errors" % (len(issues))
	return [issue for key, issue in issues.iteritems()]

def findPeakCallstack(tree):
	stack = []

	treeSize = tree['sizeInclusive']
	thresholdSize = treeSize / 2
	node = tree

	while node != None:
		stack.append(node['key'])

		if len(node['children']) > 0:
			# Find the largest child, so long as it's not below some theshold to continue walking the tree
			# - we're trying to find the root cause of the spike, ignoring minor allocations caught up in the tree
			node['children'].sort(key=itemgetter('sizeInclusive'), reverse=True)
			largestChild = node['children'][0]
			largestChildSize = largestChild['sizeInclusive']

			if largestChildSize >= thresholdSize:
				node = largestChild
			else:
				node = None
		else:
			node = None

	return stack	

def findPeakIssues(rc, skipSet, startEv, endEv):
	plot = rc.plotTotal()
	allocEvPlot = plot['byAllocEv']
	allocEvSplit = int(plot['allocEvSplit'])
	startFrameEvSlot = int(startEv / allocEvSplit)
	endFrameEvSlot = int(endEv / allocEvSplit)

	clippedPlot = allocEvPlot[startFrameEvSlot:endFrameEvSlot]

	gaussianPlot = stats.convolve(clippedPlot, stats.gaussian_kernel(50))
	residualPlot = map(lambda x, y: x - y, clippedPlot, gaussianPlot)

	peakThreshold = 1 * 1024 * 1024 + 512 * 1024
	peakWidthThreshold = 50000 / allocEvSplit

	#for i in xrange(0, len(residualPlot) - 1):
	#	dv = clippedPlot[i + 1] - clippedPlot[i] 
	#	if dv > peakThreshold:
	#		peakStartAEv = (i + startFrameEvSlot) * allocEvSplit
	#		peakEndAEv = peakStartAEv + allocEvSplit
	#		print "Found peak of %i at %i" % (dv, peakStartAEv)

	issues = dict()

	residualPlotLen = len(residualPlot)
	i = 0
	while i < residualPlotLen:
		if residualPlot[i] >= peakThreshold:
			peakStart = i - 1
			worstPeakCell = i
			while i < residualPlotLen:
				if residualPlot[worstPeakCell] < residualPlot[i]:
					worstPeakCell = i

				if residualPlot[i] < peakThreshold:
					peakEnd = i
					if peakEnd - peakStart < peakWidthThreshold:
						# Found spike from peakStart to peakEnd, with the peak of the peak at worstPeakCell
						print "Found spike at %i-%i of %i" % ((peakStart + startFrameEvSlot) * allocEvSplit, (peakEnd + startFrameEvSlot) * allocEvSplit, residualPlot[worstPeakCell])
						tree = rc.allocDiffTree(((peakStart + startFrameEvSlot) * allocEvSplit), (worstPeakCell + startFrameEvSlot) * allocEvSplit)

						peakCallstack = callstackutil.applySkipSet(rc.resolveAddresses(findPeakCallstack(tree)), skipSet)[-4:]
						issueKey = callstackutil.callstackToKey(peakCallstack)

						if issueKey not in issues:
							issues[issueKey] = Issue(IssueType.Peak, peakCallstack, 0, 0, 1.0)
						issue = issues[issueKey]
						issue.size = max(issue.size, residualPlot[worstPeakCell])
						issue.numOccurrences += 1
					break

				i += 1
		i += 1

	return [i for (k, i) in issues.iteritems()]

def getHighLevelStats(rc):
	hlstats = []

	levels = rssc.findLevels(rc)

	totalPlotGlobalDef = rc.plotTotal('global')
	totalPlotGlobal = totalPlotGlobalDef['byAllocEv']
	totalPlotGlobalSmoothed = stats.convolve(totalPlotGlobal, stats.gaussian_kernel(50))

	hlstats.append(('Real Peak', reduce(max, totalPlotGlobal)))
	hlstats.append(('Smoothed Peak', reduce(max, totalPlotGlobalSmoothed)))

	lvlAddressRanges = rssc.findLevelHeap(rc)

	for level in levels:
		startFrame = rc.getFrame(level.precacheEndEv) + 10
		endFrame = rc.getFrame(level.unloadBeginEv) - 10

		levelPlotGlobal = totalPlotGlobalDef['byFrame'][startFrame:endFrame]
		changePerFrameGlobal = int(stats.lls(levelPlotGlobal)[0])

		hlstats.append(('Level %s Change/Frame' % (level.name), changePerFrameGlobal))

		lvlLeakCount = 0
		for lvlar in lvlAddressRanges:
			diffSet = rc.allocDiffSet(level.loadStartEv, level.unloadEndEv, addressRange=(lvlar['name']))
			lvlLeakCount += len(diffSet['allocs'])

		hlstats.append(('Level %s Level Leaks' % (level.name), lvlLeakCount))

	return hlstats
	
