 
#!/usr/local/bin/yade-trunk -x
# encoding: utf-8
from yade import pack,log,utils,export,plot
import math,os,sys

tolerance=0.01
interactive=True
errors=0

tt=TriaxialTest(internalCompaction=True,numberOfGrains=100,compactionFrictionDeg=0,sphereFrictionDeg=30,importFilename='checkTestTriax.spheres')
tt.generate("checkTest.yade")
O.load("checkTest.yade")
O.run(2020,True)
if interactive:
	print O.engines[4].stress(0)[1],O.engines[4].stress(1)[1], O.engines[4].stress(2)[0], O.engines[4].stress(3)[0], O.engines[4].stress(4)[2], O.engines[4].stress(5)[2]
	os.system('gnuplot -e "plot \'./WallStresses\' using 1:3; replot \'./WallStressesCheckTest\' using 1:3; replot \'./WallStresses\' using 1:4; replot \'./WallStressesCheckTest\' using 1:4; pause -1"')

if abs((O.engines[4].stress(1)[1]-107157.2)/107157.2)>tolerance :
	print "Triaxial checkTest: difference on peak stress"
	errors+=1
if abs((O.engines[4].stress(3)[0]-50058.7)/50058.7)>tolerance :
	print "Triaxial checkTest: difference on peak stress"
	errors+=1
if errors==0:
	print "success: checkTestTriax"

