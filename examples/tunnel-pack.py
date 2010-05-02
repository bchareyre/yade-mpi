from yade import pack

"""Simple script to create tunnel with random dense packing of spheres.
The tunnel is difference between an axis-aligned box and cylinder, or which
axis is going through the bottom wall (-z) of the box.

The tunnel hole is oriented along +y, the face is in the xz plane.

The first you run this scipt, a few minutes is neede to generate the packing. It is
saved in /tmp/triaxPackCache.sqlite and at next time it will be only loaded (fast).
"""
# set some geometry parameters: domain box size, tunnel radius, radius of particles
boxSize=Vector3(5,8,5)
tunnelRad=2
rSphere=.1
# construct spatial predicate as difference of box and cylinder:
# (see scripts/test/pack-predicates.py for details)
#
# http://beta.arcig.cz/~eudoxos/yade/epydoc/yade._packPredicates.inAlignedBox-class.html
# http://beta.arcig.cz/~eudoxos/yade/epydoc/yade._packPredicates.inCylinder-class.html

pred=pack.inAlignedBox((-.5*boxSize[0],-.5*boxSize[1],0),(.5*boxSize[0],.5*boxSize[1],boxSize[2])) - pack.inCylinder((-.5*boxSize[0],0,0),(.5*boxSize[0],0,0),tunnelRad)
# Use the predicate to generate sphere packing inside 
#
# http://beta.arcig.cz/~eudoxos/yade/epydoc/yade.pack-module.html#randomDensePack
O.bodies.append(pack.randomDensePack(pred,radius=rSphere,rRelFuzz=.3,memoizeDb='/tmp/triaxPackCache.sqlite',spheresInCell=3000))

from yade import qt
qt.Controller()
qt.View()
