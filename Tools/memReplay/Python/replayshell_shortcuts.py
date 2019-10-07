import replayshell
import re

class Level:
	def __init__(self, name, loadStartEv, loadEndEv, precacheEndEv, unloadBeginEv, unloadEndEv, sessionId):
		self.name = name
		self.loadStartEv = loadStartEv
		self.loadEndEv = loadEndEv
		self.precacheEndEv = precacheEndEv
		self.unloadBeginEv = unloadBeginEv
		self.unloadEndEv = unloadEndEv
		self.sessionId = sessionId

	def __str__(self):
		return self.name

def findLevels(rc):
	labels = rc.getLabels()

	levels = []

	labelIdx = 0
	labelCount = len(labels)
	
	labelSeq = [ "loadstart", "loadend", "precacheend", "unloadstart", "unloadend" ]
	labelSeqIdx = 0
	labelEvSeq = []

	sessionId = ""

	while labelIdx < labelCount:
		labelName = labels[labelIdx]['label']
		lwrLabelName = labelName.lower()
		if lwrLabelName[:14] ==  "setsessionid: ":
			sessionId = lwrLabelName[14:]
		elif lwrLabelName[:len(labelSeq[labelSeqIdx])] == labelSeq[labelSeqIdx]:
			labelEvSeq.append(int(labels[labelIdx]['allocEv']))
			labelSeqIdx += 1

			if labelSeqIdx == len(labelSeq):
				nameMo = re.match(r'^unloadEnd\d+_(.*)$', labelName)
				if nameMo:
					name = nameMo.group(1)
				else:
					name = "unknown level"
				levels.append(Level(name, labelEvSeq[0], labelEvSeq[1], labelEvSeq[2], labelEvSeq[3], labelEvSeq[4], sessionId))
				labelSeqIdx = 0
				labelEvSeq = []

		labelIdx += 1

	return levels

def findLevelHeap(rc):
	ranges = []
	for ar in rc.getAddressRanges():
		if ar['name'] == "Level Heap":
			ranges.append(ar)
		elif ar['name'] == "Level Buckets":
			ranges.append(ar)
	return ranges

def simplify_function_name(name):
	nameMatch = re.match(r'^([^\(]*).*$', name)
	removedArgs = nameMatch.group(1)

	pieces = []

	i = 0
	l = len(removedArgs)
	tplDepth = 0
	leftAt = 0
	while i < l:
		if removedArgs[i] == '<':
			tplDepth += 1
			if tplDepth == 1:
				# Just entered, take everything up to now
				pieces.append(removedArgs[leftAt:i])
		elif removedArgs[i] == '<':
			tplDepth -= 1
			if tplDepth == 0:
				# Just left
				leftAt = i + 1
		i += 1
	
	return ''.join(pieces)

