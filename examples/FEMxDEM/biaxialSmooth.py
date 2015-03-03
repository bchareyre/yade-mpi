""" Author: Ning Guo <ceguo@connect.ust.hk>
    run `mv biaxialSmooth.yade.gz 0.yade.gz`
    to generate initial RVE packing
"""
from esys.escript import *
from esys.finley import Rectangle
from esys.weipa import saveVTK
from esys.escript.pdetools import Projector
from msFEM2D import MultiScale
from saveGauss import saveGauss2D
import time

vel = -0.0001; confining=-1.e5;
lx = 0.05; ly = 0.1; # sample size, 50mm by 100mm
nx = 8; ny = 16; # sample discretization, 8 by 16 quadrilateral elements
mydomain = Rectangle(l0=lx,l1=ly,n0=nx,n1=ny,order=2,integrationOrder=2)
dim = mydomain.getDim()
k = kronecker(mydomain)
numg = 4*nx*ny; # number of Gauss points, 4 GP each element (reduced integration)
nump = 16; # number of processes for multiprocessing

prob = MultiScale(domain=mydomain,ng=numg,np=nump,random=False,rtol=1e-2,usePert=False,pert=-2.e-5,verbose=True)

disp = Vector(0.,Solution(mydomain))

t=0

stress = prob.getCurrentStress() # initial stress
proj = Projector(mydomain)
sig = proj(stress) # project Gauss point value to nodal value
sig_bounda = interpolate(sig,FunctionOnBoundary(mydomain)) # interpolate
traction = matrix_mult(sig_bounda,mydomain.getNormal()) # boundary traction
x = mydomain.getX() # nodal coordinate
bx = FunctionOnBoundary(mydomain).getX()
topSurf = whereZero(bx[1]-sup(bx[1]))
tractTop = traction*topSurf # traction at top surface
forceTop = integrate(tractTop,where=FunctionOnBoundary(mydomain)) # resultant force at top
lengthTop = integrate(topSurf,where=FunctionOnBoundary(mydomain)) # length of top surface
fout=file('./result/biaxial_surf.dat','w')
fout.write('0 '+str(forceTop[1])+' '+str(lengthTop)+'\n')
# Dirichlet BC positions, smooth at bottom and top, fixed at the center of bottom
Dbc = whereZero(x[1])*[0,1]+whereZero(x[1]-ly)*[0,1]+whereZero(x[1])*whereZero(x[0]-.5*lx)*[1,1]
# Dirichlet BC values
Vbc = whereZero(x[1])*[0,0]+whereZero(x[1]-ly)*[0,vel]+whereZero(x[1])*whereZero(x[0]-.5*lx)*[0,0]
# Neumann BC, constant confining pressure
Nbc = whereZero(bx[0])*[-confining,0]+whereZero(bx[0]-lx)*[confining,0]

time_start = time.time()
while t < 100: # apply 100 load steps

   prob.initialize(f=Nbc, specified_u_mask=Dbc, specified_u_val=Vbc) # initialize BC
   t += 1
   du=prob.solve(iter_max=100) # get solution: nodal displacement

   disp += du
   stress=prob.getCurrentStress()
   
   dom = prob.getDomain() # domain is updated Lagrangian formulation
   proj = Projector(dom)
   sig = proj(stress)

   sig_bounda = interpolate(sig,FunctionOnBoundary(dom))
   traction = matrix_mult(sig_bounda,dom.getNormal())
   tractTop = traction*topSurf
   forceTop = integrate(tractTop,where=FunctionOnBoundary(dom))
   lengthTop = integrate(topSurf,where=FunctionOnBoundary(dom))
   fout.write(str(t*vel/ly)+' '+str(forceTop[1])+' '+str(lengthTop)+'\n')
      
   vR=prob.getLocalVoidRatio()
   fabric=prob.getLocalFabric()
   strain = prob.getCurrentStrain()
   saveGauss2D(name='./result/gauss/time_'+str(t)+'.dat',strain=strain,stress=stress,fabric=fabric)
   volume_strain = trace(strain)
   dev_strain = symmetric(strain) - volume_strain*k/dim
   shear = sqrt(2*inner(dev_strain,dev_strain))
   saveVTK("./result/vtk/biaxialSmooth_%d.vtu"%t,disp=disp,shear=shear,e=vR)

prob.getCurrentPacking(pos=(),time=t,prefix='./result/packing/')
time_elapse = time.time() - time_start
fout.write("#Elapsed time in hours: "+str(time_elapse/3600.)+'\n')   
fout.close()
prob.exitSimulation()
