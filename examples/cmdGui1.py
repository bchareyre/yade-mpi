# sample rc script for PythonUI
#
p=Preprocessor("BoxStack")
# changing attributes
p['nbBoxes']=[3,5,5]
p['boxDensity']=4000
# run the generator, returns status
p.generate('/tmp/scene.xml')
#
# create Omega instance, which controls simulation
#
o=Omega()
# load simulation
o.load("/tmp/scene.xml")
# run the simulation
o.run()
# !! views currently broken
# open one View (more views will crash...)
# v=View()
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
