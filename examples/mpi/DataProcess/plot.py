import numpy as np
import matplotlib.pyplot as plt
import os
from label_lines import *

font = {'family' : 'Arial',
        'weight' : 'roman',
        'size'   : 12}
plt.rc('font', **font)
plt.rcParams['lines.linewidth'] = 1.5
title_font = {'fontname':'Arial', 'size':'12', 'color':'black', 'weight':'normal',
              'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Arial', 'size':'12'}
tickFontSize = 12
plt.rc('legend', fontsize=10)

prePost = ['/PREHACK/','/POSTHACK/']
weakStrong = ['strongScalability/']
lines = [['g--^','b--o','m--x'],['g-^','b-o','m-x']]
alphas = [0.4,1]
preposthack=['Pre','Post']
iters=2000
pwd = os.getcwd()
fig,ax = plt.subplots()
fig2,ax2 = plt.subplots()
for i,d in enumerate(prePost):
	for j,e in enumerate(weakStrong):
		for k,f in enumerate(os.listdir(pwd+d+e)):
			dat = np.loadtxt(pwd+d+e+f,skiprows=1)
			spheres, walltime, cores = dat[:,3], dat[:,4], dat[:,0]
			cundall = spheres*iters/walltime
			line = lines[i]
			ax.plot(cores,cundall,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))
			ax2.plot(cores,walltime,line[k],alpha=alphas[i],label='%shack %.1fM Sph.' % (preposthack[i],spheres[k]/1e6))

plt.figure(fig.number)
ax.ticklabel_format(axis='y',scilimits=(0,0))
ax.set_xlabel('Number of cores (-)')
ax.set_ylabel('Cundall number (No Spheres * iterations/walltime)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/strongScalability_cundall.png',format='png')
#plt.show()

plt.figure(fig2.number)
ax2.ticklabel_format(axis='y',scilimits=(0,0))
ax2.set_xlabel('Number of cores (-)')
ax2.set_ylabel('Wall times (s)')
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('./figs/stongScalability_walltimes.png',format='png')
plt.show()


## weak scalability
tilesize = ['100$^2$','200$^2$','300$^2$']
weakStrong=['weakScalability/']
fig,ax = plt.subplots()
fig2,ax2 = plt.subplots()
for i,d in enumerate(prePost):
        for j,e in enumerate(weakStrong):
                for k,f in enumerate(os.listdir(pwd+d+e)):
                        dat = np.loadtxt(pwd+d+e+f,skiprows=1)
                        spheres, walltime, cores = dat[:,3], dat[:,4], dat[:,0]
                        cundall = spheres*iters/walltime
                        line = lines[i]
                        ax.plot(spheres/1e3,cundall,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
                        ax2.plot(spheres/1e3,walltime,line[k],alpha=alphas[i],label='%shack %s Sph./core' % (preposthack[i],tilesize[k]))
			for l,txt in enumerate(cores):
				ax2.annotate(str(int(txt)),(spheres[l]/1e3,walltime[l]),fontsize=10)
			

ax2.annotate('No. cores annotated\non each point', (0,2.1e3),fontname='Arial',fontstyle='normal',weight='ultralight',fontsize=10)

plt.figure(fig.number)
ax.ticklabel_format(axis='y',scilimits=(0,0))
ax.set_xlabel('Thousands of spheres (-)')
ax.set_ylabel('Cundall number (No. Spheres * iterations/walltime)')
plt.legend(loc='center right')
plt.tight_layout()
plt.savefig('./figs/weakScalability_cundall.png',format='png')
#plt.show()

plt.figure(fig2.number)
ax2.ticklabel_format(axis='y',scilimits=(0,0))
ax2.set_xlabel('Thousands of spheres (-)')
ax2.set_ylabel('Wall times (s)')
plt.legend(loc='upper left')
plt.tight_layout()
plt.savefig('./figs/weakScalability_walltimes.png',format='png')
plt.show()

