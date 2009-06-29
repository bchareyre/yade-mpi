""" This file shows 2 ways to fill union of triangulated surfaces:
You can either use union of 2 inGtsSurface predicates or
create union of surfaces using GTS calls first and use a single
isGtsSurface as predicate with the united surface.

Surprisingly, the first variant seems faster.

Note that GTS only moves references to surfaces around, therefore e.g. translating
surface that is part of the union will move also the part of the united surface.
Therefore, we use the copy() method for deep copy here.
"""
from yade import pack,qt
import gts

s1=gts.read(open('horse.coarse.gts'))
s2=gts.Surface(); s2.copy(s1); s2.translate(0.04,0,0)
O.bodies.append(pack.gtsSurface2Facets(s1,color=(0,1,0))+pack.gtsSurface2Facets(s2,color=(1,0,0)))

s12=gts.Surface(); s12.copy(s1.union(s2)); s12.translate(0,0,.1)
radius=0.005
O.bodies.append(pack.gtsSurface2Facets(s12,color=(0,0,1)))

qt.View()
from time import time
t0=time()
O.bodies.append(pack.regularHexa(pack.inGtsSurface(s1) | pack.inGtsSurface(s2),radius,gap=0,color=(0,1,0)))
t1=time()
print 'Using predicate union: %gs'%(t1-t0)
O.bodies.append(pack.regularHexa(pack.inGtsSurface(s12),radius,gap=0.,color=(1,0,0)))
t2=time()
print 'Using surface union: %gs'%(t2-t1)

