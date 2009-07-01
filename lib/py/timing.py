# encoding: utf-8
def _resetEngine(e):
	if e.timingDeltas: e.timingDeltas.reset()
	if e.__class__.__name__=='EngineUnit': return
	if e.__class__.__name__=='MetaEngine':
		for f in e.functors: _resetEngine(f)
	elif e.__class__.__name__=='ParallelEngine':
		for s in e.slaves: _resetEngine(s)
	e.execTime,e.execCount=0,0

def reset():
	for e in O.engines: _resetEngine(e)

_statCols={'label':40,'count':20,'time':20,'relTime':20}
_maxLev=3

def _formatLine(label,time,count,totalTime,level):
	sp,negSp=' '*level*2,' '*(_maxLev-level)*2
	raw=[]
	raw.append(label)
	raw.append(str(count) if count>=0 else '')
	raw.append((str(time/1000)+u'us') if time>=0 else '')
	raw.append(('%6.2f%%'%(time*100./totalTime)) if totalTime>0 else '')
	return u' '.join([
		(sp+raw[0]).ljust(_statCols['label']),
		(raw[1]+negSp).rjust(_statCols['count']),
		(raw[2]+negSp).rjust(_statCols['time']),
		(raw[3]+negSp).rjust(_statCols['relTime']),
	])

def _delta_stats(deltas,totalTime,level):
	ret=0
	deltaTime=sum([d[1] for d in deltas.data])
	for d in deltas.data:
		print _formatLine(d[0],d[1],d[2],totalTime,level); ret+=1
	if len(deltas.data)>1:
		print _formatLine('TOTAL',deltaTime,-1,totalTime,level); ret+=1
	return ret

def _engines_stats(engines,totalTime,level):
	lines=0; hereLines=0
	for e in engines:
		if e.__class__.__name__!='EngineUnit': print _formatLine(u'"'+e['label']+'"' if e['label'] else e.name,e.execTime,e.execCount,totalTime,level); lines+=1; hereLines+=1
		if e.timingDeltas: 
			if e.__class__.__name__=='EngineUnit':
				print _formatLine(e.name,-1,-1,-1,level); lines+=1; hereLines+=1
				execTime=sum([d[1] for d in e.timingDeltas.data])
			else: execTime=e.execTime
			lines+=_delta_stats(e.timingDeltas,execTime,level+1)
		if e.__class__.__name__=='MetaEngine': lines+=_engines_stats(e.functors,e.execTime,level+1)
		if e.__class__.__name__=='InteractionDispatcher':
			lines+=_engines_stats(e.geomDispatcher.functors,e.execTime,level+1)
			lines+=_engines_stats(e.physDispatcher.functors,e.execTime,level+1)
			lines+=_engines_stats(e.constLawDispatcher.functors,e.execTime,level+1)
		elif e.__class__.__name__=='ParallelEngine': lines+=_engines_stats(e.slave,e.execTime,level+1)
	if hereLines>1:
		print _formatLine('TOTAL',totalTime,-1,totalTime,level); lines+=1
	return lines

def stats():
	print 'Name'.ljust(_statCols['label'])+' '+'Count'.rjust(_statCols['count'])+' '+'Time'.rjust(_statCols['time'])+' '+'Rel. time'.rjust(_statCols['relTime'])
	print '-'*(sum([_statCols[k] for k in _statCols])+len(_statCols)-1)
	_engines_stats(O.engines,sum([e.execTime for e in O.engines]),0)
	print
