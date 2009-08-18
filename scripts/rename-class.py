import os,re,sys,os.path

if not os.path.exists('SConstruct'): raise 'Must be run from the top-level source directory'
oldClass,newClass=sys.argv[1],sys.argv[2]
replCnt,moveCnt=0,0

for root, dirs, files in os.walk('.'):
	for name in files:
		if 'StandAlone' in name: print root+'/'+name,oldClass
		if not name.endswith('pp'): continue
		modified=False; new=[]; fullName=root+'/'+name
		for l in open(fullName):
			nl,cnt=re.subn(r'\b'+oldClass+'\b',newClass,l)
			if cnt>0:
				new.append(nl); modified=True; replCnt+=1
			else: new.append(l)
		if modified:
			print root+'/'+name
			f=open(root+'/'+name,'w')
			for l in new: f.write(l)
			f.close()
		newName,cnt=re.subn(r'\b'+oldClass+r'\b',newClass,fullName)
		if cnt>0:
			os.system('bzr mv '+fullName+' '+newName)
			moveCnt+=1

print "Replaced %d occurences, moved %d files"%(replCnt,moveCnt)
import time,pwd,socket
#if replCnt>0:
if True:
	new=[]
	for l in open('gui/py/PythonUI_rc.py'):
		if 'END_RENAMED_CLASSES_LIST' in l: new+="\t'%s':'%s' # %s, %s@%s\n"%(oldClass,newClass,time.asctime(),pwd.getpwuid(os.getuid())[0],socket.gethostname())
		new+=l
	f=open('gui/py/PythonUI_rc.py','w')
	for l in new: f.write(l)
	f.close()



