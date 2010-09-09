# encoding: utf-8
# 2008-2009 © Václav Šmilauer <eudoxos@arcig.cz>
"""
Remote connections to yade: authenticated python command-line over telnet and anonymous socket for getting some read-only information about current simulation.

These classes are used internally in gui/py/PythonUI_rc.py and are not intended for direct use.
"""

import SocketServer,xmlrpclib,socket
import sys,time,os

from yade import *
import yade.runtime

useQThread=False
"Set before using any of our classes to use QThread for background execution instead of the standard thread module. Mixing the two (in case the qt4 UI is running, for instance) does not work well."

plotImgFormat,plotImgMimetype='png','image/png'
#plotImgFormat,plotImgMimetype='svg','image/svg+xml'

bgThreads=[] # needed to keep background threads alive

class InfoProvider:
	def basicInfo(self):
		ret=dict(iter=O.iter,dt=O.dt,stopAtIter=O.stopAtIter,realtime=O.realtime,time=O.time,id=O.tags['id'] if O.tags.has_key('id') else None,threads=os.environ['OMP_NUM_THREADS'] if os.environ.has_key('OMP_NUM_THREADS') else '0',numBodies=len(O.bodies),numIntrs=len(O.interactions))
		sys.stdout.flush(); sys.stderr.flush()
		return ret
	def plot(self):
		from yade import plot
		if len(plot.plots)==0: return None
		fig=plot.plot(subPlots=True,noShow=True)
		img=O.tmpFilename()+'.'+plotImgFormat
		fig.savefig(img,dpi=200)
		f=open(img,'rb'); data=f.read(); f.close(); os.remove(img)
		#print 'returning '+plotImgFormat
		return xmlrpclib.Binary(data)
		

class PythonConsoleSocketEmulator(SocketServer.BaseRequestHandler):
	"""Class emulating python command-line over a socket connection.

	The connection is authenticated by requiring a cookie.
	Only connections from localhost (127.0.0.*) are allowed.
	"""
	def setup(self):
		if not self.client_address[0].startswith('127.0.0'):
			print "TCP Connection from non-127.0.0.* address %s rejected"%self.client_address[0]
			return
		print self.client_address, 'connected!'
		self.request.send('Enter auth cookie: ')
	def tryLogin(self):
		if self.request.recv(1024).rstrip()==self.server.cookie:
	  		self.server.authenticated+=[self.client_address]
			self.request.send("""__   __    ____                 __  _____ ____ ____  
\ \ / /_ _|  _ \  ___    ___   / / |_   _/ ___|  _ \ 
 \ V / _` | | | |/ _ \  / _ \ / /    | || |   | |_) |
  | | (_| | |_| |  __/ | (_) / /     | || |___|  __/ 
  |_|\__,_|____/ \___|  \___/_/      |_| \____|_|    

(connected from %s:%d)
>>> """%(str(self.client_address[0]),self.client_address[1]))
  			return True
		else:
			import time
			time.sleep(5)
			print "invalid cookie"
			return False
	def displayhook(self,s):
		import pprint
		self.request.send(pprint.pformat(s))
	def handle(self):
		if self.client_address not in self.server.authenticated and not self.tryLogin(): return
		import code,cStringIO,traceback
		buf=[]
		while True:
			data = self.request.recv(1024).rstrip()
			if data=='\x04' or data=='exit' or data=='quit': # \x04 == ^D 
				return
			buf.append(data)
			orig_displayhook,orig_stdout=sys.displayhook,sys.stdout
			sio=cStringIO.StringIO()
			continuation=False
			#print "buffer:",buf
			try:
				comp=code.compile_command('\n'.join(buf))
				if comp:
					sys.displayhook=self.displayhook
					sys.stdout=sio
					exec comp
					self.request.send(sio.getvalue())
					buf=[]
				else:
					self.request.send('... '); continuation=True
			except:
				self.request.send(traceback.format_exc())
				buf=[]
			finally:
				sys.displayhook,sys.stdout=orig_displayhook,orig_stdout
				if not continuation: self.request.send('\n>>> ')
	def finish(self):
		print self.client_address, 'disconnected!'
		self.request.send('\nBye ' + str(self.client_address) + '\n')


def _runInBackground(func):
	if useQThread:
		from PyQt4.QtCore import QThread
		class WorkerThread(QThread):
			def __init__(self,func_): QThread.__init__(self); self.func=func_
			def run(self): self.func()
		wt=WorkerThread(func)
		wt.start()
		global bgThreads; bgThreads.append(wt)
	else:
		import thread; thread.start_new_thread(func,())


class GenericTCPServer:
	"Base class for socket server, handling port allocation, initial logging and thead backgrounding."
	def __init__(self,handler,title,cookie=True,minPort=9000,host='',maxPort=65536,background=True):
		import socket, random, sys
		self.port=-1
		self.host=host
		tryPort=minPort
		if maxPort==None: maxPort=minPort
		while self.port==-1 and tryPort<=maxPort:
			try:
				self.server=SocketServer.ThreadingTCPServer((host,tryPort),handler)
				self.port=tryPort
				if cookie:
					self.server.cookie=''.join([i for i in random.sample('yadesucks',6)])
					self.server.authenticated=[]
					sys.stderr.write(title+" on %s:%d, auth cookie `%s'\n"%(host if host else 'localhost',self.port,self.server.cookie))
				else:
					sys.stderr.write(title+" on %s:%d\n"%(host if host else 'localhost',self.port))
				if background: _runInBackground(self.server.serve_forever)
				else: self.server.serve_forever()
			except socket.error:
				tryPort+=1
		if self.port==-1: raise RuntimeError("No free port to listen on in range %d-%d"%(minPort,maxPort))


def runServers():
	"""Run python telnet server and info socket. They will be run at localhost on ports 9000 (or higher if used) and 21000 (or higer if used) respectively.
	
	The python telnet server accepts only connection from localhost,
	after authentication by random cookie, which is printed on stdout
	at server startup.

	The info socket provides read-only access to several simulation parameters
	at runtime. Each connection receives pickled dictionary with those values.
	This socket is primarily used by yade-multi batch scheduler.
	"""
	srv=GenericTCPServer(handler=yade.remote.PythonConsoleSocketEmulator,title='TCP python prompt',cookie=True,minPort=9000)
	yade.runtime.cookie=srv.server.cookie
	#info=GenericTCPServer(handler=yade.remote.InfoSocketProvider,title='TCP info provider',cookie=False,minPort=21000)
	## XMPRPC server for general information:
	if 1:
		from SimpleXMLRPCServer import SimpleXMLRPCServer
		port,maxPort=21000,65535 # minimum port number
		while port<maxPort:
			try:
				info=SimpleXMLRPCServer(('',port),logRequests=False,allow_none=True); break
			except socket.error: port+=1
		if port==maxPort: raise RuntimeError("No free port to listen on in range 21000-%d"%maxPort)
		# register methods, as per http://docs.python.org/library/simplexmlrpcserver.html#simplexmlrpcserver-example
		info.register_instance(InfoProvider()) # gets all defined methods by introspection
		#prov=InfoProvider()
		#for m in prov.exposedMethods(): info.register_function(m)
		_runInBackground(info.serve_forever)
		print 'XMLRPC info provider on http://localhost:%d'%port
	sys.stdout.flush()




#if __name__=='__main__':
#	p=GenericTCPServer(PythonConsoleSocketEmulator,'Python TCP server',background=False)
#	#while True: time.sleep(2)

