# default parameters or from table
# encoding: utf-8
O.bodies.append([
	utils.sphere([0,0,0],radius=.5),
])

O.engines=[
	Se3Interpolator(subscribedBodies=[0],goal=(Vector3(10,10,0),Quaternion(0,0,1,pi)),startIter=10,goalIter=1010,goalHook='print "Finished moving the thing!"; O.pause()'),
]
O.dt=1e-6
print 'Initial se3:',O.bodies[0].phys['se3']
#O.saveTmp('init'); O.run(); O.wait();
#print 'Final   se3:',O.bodies[0].phys['se3']
#quit()
