# encoding: utf-8
#
# demonstrate how to generate sphere packing based on arbitrary PSD (particle size distribution)
# show the difference between size-based and mass-based (≡ volume-based in our case) PSD
#
import matplotlib; matplotlib.rc('axes',grid=True)
from yade import pack
import pylab
# PSD given as points of piecewise-linear function
psdSizes,psdCumm=[.02,0.04,0.045,.05,.06,.08,.12],[0.,0.1,0.3,0.3,.3,.7,1.]
pylab.plot(psdSizes,psdCumm,label='precribed mass PSD')
sp0=pack.SpherePack();
sp0.makeCloud((0,0,0),(1,1,1),psdSizes=psdSizes,psdCumm=psdCumm,distributeMass=True)
sp1=pack.SpherePack();
sp1.makeCloud((0,0,0),(1,1,1),psdSizes=psdSizes,psdCumm=psdCumm,distributeMass=True,num=5000)
sp2=pack.SpherePack();
sp2.makeCloud((0,0,0),(1,1,1),psdSizes=psdSizes,psdCumm=psdCumm,distributeMass=True,num=20000)
pylab.semilogx(*sp0.psd(bins=30,mass=True),label='Mass PSD of (free) %d random spheres'%len(sp0))
pylab.semilogx(*sp1.psd(bins=30,mass=True),label='Mass PSD of (imposed) %d random spheres'%len(sp1))
pylab.semilogx(*sp2.psd(bins=30,mass=True),label='Mass PSD of (imposed) %d random spheres (scaled down)'%len(sp2))

pylab.legend()

# uniform distribution of size (sp3) and of mass (sp4)
sp3=pack.SpherePack(); sp3.makeCloud((0,0,0),(1,1,1),rMean=0.03,rRelFuzz=2/3.,distributeMass=False);
sp4=pack.SpherePack(); sp4.makeCloud((0,0,0),(1,1,1),rMean=0.03,rRelFuzz=2/3.,distributeMass=True);
pylab.figure()
pylab.plot(*(sp3.psd(mass=True)+('g',)+sp4.psd(mass=True)+('r',)))
pylab.legend(['Mass PSD of size-uniform distribution','Mass PSD of mass-uniform distribution'])

pylab.figure()
pylab.plot(*(sp3.psd(mass=False)+('g',)+sp4.psd(mass=False)+('r',)))
pylab.legend(['Size PSD of size-uniform distribution','Size PSD of mass-uniform distribution'])
pylab.show()

pylab.show()