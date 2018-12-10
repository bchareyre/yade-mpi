from mpi4py import MPI 
from yade import * 
from math import log, ceil 
from yade import *
from yade.wrapper import *


"""This module partitions the bodies based on arecursive bisecting algorithm, (not really recursive but every proc participates)
For now we just pass the body ids and positions, can be easily modified to pass bodies. This generates a tree structure. The depth of the trr is called LEVEL.
In each level, we alternate the splitting axis like a kd-tree. Note : rank 0 is not involved here (it can be), but for now it's just the worker procs that use these functions
2. If all workers have to communicate with each other, (like a series of broadcasts), this module can be modified to handle it.
--Deepak kn, email : deepak.kunhappan@3sr-grenoble.fr """

#TODO: 
#1. initalize a split_axis based on the min_max of all the spheres. currently it starts with X 
#2. To add functionality for N number of procs, currently numprocs should be 2**(N)+1 
#3 .partition bodies directly. but is it needed?, can we have such a scenario?
#4. worker_cast, send_to_master


#init mpis#

comm = MPI.COMM_WORLD
rank = comm.Get_rank() 
threads = comm.Get_size() 
numThreads = threads-1
numLevels = int(ceil(log(numThreads)))+1
_DATA_ = 9999 # tag for sending datas 


class decomp_bodies: 
  
   def __init__(self,ndim):
    
    self.numLevels = numLevels
    self.numThreads = numThreads
    self.ndim = ndim #number of dimensions
    self.split_start = 1
    self.level_count = None 
   
    

   def scatter_data(self,in_data): 
     
     #this function scatters the data to other procs, collective communication. Every proc should call this, it's like MPI_SCATTER
	send_data = [];
	prc_data = in_data

	for nlevel in range(self.numLevels):
                self.level_count = nlevel
		to_recv,source_rank = self.if_scatter_recv(nlevel,rank)  	#to check if the proc is to recv (recv first, then send!) 
		if to_recv: 
			prc_data = comm.recv(source=source_rank, tag=_DATA_)
		to_send,dest_rank = self.if_scatter_send(nlevel,rank,self.numThreads)	 #to check if the proc is to send 
		if to_send:                         
                        prc_data,send_data = self.process_data(prc_data,nlevel)
			comm.send(send_data, dest=dest_rank,tag=_DATA_)
	return prc_data
      
      
   def if_scatter_recv(self,level,inrank): 	#level_pw2 = 2**(level),level_pw2_1 = 2**(level+1),  
     
     myrank = inrank-1
     level_pw2 = 1<<level; level_pw2_1 = 1<<(level+1)
     if (myrank >= level_pw2) and (myrank < level_pw2_1):
       source_rank = myrank - level_pw2
       return True,source_rank+1
     else:
       return False, -1 
     
     
     
   def if_scatter_send(self, level, inrank, nthreads):
	myrank = inrank-1
	level_pw2 = 1<<level
	if myrank < level_pw2:
		dest_rank = myrank+level_pw2
		if dest_rank >= nthreads:
			return False, -1 
		return True, dest_rank+1
	else:
		return False, -1 
	      
	      
	      
   def gather_data(self,in_data):
     
     #this fucntion gathers the data from the other procs, similar to MPI_GATHER.
     
     if rank==0: return None
     
     recv_data = []
     nlevel = self.numLevels-1
     while nlevel>=0: 
       to_send, dest_rank = self.if_gather_send(nlevel, rank)
       if to_send: 
	 comm.send(in_data,dest=dest_rank, tag=_DATA_)
       to_recv, source_rank = self.if_gather_recv(nlevel,rank,self.numThreads)
       if to_recv: 
	 recv_data = comm.recv(source=source_rank,tag=_DATA_)
	 in_data+=recv_data
       nlevel=nlevel-1
     return in_data 
   
   
   def if_gather_send(self,level, inrank):
     
     myrank = inrank-1
     level_pw2 = 1<<level; level_pw2_1 = 1<<(level+1)
     
     if level==0 and myrank >1:
       return False, -1
     
     elif (myrank >= level_pw2) and (myrank < level_pw2_1):
       dest_rank = myrank - level_pw2
       return True, dest_rank+1
     else:
       return False, -1
     
     
     
   def if_gather_recv(self,level,inrank, nthreads):
     
     myrank=inrank-1
     level_pw2 = (1<<level)
     if level == 0 and myrank >0:
       return False, -1
     elif (myrank < level_pw2):
       source_rank = myrank + level_pw2
       if source_rank >= nthreads:
	 return False, -1
       else:
	 return True, source_rank+1
     else:
       return False, -1 
	
     
   def process_data(self,in_data,level): #sorts the data, splits it into 2 parts, the split_axis is chosen based on the level/depth.
     
     if self.level_count==0: split_axis = self.split_start
     split_axis = level%self.ndim 
     sorted_data = self.sort_data(in_data,split_axis) 
     split_pt = len(sorted_data)//2 
     in_data = sorted_data[:split_pt]; send_data = sorted_data[split_pt+1:]
     send_data.append(sorted_data[split_pt])
     return in_data,send_data
 
   
   
   def process_bodies(self,body_list, level): #process bodies calls using list of positions and ids 
     
     pos_list = []; 
     for b in body_list:
         pos_list.append((b.state.pos,b.id))
     in1, in2 = self.process_data(pos_list,level)
     
     ids1 = [x[1] for x in in1]; ids2 = [x[1] for x in in2]
    # print "ids1 = ", ids1, rank
     body_list = [O.bodies[idx] for idx in ids1]
     part_list = [O.bodies[idx] for idx in ids2]
     return body_list, part_list


   def sort_data(self,in_data,split_axis): 
     return sorted(in_data, key=lambda x:x[0][split_axis])
 


   
    
     
     
     
     
     
     
     
     
     
     
	 
     
     
     
     
     
     
  
     
     
      
      
      
    

     
     