# sample rc script for cmdGui
#
# yade-trunk -N cmdGui -- -s examples/cmdGui1.py
#
#
p=Preprocessor()
# show available generators
print "Available generators:",p.listGenerators()
p.generator="BoxStack"
# set output file (default: ../data/scene.xml)
p.output="/tmp/scene.xml"
# serializer (default: XMLFormatManager, no need to change)
print "Available serializers:",p.listSerializers()
# changing attributes
p['nbBoxes']=[3,5,5]
p['boxDensity']=4000
# run the generator, returns status
p.generate()
#
# create Omega instance, which controls simulation
#
o=Omega()
# load simulation
o.load("/tmp/scene.xml")
# run the simulation
o.run()
#
# open one View (more views will crash...)
v=View()
# get iteration number
print o.iter
# stop somewhere after iteration 10000
import time
while True:
	time.sleep(1)
	print "iteration %d\tsimulation time %gs\treal time %gs"%(o.iter,o.time,o.realtime)
	if o.iter>1000:
		o.pause()
		break
# quit yade
sys.exit(0)
