""" Author: Ning Guo <ceguo@connect.ust.hk>
    run `mv undrain.yade.gz 0.yade.gz`
    to generate initial RVE packing
"""
from esys.escript import *
from esys.finley import Rectangle
from esys.weipa import saveVTK
from esys.escript.pdetools import Projector
from msFEMup import MultiScale
from saveGauss import saveGauss2D
import time

confining = -2.e5; pore = 1.e5 # initial pore pressure
perm = 0.001**2/(180.*8.9e-4); # unscaled permeability, using KC equation
kf = 2.2e9 # fluid bulk modulus
dt = .1; vel = -0.0001 # time step and loading speed
lx = 0.05; ly = 0.1 # sample dimension
nx = 8; ny = 16 # discretization
mydomain = Rectangle(l0=lx,l1=ly,n0=nx,n1=ny,order=2,integrationOrder=2)
k = kronecker(mydomain); dim = 2.
numg = 4*nx*ny; # no. of Gauss points
mpi = True # use MPI

prob = MultiScale(domain=mydomain,pore0=pore,perm=perm,kf=kf,dt=dt,ng=numg,useMPI=mpi,rtol=1.e-2)
disp = Vector(0.,Solution(mydomain))
t = 0
ux = mydomain.getX() # disp. node coordinate
px = ReducedSolution(mydomain).getX() # press. node coordinate
bx = FunctionOnBoundary(mydomain).getX()
topSurf = whereZero(bx[1]-ly)
uDbc = whereZero(ux[1])*[0,1]+whereZero(ux[0]-lx/2.)*whereZero(ux[1])*[1,1]+whereZero(ux[1]-ly)*[0,1] # disp. Dirichlet BC mask
vDbc = whereZero(ux[1])*[0,0]+whereZero(ux[0]-lx/2.)*whereZero(ux[1])*[0,0]+whereZero(ux[1]-ly)*[0,vel*dt] # disp. Dirichlet BC value
uNbc = whereZero(bx[0])*[-confining,0]+whereZero(bx[0]-lx)*[confining,0] # disp. Neumann BC

stress = prob.getCurrentStress() # effective stress at GP
proj = Projector(mydomain)
sig = proj(stress) # effective stress at node (reduced)
sig_bound = interpolate(sig,FunctionOnBoundary(mydomain))
traction = matrix_mult(sig_bound,mydomain.getNormal())
tractTop = traction*topSurf
forceTop = integrate(tractTop,where=FunctionOnBoundary(mydomain))
lengthTop = integrate(topSurf,where=FunctionOnBoundary(mydomain))
fout = file('./result/resultantForce.dat','w')
fout.write('0 '+str(forceTop[1])+' '+str(lengthTop)+'\n')

time_start = time.time()
while t < 400:
   prob.initialize(f=uNbc,umsk=uDbc,uvalue=vDbc)
   t += 1
   du = prob.solve(globalIter=10,solidIter=50)
   disp += du
   pore = prob.getCurrentPore() # pore pressure at node (reduced)
   flux = prob.getCurrentFlux() # Darcy flux at GP
   stress = prob.getCurrentStress() # effective stress at GP
   strain = prob.getCurrentStrain() # disp. grad at GP
   volume_strain = trace(strain) # volumetric strain
   dev_strain = symmetric(strain) - volume_strain*k/dim # deviatoric strain
   shear = sqrt(2.*inner(dev_strain,dev_strain)) # shear strain
   fab = prob.getLocalFabric() # fabric tensor at GP
   dev_fab = 4.*(fab-trace(fab)/dim*k)
   anis = sqrt(.5*inner(dev_fab,dev_fab))
   p = prob.getEquivalentPorosity() # porosity at GP
   rot = prob.getLocalAvgRotation() # average rotation at GP
   saveGauss2D(name='./result/gauss/time_'+str(t)+'.dat',strain=strain,fabric=fab,stress=stress)
   dom = prob.getDomain() # domain updated (Lagrangian)
   proj = Projector(dom)
   flux = proj(flux) # Darcy flux at node (reduced)
   p = proj(p) # porosity at node (reduced)
   shear = proj(shear) # shear strain at node (reduced)
   anis = proj(anis)
   rot = proj(rot)
   saveVTK("./result/vtk/undrain_%d.vtu"%t,disp=disp,pore=pore,flux=flux,shear=shear,p=p,anis=anis,rot=rot)
   sig = proj(stress) # effective stress at node (reduced)
   sig_bound = interpolate(sig,FunctionOnBoundary(dom))
   traction = matrix_mult(sig_bound,dom.getNormal())
   tractTop = traction*topSurf
   forceTop = integrate(tractTop,where=FunctionOnBoundary(dom))
   lengthTop = integrate(topSurf,where=FunctionOnBoundary(dom))
   fout.write(str(t*vel*dt/ly)+' '+str(forceTop[1])+' '+str(lengthTop)+'\n')

prob.getCurrentPacking(time=t,prefix='./result/packing/')
time_elapse = time.time() - time_start
fout.write('#Elapsed time in hours: '+str(time_elapse/3600.)+'\n')
fout.close()
prob.exitSimulation()

