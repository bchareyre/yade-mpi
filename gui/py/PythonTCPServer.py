import SocketServer
import sys

from yade.wrapper import *

class PythonConsoleSocketEmulator(SocketServer.BaseRequestHandler):
	def setup(self):
		if not self.client_address[0].startswith('127.0.0'):
			print "TCP Connection from non-127.0.0.* address %s rejected"%self.client_address[0]
			return
		print self.client_address, 'connected!'
		self.request.send("""__   __    ____                 __  _____ ____ ____  
\ \ / /_ _|  _ \  ___    ___   / / |_   _/ ___|  _ \ 
 \ V / _` | | | |/ _ \  / _ \ / /    | || |   | |_) |
  | | (_| | |_| |  __/ | (_) / /     | || |___|  __/ 
  |_|\__,_|____/ \___|  \___/_/      |_| \____|_|    

(connected from %s:%d)
>>> """%(str(self.client_address[0]),self.client_address[1]))
	def displayhook(self,s):
		import pprint
		self.request.send(pprint.pformat(s))
	def handle(self):
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

class PythonTCPServer:
	def __init__(self,minPort=9000,host='',maxPort=65536,background=True):
		import socket
		self.port=-1
		self.host=host
		tryPort=minPort
		if maxPort==None: maxPort=minPort
		while self.port==-1 and tryPort<=maxPort:
			try:
				self.server=SocketServer.ThreadingTCPServer((host,tryPort),PythonConsoleSocketEmulator)
				self.port=tryPort
				print "Python TCP server listening on %s:%d"%(host if host else 'localhost',self.port)
				if background:
					import thread; thread.start_new_thread(self.server.serve_forever,())
				else: self.server.serve_forever()
			except socket.error:
				tryPort+=1
		if self.port==-1: raise RuntimeError("No free port to listen on in range %d-%d"%(minPort,maxPort))

if __name__=='__main__':
	PythonTCPServer(background=False)
