# -*- coding: utf-8 -*-

from yade import pack, export, ymport
import gts, os.path, locale

#### controling parameters
mesh='parallellepiped' #name of gts mesh
sizeRatio=10. # defines discretisation (sizeRatio=meshLength/particleDiameter)

#### import mesh
locale.setlocale(locale.LC_ALL,'en_US.UTF-8')   #gts is locale-dependend.  If, for example, german locale is used, gts.read()-function does not import floats normally
surface=gts.read(open(mesh+'.gts'))

print 'closed? ', surface.is_closed()

#### generate packing
if surface.is_closed():
	pred=pack.inGtsSurface(surface)
	# get characteristic dimensions
	aabb=pred.aabb()
	dim=pred.dim()
	center=pred.center()
	minDim=min(dim[0],dim[1],dim[2])
	# define discretisation
	radius=minDim/(2*sizeRatio)
	print center, dim, ' | minDim=', minDim, ' | diameter=', 2*radius
	### regular packing
	#O.bodies.append(pack.regularHexa(pred,radius=radius,gap=0.,color=(0.9,0.8,0.6)))
	#O.bodies.append(pack.regularOrtho(pred,radius=radius,gap=0.,color=(0.9,0.8,0.6)))
	sp=SpherePack()
	# random packing
	sp=pack.randomDensePack(pred,radius=radius,rRelFuzz=0.3,useOBB=True,memoizeDb='/tmp/gts-triax-packings.sqlite',returnSpherePack=True) # cropLayers=5 (not to use)
	# periodic random packing
	#sp=pack.randomDensePack(pred,radius=radius,rRelFuzz=0.3,useOBB=True,spheresInCell=2000,memoizeDb='/tmp/gts-triax-packings.sqlite',returnSpherePack=True) # cropLayers=5 (not to use)
	sp.toSimulation(color=(0.9,0.8,0.6))

#### import mesh
O.bodies.append(pack.gtsSurface2Facets(surface,color=(0.8,0.8,0.8),wire=True))

#### export packing
export.text(mesh+'_'+str(int(sizeRatio))+'.spheres')

#### VIEW
from yade import qt
qt.View()

