# encoding: utf-8
# 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
import math,os,sys

#List your scripts here
for script in ["checkTestDummy","checkTestTriax"]:
	try:
		print "running: ",script
		execfile(checksPath+"/"+script+".py")
	except:
		print script," failure"
	#this "else" is redundant with scriipt output, not needed
	#else:
		#print script," sucess, inspect results."
