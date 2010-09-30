"""
Script that shows dispatch matrices when all available functors are loaded.
Later ones will overwrite earlier ones, this is not what you will get in
reality.

Pipe the output to file and open it in browser:

$ yade-trunk dispatcher-torture.py > /tmp/aa.html
$ firefox /tmp/aa.html

"""

import collections
Dispatch=collections.namedtuple('Dispatch',['basename','types'])

dispatches=[
	Dispatch('Law',('IGeom','IPhys')),
	Dispatch('IGeom',('Shape','Shape')),
	Dispatch('IPhys',('Material','Material')),
	Dispatch('Bound',('Shape','Bound')),
	Dispatch('GlBound',('Bound',)),
	Dispatch('GlInteractionGeometry',('IGeom',)),
	Dispatch('GlInteractionPhysics',('IPhys',)),
	Dispatch('GlShape',('Shape',)),
	#Dispatch('GlState',('State',)) # broken for now
]

sys.path.append('.')
import HTML
outStr=''
for D in dispatches:
	functors=yade.system.childClasses(D.basename+'Functor')
	# create dispatcher with all available functors
	dispatcher=eval(D.basename+'Dispatcher([%s])'%(','.join(['%s()'%f for f in functors])))
	if len(D.types)==1:
		allDim0=list(yade.system.childClasses(D.types[0]))
		table=HTML.Table(header_row=allDim0)
		row=[]
		for d0 in allDim0:
			dd0=eval(d0+'()')
			try:
				f=dispatcher.dispFunctor(dd0)
				row.append(f.name if f else '-')
			except RuntimeError as strerror:
				row.append('<b>ambiguous (%s)</b>'%(strerror))
		table.rows.append(row)
	elif len(D.types)==2:
		# lists of types the dispatcher accepts
		allDim0=list(yade.system.childClasses(D.types[0]))
		allDim1=list(yade.system.childClasses(D.types[1]))
		table=HTML.Table(header_row=['']+allDim1)
		for d0 in allDim0:
			row=['<b>'+d0+'</b>']
			for d1 in allDim1:
				dd0,dd1=eval(d0+'()'),eval(d1+'()')
				try:
					f=dispatcher.dispFunctor(dd0,dd1)
					row.append(f.name if f else '-')
				except RuntimeError: # ambiguous
					row.append('<b>ambiguous</b>')
			table.rows.append(row)
	else: raise ValueError("Dispatcher must be 1D or 2D, not %dD"%len(D.types))
	outStr+='\n<h1>%sDispatcher</h1>'%D.basename
	outStr+=str(table)
print outStr
quit()
