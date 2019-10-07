from replayshell import ReplayShell
import replayshell_shortcuts
import sys

buckets = [
		(5, "-5KB"),
		(10, "5KB-10KB"),
		(50, "10KB-50KB"),
		(100, "50KB-100KB"),
		(500, "100KB-500KB"),
		(1024, "500KB-1MB"),
		(2048, "1MB-2MB"),
		(4096, "2MB-4MB"),
		(100000000, "4MB-")
		]

addressDesc = sys.argv[1]
logName = sys.argv[2]

rs = ReplayShell("file=%s" % (logName))

taAddresses = rs.expandAddressDescription(addressDesc)
taCsIds = rs.findCallstacks(taAddresses)
taAllocs = rs.allocCallstackSet(taCsIds)

minAEv = 0
maxAEv = 288230376151711744 # 2^^58
if len(sys.argv) >= 5:
	minAEv = rs.getAllocEv(sys.argv[3])
	maxAEv = rs.getAllocEv(sys.argv[4])

class BucketValue:
	def __init__(self):
		self.count = 0
		self.size = 0

bucketValues = []
for i in range(len(buckets)):
	bucketValues.append(BucketValue())

for i in xrange(len(taAllocs)):
	aev = int(taAllocs[i]['allocEv'])
	if minAEv <= aev and aev < maxAEv:
		sizeKb = int(taAllocs[i]['size']) / 1024

		for bi in range(len(buckets)):
			if sizeKb < buckets[bi][0]:
				bucketValues[bi].count += 1
				bucketValues[bi].size += sizeKb
				break

		if sizeKb >= 64:
			ctxStackId = int(taAllocs[i]['ctxId'])
			ctxStackList = rs.resolveContextStacks([ctxStackId])
			if ctxStackId in ctxStackList:
				ctxStack = ctxStackList[ctxStackId]
				last = ctxStack[-1]
				print "%i:\t%s" % (sizeKb, last['name'])
			else:
				print "%i:\tUnknown" % (sizeKb)

for bi in range(len(buckets)):
	count = bucketValues[bi].count
	totalSize = bucketValues[bi].size
	aveSize = totalSize / count if count > 0 else 0
	print "%s:\t%i\t%i\t%i" % (buckets[bi][1], count, totalSize, aveSize)

