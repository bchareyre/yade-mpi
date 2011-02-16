# encoding: utf-8
from yade import pack, qt

#### short description of script
print 'This script shows the use of the function pack.hexaNet (interactions are not initialised)'

#### define parameters for the net
# mesh geometry
mos = 0.08
a = 0.04
b = 0.04
# wire diameter
d = 2.7/1000.
# net dimension
Lx = 0.2
Ly = 0.15

# properties of particles
radius = d*4.

#### startAtCorner=True
kw = {'color':[1,1,0],'wire':True,'highlight':False,'fixed':False,'material':-1}

#### packing 1
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[0,0,0], xLength=Lx, yLength=Ly, mos=mos, a=a, b=b, startAtCorner=True, isSymmetric=True, **kw )
O.bodies.append(netpack)
print 'Packing 1:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 2
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[0.4,0,0], xLength=Lx, yLength=Ly, mos=mos, a=a, b=b, startAtCorner=True, isSymmetric=False, **kw )
O.bodies.append(netpack)
print 'Packing 2:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 3
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[0,-0.4,0], xLength=Lx, yLength=Ly+0.05, mos=mos, a=a, b=b, startAtCorner=True, isSymmetric=True, **kw )
O.bodies.append(netpack)
print 'Packing 3:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 4
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[0.4,-0.4,0], xLength=Lx, yLength=Ly+0.05, mos=mos, a=a, b=b, startAtCorner=True, isSymmetric=False, **kw )
O.bodies.append(netpack)
print 'Packing 4:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### startAtCorner=False
kw = {'color':[1,0,0],'wire':True,'highlight':False,'fixed':False,'material':-1}

#### packing 1
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[1,0,0], xLength=Lx, yLength=Ly, mos=mos, a=a, b=b, startAtCorner=False, isSymmetric=True, **kw )
O.bodies.append(netpack)
print 'Packing 1:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 2
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[1.4,0,0], xLength=Lx, yLength=Ly, mos=mos, a=a, b=b, startAtCorner=False, isSymmetric=False, **kw )
O.bodies.append(netpack)
print 'Packing 2:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 3
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[1,-0.4,0], xLength=Lx, yLength=Ly+0.05, mos=mos, a=a, b=b, startAtCorner=False, isSymmetric=True, **kw )
O.bodies.append(netpack)
print 'Packing 3:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### packing 4
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[1.4,-0.4,0], xLength=Lx, yLength=Ly+0.05, mos=mos, a=a, b=b, startAtCorner=False, isSymmetric=False, **kw )
O.bodies.append(netpack)
print 'Packing 4:'
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly

## to see it
v=qt.Controller()
v=qt.View()
