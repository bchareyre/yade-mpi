# encoding: utf-8

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from yade import *
from yade.qt.SerializableEditor import *
import yade.qt

class EngineInspector(SeqSerializable):
	def __init__(self,parent=None):
		SeqSerializable.__init__(self,parent=parent,getter=lambda:O.engines,setter=lambda x:setattr(O,'engines',x),serType=Engine)

def makeBodyLabel(b):
	ret=str(b.id)+' '
	if not b.shape: ret+=u'⬚'
	else:
		typeMap={'Sphere':u'⚫','Facet':u'△','Wall':u'┃','Box':u'⎕','Cylinder':u'⌭','ChainedCylinder':u'☡'}
		ret+=typeMap.get(b.shape.__class__.__name__,'﹖')
	if not b.dynamic: ret+=u'⚓'
	elif b.state.blockedDOFs!=[]: ret+=u'⎈'
	return ret

def getBodyIdFromLabel(label):
	try:
		return int(unicode(label).split()[0])
	except ValueError:
		print 'Error with label:',unicode(label)
		return -1

class BodyInspector(QWidget):
	def __init__(self,bodyId=0,parent=None,bodyLinkCallback=None,intrLinkCallback=None):
		QWidget.__init__(self,parent)
		self.bodyId=bodyId
		self.idGlSync=-1
		self.bodyLinkCallback,self.intrLinkCallback=bodyLinkCallback,intrLinkCallback
		self.bodyIdBox=QSpinBox(self)
		self.bodyIdBox.setMinimum(0)
		self.bodyIdBox.setMaximum(100000000)
		self.bodyIdBox.setValue(0)
		self.intrWithCombo=QComboBox(self);
		self.gotoBodyButton=QPushButton(u'→ #',self)
		self.gotoIntrButton=QPushButton(u'→ #+#',self)
		topBoxWidget=QWidget(self)
		topBox=QHBoxLayout(topBoxWidget)
		hashLabel=QLabel('#',self); hashLabel.setFixedWidth(10)
		topBox.addWidget(hashLabel)
		topBox.addWidget(self.bodyIdBox)
		self.plusLabel=QLabel('+',self)
		topBox.addWidget(self.plusLabel)
		topBox.addWidget(self.intrWithCombo)
		topBox.addWidget(self.gotoBodyButton)
		topBox.addWidget(self.gotoIntrButton)
		topBoxWidget.setLayout(topBox)
		self.grid=QGridLayout(self); self.grid.setSpacing(0); self.grid.setMargin(0)
		self.grid.addWidget(topBoxWidget,0,0)
		self.scroll=QScrollArea(self)
		self.scroll.setWidgetResizable(True)
		self.grid.addWidget(self.scroll)
		self.tryShowBody()
		self.bodyIdBox.valueChanged.connect(self.bodyIdSlot)
		self.gotoBodyButton.clicked.connect(self.gotoBodySlot)
		self.gotoIntrButton.clicked.connect(self.gotoIntrSlot)
		self.refreshTimer=QTimer(self)
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(1000)
		self.intrWithCombo.addItems(['0']); self.intrWithCombo.setCurrentIndex(0);
		self.intrWithCombo.setMinimumWidth(80)
		self.setWindowTitle('Body #%d'%self.bodyId)
		self.gotoBodySlot()
	def tryShowBody(self):
		try:
			b=O.bodies[self.bodyId]
			self.serEd=SerializableEditor(b,showType=True,parent=self)
		except IndexError:
			self.serEd=QFrame(self)
			self.bodyId=-1
		self.scroll.setWidget(self.serEd)
	def changeIdSlot(self,newId):
		self.bodyIdBox.setValue(newId);
		self.bodyIdSlot()
	def bodyIdSlot(self):
		self.bodyId=self.bodyIdBox.value()
		self.tryShowBody()
		self.setWindowTitle('Body #%d'%self.bodyId)
		self.refreshEvent()
	def gotoBodySlot(self):
		try:
			id=int(getBodyIdFromLabel(self.intrWithCombo.currentText()))
		except ValueError: return # empty id
		if not self.bodyLinkCallback:
			self.bodyIdBox.setValue(id); self.bodyId=id
		else: self.bodyLinkCallback(id)
	def gotoIntrSlot(self):
		ids=self.bodyIdBox.value(),getBodyIdFromLabel(self.intrWithCombo.currentText())
		if not self.intrLinkCallback:
			self.ii=InteractionInspector(ids)
			self.ii.show()
		else: self.intrLinkCallback(ids)
	def refreshEvent(self):
		try: O.bodies[self.bodyId]
		except: self.bodyId=-1 # invalidate deleted body
		# no body shown yet, try to get the first one...
		if self.bodyId<0 and len(O.bodies)>0:
			try:
				b=O.bodies[0]; self.bodyIdBox.setValue(0)
			except IndexError: pass
		v=yade.qt.views()
		if len(v)>0 and v[0].selection!=self.bodyId:
			if self.idGlSync==self.bodyId: # changed in the viewer, reset ourselves
				self.bodyId=self.idGlSync=v[0].selection; self.changeIdSlot(self.bodyId)
				return
			else: v[0].selection=self.idGlSync=self.bodyId # changed here, set in the viewer
		meId=self.bodyIdBox.value(); pos=self.intrWithCombo.currentIndex()
		meLabel=makeBodyLabel(O.bodies[meId])
		self.plusLabel.setText(' '.join(meLabel.split()[1:])+'  <b>+</b>') # do not repeat the id
		self.bodyIdBox.setMaximum(len(O.bodies)-1)
		others=[(i.id1 if i.id1!=meId else i.id2) for i in O.interactions.withBody(self.bodyIdBox.value()) if i.isReal]
		others.sort()
		self.intrWithCombo.clear()
		self.intrWithCombo.addItems([makeBodyLabel(O.bodies[i]) for i in others])
		if pos>self.intrWithCombo.count() or pos<0: pos=0
		self.intrWithCombo.setCurrentIndex(pos);
		other=self.intrWithCombo.itemText(pos)
		if other=='':
			self.gotoBodyButton.setEnabled(False); self.gotoIntrButton.setEnabled(False)
			other=u'∅'
		else:
			self.gotoBodyButton.setEnabled(True); self.gotoIntrButton.setEnabled(True)
		self.gotoBodyButton.setText(u'→ %s'%other)
		self.gotoIntrButton.setText(u'→ %s + %s'%(meLabel,other))
		
