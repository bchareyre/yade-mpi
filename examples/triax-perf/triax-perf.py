# Performance test for running
#
#  1. Regular TriaxialTest with 3 independent dispatchers (geom, phys, constitutive law)
#  2. TriaxialTest with InteractionDispatchers (common loop and functor cache)
#
# Run the test like this:
#
#  yade-trunk-opt-multi -j1 triax-perf.table triax-perf.py
#
# The -j1 ensures that only 1 job will run at time
# (even if other cores are free, access to memory is limiting if running multiple jobs at time)
#
# You have to collect the results by hand from log files, or run sh mkTextTable.sh and use
# triax-perf.ods to get comparison
#
utils.readParamsFromTable(fast=False,noTableOk=True)
TriaxialTest(numberOfGrains=50000,fast=fast,noFiles=True).load()
O.run(10,True) # filter out initialization
O.timingEnabled=True
O.run(200,True)
from yade import timing
timing.stats()
print 'BexContainer synced %d times'%(O.bexSyncCount)

