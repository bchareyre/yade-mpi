"""
2D/3D Function to output Gauss point tensorial quantities, e.g. stress, strain, fabric.
Author: Ning Guo <ceguo@connect.ust.hk>
"""
def saveGauss2D(name='',pos=(),**kwargs):
   fout = file(name,'w')
   for key in kwargs:
      data = kwargs[key].toListOfTuples()
      if len(pos)==0:
         fout.write('%s '%key+str(len(data))+'\n')
         for i in xrange(len(data)):
            fout.write(' '.join('%s %s'%x for x in data[i])+'\n')
      else:
         fout.write('%s '%key+str(len(pos))+'\n')
         for i in pos:
            fout.write(' '.join('%s %s'%x for x in data[i])+'\n')
   fout.close()
   
def saveGauss3D(name='',pos=(),**kwargs):
   fout = file(name,'w')
   for key in kwargs:
      data = kwargs[key].toListOfTuples()
      if len(pos)==0:
         fout.write('%s '%key+str(len(data))+'\n')
         for i in xrange(len(data)):
            fout.write(' '.join('%s %s %s'%x for x in data[i])+'\n')
      else:
         fout.write('%s '%key+str(len(pos))+'\n')
         for i in pos:
            fout.write(' '.join('%s %s %s'%x for x in data[i])+'\n')
   fout.close()
   
