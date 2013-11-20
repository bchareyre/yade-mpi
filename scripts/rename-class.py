#!/usr/bin/python
import os,re,sys,os.path

if not os.path.exists('SConstruct'): raise 'Must be run from the top-level source directory'
oldClass,newClass=sys.argv[1],sys.argv[2]
replCnt,moveCnt,moveDirCnt=0,0,0

for root, dirs, files in os.walk('.'):
	for name in files:
		if os.path.isdir(root+'/'+name): continue
		if not name.endswith('pp') and name!='SConscript': continue
		modified=False; new=[]; fullName=root+'/'+name
		if fullName.startswith('./lib/') or fullName.startswith('./extra'): continue
		for l in open(fullName):
			nl,cnt=re.subn(r'\b'+oldClass+r'\b',newClass,l)
			if cnt>0:
				new.append(nl); modified=True; replCnt+=1
			else: new.append(l)
		# write back the file if it was modified
		if modified:
			print 'Updated: ',root+'/'+name
			f=open(root+'/'+name,'w')
			for l in new: f.write(l)
			f.close()
		# try to replace the class within the filename, move using bzr if it has changed
		newName,cnt=re.subn(r'\b'+oldClass+r'\b',newClass,name)
		if cnt>0:
			newFullName=root+'/'+newName
			os.system('bzr mv '+fullName+' '+newFullName)
			moveCnt+=1

# walk directories and change them with bzr if they match exactly (rarely useful)
for root, dirs, files in os.walk('.'):
	for d in dirs:
		if d==oldClass:
			os.system('bzr mv %s/%s %s/%s'%(root,oldClass,root,newClass))
			moveDirCnt+=1

print "Replaced %d occurences, moved %d files and %d directories"%(replCnt,moveCnt,moveDirCnt)
print "Update python scripts (if wanted) by running: perl -pi -e 's/\\b%s\\b/%s/g' `ls **/*.py **/*.rst |grep -v py/system.py`"%(oldClass,newClass)
import time,pwd,socket
# update python deprecation records
if replCnt+moveCnt+moveDirCnt==0:
	print "No replaces, not updating py/system.py deprecated names map."
if True:
	new=[]
	for l in open('py/system.py'):
		if 'END_RENAMED_CLASSES_LIST' in l: new+="\t'%s':'%s', # %s, %s@%s\n"%(oldClass,newClass,time.asctime(),pwd.getpwuid(os.getuid())[0],socket.gethostname())
		new+=l
	f=open('py/system.py','w')
	for l in new: f.write(l)
	f.close()



