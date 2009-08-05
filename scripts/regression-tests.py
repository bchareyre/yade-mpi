"""
Run all defined regression tests, then exit.
Exit status is 0 on success and nonzero on failure.
"""
import yade.tests
result=yade.tests.testAll()
if result.wasSuccessful():
	print "*** ALL TESTS PASSED ***"
else:
	print "********************** SOME TESTS FAILED ************************"
sys.exit(0 if result.wasSuccessful() else 1)
