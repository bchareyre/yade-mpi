from yade import log
import shutil, tempfile
#
# Generate box or cylindrical packing that is rather compact using TriaxialTest.
#

# if non-negative, the resulting sample will be cylindrical; if < 0, generate box
cylAxis=1
# how many layers of average spheres to cut away from flat parts (relative to mean sphere radius); sphere center count
cutoffFlat=3.5
# how many layers of average spheres to cut away from round surfaces (relative to mean sphere radius); sphere centers count
cutoffRound=2

tt=TriaxialTest(
	numberOfGrains=5000,
	radiusMean=3e-4,
	# this is just size ratio if radiusMean is specified
	# if you comment out the line above, it will be the corner (before compaction) and radiusMean will be set accordingly
	upperCorner=[2,4,2],
	radiusStdDev=0, # all spheres exactly the same radius
	
	##
	## no need to touch any the following, till the end of file
	##
	noFiles=True,
	lowerCorner=[0,0,0],
	sigmaIsoCompaction=1e7,
	sigmaLateralConfinement=1e3,
	StabilityCriterion=.05,
	strainRate=.2,
	fast=True,
	maxWallVelocity=.1,
	wallOversizeFactor=2,
	autoUnload=True, # unload after isotropic compaction
	autoCompressionActivation=False # stop once unloaded
)

tt.load()
log.setLevel('TriaxialCompressionEngine',log.WARN)
O.run() ## triax stops by itself once unloaded
O.wait()

# resulting specimen geometry
ext=utils.aabbExtrema()
rSphere=tt['radiusMean']

outFile=tempfile.NamedTemporaryFile(delete=False).name

if cylAxis<0: # box-shaped packing
	aabbMin,aabbMax=tuple([ext[0][i]+rSphere*cutoffFlat for i in 0,1,2]),tuple([ext[1][i]-rSphere*cutoffFlat for i in 0,1,2])
	def isInBox(id):
		pos=O.bodies[id].phys.pos
		return utils.ptInAABB(pos,aabbMin,aabbMax)
	nSpheres=utils.spheresToFile(outFile,consider=isInBox)
else: # cylinger packing
	mid   =[.5*(ext[1][i]+ext[0][i]) for i in [0,1,2]]
	extent=[.5*(ext[1][i]-ext[0][i]) for i in [0,1,2]]
	ax1,ax2=(cylAxis+1)%3,(cylAxis+2)%3
	cylRadius=min(extent[ax1],extent[ax2])-cutoffRound*rSphere
	cylHalfHt=extent[cylAxis]-cutoffFlat*rSphere
	def isInCyl(id):
		pos=O.bodies[id].phys.pos
		axisDist=sqrt((pos[ax1]-mid[ax1])**2+(pos[ax2]-mid[ax2])**2)
		if axisDist>cylRadius: return False
		axialDist=abs(pos[cylAxis]-mid[cylAxis])
		if axialDist>cylHalfHt: return False
		return True
	nSpheres=utils.spheresToFile(outFile,consider=isInCyl)

outFile2='pack-%d-%s.spheres'%(nSpheres,'box' if cylAxis<0 else 'cyl')
shutil.move(outFile,outFile2)
print nSpheres,'spheres written to',outFile2
quit()
