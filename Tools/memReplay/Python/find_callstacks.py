from replayshell import ReplayShell
import sys

logName = sys.argv[1]
desc = sys.argv[2]

rs = ReplayShell("file=%s" % (logName))

addresses = rs.expandAddressDescription(desc)
callstackIds = rs.findCallstacks(addresses)
callstacks = rs.resolveCallstacks(callstackIds)

trimmedCallstacks = dict()

for id, cs in callstacks.iteritems():
	trimmedcs = cs[-8:]
	callstackAddresses = [a['address'] for a in trimmedcs]
	callstackKey = ','.join([str(a) for a in callstackAddresses])
	trimmedCallstacks[callstackKey] = trimmedCallstacks.get(callstackKey, trimmedcs)

for id, cs in trimmedCallstacks.iteritems():
	for csi in cs:
		print "%08x: %s" % (csi['address'], csi['name'])
	print "\n"

