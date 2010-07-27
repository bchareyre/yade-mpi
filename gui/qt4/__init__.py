# encoding: utf-8
from PyQt4.QtGui import *
from PyQt4 import QtCore

from yade.qt.ui_controller import Ui_Controller
from yade.qt.SerializableEditor import SerializableEditor,SeqSerializable
from yade.qt.Inspector import *
from yade import *
import yade.system

from yade.qt._GLViewer import *

maxWebWindows=2
"Number of webkit windows that will be cycled to show help on clickable objects"
webWindows=[] 
"holds instances of QtWebKit windows; clicking an url will open it in the window that was the least recently updated"
sphinxOnlineDocPath='https://www.yade-dem.org/sphinx/'
"Base URL for the documentation. Packaged versions should change to the local installation directory."

# find if we have docs installed locally from package
import yade.config
import os.path
sphinxLocalDocPath=yade.config.prefix+'/share/doc/yade'+yade.config.suffix+'/html/'
# sorry
_eudoxosLocalPathHack=sphinxLocalDocPath='/home/vaclav/yt/doc/sphinx/_build/html/'
if os.path.exists(_eudoxosLocalPathHack): sphinxLocalDocPath=_eudoxosLocalPathHack
# end sorry
sphinxWrapperPart='yade.wrapper.html'
sphinxDocWrapperPage=(('file://'+sphinxLocalDocPath) if os.path.exists(sphinxLocalDocPath+'/'+sphinxWrapperPart) else sphinxOnlineDocPath)+'/'+sphinxWrapperPart


def openUrl(url):
	from PyQt4 import QtWebKit
	global maxWebWindows,webWindows
	reuseLast=False
	# use the last window if the class is the same and only the attribute differs
	try:
		reuseLast=(len(webWindows)>0 and str(webWindows[-1].url()).split('#')[-1].split('.')[2]==url.split('#')[-1].split('.')[2])
	except: pass
	if not reuseLast:
		if len(webWindows)<maxWebWindows: webWindows.append(QtWebKit.QWebView())
		else: webWindows=webWindows[1:]+[webWindows[0]]
	web=webWindows[-1]
	web.load(QUrl(url)); web.setWindowTitle(url);
	if 1:
		def killSidebar(result):
			frame=web.page().mainFrame()
			frame.evaluateJavaScript("var bv=$('.bodywrapper'); bv.css('margin','0 0 0 0');")
			frame.evaluateJavaScript("var sbw=$('.sphinxsidebarwrapper'); sbw.css('display','none');")
			frame.evaluateJavaScript("var sb=$('.sphinxsidebar'); sb.css('display','none'); ")
			frame.evaluateJavaScript("var sb=$('.sidebar'); sb.css('width','0px'); ")
			web.loadFinished.disconnect(killSidebar)
		web.loadFinished.connect(killSidebar)
	web.show();	web.raise_()


global _controller
controller=None

