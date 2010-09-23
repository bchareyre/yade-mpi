print 30*'*'+' WARNING '+30*'*'+'\nFor hardware/driver/...? reasons related to 3d, this script might\nsometimes crash when the first snapshot is taken with message such as\n\n\tQGLContext::makeCurrent(): Failed.\n\nor\n\n\tFatal IO error 11 (Resource temporarily unavailable) on X server :0.0.\n\nA workaround is to open the 3d view by hand, rather than having it\nopen by SnapshotEngine automatically when the first snapshot is\nabout to be taken. Sometimes only the message is displayed,\nwithout crash.\n'+25*'*'+' This is a known bug. '+25*'*'
TriaxialTest(noFiles=True).load()
from yade import qt,utils
O.engines=O.engines+[
	qt.SnapshotEngine(fileBase=O.tmpFilename(),label='snapshotter',iterPeriod=5,ignoreErrors=False),
	PyRunner(iterPeriod=500,command='finito()')
]
rr=qt.Renderer()
rr.shape,rr.intrPhys=False,True

def finito():
	"""This function will be called after 500 steps. Since SnapshotEngine waits for a new 3d view to open,
	it must run after the script has finished and the command line appears
	(see https://bugs.launchpad.net/yade/+bug/622669).

	For that reason, O.run() is at the end of the script and this function will be called
	once we want to exit really.
	"""
	utils.makeVideo(snapshotter.snapshots,out='/tmp/video.avi')
	print "Video saved in /tmp/video.avi"
	import sys
	sys.exit(0)

O.run()

