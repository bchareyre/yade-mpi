# encoding: utf-8
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
#from PyQt4 import Qwt5

import re,itertools
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
	def setAttribute(self,ser,attr,val):
		try: setattr(ser,attr,val)
		except AttributeError: self.setEnabled(False) # read-only attribute

class AttrEditor_Bool(AttrEditor,QCheckBox):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QCheckBox.__init__(self,parent)
		self.clicked.connect(self.update)
	def refresh(self):
		self.setChecked(getattr(self.ser,self.attr))
	def update(self): self.setAttribute(self.ser,self.attr,self.isChecked())

class AttrEditor_Int(AttrEditor,QSpinBox):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QSpinBox.__init__(self,parent)
		self.setRange(int(-1e10),int(1e10)); self.setSingleStep(1);
		self.valueChanged.connect(self.update)
	def refresh(self):
		val=getattr(self.ser,self.attr)
		#print 'INT',self.ser,self.attr,val
		self.setValue(val)
	def update(self):
		sim,ui=getattr(self.ser,self.attr),self.value()
		if sim!=ui: self.setAttribute(self.ser,self.attr,ui)

class AttrEditor_Str(AttrEditor,QLineEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QLineEdit.__init__(self,parent)
		self.textEdited.connect(self.isHot)
		self.editingFinished.connect(self.update)
	def refresh(self): self.setText(getattr(self.ser,self.attr))
	def update(self): self.setAttribute(self.ser,self.attr,str(self.text())); self.isHot(False)

class AttrEditor_Float(AttrEditor,QLineEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QLineEdit.__init__(self,parent)
		self.textEdited.connect(self.isHot)
		self.editingFinished.connect(self.update)
	def refresh(self): self.setText(str(getattr(self.ser,self.attr)))
	def update(self):
		try: self.setAttribute(self.ser,self.attr,float(self.text()))
		except ValueError: self.refresh()
		self.isHot(False)

class AttrEditor_MatrixX(AttrEditor,QFrame):
	def __init__(self,parent,ser,attr,rows,cols,idxConverter):
		'idxConverter converts row,col tuple to either (row,col), (col) etc depending on what access is used for []'
		AttrEditor.__init__(self,ser,attr)
		QFrame.__init__(self,parent)
		self.rows,self.cols=rows,cols
		self.idxConverter=idxConverter
		self.setContentsMargins(0,0,0,0)
		self.grid=QGridLayout(self); self.grid.setSpacing(0); self.grid.setMargin(0)
		for row,col in itertools.product(range(self.rows),range(self.cols)):
			w=QLineEdit('');
			self.grid.addWidget(w,row,col);
			w.textEdited.connect(self.isHot)
			w.editingFinished.connect(self.update)
	def refresh(self):
		val=getattr(self.ser,self.attr)
		for row,col in itertools.product(range(self.rows),range(self.cols)):
			self.grid.itemAtPosition(row,col).widget().setText(str(val[self.idxConverter(row,col)]))
	def update(self):
		try:
			val=getattr(self.ser,self.attr)
			for row,col in itertools.product(range(self.rows),range(self.cols)):
				w=self.grid.itemAtPosition(row,col).widget()
				if w.isModified(): val[self.idxConverter(row,col)]=float(w.text())
			logging.debug('setting'+str(val))
			self.setAttribute(self.ser,self.attr,val)
		except ValueError: self.refresh()
		self.isHot(False)

class AttrEditor_Vector3(AttrEditor_MatrixX):
	def __init__(self,parent,ser,attr):
		AttrEditor_MatrixX.__init__(self,parent,ser,attr,1,3,lambda r,c:c)
class AttrEditor_Matrix3(AttrEditor_MatrixX):
	def __init__(self,parent,ser,attr):
		AttrEditor_MatrixX.__init__(self,parent,ser,attr,3,3,lambda r,c:(r,c))
class AttrEditor_Quaternion(AttrEditor_MatrixX):
	def __init__(self,parent,ser,attr):
		AttrEditor_MatrixX.__init__(self,parent,ser,attr,1,4,lambda r,c:c)
class AttrEditor_Se3(AttrEditor_MatrixX):
	def __init__(self,parent,ser,attr):
		AttrEditor_MatrixX.__init__(self,parent,ser,attr,1,4,lambda r,c:c)

class AttrEditor_ListStr(AttrEditor,QPlainTextEdit):
	def __init__(self,parent,ser,attr):
		AttrEditor.__init__(self,ser,attr)
		QPlainTextEdit.__init__(self,parent)
		self.setLineWrapMode(QPlainTextEdit.NoWrap)
		self.setFixedHeight(60)
		self.textChanged.connect(self.update)
	def refresh(self):
		lst=getattr(self.ser,self.attr)
		self.setPlainText('\n'.join(lst))
	def update(self):
		if self.hasFocus(): self.isHot()
		t=self.toPlainText()
		self.setAttribute(self.ser,self.attr,str(t).strip().split('\n'))
		if not self.hasFocus(): self.isHot(False)

class SerializableEditor(QFrame):
	"Class displaying and modifying serializable attributes of a yade object."
	import collections
	import logging
	# each attribute has one entry associated with itself
	class EntryData:
		def __init__(self,name,T):
			self.name,self.T=name,T
			self.lineNo,self.widget=None,None
	def __init__(self,ser,parent=None,ignoredAttrs=set(),showType=False):
		"Construct window, *ser* is the object we want to show."
		QtGui.QFrame.__init__(self,parent)
		self.ser=ser
		self.showType=showType
		self.hot=False
		self.entries=[]
		self.ignoredAttrs=ignoredAttrs
		logging.debug('New Serializable of type %s'%ser.__class__.__name__)
		self.setWindowTitle(str(ser))
		self.mkWidgets()
		self.refreshTimer=QTimer(self)
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(500)
	def getListTypeFromDocstring(self,attr):
		"Guess type of array by scanning docstring for :yattrtype: and parsing its argument; ugly, but works."
		doc=getattr(self.ser.__class__,attr).__doc__
		if doc==None:
			logging.error("Attribute %s has no docstring."%attr)
			return None
		m=re.search(r':yattrtype:`([^`]*)`',doc)
		if not m:
			logging.error("Attribute %s does not contain :yattrtype:`....` (docstring is '%s'"%(attr,doc))
			return None
		cxxT=m.group(1)
		logging.debug('Got type "%s" from :yattrtype:'%cxxT)
		def vecTest(T,cxxT):
			#regexp=r'^\s*(std\s*::)?\s*vector\s*<\s*(std\s*::)?\s*('+T+r')\s*>\s*$'
			regexp=r'^\s*(std\s*::)?\s*vector\s*<\s*(shared_ptr\s*<\s*)?\s*(std\s*::)?\s*('+T+r')(\s*>)?\s*>\s*$'
			m=re.match(regexp,cxxT)
			return m
		vecMap={
			'int':int,'long':int,'body_id_t':long,'size_t':long,
			'Real':float,'float':float,'double':float,
			'Vector3r':Vector3,'Matrix3r':Matrix3,
			'string':str,
			'BodyCallback':BodyCallback,'IntrCallback':IntrCallback,'BoundFunctor':BoundFunctor,'InteractionGeometryFunctor':InteractionGeometryFunctor,'InteractionPhysicsFunctor':InteractionPhysicsFunctor,'LawFunctor':LawFunctor
		}
		for T,ret in vecMap.items():
			if vecTest(T,cxxT):
				logging.debug("Got type %s from cxx type %s"%(ret.__name__,cxxT))
				return (ret,)
		logging.error("Unable to guess python type from cxx type '%s'"%cxxT)
		return None
	def mkAttrEntries(self):
		try:
			d=self.ser.dict()
		except TypeError:
			logging.error('TypeError when getting attributes of '+str(self.ser)+',skipping. ')
			import traceback
			traceback.print_exc()
		for attr in self.ser.dict():
			val=getattr(self.ser,attr) # get the value using serattr, as it might be different from what the dictionary provides (e.g. Body.blockedDOFs)
			t=None
			if attr in self.ignoredAttrs or attr=='blockedDOFs': # HACK here
				continue
			if isinstance(val,list):
				t=self.getListTypeFromDocstring(attr)
				if not t and len(val)==0: t=(val[0].__class__,) # 1-tuple is list of the contained type
				#if not t: raise RuntimeError('Unable to guess type of '+str(self.ser)+'.'+attr)
			else: t=val.__class__
			#logging.debug('Attr %s is of type %s'%(attr,((t[0].__name__,) if isinstance(t,tuple) else t.__name__)))
			self.entries.append(self.EntryData(name=attr,T=t))
	def mkWidget(self,entry):
		if not entry.T: return None
		typeMap={bool:AttrEditor_Bool,str:AttrEditor_Str,int:AttrEditor_Int,float:AttrEditor_Float,Quaternion:AttrEditor_Quaternion,Vector3:AttrEditor_Vector3,Matrix3:AttrEditor_Matrix3,(str,):AttrEditor_ListStr}
		Klass=typeMap.get(entry.T,None)
		if Klass:
			widget=Klass(self,self.ser,entry.name)
			widget.setFocusPolicy(Qt.StrongFocus)
			return widget
		if entry.T.__class__==tuple:
			if (issubclass(entry.T[0],Serializable) or entry.T[0]==Serializable):
				widget=SeqSerializable(self,lambda: getattr(self.ser,entry.name),lambda x: setattr(self.ser,entry.name,x),entry.T[0])
				return widget
			return None
		if issubclass(entry.T,Serializable) or entry.T==Serializable:
			widget=SerializableEditor(getattr(self.ser,entry.name),parent=self,showType=self.showType)
			widget.setFrameShape(QFrame.Box); widget.setFrameShadow(QFrame.Raised); widget.setLineWidth(1)
			return widget
		return None
	def mkWidgets(self):
		self.mkAttrEntries()
		grid=QFormLayout()
		grid.setContentsMargins(2,2,2,2)
		grid.setVerticalSpacing(0)
		grid.setLabelAlignment(Qt.AlignRight)
		if self.showType:
			lab=QLabel(u'<b>→  '+self.ser.__class__.__name__+u'  ←</b>')
			lab.setFrameShape(QFrame.Box); lab.setFrameShadow(QFrame.Sunken); lab.setLineWidth(2); lab.setAlignment(Qt.AlignHCenter)
			grid.setWidget(0,QFormLayout.SpanningRole,lab)
		for entry in self.entries:
			entry.widget=self.mkWidget(entry)
			grid.addRow(entry.name,entry.widget if entry.widget else QLabel('<i>unhandled type</i>'))
		self.setLayout(grid)
		self.refreshEvent()
	def refreshEvent(self):
		for e in self.entries:
			if e.widget and not e.widget.hot: e.widget.refresh()
	def refresh(self): pass

#import sys; sys.path.append('.')
from yade.qt.ui_SeqSerializable import Ui_SeqSerializable

class SeqSerializable(QFrame,Ui_SeqSerializable):
	def __init__(self,parent,getter,setter,serType):
		'getter and setter are callables that update/refresh the underlying object.'
		QFrame.__init__(self,parent)
		self.setupUi(self)
		self.getter,self.setter=getter,setter
		self.tb=self.objectToolBox
		self.tb.sizeHint=lambda: QSize(120,15)
		self.hot=False
		self.serType=serType
		self.clearToolBox()
		self.fillToolBox()
		self.changedSlot()
		self.refreshTimer=QTimer(self)
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(1000) # 1s should be enough
	def clearToolBox(self):
		for i in range(self.tb.count()-1,-1,-1): self.tb.removeItem(i)
	def mkItemLabel(self,ser,pos=-1):
		ret=ser.label if (hasattr(self,'label') and len(ser.label)>0) else str(ser).replace('instance at','')
		return ('' if pos<0 else str(pos)+'. ')+ret
	def relabelItems(self):
		for i in range(self.tb.count()): self.tb.setItemText(i,self.mkItemLabel(self.tb.widget(i).ser,i))
	def fillToolBox(self):
		for i,ser in enumerate(self.getter()):
			self.tb.addItem(SerializableEditor(ser,parent=None),self.mkItemLabel(ser,i))
	def moveUpSlot(self):
		ix=self.tb.currentIndex();
		if ix==0: return # already all way up
		w,t=self.tb.widget(ix),self.tb.itemText(ix);
		self.tb.removeItem(ix); self.tb.insertItem(ix-1,w,t); self.tb.setCurrentIndex(ix-1)
		self.update(); self.relabelItems()
	def moveDownSlot(self,*args):
		ix=self.tb.currentIndex();
		if ix==self.tb.count()-1: return # already down
		w,t=self.tb.widget(ix),self.tb.itemText(ix)
		self.tb.removeItem(ix); self.tb.insertItem(ix+1,w,t); self.tb.setCurrentIndex(ix+1)
		self.update(); self.relabelItems()
	def killSlot(self):
		w=self.tb.currentWidget();
		self.tb.removeItem(self.tb.currentIndex())
		w.close()
		self.update(); self.relabelItems()
	def changedSlot(self):
		cnt=self.tb.count()
		ix=self.tb.currentIndex()
		self.moveUpButton.setEnabled(ix>0 and cnt>0)
		self.moveDownButton.setEnabled(ix<cnt-1 and cnt>0)
		self.killButton.setEnabled(cnt>0)
	def newSlot(self,*args):
		base=self.serType.__name__;
		#klass,ok=QInputDialog.getItem(self,'Insert new item','Classes inheriting from '+base,childs,editable=False)
		#if not ok or len(klass)==0: return
		dialog=NewSerializableDialog(self,base,includeBase=True)
		if not dialog.exec_(): return # cancelled
		ser=dialog.result()
		ix=self.tb.currentIndex(); 
		self.tb.insertItem(ix,SerializableEditor(ser,parent=None),self.mkItemLabel(ser,ix))
		self.tb.setCurrentIndex(ix)
		self.update(); self.relabelItems()
	def update(self):
		self.setter([self.tb.widget(i).ser for i in range(0,self.tb.count())])
	def refreshEvent(self):
		curr=self.getter()
		# == and != compares addresses on Serializables
		if len(curr)==self.tb.count() and sum([curr[i]!=self.tb.widget(i).ser for i in range(self.tb.count())])==0: return # same length and contents
		# something changed in the sequence order, so we have to rebuild from scratch; keep index at least
		logging.debug('Rebuilding list from scratch')
		ix=self.tb.currentIndex()
		self.clearToolBox();	self.fillToolBox()
		self.tb.setCurrentIndex(ix)
	def refresh(self): pass # refreshEvent(self)

class NewSerializableDialog(QDialog):
	def __init__(self,parent,baseClassName,includeBase=True):
		import yade.system
		QDialog.__init__(self,parent)
		self.setWindowTitle('Create new object of type %s'%baseClassName)
		self.layout=QVBoxLayout(self)
		self.combo=QComboBox(self)
		childs=list(yade.system.childClasses(baseClassName,includeBase=includeBase)); childs.sort()
		self.combo.addItems(childs)
		self.combo.currentIndexChanged.connect(self.comboSlot)
		self.scroll=QScrollArea(self)
		self.buttons=QDialogButtonBox(QDialogButtonBox.Ok|QDialogButtonBox.Cancel);
		self.buttons.accepted.connect(self.accept)
		self.buttons.rejected.connect(self.reject)
		self.layout.addWidget(self.combo)
		self.layout.addWidget(self.scroll)
		self.layout.addWidget(self.buttons)
		self.ser=None
		self.combo.setCurrentIndex(0); self.comboSlot(0)
		self.setWindowModality(Qt.WindowModal)
	def comboSlot(self,index):
		item=str(self.combo.itemText(index))
		self.ser=eval(item+'()')
		self.scroll.setWidget(SerializableEditor(self.ser,self.scroll))
		self.scroll.show()
	def result(self): return self.ser
	def sizeHint(self): return QSize(180,400)



#print __name__
#if __name__=='__main__':
if 0:
	#vr=VTKRecorder()
	#s1=SerializableEditor(vr)
	#s1.show()
	#s2=SerializableEditor(OpenGLRenderer())
	#s2.show()
	#O.engines=[InsertionSortCollider(),VTKRecorder(),NewtonIntegrator()]
	#ss=SeqSerializable(parent=None,getter=lambda:O.engines,setter=lambda x:setattr(O,'engines',x),serType=Engine)
	#ss.show()
	nsd=NewSerializableDialog(None,'Engine')
	if nsd.exec_():
		retVal=nsd.result()
		print 'Returning',retVal
	

