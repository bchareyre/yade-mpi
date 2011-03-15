# encoding: utf-8
# 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
import yade,math,os,sys

scriptsToRun=os.listdir(checksPath)
resultStatus = 0
for script in scriptsToRun:
	if (script[len(script)-3:]==".py" and not(script=="checkList.py")):
		try:
	 		print "###################################"
			print "running: ",script
			execfile(checksPath+"/"+script)
			if (resultStatus):
				print "Status: FAILURE!!!"
				break
			else:
				print "Status: success"
			print "___________________________________"
		except:
			print script," failure"
			break
		O.reset()
		
if (resultStatus):
	print "Some tests are failed!"
	sys.exit(1)
else:
	sys.exit(0)

