# default parameters or from table
# encoding: utf-8

""" THIS SCRIPT IS NOT WORKING!
ERROR MESSAGE:

Running script Se3Interpolator.py                                                                                                    
Traceback (most recent call last):                                                                                                   
  File "/home/me/YADE/YADE3041/bin/yade-bzr3041", line 182, in runScript                                                             
    execfile(script,globals())                                                                                                       
  File "Se3Interpolator.py", line 8, in <module>                                                                                     
    Se3Interpolator(ids=[0],goal=(Vector3(10,10,0),Quaternion(0,0,1,pi)),startIter=10,goalIter=1010,goalHook='print "Finished moving the thing!"; O.pause()'),                                                                                                            
NameError: name 'Se3Interpolator' is not defined  
"""

O.bodies.append([
	sphere([0,0,0],radius=.5),
])

O.engines=[
	Se3Interpolator(ids=[0],goal=(Vector3(10,10,0),Quaternion(0,0,1,pi)),startIter=10,goalIter=1010,goalHook='print "Finished moving the thing!"; O.pause()'),
]
O.dt=1e-6
print 'Initial se3:',O.bodies[0].phys['se3']
#O.saveTmp('init'); O.run(); O.wait();
#print 'Final   se3:',O.bodies[0].phys['se3']
#quit()
