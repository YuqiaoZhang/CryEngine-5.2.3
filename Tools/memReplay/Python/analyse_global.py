from replayshell import ReplayShell
import sys
import analysis

logName = sys.argv[1]

rc = ReplayShell("file=%s" % (logName))

stats = analysis.getHighLevelStats(rc)

for stat in stats:
	print "%s: %i" % (stat[0], stat[1])


