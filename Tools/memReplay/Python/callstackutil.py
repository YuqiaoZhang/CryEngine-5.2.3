import re

def callstackToKey(cs):
	return ' '.join([str(c['address']) for c in cs])

def applySkipSet(cs, skipSet):
	for csi in xrange(len(cs)-1, -1, -1):
		if int(cs[csi]['address']) not in skipSet:
			return cs[0:csi+1]
	return []

def makeXmlRpcSafe(cs):
	return [{'file': csi['file'], 'line': csi['line'], 'name': csi['name'], 'address': "0x%x" % (csi['address'])} for csi in cs]

