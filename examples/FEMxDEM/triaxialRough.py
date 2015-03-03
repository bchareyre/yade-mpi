""" Author: Ning Guo <ceguo@connect.ust.hk>
    run `mv triaxialRough.yade.gz 0.yade.gz`
    to generate initial RVE packing
"""
from esys.escript import *
from esys.finley import Brick
from esys.weipa import saveVTK
from esys.escript.pdetools import Projector
from msFEM3D import MultiScale
from saveGauss import saveGauss3D
import time

vel = -0.0001; confining=-1.e5;
lx = 0.05; ly = 0.05; lz = 0.1; # sample dimension
nx = 8; ny = 8; nz = 16; # discretization
mydomain = Brick(l0=lx,l1=ly,l2=lz,n0=nx,n1=ny,n2=nz,order=2,integrationOrder=2) # 20-noded,8-Gauss hexahedral element
dim = 3; k = kronecker(mydomain)
numg = 8*nx*ny*nz; # number of Gauss points
packNo=range(0,numg,8)

prob = MultiScale(domain=mydomain,ng=numg,useMPI=True,rtol=1e-2) # mpi is activated

disp = Vector(0.,Solution(mydomain))

t=0
stress = prob.getCurrentStress()
proj = Projector(mydomain)
sig = proj(stress)
sig_bounda = interpolate(sig,FunctionOnBoundary(mydomain))
traction = matrix_mult(sig_bounda,mydomain.getNormal()) # traction on boundary
x = mydomain.getX()
bx = FunctionOnBoundary(mydomain).getX()
topSurf = whereZero(bx[2]-lz)
tractTop = traction*topSurf # traction on top surface
forceTop = integrate(tractTop,where=FunctionOnBoundary(mydomain)) # force on top
areaTop = integrate(topSurf,where=FunctionOnBoundary(mydomain))
fout=file('./result/resultant.dat','w')
fout.write('0 '+str(forceTop[2])+' '+str(areaTop)+'\n')

# Dirichlet BC, rough loading ends
Dbc = whereZero(x[2])*[1,1,1]+whereZero(x[2]-lz)*[1,1,1]
Vbc = whereZero(x[2])*[0,0,0]+whereZero(x[2]-lz)*[0,0,vel]
# Neumann BC, constant lateral confining
Nbc = whereZero(bx[0])*[-confining,0,0]+whereZero(bx[0]-sup(bx[0]))*[confining,0,0]+whereZero(bx[1])*[0,-confining,0]+whereZero(bx[1]-sup(bx[1]))*[0,confining,0]

time_start = time.time()
while t < 100:

   prob.initialize(f=Nbc, specified_u_mask=Dbc, specified_u_val=Vbc)
   t += 1
   du=prob.solve(iter_max=100)

   disp += du
   stress=prob.getCurrentStress()
   
   dom = prob.getDomain()
   proj = Projector(dom)
   sig = proj(stress)

   sig_bounda = interpolate(sig,FunctionOnBoundary(dom))
   traction = matrix_mult(sig_bounda,dom.getNormal())
   tractTop = traction*topSurf
   forceTop = integrate(tractTop,where=FunctionOnBoundary(dom))
   areaTop = integrate(topSurf,where=FunctionOnBoundary(dom))
   fout.write(str(t*vel/lz)+' '+str(forceTop[2])+' '+str(areaTop)+'\n')
      
   vR=prob.getLocalVoidRatio()
   rotation=prob.getLocalAvgRotation()
   fabric=prob.getLocalFabric()
   strain = prob.getCurrentStrain()
   saveGauss3D(name='./result/gauss/time_'+str(t)+'.dat',strain=strain,stress=stress,fabric=fabric)
   volume_strain = trace(strain)
   dev_strain = symmetric(strain) - volume_strain*k/dim
   shear = sqrt(2./3.*inner(dev_strain,dev_strain))
   saveVTK("./result/vtk/triaxialRough_%d.vtu"%t,disp=disp,shear=shear,e=vR,rot=rotation)
   
prob.getCurrentPacking(pos=packNo,time=t,prefix='./result/packing/')
time_elapse = time.time() - time_start
fout.write("#Elapsed time in hours: "+str(time_elapse/3600.)+'\n')   
fout.close()
prob.exitSimulation()
