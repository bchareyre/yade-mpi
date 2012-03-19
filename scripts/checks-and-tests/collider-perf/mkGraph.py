#encoding: utf-8
dta={'QS':{},'IS':{},'ISS':{}}
import sys
for f in sys.argv[1:]:
	print f,'',
	N=f.split('.')[1];
	assert(N[-1]=='k'); N=1000*int(N[:-1])
	if '.q.' in f: collider='QS'
	elif '.i.' in f: collider='IS'
	elif '.is.' in f: collider='ISS'
	else: raise RuntimeError("Unknown collider type for file "+f)
	for l in open(f):
		if 'Collider' in l:
			t=l.split()[2]; assert(t[-2:]=='us'); t=float(t[:-2])/1e6
			if not dta[collider].has_key(N): dta[collider][N]=[t]
			else: dta[collider][N]+=[t*0.01] # the second time is per 100 iterations
print 

ISS_N=dta['ISS'].keys(); ISS_N.sort()
QS_N=dta['QS'].keys(); QS_N.sort()
IS_N=dta['IS'].keys(); IS_N.sort()
ISSinit=[dta['ISS'][N][0] for N in ISS_N]; ISSstep=[dta['ISS'][N][1] for N in ISS_N]
QSinit=[dta['QS'][N][0] for N in QS_N]; QSstep=[dta['QS'][N][1] for N in QS_N]
ISinit=[dta['IS'][N][0] for N in IS_N]; ISstep=[dta['IS'][N][1] for N in IS_N]
from pylab import *
plot(IS_N,ISinit,'y',ISS_N,ISSinit)
gca().set_yscale('log')
xlabel("Number of spheres")
ylabel(u"Log time for the 1st collider step [s]")
title("Colliders performance (QS=QuickSoft, IS=InsertionSort, IS/s=IS+stride)")
legend(('IS init','IS/s init',),'upper left')
ax2=twinx()
plot(IS_N,ISstep,'k-',ISS_N,ISSstep,'r-',QS_N,QSstep,'g-',QS_N,QSinit,'b-')
ylabel(u"Linear time per 1 step [s]")
legend(('IS step','IS/s step','QS step','QS init'),'right')
grid()
savefig('colliders.svg')
show()
