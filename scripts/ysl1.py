#!/usr/bin/env python
# -*- coding: utf-8 -*-


# 1. Move all files (.*)/([^/]*)/src/\2/(.*)$ -> \1/\3 using svn
# 2. Rename all directories named yade-([a-z]+)-([a-z]+) -> \2 using svn

action=['*/src/*','*/src','yade-*-*','pkg-*/*/*/*'][3]
import re,os
#import pysvn
#client = pysvn.Client()

dd=open('/tmp/d','w')
emptyDirs=[]
for root, dirs, files in os.walk('..'):
	for d in ('.svn','yade-flat','include'):
		try: dirs.remove(d)
		except ValueError: pass
	for d in dirs:	emptyDirs.append(d)
	for f in files+dirs:
		rootf=root+'/'+f
		if action=='*/src/*':
			m=re.match(r'(.*)/([^/]*)/src/\2/(.*)$',rootf)
			if m:
				newFile=m.group(1)+'/'+m.group(2)+'/'+m.group(3)
				#client.move(rootf,newFile)
				os.system('svn move %s %s'%(rootf,newFile))
				dd.write(root+'\n')
		elif action=='*/src':
			m=re.match(r'(.*)/([^/]*)/src/([^/]*)$',rootf)
			if m:
				newFile=m.group(1)+'/'+m.group(2)+'/'+m.group(3)
				#client.move(rootf.newFile)
				os.system('svn move %s %s'%(rootf,newFile))
				#print rootf,newFile
		elif action=='yade-*-*':
			m=re.match(r'(.*)/yade-([a-z]+)-([a-z-]+)$',rootf)
			if m:
				newDir=m.group(1)+'/'+m.group(3)
				if m.group(3)=='dem-simulator': newDir=m.group(1)+'/spherical-dem-simulator'
				os.system('svn move %s %s'%(rootf,newDir))
				#print rootf,newDir
		elif action=='pkg-*/*/*/*':
			# target structure:
			# {DataClass,Engine}/*/*.?pp
			# {RenderingEngine,PreProcessor,Container}/*.?pp
			if os.path.isdir(rootf): continue # skip dirs here
			m=re.match(r'(.*)/pkg/([^/]+)/(Engine|DataClass|RenderingEngine|PreProcessor|Container)/([^/]*)/([^/]*/)?(.+\..pp)$',rootf)
			#deep1=re.match(r'(.*)/pkg/([^/]+)/(PreProcessor|Container)/([^/]*)/([^/]*)/?(.*)$',rootf)
			cmd=None
			if m:
				gg=m.groups()
				## gg[4] will be skipped
				# replace any references to it in SConscript files
				script="%s/pkg/%s/SConscript"%(gg[0],gg[1])
				if gg[2] in ['Engine','DataClass','RenderingEngine']:
					skipped=''
					if gg[4]: skipped+="/%s"%gg[4][:-1] ## gg[4] has trailing /
					if len(skipped)==0: continue
					cmd=["perl -pi~ -e's@(%s/%s)%s([^.])@\\1\\2@' %s"%(gg[2],gg[3],skipped,script)]
					newFile="%s/pkg/%s/%s/%s/%s"%(gg[0],gg[1],gg[2],gg[3],gg[5])
				elif gg[2] in ['PreProcessor','Container']:
					skipped='/%s'%gg[3]
					if gg[4]: skipped+="/%s"%gg[4][:-1]
					cmd=["perl -pi~ -e's@(%s)%s([^.])@\\1\\2@' %s"%(gg[2],skipped,script)]
					newFile="%s/pkg/%s/%s/%s"%(gg[0],gg[1],gg[2],gg[5])
				else: raise RuntimeError
				cmd+=["svn move %s %s"%(rootf,newFile)]
				dd.write(root+'\n')
				#print cmd[0]
				#print cmd[1]
				for cc in cmd: os.system(cc)
				#os.system(cmd)
			#	print cmd

dd.close()



