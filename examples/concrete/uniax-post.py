#
# demonstration of the yade.post2d module (see its documentation for details)
#
from yade import post2d
import pylab # the matlab-like interface of matplotlib

loadFile='/tmp/uniax-tension.xml.bz2'
if not os.path.exists(loadFile): raise RuntimeError("Run uniax.py first so that %s is created"%loadFile)
O.load(loadFile)

# flattener that project to the xz plane
flattener=post2d.AxisFlatten(useRef=False,axis=1)
# return scalar given a Body instance
extractDmg=lambda b: b.state.normDmg
# will call flattener.planar implicitly
# the same as: extractVelocity=lambda b: flattener.planar(b,b.state['vel'])
extractVelocity=lambda b: b.state.vel

# create new figure
pylab.figure()
# plot raw damage
post2d.plot(post2d.data(extractDmg,flattener))
pylab.suptitle('damage')

# plot smooth damage into new figure
pylab.figure(); ax,map=post2d.plot(post2d.data(extractDmg,flattener,stDev=2e-3))
pylab.suptitle('smooth damage')
# show color scale
pylab.colorbar(map,orientation='horizontal')

# shear stress
pylab.figure()
post2d.plot(post2d.data(lambda b: b.state.sigma,flattener))
pylab.suptitle('sigma')
pylab.figure()
post2d.plot(post2d.data(lambda b: b.state.tau,flattener,stDev=2e-3))
pylab.suptitle('smooth tau (in grid)')

# raw velocity (vector field) plot
pylab.figure(); post2d.plot(post2d.data(extractVelocity,flattener))
pylab.suptitle('velocity')

# smooth velocity plot; data are sampled at regular grid
pylab.figure(); ax,map=post2d.plot(post2d.data(extractVelocity,flattener,stDev=1e-3))
pylab.suptitle('smooth velocity')
# save last (current) figure to file
pylab.gcf().savefig('/tmp/foo.png') 

# show the figures
pylab.show()
