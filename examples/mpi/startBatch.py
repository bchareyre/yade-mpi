import subprocess
import os
import numpy as np
import matplotlib.pyplot as plt

threadsMax = 10
threadsInterval = 1
threadsList = np.linspace(2,threadsMax,num=threadsMax/threadsInterval) # need at least one master and one worker

for numThreads in threadsList:
	#os.environ["IDENTIFIER"]='strongScalability-'+str(int(numThreads))+'-threads'
	subprocess.call(['mpiexec','-n',str(int(numThreads)), 'yadempi-daily', '-x', '-n', 'testMPI_strongScalability.py', str(int(numThreads))]) 		
