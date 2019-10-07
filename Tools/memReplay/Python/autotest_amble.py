import amble
import amblelib

import time
import xmlrpclib
import json
import datetime
import sys

import amble_capture
import autotest_tests

if len(sys.argv) < 2:
	raise Exception()

regressionSrv = sys.argv[1]

def getBundleTargetFolder():
	now = datetime.date.today()
	return "\\\\ukautotest\\Daily\\%02d\\ReplayAutoTestLogs" % (now.day)

regressionDb = xmlrpclib.ServerProxy("http://%s/xml_rpc_srv/" % (regressionSrv))

def submitBundle(impl, testName, wasSuccess):
	bundleTargetFolder = getBundleTargetFolder()

	print "Bundling to %s..." % (bundleTargetFolder)
	bundlePath = impl.CopyReportBundle(bundleTargetFolder, True)

	print "Bundled to %s" % (bundlePath)

	print "Submitting to regression db..."
	regressionDb.add_log(bundlePath, impl.InstalledBuildId.lower(), testName)#, wasSuccess)

impl = amble.GetConsoleConnection()
impl.ShowTTY = 0

hasInstalled = False

while True:
	buildId = impl.InstalledBuildId
	
	if buildId != "":
		print "Testing build %s" % (impl.InstalledBuildId)

		testDetails = regressionDb.get_test_to_run(buildId.lower())
		while testDetails != False:
			print "Received test %s" % (testDetails[0])

			if hasInstalled == False:
				amblelib.waitForInstall(impl)
				hasInstalled = True

			wasSuccess = False

			testArgs = json.loads(testDetails[1])
			testType = testArgs['test']
			if testType == 'load':
				wasSuccess = amble_capture.capture(impl, autotest_tests.makeLevelLoadTest(testArgs['level']))
			elif testType == 'soak':
				wasSuccess = amble_capture.capture(impl, autotest_tests.makeSoakTest(testArgs['level']))
			elif testType == 'round-cycle':
				wasSuccess = amble_capture.capture(impl, autotest_tests.makeMpRoundCycleTest(testArgs['level']))
			elif testType == 'chainload':
				wasSuccess = amble_capture.capture(impl, autotest_tests.makeChainLoadTest())

			submitBundle(impl, testDetails[0], wasSuccess)

			testDetails = regressionDb.get_test_to_run(buildId.lower())

		print "Run out of tests."

	# Get latest build

	time.sleep(10)

	latestBuild = impl.ListBuilds()[0]
	if latestBuild.Name.lower() == buildId.lower():
		break

	print "Fetching build %s..." % (latestBuild.Name)
	impl.FetchBuild(latestBuild)
	hasInstalled = False

