from __main__ import Vector,Quaternion

def bodyPropDict(): return {'id':0,'clumpId':1,'x':2,'r':3,'v':4,'w':5,'m':6,'I':7,'isClump':8,'isClumpMember':9,'isStandalone':10}
def simulPropDict(): return {'t':0,'dt':1,'i':2,'file':3,'nBodies':4,'sel':5}
def bodyProp(id,p):
	#return {0:id,1:3,2:(2.,2.,2.),3:(3.,0.,0.,0.),4:(4.,4.,4.),5:5.,6:(6.,6.,6.),7:0,8:1,9:0,10:0}[p]
	return {0:id,1:3,2:Vector(2.,2.,2.),3:Quaternion(3.,0.,0.,0.),4:Vector(4.,4.,4.),5:5.,6:Vector(6.,6.,6.),7:0,8:1,9:0,10:0}[p]
def simulProp(p):
	return {0:55.5,1:2.44e-4,2:13452,3:'simulation.xml',4:64}[p]
