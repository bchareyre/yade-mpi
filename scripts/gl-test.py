#
# Test script for OpenGL from python (and from other threads in general)
# With GL locking, 1 view seems to work reliably, not two views, though.
#
# Some drivers (nvidia) raise XBadAccess if there is an attempt to access
#  OpenGL context concurrently from different threads.
# Other drivers (intel, at least on my laptop) freeze the videocard completely
#  if that is the case.
#
# You can enable the "if False:" part (say "if True:" instead) to see what
# it does in your case.
#

from yade import qt
from yade import utils

O.bodies.append(utils.box([0,0,0],[1,1,1],color=[1,0,0]))
O.bodies.append(utils.sphere([0,2,0],1,color=[0,1,0]))

print 'Begin here.'
qt.View(); print 'Created view'
glv0=qt.GLViewer(); print 'view #0' # get GLViewer instance for the primary view
glv0.grid=True,True,False; print 'grid x,y'
glv0.axes=True; print 'axes shown'
glv0.screenSize=200,200; print 'screen size set to 200x200'
glv0.center(True); print 'median center' # median center - will fallback since there are only 2 bodies

##
## !!! If you enable this, you may crash your machine !!!
##
if False: 
	glv1=qt.View(); print 'Created 2nd view' # create new view
	glv1.fps=True; print 'fps shown'
	glv1.lookAt=(0,1,0); print 'lookAt set'
	glv1.viewDirection=(0,-1,0); print 'viewDirection set'
	glv1.center(); print 'center'

print 'list of views:',qt.views() # list all views we have
print "CONGRATULATIONS! You passed the OpenGL test without crash or freeze :-)"

