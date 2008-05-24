#!/usr/local/bin/yade-trunk
# coding=UTF-8
# this must be run inside yade
#
# pass 'mail' as an argument so that the crash report is e-mailed 
#
#


import os,time,sys
import yade.runtime
simulFile='/tmp/yade-test-%d.xml'%(os.getpid()) # generated simulations here
pyCmdFile='/tmp/yade-test-%d.py'%(os.getpid()) # generated simulations here
msgFile='/tmp/yade-test-%d.speed'%(os.getpid()) # write speed hiere
runSimul="""
# generated file
simulFile='%s'
msgFile='%s'
nIter=%d
import time
o=Omega()
o.load(simulFile)
o.run(10); o.wait() # run first 10 iterations
start=time.time(); o.run(nIter); o.wait(); finish=time.time() # run nIter iterations, wait to finish, measure elapsed time
speed=nIter/(finish-start) # rough estimate
open(msgFile,'w').write('%%g iter/sec'%%speed)
quit()
"""%(simulFile,msgFile,100)

runGenerator="""
#generated file
p=Preprocessor('%%s'%%s)
p.generate('%s')
quit()
"""%(simulFile)


#f=open(pyCmdFile,'w'); f.write(runSimul); f.close()

broken=['SDECLinkedSpheres','SDECMovingWall','SDECSpheresPlane','ThreePointBending']
genParams={
	#'USCTGen':{'spheresFile':'examples/small.sdec.xyz'}
	}
#nIter=100
summary=[]
o=Omega()
broken=[]

def crashProofRun(cmd,quiet=True):
	import subprocess,os,os.path,yade.runtime
	f=open(pyCmdFile,'w'); f.write(cmd); f.close(); 
	if os.path.exists(msgFile): os.remove(msgFile)
	p=subprocess.Popen([yade.runtime.executable,'-N','PythonUI','--','-s',pyCmdFile],stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
	pout=p.communicate()[0]
	retval=p.wait()
	if not quiet: print pout
	msg=''
	if os.path.exists(msgFile): msg=open(msgFile,'r').readlines()[0]
	if retval==0: return True,msg,pout
	else: return False,msg,pout

generators=o.childClasses('FileGenerator')

reports=[]

for pp in o.childClasses('FileGenerator'): #['ThreePointBending','SDECLinkedSpheres']:
	if pp in broken:
		reports.append(pp,'skipped (broken)','')
	params='' if pp not in genParams else (",{"+",".join(["'%s':%s"%(k,repr(genParams[pp][k])) for k in genParams[pp]])+"}")
	ok1,msg1,out1=crashProofRun(runGenerator%(pp,params))
	if not ok1:
		reports.append([pp,'generator CRASH',out1])
		summary.append([pp,'generator CRASH'])
		continue
	ok2,msg2,out2=crashProofRun(runSimul)
	if not ok2:
		reports.append([pp,'simulation CRASH',out2])
		summary.append([pp,'simulation CRASH'])
	else:
		summary.append([pp,'generated, passed (%s)'%msg2])
	print summary[-1][0]+':',summary[-1][1]

reportText='\n'.join([80*'#'+'\n'+r[0]+': '+r[1]+'\n'+80*'#'+'\n'+r[2] for r in reports])

if 'mail' in yade.runtime.args:
	me,you='yade-tester.noreply@arcig.cz','yade-dev@lists.berlios.de'
	from email.mime.text import MIMEText
	msg=MIMEText(reportText)
	msg['Subject']="Automated crash report for "+yade.runtime.executable+": "+",".join([r[0] for r in reports])
	msg['From']=me
	msg['To']=you
	#print msg.as_string()
	import smtplib
	s=smtplib.SMTP()
	s.connect()
	s.sendmail(me,[you],msg.as_string())
	print msg.as_string()
	s.close()
else:
	print "\n\n=================================== PROBLEM DETAILS ===================================\n"
	print reportText

print "\n\n========================================= SUMMARY ======================================\n"
for l in summary: print "%30s: %s"%(l[0],l[1])

