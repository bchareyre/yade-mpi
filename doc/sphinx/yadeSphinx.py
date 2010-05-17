#!/usr/bin/python
# encoding: utf-8
#
# module documentation
#
import sys,os,os.path
outDir=sys.argv[2] if len(sys.argv)>2 else '_build'
for d in (outDir,outDir+'/latex',outDir+'/html'):
	if not os.path.exists(d):
		os.mkdir(d)

def moduleDoc(module,submodules=[]):
	f=open('yade.'+module+'.rst','w')
	modFmt=""".. automodule:: yade.%s
	:members:
	:undoc-members:

"""
	f.write(""".. _yade.%s:

yade.%s module
==========================================

"""%(module,module))
	#f.write(".. module:: yade.%s\n\n"%module+modFmt%module)
	f.write(modFmt%module)
	for sub in submodules:
		#f.write(".. submodule:: yade.%s\n\n"%sub+modFmt%sub)
		f.write(".. currentmodule:: yade.%s\n\n%s\n\n"%(module,modFmt%sub))
	f.close()
#
# put all yade modules here, as {module:[submodules]}
#
# don't forget to put the module in index.rst as well!
#
mods={'eudoxos':['_eudoxos'],'log':[],'post2d':[],'pack':['_packSpheres','_packPredicates','_packObb','_packSpherePadder'],'plot':[],'timing':[],'utils':['_utils'],'ymport':[],'qt':['_qt'],'linterpolation':[]}
#
# generate documentation, in alphabetical order
mm=mods.keys(); mm.sort()
for m in mm: moduleDoc(m,mods[m])

with open('modules.rst','w') as f:
	f.write("Yade modules\n=============\n\n.. toctree::\n\t:maxdepth: 2\n\n")
	for m in mm: f.write('\tyade.%s.rst\n\n'%m)



#import sys
#sys.exit(0)

#
# wrapper documentation
#

# classes already documented
docClasses=set()

def autodocClass(klass):
	global docClasses
	docClasses.add(klass)
	return ".. autoclass:: %s\n\t:members:\n\t:undoc-members:\n\n"%(klass) # \t:inherited-members:\n # \n\t:show-inheritance:
def autodocDerived(klass,doSections=True):
	ret=''
	if doSections: ret+='%s\n'%klass+'^'*100+'\n\n'
	#ret+='.. inheritance-diagram:: %s\n\n'%klass
	ret+=inheritanceDiagram(klass)
	ret+=autodocClass(klass)
	childs=list(yade.system.childClasses(klass));
	childs.sort();
	for k in childs:
		ret+=autodocClass(k)
	return ret
def inheritanceDiagram(klass):
	"Generate simple inheritance graph for given classname *klass* using dot (http://www.graphviz.org) syntax."
	def mkUrl(c):
		global writer
		# useless, doesn't work in LaTeX anyway...
		return ('#yade.wrapper.%s'%c if writer=='html' else '%%yade.wrapper#yade.wrapper.%s'%c) # HTML/LaTeX
	def mkNode(c): return '\t\t"%s" [shape="box",fontsize=8,style="setlinewidth(0.5)",height=0.2,URL="yade.wrapper.html#yade.wrapper.%s"];\n'%(c,c)
	ret=".. graphviz::\n\n\tdigraph %s {\n\t\trankdir=RL;\n\t\tmargin=.2;\n"%klass+mkNode(klass)
	childs=yade.system.childClasses(klass)
	if len(childs)==0: return ''
	for c in childs:
		try:
			base=eval(c).__bases__[0].__name__
			ret+=mkNode(c)
			ret+='\t\t"%s" -> "%s" [arrowsize=0.5,style="setlinewidth(0.5)"];\n'%(c,base)
		except NameError:
			pass
			#print 'WARN: unable to find class object for',c
	return ret+'\t}\n\n'


def sect(title,text,tops):
	return title+'\n'+100*'-'+'\n\n'+text+'\n\n'+'\n\n'.join([autodocDerived(top,doSections=(len(tops)>1)) for top in tops])+'\n'


