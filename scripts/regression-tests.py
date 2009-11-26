"""
Run all defined regression tests, then exit.
Exit status is 0 on success and nonzero on failure.
"""
import yade.tests
try:
	result=yade.tests.testAll()
except:
	print "*********************** UNEXPECTED EXCEPTION WHILE RUNNING TESTS ******************"
	print "******************",sys.exc_info()[0]
	print "****************** Please report bug at http://bugs.launchpad.net/yade providing the following traceback:"
	import traceback
	traceback.print_exc()
	print "***************************************** Thank you *******************************"
	sys.exit(2)
if result.wasSuccessful():
	print "*** ALL TESTS PASSED ***"
else:
	print "********************** SOME TESTS FAILED ************************"
sys.exit(0 if result.wasSuccessful() else 1)