class InteractionInspector(QWidget):
	def __init__(self,ids=None,parent=None,bodyLinkCallback=None):
		QWidget.__init__(self,parent)
		self.bodyLinkCallback=bodyLinkCallback
		self.ids=ids
		self.gotoId1Button=QPushButton(u'#…',self)
		self.gotoId2Button=QPushButton(u'#…',self)
		self.gotoId1Button.clicked.connect(self.gotoId1Slot)
		self.gotoId2Button.clicked.connect(self.gotoId2Slot)
		topBoxWidget=QWidget(self)
		topBox=QHBoxLayout(topBoxWidget)
		topBox.addWidget(self.gotoId1Button)
		labelPlus=QLabel('+',self); labelPlus.setAlignment(Qt.AlignHCenter)
		topBox.addWidget(labelPlus)
		topBox.addWidget(self.gotoId2Button)
		topBoxWidget.setLayout(topBox)
		self.setWindowTitle(u'No interaction')
		self.grid=QGridLayout(self); self.grid.setSpacing(0); self.grid.setMargin(0)
		self.grid.addWidget(topBoxWidget,0,0)
		self.scroll=QScrollArea(self)
		self.scroll.setWidgetResizable(True)
		self.grid.addWidget(self.scroll)
		self.refreshTimer=QTimer(self)
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(1000)
		if self.ids: self.setupInteraction()
	def setupInteraction(self):
		try:
			intr=O.interactions[self.ids[0],self.ids[1]]
			self.serEd=SerializableEditor(intr,showType=True,parent=self.scroll)
			self.scroll.setWidget(self.serEd)
			self.gotoId1Button.setText('#'+makeBodyLabel(O.bodies[self.ids[0]]))
			self.gotoId2Button.setText('#'+makeBodyLabel(O.bodies[self.ids[1]]))
			self.setWindowTitle('Interaction #%d + #%d'%(self.ids[0],self.ids[1]))
		except IndexError:
			if self.ids:  # reset view (there was an interaction)
				self.ids=None
				self.serEd=QFrame(self.scroll); self.scroll.setWidget(self.serEd) 
				self.setWindowTitle('No interaction')
				self.gotoId1Button.setText(u'#…'); self.gotoId2Button.setText(u'#…');
	def gotoId(self,bodyId):
		if self.bodyLinkCallback: self.bodyLinkCallback(bodyId)
		else: self.bi=BodyInspector(bodyId); self.bi.show()
	def gotoId1Slot(self): self.gotoId(self.ids[0])
	def gotoId2Slot(self): self.gotoId(self.ids[1])
	def refreshEvent(self):
		# no ids yet -- try getting the first interaction, if it exists
		if not self.ids:
			try:
				i=O.interactions.nth(0)
				self.ids=i.id1,i.id2
				self.setupInteraction()
				return
			except IndexError: return # no interaction exists at all
		try: # try to fetch an existing interaction
			O.interactions[self.ids[0],self.ids[1]]
		except IndexError:
			self.ids=None
			
class SimulationInspector(QWidget):
	def __init__(self,parent=None):
		QWidget.__init__(self,parent)
		self.setWindowTitle("Simulation Inspection")
		self.tabWidget=QTabWidget(self)

		self.engineInspector=EngineInspector(parent=None)
		self.bodyInspector=BodyInspector(parent=None,intrLinkCallback=self.changeIntrIds)
		self.intrInspector=InteractionInspector(parent=None,bodyLinkCallback=self.changeBodyId)

		for i,name,widget in [(0,'Engines',self.engineInspector),(1,'Bodies',self.bodyInspector),(2,'Interactions',self.intrInspector)]:
			self.tabWidget.addTab(widget,name)
		grid=QGridLayout(self); grid.setSpacing(0); grid.setMargin(0)
		grid.addWidget(self.tabWidget)
		self.setLayout(grid)
	def changeIntrIds(self,ids):
		self.tabWidget.removeTab(2); self.intrInspector.close()
		self.intrInspector=InteractionInspector(ids=ids,parent=None,bodyLinkCallback=self.changeBodyId)
		self.tabWidget.insertTab(2,self.intrInspector,'Interactions')
		self.tabWidget.setCurrentIndex(2)
	def changeBodyId(self,id):
		self.bodyInspector.changeIdSlot(id)
		self.tabWidget.setCurrentIndex(1)
		
