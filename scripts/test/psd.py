# encoding: utf-8
#
# demonstrate how to generate sphere packing based on arbitrary PSD (particle size distribution)
# show the difference between size-based and mass-based (≡ volume-based in our case) PSD
#
import matplotlib; matplotlib.rc('axes',grid=True)
from yade import pack
import pylab
# PSD given as points of piecewise-linear function
psdSizes,psdCumm=[.02,.04,.06,.08,.1],[0.,.1,.7,.9,1.]
pylab.plot(psdSizes,psdCumm,label='precribed mass PSD')
sp1=pack.SpherePack(); sp1.psdScaleExponent=2 # this exponent is somewhat empirical, see docs of SpherePack
sp1.makeCloud((0,0,0),(1,1,1),psdSizes=psdSizes,psdCumm=psdCumm,distributeMass=True)
pylab.semilogx(*sp1.psd(bins=30,mass=True),label='Mass PSD of %d random spheres'%len(sp1))
pylab.semilogx(*sp1.psd(bins=30,mass=False),label='Size PSD of %d andom spheres'%len(sp1))
pylab.legend()

# uniform distribution of size (sp2) and of mass (sp3)
sp2=pack.SpherePack(); sp2.makeCloud((0,0,0),(1,1,1),rMean=0.03,rRelFuzz=2/3.);
sp3=pack.SpherePack(); sp3.makeCloud((0,0,0),(1,1,1),rMean=0.03,rRelFuzz=2/3.,distributeMass=True);
pylab.figure()
pylab.plot(*(sp2.psd(mass=True)+('g',)+sp3.psd(mass=True)+('r',)))
pylab.legend(['Mass PSD of size-uniform distribution','Mass PSD of mass-uniform distribution'])

pylab.figure()
pylab.plot(*(sp2.psd()+('g',)+sp3.psd()+('r',)))
pylab.legend(['Size PSD of size-uniform distribution','Size PSD of mass-uniform distribution'])
pylab.show()

pylab.show()


