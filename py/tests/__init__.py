import unittest 

# add any new test suites to the list here, so that they are picked up by testAll
allTests=['wrapper',]

# fully qualified module names
allTestsFQ=['yade.tests.'+test for test in allTests]

def testModule(module):
	"""Run all tests defined in the module specified, return TestResult object 
	(http://www.python.org/doc/2.6/lib/unittest.html#testresult-objects.html)
	for further processing.

	@param module: fully-qualified module name, e.g. yade.tests.wrapper
	"""
	suite=unittest.defaultTestLoader().loadTestsFromName(module)
	return unittest.TextTestRunner(verbosity=2).run(suite)

def testAll():
	"""Run all tests defined in all yade.tests.* modules and return
	TestResult object for further examination."""
	suite=unittest.defaultTestLoader.loadTestsFromNames(allTestsFQ)
	return unittest.TextTestRunner(verbosity=2).run(suite)

	

