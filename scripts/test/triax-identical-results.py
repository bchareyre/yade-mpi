# Test if different algorithm version give same results for TriaxialTest.
#
# The first run creates initial sphere packing, and saves resulting positions
# after 2000 steps. Subsequent runs only save resulting positions.
#
# Compare output files with diff to see if the are 100% identical
#
from os.path import exists
sph='triax-identical-results'
i=0; outSph=''
while True:
	outSph='%s-out%02d.spheres'%(sph,i)
	if not exists(outSph): break
	i+=1
inSph='%s-in.spheres'%sph
if exists(inSph): print "Using existing initial configuration",inSph
else:
	Preprocessor('TriaxialTest').load()
	print "Using new initial configuration in",inSph
	utils.spheresToFile(inSph)
Preprocessor('TriaxialTest',{'importFilename':inSph}).load()
O.usesTimeStepper=False
O.dt=utils.PWaveTimeStep()
#
# uncomment this line to enable shear computation in SpheresContactGeometry and then compare results with this line commented
#
[e for e in O.engines if e.name=='ElasticContactLaw'][0]['useShear']=True
if 1:
	O.run(2000,True)
	utils.spheresToFile(outSph)
	print "Results saved to",outSph
	quit()
