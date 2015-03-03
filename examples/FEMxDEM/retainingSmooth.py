""" Author: Ning Guo <ceguo@connect.ust.hk>
    run `mv retainingSmooth.yade.gz 0.yade.gz`
    to generate initial RVE packing
"""
from esys.escript import *
from esys.finley import Rectangle
from esys.weipa import saveVTK
from esys.escript.pdetools import Projector
from msFEM2D import MultiScale
from saveGauss import saveGauss2D
import time

vel = -0.00017; surcharge=-2.e4; # surcharge equals to the initial vertical stress of the RVE packing; vel<0 passive failure; vel>0 active failure
B = 0.4; H = 0.2; wallH = 0.17; baseH = H-wallH; # setup dimensions
nx = 40; ny = 20 # discretization with 40x20 quads
mydomain = Rectangle(l0=B,l1=H,n0=nx,n1=ny,order=2,integrationOrder=2)
dim = mydomain.getDim()
k = kronecker(mydomain)
numg = 4*nx*ny; nump = 16;
packNo=range(0,numg,16)

disp = Vector(0.,Solution(mydomain))

prob = MultiScale(domain=mydomain,ng=numg,np=nump,random=False,rtol=1e-2,usePert=False,pert=-2.e-5,verbose=True)

t=0
time_start = time.time()

x = mydomain.getX()
bx = FunctionOnBoundary(mydomain).getX()
left = whereZero(x[0])
right = whereZero(x[0]-B)
bottom = whereZero(x[1])
top = whereZero(bx[1]-H)
base = whereZero(x[0]-B)*whereNegative(x[1]-baseH)
wall = whereZero(x[0]-B)*whereNonNegative(x[1]-baseH)
wallBF = whereZero(bx[0]-B)*whereNonNegative(bx[1]-baseH)
# Dirichlet BC, all fixed in space except wall (only fixed in x direction, smooth)
Dbc = left*[1,1]+base*[1,1]+bottom*[1,1]+wall*[1,0]
Vbc = left*[0,0]+base*[0,0]+bottom*[0,0]+wall*[vel,0]
# Neumann BC, apply surcharge at the top surface
Nbc = top*[0,surcharge]

stress = prob.getCurrentStress()
proj = Projector(mydomain)
sig = proj(stress)
sig_bounda = interpolate(sig,FunctionOnBoundary(mydomain))
traction = matrix_mult(sig_bounda,mydomain.getNormal())
tract = traction*wallBF # traction on wall
forceWall = integrate(tract,where=FunctionOnBoundary(mydomain)) # force on wall
lengthWall = integrate(wallBF,where=FunctionOnBoundary(mydomain))
fout=file('./result/pressure.dat','w')
fout.write('0 '+str(forceWall[0])+' '+str(lengthWall)+'\n')

while t < 100:

   prob.initialize(f=Nbc,specified_u_mask=Dbc,specified_u_val=Vbc)
   t += 1
   du=prob.solve(iter_max=100)

   disp += du
   stress=prob.getCurrentStress()
   
   dom = prob.getDomain()
   proj = Projector(dom)
   sig = proj(stress)

   sig_bounda = interpolate(sig,FunctionOnBoundary(dom))
   traction = matrix_mult(sig_bounda,dom.getNormal())
   tract = traction*wallBF
   forceWall = integrate(tract,where=FunctionOnBoundary(dom))
   lengthWall = integrate(wallBF,where=FunctionOnBoundary(dom))
   fout.write(str(t*vel)+' '+str(forceWall[0])+' '+str(lengthWall)+'\n')
      
   vR=prob.getLocalVoidRatio()
   rotation=prob.getLocalAvgRotation()
   fabric=prob.getLocalFabric()
   strain = prob.getCurrentStrain()
   saveGauss2D(name='./result/gauss/time_'+str(t)+'.dat',strain=strain,stress=stress,fabric=fabric)
   volume_strain = trace(strain)
   dev_strain = symmetric(strain) - volume_strain*k/dim
   shear = sqrt(2*inner(dev_strain,dev_strain))
   saveVTK("./result/vtk/retainingSmooth_%d.vtu"%t,disp=disp,stress=stress,shear=shear,e=vR,rot=rotation)

prob.getCurrentPacking(pos=packNo,time=t,prefix='./result/packing/')
time_elapse = time.time() - time_start
fout.write("#Elapsed time in hours: "+str(time_elapse/3600.)+'\n')
fout.close()
prob.exitSimulation()
