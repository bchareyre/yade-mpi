from numpy import arange
from yade import pack
import pylab
# define the section shape as polygon in 2d; repeat first point at the end to close the polygon
sq2=sqrt(2)
poly=((3+.1,0),(3+0,.1),(3+sq2,.1+sq2),(3+.1+sq2,sq2),(3+.1,0))
#pylab.plot(*zip(*poly)); pylab.xlim(xmin=0); pylab.grid(); pylab.title('Meridian of the revolution surface\n(close to continue)'); pylab.gca().set_aspect(aspect='equal',adjustable='box'); pylab.show()
thetas=arange(0,pi/8,pi/24)
pts=pack.revolutionSurfaceMeridians([poly for theta in thetas],thetas,origin=Vector3(-4,0,-1),orientation=Quaternion().IDENTITY)
surf=pack.sweptPolylines2gtsSurface(pts,capStart=True,capEnd=True,threshold=1e-4)
O.bodies.append(pack.gtsSurface2Facets(surf,color=(1,0,1)))
# fill this solid with triaxial packing; it will compute minimum-volume oriented bounding box
# to minimize the number of throw-away spheres.
# It does away with about 3k spheres for radius 3e-2
O.bodies.append(pack.randomDensePack(pack.inGtsSurface(surf),radius=3e-2,rRelFuzz=1e-1,memoizeDb='/tmp/gts-triax-packings.sqlite'))
# translate the surface away and pack it again with sphere, but without the oriented bounding box (useOBB=False)
# Here, we need 20k spheres (with more or less the same result)
surf.translate(0,0,1);
O.bodies.append(pack.gtsSurface2Facets(surf,color=(1,0,0)))
O.bodies.append(pack.randomDensePack(pack.inGtsSurface(surf),radius=3e-2,rRelFuzz=1e-1,memoizeDb='/tmp/gts-triax-packings.sqlite',useOBB=False))
