import time
# change this line to load your reference simulation
O.load2('ref.boost.bin.gz')
base='~sim~'

# http://blogmag.net/blog/read/38/Print_human_readable_file_size
def sizeof_fmt(num):
	for x in ['bytes','KB','MB','GB','TB']:
		if num<1024.0: return "%3.1f%s"%(num,x)
		num/=1024.0


def io(ext,load,noBoost=False):
	t0=time.time()
	f=base+('.yade.' if noBoost else '.boost.')+ext
	((O.load if noBoost else O.load2) if load else (O.save if noBoost else O.save2))(f)
	print ('Loaded' if load else 'Saved '),('yade ' if noBoost else 'boost'),'%-7s '%ext,'%7s'%sizeof_fmt(os.path.getsize(f)),'%.1fs'%(time.time()-t0)
for ext in ['xml','xml.bz2']:
	io(ext,False,True)
	io(ext,True,True)
for ext in ['xml','xml.gz','xml.bz2','bin','bin.gz','bin.bz2']:
	io(ext,False)
	io(ext,True)
