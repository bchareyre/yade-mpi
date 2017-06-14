from yade import pack
import random
random.seed(1)

# a cube
p1 = Vector3(-9,-2,-2)
p2 = p1 + (1,1,1)
pred1 = pack.inConvexPolyhedron((
	(p1, (+1, 0, 0)),
	(p1, ( 0,+1, 0)),
	(p1, ( 0, 0,+1)),
	(p2, (-1, 0, 0)),
	(p2, ( 0,-1, 0)),
	(p2, ( 0, 0,-1)),
))

# a tetrahedron
p1 = Vector3(-7,-2,-2)
p2 = p1 + (1,1,1)
pred2 = pack.inConvexPolyhedron((
	(p1, (+1, 0, 0)),
	(p1, ( 0,+1, 0)),
	(p1, ( 0, 0,+1)),
	(p2, (-1,-1,-1)),
))

# random polyhedron
center = Vector3(-2,-2,-2)
dMin = 1
dMax = 1.2
n = 20
planes = []
for i in xrange(n):
	d = random.random()*(dMax-dMin) + dMin # distance of plane from center, random number in range (dMin,dMax)
	# http://mathworld.wolfram.com/HyperspherePointPicking.html
	normal = Vector3([random.gauss(0,10) for _ in xrange(3)]) # random normal vector
	normal.normalize()
	planes.append((center-d*normal,normal))
pred3 = pack.inConvexPolyhedron(planes)

try: # should be ValueError, since the 3 planes does not form closed polyhedron and finding its bounds should fail
	pred4 = pack.inConvexPolyhedron((
		((0,0,0), (+1, 0, 0)),
		((0,0,0), ( 0,+1, 0)),
		((0,0,0), ( 0, 0,+1)),
	))
except ValueError:
	print 'ValueError successfully detected'
else:
	raise RuntimeError, "ValueError should have been detected..."

r = .05
for p in (pred1,pred2,pred3):
	O.bodies.append(pack.regularHexa(p,r,0,color=randomColor()))


try:
	from yade import qt
	v = qt.View()
	v.axes = True
except:
	pass
