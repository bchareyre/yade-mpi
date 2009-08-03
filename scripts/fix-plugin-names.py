import os,re
for root, dirs, files in os.walk('.'):
	for name in files:
		if not name.endswith('.cpp'): continue
		#if name!='BssSnowGrain.cpp': continue
		modified=False
		new=[]
		for l in open(root+'/'+name):
			m=re.match('(.*)YADE_PLUGIN\(([^)]*)\)(.*)',l)
			if m:
				modified=True
				plugins=''.join(['('+pl.strip()[1:-1]+')' for pl in m.group(2).split(',')])
				new.append(m.group(1)+'YADE_PLUGIN('+plugins+')'+m.group(3))
			else: new.append(l)
		if modified:
			print root+'/'+name
			f=open(root+'/'+name,'w')
			for l in new: f.write(l)
			f.close()



