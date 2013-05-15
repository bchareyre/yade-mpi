# Test if different algorithm version give same results for TriaxialTest.
#
# The first run creates initial sphere packing, and saves resulting positions
# after 2000 steps. Subsequent runs only save resulting positions.
#
# Compare output files with diff to see if the are 100% identical
#

""" THIS SCRIPT IS NOT WORKING!
ERROR MESSAGE:

Running script triax-identical-results.py                                                                                            
Using new initial configuration in triax-identical-results-in.spheres                                                                
Traceback (most recent call last):                                                                                                   
  File "/home/me/YADE/YADE3041/bin/yade-bzr3041", line 182, in runScript                                                             
    execfile(script,globals())                                                                                                       
  File "triax-identical-results.py", line 20, in <module>                                                                            
    spheresToFile(inSph)                                                                                                       
AttributeError: 'module' object has no attribute 'spheresToFile' 
"""

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
	TriaxialTest(noFiles=True).load()
	print "Using new initial configuration in",inSph
	spheresToFile(inSph)
TriaxialTest(importFilename=inSph,noFiles=True).load()
O.usesTimeStepper=False
O.dt=PWaveTimeStep()
#
# uncomment this line to enable shear computation in ScGeom and then compare results with this line commented
#
[e for e in O.engines if e.name=='ElasticContactLaw'][0]['useShear']=True
if 1:
	#for i in range(0,100):
	#	#	O.save('/tmp/a.%03d.xml'%O.iter)
	#	O.step()
	O.run(2000,True)
	spheresToFile(outSph)
	print "Results saved to",outSph
	#quit()
