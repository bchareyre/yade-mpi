# install pyqglviewer from https://launchpad.net/~nakednous/+ppa-packages
from PyQGLViewer import *

class Viewer(QGLViewer):
	def __init__(self):
		QGLViewer.__init__(self)
		self.renderer=OpenGLRenderer()
	def init(self):
		self.setAxisIsDrawn(True)
		self.setGridIsDrawn(True)
	def draw(self):
		self.renderer.render()

O.bodies.append([utils.sphere((0,0,-.4),.3),utils.sphere((0,0,.4),.3)])

viewer=Viewer()
viewer.setWindowTitle('Yade')
viewer.show()



