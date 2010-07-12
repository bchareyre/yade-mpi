"""
Script that shows dispatch matrices when all available functors are loaded.
Later ones will overwrite earlier ones, this is not what you will get in
reality.

Pipe the output to file and open it in browser:

$ yade-trunk dispatcher-torture.py > /tmp/aa.html
$ firefox /tmp/aa.html

"""

dispatches={'Law':('InteractionGeometry','InteractionPhysics'),
'InteractionGeometry':('Shape','Shape'),
'InteractionPhysics':('Material','Material'),
'Bound':('Shape','Bound')
}

sys.path.append('.')
import HTML
outStr=''
for D in dispatches.keys():
	functors=yade.system.childClasses(D+'Functor')
	# create dispatcher with all available functors
	dispatcher=eval(D+'Dispatcher([%s])'%(','.join(['%s()'%f for f in functors])))
	if len(dispatches[D])!=2: raise NotImplementedError("Only 2d dispatchers implemented now.")
	# lists of types the dispatcher accepts
	allDim0=list(yade.system.childClasses(dispatches[D][0]))
	allDim1=list(yade.system.childClasses(dispatches[D][1]))
	table=HTML.Table(header_row=['']+allDim1)
	for d0 in allDim0:
		row=[d0]
		for d1 in allDim1:
			dd0,dd1=eval(d0+'()'),eval(d1+'()')
			try:
				f=dispatcher.dispFunctor(dd0,dd1)
				row.append(f.name if f else '-')
			except RuntimeError: # ambiguous
				row.append('<b>ambiguous<b>')
		table.rows.append(row)
	outStr+='\n<h1>%sDispatcher</h1>'%D
	outStr+=str(table)
print outStr
quit()
