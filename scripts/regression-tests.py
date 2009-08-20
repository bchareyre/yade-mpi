"""
Run all defined regression tests, then exit.
Exit status is 0 on success and nonzero on failure.
"""
import yade.tests
try:
	result=yade.tests.testAll()
except:
	print "*********************** UNEXPECTED EXCEPTION WHILE RUNNING TESTS ******************"
	print "****************** please report bug"
	print "******************",sys.exc_info()[0]
	sys.exit(2)
if result.wasSuccessful():
	print "*** ALL TESTS PASSED ***"
else:
	print "********************** SOME TESTS FAILED ************************"
sys.exit(0 if result.wasSuccessful() else 1)