class ControllerClass(QWidget,Ui_Controller):
	def __init__(self,parent=None):
		QWidget.__init__(self)
		self.setupUi(self)
		self.generator=None # updated automatically
		self.renderer=Renderer() # only hold one instance, managed by OpenGLManager
		self.addPreprocessors()
		self.addRenderers()
		global controller
		controller=self
		self.refreshTimer=QtCore.QTimer()
		self.refreshTimer.timeout.connect(self.refreshEvent)
		self.refreshTimer.start(200)
		self.dtEditUpdate=True # to avoid updating while being edited
		# show off with this one as well now
	def addPreprocessors(self):
		for c in yade.system.childClasses('FileGenerator'):
			self.generatorCombo.addItem(c)
	def addRenderers(self):
		self.displayCombo.addItem('OpenGLRenderer'); afterSep=1
		for bc in ('GlShapeFunctor','GlStateFunctor','GlBoundFunctor','GlInteractionGeometryFunctor','GlInteractionPhysicsFunctor'):
			if afterSep>0: self.displayCombo.insertSeparator(10000); afterSep=0
			for c in yade.system.childClasses(bc) | set([bc]):
				inst=eval(c+'()');
				if len(set(inst.dict().keys())-set(['label']))>0:
					self.displayCombo.addItem(c); afterSep+=1
	def inspectSlot(self):
		self.inspector=SimulationInspector(parent=None)
		self.inspector.show()
	def setTabActive(self,what):
		if what=='simulation': ix=0
		elif what=='display': ix=1
		elif what=='generator': ix=2
		elif what=='python': ix=3
		else: raise ValueErorr("No such tab: "+what)
		self.controllerTabs.setCurrentIndex(ix)
	def generatorComboSlot(self,genStr):
		"update generator parameters when a new one is selected"
		gen=eval(str(genStr)+'()')
		self.generator=gen
		se=SerializableEditor(gen,parent=self.generatorArea,ignoredAttrs=set(['outputFileName']),showType=True)
		self.generatorArea.setWidget(se)
	def pythonComboSlot(self,cmd):
		try:
			code=compile(str(cmd),'<UI entry>','exec')
			exec code in globals()
		except:
			import traceback
			traceback.print_exc()
	def generateSlot(self):
		filename=str(self.generatorFilenameEdit.text())
		if self.generatorMemoryCheck.isChecked(): filename=':memory:'+filename
		print 'Will save to ',filename
		self.generator.generate(filename)
		if self.generatorAutoCheck:
			O.load(filename)
			self.setTabActive('simulation')
			if len(views())==0:
				v=View(); v.center()
	def displayComboSlot(self,dispStr):
		ser=(self.renderer if dispStr=='OpenGLRenderer' else eval(str(dispStr)+'()'))
		se=SerializableEditor(ser,parent=self.displayArea,ignoredAttrs=set(['label']),showType=True)
		self.displayArea.setWidget(se)
	def loadSlot(self):
		f=QFileDialog.getOpenFileName(self,'Load simulation','','Yade simulations (*.xml *.xml.bz2 *.xml.gz *.yade *.yade.gz *.yade.bz2);; *.*')
		f=str(f)
		if not f: return # cancelled
		self.deactivateControls()
		O.load(f)
	def saveSlot(self):
		f=QFileDialog.getSaveFileName(self,'Save simulation','','Yade simulations (*.xml *.xml.bz2 *.xml.gz *.yade *.yade.gz *.yade.bz2);; *.*')
		f=str(f)
		if not f: return # cancelled
		O.save(f)
	def reloadSlot(self):
		self.deactivateControls()
		O.reload()
	def dtFixedSlot(self):
		O.dt=O.dt
	def dtDynSlot(self):
		O.dt=-O.dt
	def dtEditNoupdateSlot(self):
		self.dtEditUpdate=False
	def dtEditedSlot(self):
		try:
			t=float(self.dtEdit.text())
			O.dt=t
		except ValueError: pass
		self.dtEdit.setText(str(O.dt))
		self.dtEditUpdate=True
	def playSlot(self):
		O.run()
	def pauseSlot(self):
		O.pause()
	def stepSlot(self):
		O.step()
	def new3dSlot(self):
		View()
	def setReferenceSlot(self):
		for b in O.bodies:
			b.state.refPos=b.state.pos
			b.state.refOri=b.state.ori
	def centerSlot(self):
		centerAllViews()
	def setViewAxes(self,dir,up):
		try:
			v=views()[0]
			v.viewDir=dir
			v.upVector=up
			v.center()
		except IndexError: pass
	def xyzSlot(self): self.setViewAxes((0,0,-1),(0,1,0))
	def yzxSlot(self): self.setViewAxes((-1,0,0),(0,0,1))
	def zxySlot(self): self.setViewAxes((0,-1,0),(1,0,0))
	def refreshEvent(self):
		self.refreshValues()
		self.activateControls()
	def deactivateControls(self):
		self.realTimeLabel.setText('')
		self.virtTimeLabel.setText('')
		self.iterLabel.setText('')
		self.fileLabel.setText('<i>[loading]</i>')
		self.playButton.setEnabled(False)
		self.pauseButton.setEnabled(False)
		self.stepButton.setEnabled(False)
		self.reloadButton.setEnabled(False)
		self.dtFixedRadio.setEnabled(False)
		self.dtDynRadio.setEnabled(False)
		self.dtEdit.setEnabled(False)
		self.dtEdit.setText('')
		self.dtEditUpdate=True
	def activateControls(self):
		hasSim=len(O.engines)>0
		running=O.running
		if hasSim:
			self.playButton.setEnabled(not running)
			self.pauseButton.setEnabled(running)
			self.reloadButton.setEnabled(O.filename is not None)
			self.stepButton.setEnabled(not running)
		else:
			self.playButton.setEnabled(False)
			self.pauseButton.setEnabled(False)
			self.reloadButton.setEnabled(False)
			self.stepButton.setEnabled(False)
		self.dtFixedRadio.setEnabled(True)
		self.dtDynRadio.setEnabled(O.dynDtAvailable)
		dynDt=O.dynDt
		self.dtFixedRadio.setChecked(not dynDt)
		self.dtDynRadio.setChecked(dynDt)
		if dynDt or self.dtEditUpdate:
			self.dtEdit.setText(str(O.dt))
		if dynDt: self.dtEditUpdate=True
		self.dtEdit.setEnabled(not dynDt)
		fn=O.filename
		self.fileLabel.setText(fn if fn else '<i>[no file]</i>')

	def refreshValues(self):
		rt=int(O.realtime); t=O.time; iter=O.iter; iterPerSec=iter/(rt if rt>0 else 1.); stopAtIter=O.stopAtIter
		if stopAtIter<=iter:
			self.realTimeLabel.setText('%02d:%02d:%02d'%(rt//3600,rt//60,rt%60))
			self.iterLabel.setText('#%ld, %.1f/s'%(iter,iterPerSec))
		else:
			e=int((stopAtIter-iter)*iterPerSec)
			self.realTimeLabel.setText('%02d:%02d:%02d (ETA %02d:%02d:%02d)'%(rt//3600,rt//60,rt%60,e//3600,e//60,e%60))
			self.iterLabel.setText('#%ld / %ld, %.1f/s'%(O.iter,stopAtIter,iterPerSec))
		s=int(t); ms=int(t*1000)%1000; us=int(t*1000000)%1000; ns=int(t*1000000000)%1000
		self.virtTimeLabel.setText(u'%03ds%03dm%03dμ%03dn'%(s,ms,us,ns))
		
def Generator():
	global controller
	if not controller: controller=ControllerClass();
	controller.show(); controller.raise_()
	controller.setTabActive('generator')
def Controller():
	global controller
	if not controller: controller=ControllerClass();
	controller.show(); controller.raise_()
	controller.setTabActive('simulation')

#if __name__=='__main__':
#	from PyQt4 import QtGui
#	import sys
#	qapp=QtGui.QApplication(sys.argv)
#	c=Controller().show()
#	qapp.exec_()
