# encoding: utf-8
# 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
import yade,math,os,sys

scriptsToRun=os.listdir(checksPath)
resultStatus = 0
nFailed=0

skipScripts = ['checkList.py']

for script in scriptsToRun:
	if (script[len(script)-3:]==".py" and script not in skipScripts):
		try:
	 		print "###################################"
			print "running: ",script
			execfile(checksPath+"/"+script)
			if (resultStatus>nFailed):
				print "Status: FAILURE!!!"
				nFailed=resultStatus
			else:
				print "Status: success"
			print "___________________________________"
		except:
			print script," failure"
		O.reset()
	elif (script in skipScripts):
		print "###################################"
		print "Skipping %s, because it is in SkipScripts"%script
		
if (resultStatus>0):
	print resultStatus, " tests are failed"
	sys.exit(1)
else:
	sys.exit(0)

