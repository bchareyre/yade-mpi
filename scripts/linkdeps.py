#!/usr/bin/python

from sys import *
import os,re
from string import *

srcRoot='./'

from os.path import sep
import shutil

def getModule(dir):
	m=re.match('^.*?'+sep+'((extra|core)|((gui|lib|pkg)'+sep+'.*?))(|'+sep+'.*)$',dir)
	assert(m)
	return m.group(1).replace(sep,'-')

def walkSourceFiles():
	ret=[]
	for root, dirs, files in os.walk(srcRoot,topdown=True):
		for d in ('.svn','mgpost','SpherePadder','QGLViewer','triangulation','sqlite3x','miniWm3','py'):
			try: dirs.remove(d)
			except ValueError: pass
		for f in files:
			if f in ('stdafx.cpp',): continue
			if f.split('.')[-1] in ('cpp','cc','C'):
				ret+=((root,f),)
	return ret

sources=walkSourceFiles()
plugins=set()
for dir,f in sources:
	for l in open(dir+sep+f):
		if 'YADE_PLUGIN' in l and getModule(dir) not in ('core',): plugins.add(f.split('.')[0])

maxIncludeLevel=4
def grepInc(path,f,level=0):
	fullF=path+sep+f; baseName=f.split('.')[0]
	deps=set()
	if level==maxIncludeLevel: return set()
	#print path,f
	for l in open(fullF):
		m=re.match('^#include<yade/([^/]*)/(.*)>.*$',l)
		if m:
			incMod=m.group(1); incHead=m.group(2); baseName=incHead.split('.')[0]; assert(len(incHead.split('.'))==2)
			if incMod=='core': continue
			#if baseName not in plugins: print f,incHead
			deps.add(incHead.split('.')[0])
			continue
		m=re.match('^#include\s*"([^/]*)".*$',l)
		if m:
			inc=m.group(1); incBaseName=m.group(1).split('.')[0]
			if not os.path.exists(path+sep+m.group(1)):
				print "WARNING: file %s included from %s doesn't exist"%(m.group(1),fullF)
				pass
			else:
				if m.group(1).split('.')[0] not in plugins or incBaseName==baseName:
					deps.update(grepInc(path,m.group(1),level=level+1))
	return deps
#for dir,f in [('./pkg/common/Engine/DeusExMachina','JumpChangeSe3.cpp'),]: #sources:
for dir,f in sources:
	if getModule(dir) in ('core',): continue
	d=grepInc(dir,f)
	print f ,' '.join(d)

