import numpy as np
import matplotlib.pyplot as plt
import os
from label_lines import *

font = {'family' : 'Arial',
        'weight' : 'roman',
        'size'   : 15}
plt.rc('font', **font)
plt.rcParams['lines.linewidth'] = 1.5
title_font = {'fontname':'Arial', 'size':'12', 'color':'black', 'weight':'normal',
              'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Arial', 'size':'17'}
tickFontSize = 15
plt.rc('legend', fontsize=10)

totalWallTime = 0
numMerge = 5
prePost = ['/PREHACK/','/POSTHACK/']
weakStrong = ['strongScalability/']
lines = [['g--^','b--o','m--x'],['g-^','b-o','m-x']]
alphas = [0.4,1]
preposthack=['Pre','Post']
iters=2000
pwd = os.getcwd()
fig,ax = plt.subplots()
fig2,ax2 = plt.subplots()
fig3,ax3 = plt.subplots()
fig4,ax4 = plt.subplots()
for i,d in enumerate(prePost):
	for j,e in enumerate(weakStrong):
		EngineFileNames = sorted(os.listdir(pwd+d+e+'EngineTimings'))
		WallFileNames = sorted(os.listdir(pwd+d+e+'WallTimes')) 
		for k in range(0,len(EngineFileNames)):
			engineDat = np.loadtxt(pwd+d+e+'EngineTimings/'+EngineFileNames[k],skiprows=1)
			wallTimeDat = np.loadtxt(pwd+d+e+'WallTimes/'+WallFileNames[k],skiprows=1)			
			runtime = engineDat[:,5]
			spheres, walltime, cores = wallTimeDat[:,3], wallTimeDat[:,4], wallTimeDat[:,0]
			totalWallTime += sum(walltime * cores)
			mergeTime = (walltime - runtime)
			cundall_MergeTime = spheres*numMerge/mergeTime		
			cundall_WallTime = spheres*iters/walltime
			line = lines[i]
			ax.plot(cores,cundall_WallTime,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))
			ax2.plot(cores,walltime,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))
			ax3.plot(cores,cundall_MergeTime,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))
			ax4.plot(cores,mergeTime/numMerge,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))

plt.figure(fig.number)
ax.ticklabel_format(axis='y',scilimits=(0,0))
ax.set_xlabel('Number of cores (-)')
ax.set_ylabel('Cundall$_{wall}$\n(No Spheres * iterations/walltime)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/strongScalability_cundall_walltime.png',format='png')

plt.figure(fig2.number)
ax2.ticklabel_format(axis='y',scilimits=(0,0))
ax2.set_xlabel('Number of cores (-)')
ax2.set_ylabel('Wall times (s)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/stongScalability_walltimes.png',format='png')

plt.figure(fig3.number)
ax3.ticklabel_format(axis='y',scilimits=(0,0))
ax3.set_xlabel('Number of cores (-)')
ax3.set_ylabel('Cundall$_{merge}$\n(No Spheres * merges/mergetime)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/strongScalability_cundall_mergetime.png',format='png')

plt.figure(fig4.number)
ax4.ticklabel_format(axis='y',scilimits=(0,0))
ax4.set_xlabel('Number of cores (-)')
ax4.set_ylabel('Avg merge time (s)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/stongScalability_mergetime.png',format='png')
#plt.show()


## weak scalability
tilesize = ['100$^2$','200$^2$','300$^2$']
weakStrong=['weakScalability/']
fig,ax = plt.subplots()
fig2,ax2 = plt.subplots()
fig3,ax3 = plt.subplots()
fig4,ax4 = plt.subplots()
for i,d in enumerate(prePost):
	for j,e in enumerate(weakStrong):
		EngineFileNames = sorted(os.listdir(pwd+d+e+'EngineTimings'))
		WallFileNames = sorted(os.listdir(pwd+d+e+'WallTimes')) 
		for k in range(0,len(EngineFileNames)):
			engineDat = np.loadtxt(pwd+d+e+'EngineTimings/'+EngineFileNames[k],skiprows=1)
			wallTimeDat = np.loadtxt(pwd+d+e+'WallTimes/'+WallFileNames[k],skiprows=1)			
			runtime = engineDat[:,5]
			spheres, walltime, cores = wallTimeDat[:,3], wallTimeDat[:,4], wallTimeDat[:,0]
			totalWallTime += sum(walltime * cores)
			mergeTime = (walltime - runtime)
			cundall_MergeTime = spheres*numMerge/mergeTime		
			cundall_WallTime = spheres*iters/walltime
			line = lines[i]
                        ax.plot(spheres/1e3,cundall_WallTime,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
                        ax2.plot(spheres/1e3,walltime,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
			for l,txt in enumerate(cores):
				ax2.annotate(str(int(txt)),(spheres[l]/1e3,walltime[l]),fontsize=10)
			ax3.plot(spheres/1e3,cundall_MergeTime,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
			ax4.plot(spheres/1e3,mergeTime/numMerge,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
			for l,txt in enumerate(cores):
				ax4.annotate(str(int(txt)),(spheres[l]/1e3,mergeTime[l]/numMerge),fontsize=10)

ax2.annotate('No. cores annotated\non each point', (0,1.8e3),fontname='Arial',fontstyle='normal',weight='ultralight',fontsize=10)
ax4.annotate('No. cores annotated\non each point', (0,0.3e3),fontname='Arial',fontstyle='normal',weight='ultralight',fontsize=10)
plt.figure(fig.number)
ax.ticklabel_format(axis='y',scilimits=(0,0))
ax.set_xlabel('Thousands of spheres (-)')
ax.set_ylabel('Cundall$_{wall}$\n(No. Spheres * iterations/walltime)')
plt.legend(loc='center right')
plt.tight_layout()
plt.savefig('./figs/weakScalability_cundall_walltimes.png',format='png')

plt.figure(fig2.number)
ax2.ticklabel_format(axis='y',scilimits=(0,0))
ax2.set_xlabel('Thousands of spheres (-)')
ax2.set_ylabel('Wall times (s)')
plt.legend(loc='upper left')
plt.tight_layout()
plt.savefig('./figs/weakScalability_walltimes.png',format='png')

plt.figure(fig3.number)
ax3.ticklabel_format(axis='y',scilimits=(0,0))
ax3.set_xlabel('Thousands of spheres (-)')
ax3.set_ylabel('Cundall$_{merge}$\n(No. Spheres * merges/mergetime)')
plt.legend(loc='center right')
plt.tight_layout()
plt.savefig('./figs/weakScalability_cundall_merge.png',format='png')
#plt.show()

plt.figure(fig4.number)
ax4.ticklabel_format(axis='y',scilimits=(0,0))
ax4.set_xlabel('Thousands of spheres (-)')
ax4.set_ylabel('Avg merge time (s)')
plt.legend(loc='upper left')
plt.tight_layout()
plt.savefig('./figs/weakScalability_mergetimes.png',format='png')

print totalWallTime


