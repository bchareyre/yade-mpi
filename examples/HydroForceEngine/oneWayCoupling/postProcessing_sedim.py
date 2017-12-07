#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 24/11/2017
#
# Post processing script, to extract and plot the results of sedimentTransportExample scripts, in terms of dimensionless sediment transport,
# and time-averaged depth profiles of streamwise fluid and solid velocity, solid volume fraction and sediment transport rate density. 
# For example of applications, see the articles Maurin et al (2015,2016), and the PhD of Maurin (2015), available in the references of YADE.
#
############################################################################################################################################################################


import numpy as np
import os
from matplotlib.pyplot import *
import matplotlib.gridspec as gridspec

#######
# INPUT
timeStepSave = 0.1	# Data saved every timeStepSave seconds in the simulation
startFile = 0		# File where the post-processing begins
endFile = 400		# File where the post-processing ends

scriptPath = os.path.abspath(os.path.dirname(sys.argv[-1])) #Path where the script is stored
if os.path.exists(scriptPath +'/data/')==False:	#If the data folder does not exist, no data to extract, exit. 
	print '\n There is no data to extract in this folder ! \n'
	exit()
else:	#Else, extract the first file in order to get the size of the vectors, ndimz
	execfile(scriptPath +'/data/0.py')
	ndimz = len(phiPartPY)	#Size of the vectors, mesh parameter. 

#Initilization of the variable to extract and plot
[qs,time,phiPart,vxPart,vxFluid] = [[],[],np.zeros(ndimz),np.zeros(ndimz),np.zeros(ndimz)]

########
# LOOP over time to EXTRACT the data
########
for i in range(startFile,endFile):
	nameFile = scriptPath +'/data/' + str(i)+'.py'	#Name of the file at the considered time step
	if  os.path.exists(nameFile)==False:	#Check if the file exist
		print('\nThe file {0} does not exist, stop the post processing at this stage !\n'.format(nameFile))
		endFile = i-1	#Last file processed
		break
	#Extract the data from at the time (file) considered. Assign the vectors to qsMean, phiPartPY, vxPartPY, vxFluidPY, and zAxis see the structure of a saved file X.py. 
	execfile(nameFile)
	time+=[i*timeStepSave]		#Recreate the time scale
	qs+=[qsMean]			#Extract the sediment transport rate as a function of time
	phiPart+=phiPartPY		#Average over time the solid volume fraction vertical profile
	vxPart+=vxPartPY*phiPartPY	#Average over time the solid velocity profile. The spatial averaging is weighted by the particle volume so that it is necessary to re-multiply the averaging by the solid volume fraction before re-normalizing. See the paper Maurin et al (2015) in the references for more details on the averaging. 
	vxFluid+=vxFluidPY		#Average over time the fluid velocity vertical profile
#Normalization
toto = np.where(phiPart>0)
vxPart[toto]/=phiPart[toto]	#Avoid division by zero
phiPart/=(endFile-startFile+1)
vxFluid/=(endFile-startFile+1)


######
# PLOT

# sediment transport rate as a function of time
fig1 = figure()		#Create a figure
gs = gridspec.GridSpec(1,1)	#Split the figure in 1 row and 1 column
ax=subplot(gs[0,0])		#Assign the axis
ax.plot(time,qs,'ok')		#Plot the sediment transport rate as a function of time
ax.grid()			#Put a grid on the figure
ax.set_xlabel('t (s)')		#Assign the x axis label
ax.set_ylabel(r'$Q_s^*$',rotation='horizontal')#Assign the y axis label
savefig('transportRate.png')	#Save the figure

# Solid and fluid velocity depth profiles, solid volume fraction and sediment transport rate density depth profiles
fig2 = figure()
gs = gridspec.GridSpec(1,3)	#Split the figure in 1 row and 3 column
ax1=subplot(gs[0,0])		#ax1 = subfigure corresponding to 1st row 1st column
ax2=subplot(gs[0,1])		#ax2 = subfigure corresponding to 1st row 2nd column
ax3=subplot(gs[0,2])		#ax3 = subfigure corresponding to 1st row 3rd column
ax1.plot(vxPart,zAxis,'-r')	#Plot the solid velocity profile on first subfigure
ax1.plot(vxFluid,zAxis,'-b')	#Plot the fluid velocity profile on first subfigure
ax2.plot(phiPart,zAxis,'-r')	#Plot the solid volume fraction on second subfigure
ax3.plot(phiPart*vxPart,zAxis,'-r')	#Plot the sediment transport rate density on the third subfigure
ax1.set_xlabel(r'$<v^k_x>$ (m/s)')	#Assign the x axis label of the first subfigure
ax1.set_ylabel(r'$z/d$',rotation='horizontal')	#Assign the y axis label of the first subfigure
ax2.set_xlabel(r'$\phi$') 	#Assign the x axis label of the second subfigure
ax3.set_xlabel(r'$q (m/s)$')	#Assign the x axis label of the third subfigure
ax2.set_yticklabels([])	#Remove the vertical scale for the second subfigure, same as first
ax3.set_yticklabels([])	#Remove the vertical scale for the third subfigure, same as first
ax1.grid()	#Put a grid on each subfigure
ax2.grid()
ax3.grid()
fig2.subplots_adjust(left=0.08, bottom=0.11, right=0.98, top=0.98, wspace=.1)	#Adjust the spacing around the full figure (left, bottom, right, top) and between the subfigure (wspace), in percent of the figure size
savefig('depthProfiles.png')	#Save the figure

show()	#Show the two figures created
