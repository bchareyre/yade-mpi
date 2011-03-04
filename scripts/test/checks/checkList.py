# encoding: utf-8
# 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
import yade,math,os,sys

scriptsToRun=os.listdir(checksPath)
for script in scriptsToRun:
	if (script[len(script)-3:]==".py" and not(script=="checkList.py")):
		try:
	 		print "###################################"
			print "running: ",script
			execfile(checksPath+"/"+script)
			print "___________________________________"
		except:
			print script," failure"
		O.reset()
