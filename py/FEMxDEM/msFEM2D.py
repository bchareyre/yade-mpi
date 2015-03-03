__author__="Ning Guo, ceguo@connect.ust.hk"
__supervisor__="Jidong Zhao, jzhao@ust.hk"
__institution__="The Hong Kong University of Science and Technology"

""" 2D model for multiscale simulation
which implements a Newton-Raphson scheme
into FEM framework to solve the nonlinear
problem where the tangent operator is obtained
from DEM simulation by calling simDEM modules"""

# import Escript modules
import esys.escript as escript
from esys.escript import util
from esys.escript.linearPDEs import LinearPDE,SolverOptions
# import YADE modules
from simDEM import *
# other python modules
from itertools import repeat

""" function to return pool for parallelization
    supporting both MPI (experimental) on distributed
    memory and multiprocessing on shared memory.
"""
def get_pool(mpi=False,threads=1):
   if mpi: # using MPI
      from mpipool import MPIPool
      pool = MPIPool()
      pool.start()
      if not pool.is_master():
         sys.exit(0)
   elif threads>1: # using multiprocessing
      from multiprocessing import Pool
      pool = Pool(processes=threads)
   else:
      raise RuntimeError,"Wrong arguments: either mpi=True or threads>1."
   return pool

class MultiScale(object):
   """
   problem description:
   -(A_{ijkl} u_{k,l})_{,j} = -X_{ij,j} + Y_i
   Neumann boundary: n_j A_{ijkl} u_{k,l} = n_j X_{ij} + y_i
   Dirichlet boundary: u_i = r_i where q_i > 0
   :var u: unknown vector, displacement
   :var A: elastic tensor / tangent operator
   :var X: old/current stress tensor
   :var Y: vector, body force
   :var y: vector, Neumann bc traction
   :var q: vector, Dirichlet bc mask
   :var r: vector, Dirichlet bc value
   """
   def __init__(self,domain,ng=1,useMPI=False,np=1,random=False,rtol=1.e-2,usePert=False,pert=-2.e-6,verbose=False):
      """
      initialization of the problem, i.e. model constructor
      :param domain: type Domain, domain of the problem
      :param ng: type integer, number of Gauss points
      :param useMPI: type boolean, use MPI or not
      :param np: type integer, number of processors
      :param random: type boolean, if or not use random density field
      :param rtol: type float, relevative tolerance for global convergence
      :param usePert: type boolean, if or not use perturbation method
      :param pert: type float, perturbated strain applied to DEM to obtain tangent operator
      :param verbose: type boolean, if or not print messages during calculation
      """
      self.__domain=domain
      self.__pde=LinearPDE(domain,numEquations=self.__domain.getDim(),numSolutions=self.__domain.getDim())
      self.__pde.getSolverOptions().setSolverMethod(SolverOptions.DIRECT)
      self.__pde.setSymmetryOn()
      #self.__pde.getSolverOptions().setTolerance(rtol**2)
      #self.__pde.getSolverOptions().setPackage(SolverOptions.UMFPACK)
      self.__numGaussPoints=ng
      self.__rtol=rtol
      self.__usepert=usePert
      self.__pert=pert
      self.__verbose=verbose
      self.__pool=get_pool(mpi=useMPI,threads=np)
      self.__scenes=self.__pool.map(initLoad,range(ng))
      self.__strain=escript.Tensor(0,escript.Function(self.__domain))
      self.__stress=escript.Tensor(0,escript.Function(self.__domain))
      self.__S=escript.Tensor4(0,escript.Function(self.__domain))
      
      if self.__usepert:
         s = self.__pool.map(getStressTensor,self.__scenes)
         t = self.__pool.map(getTangentOperator,zip(self.__scenes,repeat(pert)))
         for i in xrange(ng):
            self.__stress.setValueOfDataPoint(i,s[i])
            self.__S.setValueOfDataPoint(i,t[i])
      else:
         st = self.__pool.map(getStressAndTangent2D,self.__scenes)
         for i in xrange(ng):
            self.__stress.setValueOfDataPoint(i,st[i][0])
            self.__S.setValueOfDataPoint(i,st[i][1])
                     
   def initialize(self, b=escript.Data(), f=escript.Data(), specified_u_mask=escript.Data(), specified_u_val=escript.Data()):
      """
      initialize the model for each time step, e.g. assign parameters
      :param b: type vector, body force on FunctionSpace, e.g. gravity
      :param f: type vector, boundary traction on FunctionSpace (FunctionOnBoundary)
      :param specified_u_mask: type vector, mask of location for Dirichlet boundary
      :param specified_u_val: type vector, specified displacement for Dirichlet boundary
      """
      self.__pde.setValue(Y=b,y=f,q=specified_u_mask,r=specified_u_val)
      
   def getDomain(self):
      """
      return model domain
      """
      return self.__domain
      
   def getRelTolerance(self):
      """
      return relative tolerance for convergence
      type float
      """
      return self.__rtol
  
   def getCurrentPacking(self,pos=(),time=0,prefix=''):
      if len(pos) == 0:
         # output all Gauss points packings
         self.__pool.map(outputPack,zip(self.__scenes,repeat(time),repeat(prefix)))
      else:
         # output selected Gauss points packings
         scene = [self.__scenes[i] for i in pos]
         self.__pool.map(outputPack,zip(scene,repeat(time),repeat(prefix)))
   
   def getLocalVoidRatio(self):
      void=escript.Scalar(0,escript.Function(self.__domain))
      e = self.__pool.map(getVoidRatio2D,self.__scenes)
      for i in xrange(self.__numGaussPoints):
         void.setValueOfDataPoint(i,e[i])
      return void
   
   def getLocalAvgRotation(self):
      rot=escript.Scalar(0,escript.Function(self.__domain))
      r = self.__pool.map(avgRotation2D,self.__scenes)
      for i in xrange(self.__numGaussPoints):
         rot.setValueOfDataPoint(i,r[i])
      return rot
   
   def getLocalFabric(self):
      fabric=escript.Tensor(0,escript.Function(self.__domain))
      f = self.__pool.map(getFabric2D,self.__scenes)
      for i in xrange(self.__numGaussPoints):
         fabric.setValueOfDataPoint(i,f[i])
      return fabric
   
   """ used for clumped particle model only
   def getLocalParOriFab(self):
      fabric=escript.Tensor(0,escript.Function(self.__domain))
      f = self.__pool.map(getParOriFabric,self.__scenes)
      for i in xrange(self.__numGaussPoints):
         fabric.setValueOfDataPoint(i,f[i])
      return fabric
   """
   
   """ used for cohesive particle model only
   def getLocalBondBreakage(self,oriIntr=[]):
      debond = escript.Scalar(0,escript.Function(self.__domain))
      num = self.__pool.map(getDebondingNumber,zip(self.__scenes,repeat(oriIntr)))
      for i in xrange(self.__numGaussPoints):
         debond.setValueOfDataPoint(i,num[i])
      return debond
   """
      
   def getCurrentTangent(self):
      """
      return current tangent operator
      type Tensor4 on FunctionSpace
      """
      return self.__S
      
   def getCurrentStress(self):
      """
      return current stress
      type: Tensor on FunctionSpace
      """
      return self.__stress
      
   def getCurrentStrain(self):
      """
      return current strain
      type: Tensor on FunctionSpace
      """
      return self.__strain
   
   def exitSimulation(self):
      """finish the whole simulation, exit"""
      self.__pool.close()
   
   def solve(self, iter_max=100):
      """
      solve the equation using Newton-Ralphson scheme
      """
      iterate=0
      rtol=self.getRelTolerance()
      stress=self.getCurrentStress()
      s=self.getCurrentTangent()
      x_safe=self.__domain.getX()
      self.__pde.setValue(A=s, X=-stress)
      #residual0=util.L2(self.__pde.getRightHandSide()) # using force error
      u=self.__pde.getSolution()  # trial solution, displacement
      D=util.grad(u)              # trial strain tensor
      # !!!!!! obtain stress and tangent operator from DEM part
      update_stress,update_s,update_scenes=self.applyStrain_getStressTangentDEM(st=D)
      err=1.0 # initial error before iteration
      converged=(err<rtol)
      while (not converged) and (iterate<iter_max):
         if self.__verbose:
            print "Not converged after %d iteration(s)! Relative error: %e"%(iterate,err)
         iterate+=1
         self.__domain.setX(x_safe+u)
         self.__pde.setValue(A=update_s,X=-update_stress,r=escript.Data())
         #residual=util.L2(self.__pde.getRightHandSide())
         du=self.__pde.getSolution()
         u+=du
         l,d=util.L2(u),util.L2(du)
         err=d/l # displacement error, alternatively using force error 'residual'
         converged=(err<rtol)
         if err>rtol*0.001: # only update DEM parts when error is large enough
            self.__domain.setX(x_safe)
            D=util.grad(u)
            update_stress,update_s,update_scenes=self.applyStrain_getStressTangentDEM(st=D)

         #if err>err_safe: # to ensure consistent convergence, however this may not be achieved due to fluctuation!
         #   raise RuntimeError,"No improvement of convergence with iterations! Relative error: %e"%err
      """
      update 'domain geometry', 'stress', 'tangent operator',
      'accumulated strain' and 'simulation scenes'.
      """
      self.__domain.setX(x_safe+u)
      self.__stress=update_stress
      self.__S=update_s
      self.__strain+=D
      self.__scenes=update_scenes
      if self.__verbose:
         print "Convergence reached after %d iteration(s)! Relative error: %e"%(iterate,err)
      return u
      
   """
   apply strain to DEM packing,
   get stress and tangent operator (including two methods)
   """
   def applyStrain_getStressTangentDEM(self,st=escript.Data()):
      st = st.toListOfTuples()
      st = numpy.array(st).reshape(-1,4)
      stress = escript.Tensor(0,escript.Function(self.__domain))
      S = escript.Tensor4(0,escript.Function(self.__domain))
      scenes = self.__pool.map(shear2D,zip(self.__scenes,st))
      if self.__usepert:
         s = self.__pool.map(getStressTensor,scenes)
         t = self.__pool.map(getTangentOperator,zip(scenes,repeat(self.__pert)))
         for i in xrange(self.__numGaussPoints):
            stress.setValueOfDataPoint(i,s[i])
            S.setValueOfDataPoint(i,t[i])
      else:
         ST = self.__pool.map(getStressAndTangent2D,scenes)
         for i in xrange(self.__numGaussPoints):
            stress.setValueOfDataPoint(i,ST[i][0])
            S.setValueOfDataPoint(i,ST[i][1])
      return stress,S,scenes
      
