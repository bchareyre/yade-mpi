""" Author: Ning Guo <ceguo@connect.ust.hk>
    run `mv footing.yade.gz 0.yade.gz`
    to generate initial RVE packing
"""
from esys.escript import *
from esys.finley import ReadGmsh
from esys.weipa import saveVTK
from esys.escript.pdetools import Projector
from msFEM import MultiScale
from saveGauss import saveGauss2D
import time

vel = -0.00025; surcharge=-20.e3; # surcharge equals to the initial vertical stress of the RVE packing
dim = 2; B = 0.05; L = 0.6; H = 0.4;
mydomain = ReadGmsh('footing.msh',numDim=dim,integrationOrder=2)  # read Gmsh mesh with 6-node triangle element (2500 tri6); each element has 3 Gauss points
k = kronecker(mydomain)
numg = 3*2500; # number of Gauss points
nump = 16; # number of processes in multiprocessing
packNo=range(0,numg,50)

prob = MultiScale(domain=mydomain,ng=numg,np=nump,random=False,rtol=1e-2,usePert=False,pert=-2.e-5,verbose=False)

disp = Vector(0.,Solution(mydomain))

t=0
stress = prob.getCurrentStress()
proj = Projector(mydomain)
sig = proj(stress)
sig_bounda = interpolate(sig,FunctionOnBoundary(mydomain))
traction = matrix_mult(sig_bounda,mydomain.getNormal())
x = mydomain.getX()
bx = FunctionOnBoundary(mydomain).getX()
footingBase = whereZero(bx[1]-sup(bx[1]))*whereNonPositive(bx[0]-B)
tractFoot = traction*footingBase
forceFoot = integrate(tractFoot,where=FunctionOnBoundary(mydomain))
lengthFoot = integrate(footingBase,where=FunctionOnBoundary(mydomain))
fout=file('./result/bearing.dat','w')
fout.write('0 '+str(forceFoot[0])+' '+str(forceFoot[1])+' '+str(lengthFoot)+'\n')

# Dirichlet BC, rollers at left and right, fixties at bottom, rigid and rough footing
Dbc = whereZero(x[0])*[1,0]+whereZero(x[0]-sup(x[0]))*[1,0]+whereZero(x[1]-inf(x[1]))*[1,1]+whereZero(x[1]-sup(x[1]))*whereNonPositive(x[0]-B)*[1,1]
Vbc = whereZero(x[0])*[0,0]+whereZero(x[0]-sup(x[0]))*[0,0]+whereZero(x[1]-inf(x[1]))*[0,0]+whereZero(x[1]-sup(x[1]))*whereNonPositive(x[0]-B)*[0,vel]
# Neumann BC, surcharge at the rest area of the top surface
Nbc = whereZero(bx[1]-sup(bx[1]))*wherePositive(bx[0]-B)*[0,surcharge]

time_start = time.time()
while t < 58: # apply 58 loading step; further loading would abort the program due to severe mesh distortion

   prob.initialize(f=Nbc, specified_u_mask=Dbc, specified_u_val=Vbc)
   t += 1
   du=prob.solve(iter_max=100)

   disp += du
   stress=prob.getCurrentStress()
   
   dom = prob.getDomain() # domain updated (Lagrangian)
   proj = Projector(dom)
   sig = proj(stress)

   sig_bounda = interpolate(sig,FunctionOnBoundary(dom))
   traction = matrix_mult(sig_bounda,dom.getNormal())
   tractFoot = traction*footingBase
   forceFoot = integrate(tractFoot,where=FunctionOnBoundary(dom))
   lengthFoot = integrate(footingBase,where=FunctionOnBoundary(dom))
   fout.write(str(t*vel)+' '+str(forceFoot[0])+' '+str(forceFoot[1])+' '+str(lengthFoot)+'\n')
      
   vR=prob.getLocalVoidRatio()
   rotation=prob.getLocalAvgRotation()
   fabric=prob.getLocalFabric()
   strain = prob.getCurrentStrain()
   saveGauss2D(name='./result/gauss/time_'+str(t)+'.dat',strain=strain,stress=stress,fabric=fabric)
   volume_strain = trace(strain)
   dev_strain = symmetric(strain) - volume_strain*k/dim
   shear = sqrt(2*inner(dev_strain,dev_strain))
   saveVTK("./result/vtk/footing_%d.vtu"%t,disp=disp,stress=stress,shear=shear,e=vR,rot=rotation)

prob.getCurrentPacking(pos=packNo,time=t,prefix='./result/packing/') # output packing
time_elapse = time.time() - time_start
fout.write("#Elapsed time in hours: "+str(time_elapse/3600.)+'\n')   
fout.close()
prob.exitSimulation()
