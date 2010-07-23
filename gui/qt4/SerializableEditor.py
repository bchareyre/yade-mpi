from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
#from PyQt4 import Qwt5

import re
import logging
logging.basicConfig(level=logging.INFO)
#from logging import debug,info,warning,error
from yade import *


class AttrEditor():
	"""Abstract base class handing some aspects common to all attribute editors.
	Holds exacly one attribute which is updated whenever it changes."""
	def __init__(self,ser,attr):
		self.ser,self.attr=ser,attr
		self.hot=False
		self.widget=None
	def refresh(self): pass
	def update(self): pass
	def isHot(self,hot=True):
		"Called when the widget gets focus; mark it hot, change colors etc."
		if hot==self.hot: return
		# changing the color does not work...
		self.hot=hot
		p=QPalette();
		if hot: p.setColor(QPalette.WindowText,Qt.red);
		self.setPalette(p)
		self.repaint()
		#print self.attr,('hot' if hot else 'cold')
	def sizeHint(self): return QSize(150,12)

class AttrEditor_Bool(AttrEditor,QCheckBox):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QCheckBox.__init__(self,parent)
		self.clicked.connect(self.update)
	def refresh(self):
		self.setChecked(getattr(self.ser,self.attr))
	def update(self): setattr(self.ser,self.attr,self.isChecked())

class AttrEditor_Int(AttrEditor,QSpinBox):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QSpinBox.__init__(self,parent)
		self.setRange(int(-1e10),int(1e10)); self.setSingleStep(1);
		self.valueChanged.connect(self.update)
	def refresh(self): self.setValue(getattr(self.ser,self.attr))
	def update(self): setattr(self.ser,self.attr,self.value()); self.isHot(False)

class AttrEditor_Str(AttrEditor,QLineEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QLineEdit.__init__(self,parent)
		self.textEdited.connect(self.isHot)
		self.editingFinished.connect(self.update)
	def refresh(self): self.setText(getattr(self.ser,self.attr))
	def update(self): setattr(self.ser,self.attr,str(self.text())); self.isHot(False)

class AttrEditor_Float(AttrEditor,QLineEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QLineEdit.__init__(self,parent)
		self.textEdited.connect(self.isHot)
		self.editingFinished.connect(self.update)
	def refresh(self): self.setText(str(getattr(self.ser,self.attr)))
	def update(self):
		try: setattr(self.ser,self.attr,float(self.text()))
		except ValueError: self.refresh()
		self.isHot(False)

class AttrEditor_Vector3(AttrEditor,QFrame):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QFrame.__init__(self,parent)
		self.setContentsMargins(0,0,0,0)
		self.box=QHBoxLayout(self); self.box.setSpacing(0); self.box.setMargin(0)
		self.widgets=[]
		for i in range(3):
			w=QLineEdit('');
			self.box.addWidget(w); self.widgets.append(w)
			w.textEdited.connect(self.isHot)
			w.editingFinished.connect(self.update)
	def refresh(self):
		val=getattr(self.ser,self.attr)
		for i in range(3): self.widgets[i].setText(str(val[i]))
	def update(self):
		try:
			val=getattr(self.ser,self.attr)
			for i in range(3):
				if self.widgets[i].isModified(): val[i]=float(self.widgets[i].text())
			print 'setting',val
			setattr(self.ser,self.attr,val)
		except ValueError: self.refresh()
		self.isHot(False)
class AttrEditor_ListStr(AttrEditor,QPlainTextEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QPlainTextEdit.__init__(self,parent)
		self.setLineWrapMode(QPlainTextEdit.NoWrap)
		self.setFixedHeight(80)
		self.textChanged.connect(self.update)
	def refresh(self):
		lst=getattr(self.ser,self.attr)
		self.setPlainText('\n'.join(lst))
	def update(self):
		if self.hasFocus(): self.isHot()
		t=self.toPlainText()
		setattr(self.ser,self.attr,str(t).strip().split('\n'))
		if not self.hasFocus(): self.isHot(False)


class SerializableEditor(QWidget):
	"Class displaying and modifying serializable attributes of a yade object."
	import collections
	import logging
	# each attribute has one entry associated with itself
	class EntryData:
		def __init__(self,name,T):
			self.name,self.T=name,T
			self.lineNo,self.widget=None,None
	def __init__(self,ser,parent=None,ignoredAttrs=set()):
		"Construct window, *ser* is the object we want to show."
		QtGui.QWidget.__init__(self,parent)
		self.ser=ser
		self.entries=[]
		self.ignoredAttrs=ignoredAttrs
		logging.debug('New Serializable of type %s'%ser.__class__.__name__)
		self.setWindowTitle(str(ser))
		self.mkWidgets()
		self.refreshTimer=QTimer()
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(500)
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
		logging.debug('Got type "%s" from :yattrtype:'%cxxT)
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
				logging.debug("Got type %s from cxx type %s"%(ret.__name__,cxxT))
				return (ret,)
		error("Unable to guess python type from cxx type '%s'"%cxxT)
		return None
	def mkAttrEntries(self):
		d=self.ser.dict()
		for attr,val in self.ser.dict().items():
			if attr in self.ignoredAttrs:
				continue
			if isinstance(val,list):
				if len(val)==0: t=self.getListTypeFromDocstring(attr)
				else: t=(val[0].__class__,) # 1-tuple is list of the contained type
			else: t=val.__class__
			#logging.debug('Attr %s is of type %s'%(attr,((t[0].__name__,) if isinstance(t,tuple) else t.__name__)))
			self.entries.append(self.EntryData(name=attr,T=t))
	def mkWidget(self,entry):
		typeMap={bool:AttrEditor_Bool,str:AttrEditor_Str,int:AttrEditor_Int,float:AttrEditor_Float,Vector3:AttrEditor_Vector3,(str,):AttrEditor_ListStr}
		Klass=typeMap.get(entry.T,None)
		if not Klass: return None
		widget=Klass(self,self.ser,entry.name)
		widget.setFocusPolicy(Qt.StrongFocus)
		return widget
	def mkWidgets(self):
		self.mkAttrEntries()
		grid=QtGui.QGridLayout()
		for lineNo,entry in enumerate(self.entries):
			grid.addWidget(QtGui.QLabel(entry.name),lineNo,0)
			entry.widget=self.mkWidget(entry)
			if entry.widget: grid.addWidget(entry.widget,lineNo,1)
			else: grid.addWidget(QtGui.QLabel('<i>unhandled type</i>'),lineNo,1)
		self.setLayout(grid)
		self.refreshEvent()
	def refreshEvent(self):
		for e in self.entries:
			if e.widget and not e.widget.hot: e.widget.refresh()


if __name__=='__main__':
	vr=VTKRecorder()
	s1=SerializableEditor(vr)
	s1.show()
	s2=SerializableEditor(OpenGLRenderingEngine())
	s2.show()

