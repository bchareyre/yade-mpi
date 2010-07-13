from PyQt4.QtCore import *
from PyQt4.QtGui import *
#from PyQt4 import Qwt5

import re
import logging
logging.basicConfig(level=logging.DEBUG)
from logging import debug,info,warning,error

class SerializableData(QWidget):
	"Class displaying and modifying serializable attributes of a yade object."
	import collections
	# each attribute has one entry associated with itself
	class EntryData:
		def __init__(self,name,T):
			self.name,self.T=name,T
			self.lineNo,self.widget=None,None
	def __init__(self,ser,parent=None):
		"Construct window, *ser* is the object we want to show."
		QWidget.__init__(self,parent)
		self.ser=ser
		self.entries=[]
		print self.ser
		debug('New Serializable of type %s'%ser.__class__.__name__)
		self.setWindowTitle(str(ser))
		self.mkWidgets()
	def getListTypeFromDocstring(self,attr):
		"Guess type of array by scanning docstring for :yattrtype: and parsing its argument; ugly, but works."
		doc=getattr(self.ser.__class__,attr).__doc__
		if doc==None:
			error("Attribute %s has no docstring."%attr)
			return None
		m=re.search(r':yattrtype:`([^`]*)`',doc)
		if not m:
			error("Attribute %s does not contain :yattrtype:`....` (docstring is '%s'"%(attr,doc))
			return None
		cxxT=m.group(1)
		#debug('Got type "%s" from :yattrtype:'%cxxT)
		def vecTest(T,cxxT):
			regexp=r'^\s*(std\s*::)?\s*vector\s*<\s*(std\s*::)?\s*('+T+r')\s*>\s*$'
			m=re.match(regexp,cxxT)
			return m
		vecMap={
			'int':int,'long':int,'body_id_t':long,'size_t':long,
			'Real':float,'float':float,'double':float,
			'Vector3r':Vector3,'Matrix3r':Matrix3,
			'string':str
		}
		for T,ret in vecMap.items():
			if vecTest(T,cxxT):
				debug("Got type %s from cxx type %s"%(ret.__name__,cxxT))
				return (ret,)
		error("Unable to guess python type from cxx type '%s'"%cxxT)
		return None
	def mkAttrEntries(self):
		d=self.ser.dict()
		for attr,val in self.ser.dict().items():
			if isinstance(val,list):
				if len(val)==0: t=self.getListTypeFromDocstring(attr)
				else: t=(val[0].__class__,) # 1-tuple is list of the contained type
			else: t=val.__class__
			debug('Attr %s is of type %s'%(attr,((t[0].__name__,) if isinstance(t,tuple) else t.__name__)))
			self.entries.append(self.EntryData(name=attr,T=t))
	def mkWidget(self,entry):
		self.chgPalette=QPalette()
		self.chgPalette.setColor(QPalette.Window,QColor('red'))
		widget=None
		if entry.T==bool:
			widget=QCheckBox('',self)
			widget.setChecked(getattr(self.ser,entry.name))
			self.connect(widget,SIGNAL('stateChanged(int)'),lambda num: setattr(self.ser,entry.name,(True if num>0 else False)))
		elif entry.T==str:
			widget=QLineEdit('',self)
			widget.setText(getattr(self.ser,entry.name))
			self.connect(widget,SIGNAL('editingFinished()'),lambda: setattr(self.ser,entry.name,str(widget.text())))
		elif entry.T==int:
			widget=QSpinBox(self)
			widget.setRange(int(-1e10),int(1e10)); widget.setSingleStep(1);
			widget.setValue(getattr(self.ser,entry.name))
			self.connect(widget,SIGNAL('valueChanged(int)'),lambda val: setattr(self.ser,entry.name,val))
		elif entry.T==float:
			widget=QLineEdit('',self)
			widget.setText(str(getattr(self.ser,entry.name)))
			self.connect(widget,SIGNAL('editingFinished()'),lambda val: setattr(self.ser,entry.name,float(str(widget.text()))))
		elif entry.T==Vector3:
			widget=QFrame(self); widget.setContentsMargins(0,0,0,0)
			box=QHBoxLayout(widget); box.setSpacing(0)
			vec=getattr(self.ser,entry.name)
			for i in range(3):
				subw=QLineEdit('')
				subw.setText(str(vec[i]))
				subw.setSizePolicy(QSizePolicy().setHorizontalPolicy(
				class updateVec:
					"bind local args... ugly"
					def __init__(self,i,ser,name,subw): self.i,self.ser,self.name,self.subw=i,ser,name,subw
					def __call__(self): 
						newVal=float(str(self.subw.text()))
						debug('updating %d with %s'%(self.i,newVal))
						v=getattr(self.ser,self.name); v[self.i]=newVal; setattr(self.ser,self.name,v)
				self.connect(subw,SIGNAL('editingFinished()'), updateVec(i,self.ser,entry.name,subw))
				box.addWidget(subw)
		return widget
	def mkWidgets(self):
		self.mkAttrEntries()
		grid=QtGui.QGridLayout()
		for lineNo,entry in enumerate(self.entries):
			grid.addWidget(QLabel(entry.name),lineNo,0)
			entry.widget=self.mkWidget(entry)
			if entry.widget: grid.addWidget(entry.widget,lineNo,1)
			else: grid.addWidget(QLabel('<i>unhandled type</i>'),lineNo,1)
		self.setLayout(grid)




s1=SerializableData(VTKRecorder())
s2=SerializableData(OpenGLRenderingEngine())
s1.show()
s2.show()
