import os,re
for root, dirs, files in os.walk('.'):
	for name in files:
		if not name.endswith('.cpp'): continue
		#if name!='BssSnowGrain.cpp': continue
		modified=False
		new=[]
		for l in open(root+'/'+name):
			m=re.match('(.*)YADE_PLUGIN\(\)(.*)',l)
			if m:
				modified=True
				plugin=name[:-4] # strip the .cpp
				new.append(m.group(1)+'YADE_PLUGIN("'+plugin+'")'+m.group(2))
			else: new.append(l)
		if modified:
			print root+'/'+name
			f=open(root+'/'+name,'w')
			for l in new: f.write(l)
			f.close()



