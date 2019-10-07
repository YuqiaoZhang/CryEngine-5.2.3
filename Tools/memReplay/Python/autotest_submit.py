import xmlrpclib
import sys

def submit(regressionSrv, build, test, bundle):
	regressionDb = xmlrpclib.ServerProxy("http://%s/xml_rpc_srv/" % (regressionSrv))
	regressionDb.add_log(bundle, build, test)

if len(sys.argv) < 4:
	print "Usage: autotest_submit.py <regression server> <build> <test> <bundle>"
else:
	submit(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])


