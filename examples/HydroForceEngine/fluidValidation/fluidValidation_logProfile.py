#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 22/11/2017
#
# Script to validate the 1D volume-averaged fluid resolution of HydroForceEngine, on the classical logarithmic profile. 
# No particles are simulated and only the newtonian fluid resolution is tested and compared to the analytical solution
#
# Consider a newtonian free-surface (iusl = 1) fluid flow driven by gravity (channel inclination angle "alpha") on a smooth bottom wall. 
# Impose the free-surface position fluidHeight and let the fluid resolution evolve toward equilibrium. 
# The turbulent stresses are modelled using an eddy viscosity concept with the classical Prandtl mixing length closure (ilm = 0)
# Account for the presence of a viscous sublayer close to the wall (viscousSubLayer = 1)
# 
# The obtained 1D fluid velocity profile (vxFluid) is compared to the analytical solution in the output figure "figProfiLog.png". It is necessary 
# to have a dense mesh in order to solve correctly the viscous sublayer. In addition, discrepancies can be observed close to the fluid free-surface, 
# as the analytical solution does not account for its presence. 
# 
#########################################################################################################################################################################
import numpy as np


################################
# PARAMETERS AND CONFIGURATION DEFINITIONS
#Fluid flow characteristics
alpha = 0.5e-1        #Channel inclination angle, in radian
fluidHeight = 1e-1	#Fluid height, in m
densFluid = 1e3		#Fluid density kg/m^3, water
kinematicViscoFluid = 1e-6	#Kinematic fluid viscosity, m^2/s, water

#Simulation characteristic
tfin = 1e3		#Total simulated time, in s
dtFluid = 2e-1		#Resolution time step, in s
#Mesh
ndimz = 10000   #Number of grid cells in the height. Needs to be high in order to solve accurately the viscous sublayer
dz =  fluidHeight/(1.0*(ndimz-1))	#spatial step between two mesh nodes

# Initialization of the fluid velocity
vxFluid =np.zeros(ndimz)
gravityVector = Vector3(9.81*sin(alpha),0,-9.81*cos(alpha))


################################
# CREATE THE ASSOCIATED HYDROFORCEENGINE OBJECT FOR THE FLUID RESOLUTION
a = HydroForceEngine(densFluid=densFluid,viscoDyn=kinematicViscoFluid*densFluid,deltaZ=dz,nCell=ndimz,vxFluid=np.array(vxFluid),gravity=gravityVector,phiMax=0.61,turbulentViscosity = np.zeros(ndimz),iturbu=1,ilm=0,iusl=1,viscousSubLayer=1,phiPart=np.zeros(ndimz),vxPart = np.zeros(ndimz),averageDrag=np.zeros(ndimz))# Define the minimum parameters of the HydroForceEngine for the fluid resolution

################################
# FLUID RESOLUTION
a.fluidResolution(tfin,dtFluid)# Call the fluid resolution associated with the defined HydroForceEngine. Use the parameters defined just above. 
print "\nFluid Resolution finished !  ","max(vxFluid)=",np.max(a.vxFluid),"\n"




################################
# COMPARISON TO THE ANALYTICAL SOLUTION
print "Comparison to the analytical solution\n"
vxFluid = np.array(a.vxFluid)


## Analytical Solution
kappa=0.41    # Von Karman constant
ustar=sqrt(abs(gravityVector[0])*fluidHeight)	# Shear velocity
dz_ap=1.*kinematicViscoFluid/ustar		# Dimensionless vertical scale unit
nz=int(round(fluidHeight/dz_ap))		# Number of unit over the fluid depth
ztrans=11.3	#Vertical position of the transition from the viscous sublayer to the log law, in dimensionless unit z+. Take it classically as 11.3, similarly to the fluid resolution
utrans=ztrans*ustar	#Fluid velocity transition from the viscous sublayer to the log law, in dimensionless unit z+
z_analytic=np.linspace(0,1e0,nz)*fluidHeight	#Re-create the vertical scale
zplus=z_analytic*ustar/kinematicViscoFluid	#Dimensionless vertical scale
vxFluid_analytic =np.zeros(nz)	#Initialization
for i in range(nz):
	if (zplus[i]<=ztrans): 
		vxFluid_analytic[i]=ustar*zplus[i]	#Linear fluid profile in the viscous sub layer
	else:
		vxFluid_analytic[i]=ustar/kappa*np.log(zplus[i]/ztrans)+utrans;	#Logarythmic fluid profile above
            

##############################
#### PLOT AND COMPARISON
from pylab import *
from matplotlib import pyplot
import matplotlib.gridspec as gridspec

# Re-create the fluid resolution mesh: regular spacing, with differences at the bottom (0) and at the top (ndimz-2). 
dsig = np.ones(ndimz-1)*dz
dsig[0]=1.5*dz
dsig[-1]=0.5*dz
sig=np.zeros(ndimz)
for i in range(1,ndimz):
	sig[i]=sig[i-1]+dsig[i-1]
#Figure creation: compare the analytical solution and the fluid resolution
pyplot.figure(1)
gs = gridspec.GridSpec(1, 1)
ax1 = subplot(gs[0,0])
p1=ax1.plot(vxFluid,sig,'-ob',label='u_f')	#fluid resolution results: in blue points
p2=ax1.semilogy(vxFluid_analytic,z_analytic,'-r',label='u_f^ex')	#Analytical solution: red line
xlabel('<v_x^f> (m/s)')
ylabel('z (m)')
savefig('figProfiLog.png')
show()

print "finished! exit\n"
exit()	#Leave the simulation
