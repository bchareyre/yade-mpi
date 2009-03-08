#encoding: utf-8
dta={'QS':{},'SAP':{}}
import sys
for f in sys.argv[1:]:
	print f,'',
	N=f.split('.')[1];
	assert(N[-1]=='k'); N=1000*int(N[:-1])
	if '.q.' in f: collider='QS'
	else: collider='SAP'
	for l in open(f):
		if 'Collider' in l:
			t=l.split()[2]; assert(t[-2:]=='us'); t=float(t[:-2])/1e6
			if not dta[collider].has_key(N): dta[collider][N]=[t]
			else: dta[collider][N]+=[t*0.01] # the second time is per 100 iterations
print 

SAP_N=dta['SAP'].keys(); SAP_N.sort()
QS_N=dta['QS'].keys(); QS_N.sort()
SAPinit=[dta['SAP'][N][0] for N in SAP_N]; SAPstep=[dta['SAP'][N][1] for N in SAP_N]
QSinit=[dta['QS'][N][0] for N in QS_N]; QSstep=[dta['QS'][N][1] for N in QS_N]
from pylab import *
plot(SAP_N,SAPinit,'m')
gca().set_yscale('log')
xlabel("Number of spheres")
ylabel(u"Log (!) time for the 1st SAP collider step [s]")
title("SAP vs. QuickSort colliders performance")
legend(('SAP init',),'upper left')

ax2=twinx()
plot(SAP_N,SAPstep,'r-',QS_N,QSstep,'g-',QS_N,QSinit,'b-')
ylabel(u"Linear time per 1 step [s]")
legend(('SAP step','QuickSort step','QuickSort init'),'right')
grid()
savefig('colliders.svg')
show()
