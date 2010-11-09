# create a few clump configurations by hand
from yade import pack
c1=pack.SpherePack([((0,0,0),.5),((.5,0,0),.5),((0,.5,0),.3)])
c2=pack.SpherePack([((0,0,0),.5),((.7,0,0),.3),((.9,0,0),.2)])
sp=pack.SpherePack()
sp.makeClumpCloud((0,0,0),(10,10,10),[c1,c2],periodic=False)
sp.toSimulation()