def genWrapperRst():
	wrapper=file('yade.wrapper.rst','w')
	wrapper.write(""".. _yade.wrapper
Class reference (yade.wrapper module)
=======================================

.. toctree::
  :maxdepth: 2


.. currentmodule:: yade.wrapper
.. autosummary::

"""+
	sect('Bodies','',['Body','Shape','State','Material','Bound'])+
	sect('Interactions','',['Interaction','InteractionGeometry','InteractionPhysics'])+
	sect('Global engines','',['GlobalEngine'])+
	sect('Partial engines','',['PartialEngine'])+
	sect('Bounding volume creation','',['BoundFunctor','BoundDispatcher'])+
	sect('Interaction Geometry creation','',['InteractionGeometryFunctor','InteractionGeometryDispatcher'])+
	sect('Interaction Physics creation','',['InteractionPhysicsFunctor','InteractionPhysicsDispatcher'])+
	sect('Constitutive laws','',['LawFunctor','LawDispatcher'])+
	sect('Callbacks','',['BodyCallback','IntrCallback'])+
	sect('Preprocessors','',['FileGenerator'])+
	sect('Rendering','',['OpenGLRenderingEngine','GlShapeFunctor','GlStateFunctor','GlBoundFunctor','GlInteractionGeometryFunctor','GlInteractionPhysicsFunctor'])+
	sect('Simulation data','',['Omega','BodyContainer','InteractionContainer','ForceContainer'])
	+"""
Other classes
---------------

"""
	+''.join([autodocClass(k) for k in (yade.system.childClasses('Serializable')-docClasses)|set(['Serializable','TimingDeltas','Cell'])])
	)

	wrapper.close()

def makeBaseClassesClickable(f,writer):
	out=[]
	import re,shutil
	changed=False
	for l in open(f):
		if writer=='html':
			if not '(</big><em>inherits ' in l:
				out.append(l)
				continue
			m=re.match(r'(^.*\(</big><em>inherits )([a-zA-Z0-9_ →]*)(</em><big>\).*$)',l)
			if not m:
				out.append(l)
				continue
				#raise RuntimeError("Line %s unmatched?"%l)
			bases=m.group(2)
			bb=bases.split(' → ')
			#print bases,'@',bb	
			bbb=' → '.join(['<a class="reference external" href="yade.wrapper.html#yade.wrapper.%s">%s</a>'%(b,b) for b in bb])
			out.append(m.group(1)+bbb+m.group(3))
		elif writer=='latex':
			if not (r'\pysiglinewithargsret{\strong{class }\code{yade.wrapper.}\bfcode{' in l and r'\emph{inherits' in l):
				out.append(l)
				continue
			#print l
			m=re.match(r'(^.*}}{\\emph{inherits )([a-zA-Z0-9_\\() -]*)(}}.*$)',l)
			if not m: raise RuntimeError("Line %s unmatched?"%l)
			bases=m.group(2)
			bb=bases.split(r' \(\rightarrow\) ')
			bbb=r' \(\rightarrow\) '.join([r'\hyperref[yade.wrapper:yade.wrapper.%s]{%s}'%(b.replace(r'\_',r'_'),b) for b in bb])
			out.append(m.group(1)+bbb+m.group(3)+'\n')
		changed=True
	if changed:
		shutil.move(f,f+'~')
		ff=open(f,'w')
		for l in out:
			ff.write(l)
		ff.close()

def genReferences():
	import sys
	sys.path.append('.')
	import bib2rst # our module
	f=open('../references.rst','w')
	f.write('References\n'+40*'='+'\n\n\n')
	f.write(bib2rst.bib2rst('references.bib'))
	f.close()


import sphinx,sys,shutil

genReferences()
shutil.copyfile('../references.bib',outDir+'/latex/references.bib')

global writer
writer=None

for w in ['latex','html']: #['html','latex']:
	writer=w
	genWrapperRst()
	# HACK: must rewrite sys.argv, since reference generator in conf.py determines if we output latex/html by inspecting it
	sys.argv=['sphinx-build','-a','-E','-b','%s'%writer,'-d',outDir+'/doctrees','.',outDir+'/%s'%writer]
	sphinx.main(sys.argv)
	makeBaseClassesClickable((outDir+'/html/yade.wrapper.html' if writer=='html' else outDir+'/latex/Yade.tex'),writer)

sys.exit()
