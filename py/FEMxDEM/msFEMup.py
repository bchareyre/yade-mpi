__author__="Ning Guo, ceguo@connect.ust.hk"
__supervisor__="Jidong Zhao, jzhao@ust.hk"
__institution__="The Hong Kong University of Science and Technology"

""" 2D model for multiscale simulation of a 
hydromechanical system based on u-p formulation;
iterative scheme (fixed-stress split) is used for
the coupled PDEs; Newton-Raphson scheme is used to
solve the nonlinear PDE for displacement, pore
pressure is solved from the mass conservation equation;
total stress is a superposition of the effective stress
and pore pressure."""

# import Escript modules
import esys.escript as escript
from esys.escript import util
from esys.escript.linearPDEs import LinearPDE,SolverOptions
from simDEM import *
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
   problem description
   1. displacement:
   -(A_{ijkl} u_{k,l})_{,j} = -X_{ij,j} + Y_i
   Neumann boundary: n_j A_{ijkl} u_{k,l} = n_j X_{ij} + y_i
   Dirichlet boundary: u_i = r_i where q_i > 0
   :var u: unknown vector, displacement
   :var A: elastic tensor / tangent operator
   :var X: tensor, minus old stress
   :var Y: vector, gamma - grad(p)
   :var y: vector, Neumann bc traction
   :var q: vector, Dirichlet bc mask
   :var r: vector, Dirichlet bc value
   2. pore pressure:
   -(A_{ij} p_{,j})_{,i} + D p = Y
   Neumann boundary: n_j A_{jl} p_{,l} = y
   Dirichlet boundary: p = r where q > 0
   :var p: unknown scalar, pore pressure
   :var A: permeability tensor
   :var D: scalar, n / (K_f dt)
   :var Y: scalar, -dot(u)_{i,i} + n p_pre / (K_f dt)
   :var y: scalar, Neumann bc flux
   :var q: scalar, Dirichlet bc mask
   :var r: scalar, Dirichlet bc value
   """
   def __init__(self,domain,pore0=0.,perm=1.e-5,kf=2.2e9,dt=0.001,ng=1,useMPI=False,np=1,rtol=1.e-2):
      """
      initialization of the problem, i.e. model constructor
      :param domain: type Domain, domain of the problem
      :param pore0: type float, initial pore pressure
      :param perm: type float, d^2/(150 mu_f) in KC equation
      :param kf: type float, bulk modulus of the fluid
      :param dt: type float, time step for calculation
      :param ng: type integer, number of Gauss points
      :param useMPI: type boolean, use MPI or not
      :param np: type integer, number of processors
      :param rtol: type float, relevative tolerance for global convergence
      """
      self.__domain=domain
      self.__upde=LinearPDE(domain,numEquations=domain.getDim(),numSolutions=domain.getDim())
      self.__ppde=LinearPDE(domain,numEquations=1,numSolutions=1)
      # use reduced interpolation for pore pressure
      self.__ppde.setReducedOrderOn()
      
      self.__upde.getSolverOptions().setSolverMethod(SolverOptions.DIRECT)
      self.__ppde.getSolverOptions().setSolverMethod(SolverOptions.DIRECT)
      self.__upde.setSymmetryOn()
      self.__ppde.setSymmetryOn()
      
      self.__dt=dt
      self.__bulkFluid=kf
      self.__numGaussPoints=ng
      self.__rtol=rtol
      self.__stress=escript.Tensor(0,escript.Function(domain))
      self.__S=escript.Tensor4(0,escript.Function(domain))
      self.__pool=get_pool(mpi=useMPI,threads=np)
      self.__scenes=self.__pool.map(initLoad,range(ng))
      st = self.__pool.map(getStressAndTangent2D,self.__scenes)
      for i in xrange(ng):
         self.__stress.setValueOfDataPoint(i,st[i][0])
         self.__S.setValueOfDataPoint(i,st[i][1])
      self.__strain=escript.Tensor(0,escript.Function(domain))
      self.__pore=escript.Scalar(pore0,escript.ReducedSolution(domain))
      self.__pgauss=util.interpolate(pore0,escript.Function(domain))
      self.__permeability=perm
      self.__meanStressRate=escript.Scalar(0,escript.Function(domain))
      self.__r=escript.Vector(0,escript.Solution(domain)) #Dirichlet BC for u

   def initialize(self, b=escript.Data(), f=escript.Data(), umsk=escript.Data(), uvalue=escript.Data(), flux=escript.Data(), pmsk=escript.Data(), pvalue=escript.Data()):
      """
      initialize the model for each time step, e.g. assign parameters
      :param b: type vector, body force on FunctionSpace, e.g. gravity
      :param f: type vector, boundary traction on FunctionSpace (FunctionOnBoundary)
      :param umsk: type vector, mask of location for Dirichlet boundary
      :param uvalue: type vector, specified displacement for Dirichlet boundary
      """
      self.__upde.setValue(Y=b,y=f,q=umsk,r=uvalue)
      self.__ppde.setValue(y=flux,q=pmsk,r=pvalue)
      self.__r=uvalue                  
   
   def getDomain(self):
      """
      return model domain
      """
      return self.__domain
      
   def setTimeStep(self,dt=1.0):
      self.__dt = dt

   def getCurrentPacking(self,pos=(),time=0,prefix=''):
      if len(pos) == 0: # output all Gauss points packings
         self.__pool.map(outputPack,zip(self.__scenes,repeat(time),repeat(prefix)))
      else: # output selected Gauss points packings
         scene = [self.__scenes[i] for i in pos]
         self.__pool.map(outputPack,zip(scene,repeat(time),repeat(prefix)))
   
   def getEquivalentPorosity(self):
      porosity=escript.Scalar(0,escript.Function(self.__domain))
      p = self.__pool.map(getEquivalentPorosity,self.__scenes)
      for i in xrange(self.__numGaussPoints):
         porosity.setValueOfDataPoint(i,p[i])
      return porosity
   
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
   
   def getCurrentTangent(self):
      """
      return current tangent operator
      type Tensor4 on FunctionSpace
      """
      return self.__S
      
   def getCurrentStress(self):
      """
      return current stress (effective)
      type: Tensor on FunctionSpace
      """
      return self.__stress
      
   def getCurrentPore(self):
      """
      return current pore pressure
      type: Scalar on ReducedSolution
      """
      return self.__pore
      
   def getCurrentStrain(self):
      """
      return current strain
      type: Tensor on FunctionSpace
      """
      return self.__strain

   def getCurrentFlux(self):
      """
      return current Darcy flux
      type: Vector on FunctionSpace
      """
      n=self.getEquivalentPorosity()
      perm=self.__permeability*n**3/(1.-n)**2
      flux=-perm*util.grad(self.__pore)
      return flux
   
   def exitSimulation(self):
      """finish the whole simulation, exit"""
      self.__pool.close()
   
   def solveSolid(self, p_iter_gauss=escript.Data(), iter_max=50):
      """
      solve the pde for displacement using Newton-Ralphson scheme
      """
      k=util.kronecker(self.__domain)
      p=p_iter_gauss*k
      iterate=0
      rtol=self.__rtol
      stress_safe=self.__stress
      s_safe=self.__S
      x_safe=self.__domain.getX()
      self.__upde.setValue(A=s_safe, X=-stress_safe+p, r=self.__r)
      #residual0=util.L2(self.__pde.getRightHandSide()) # using force error
      u=self.__upde.getSolution()  # trial solution, displacement
      D=util.grad(u)               # trial strain tensor
      # !!!!!! obtain stress and tangent operator from DEM part
      update_stress,update_s,update_scenes=self.applyStrain_getStressTangentDEM(st=D)
      err=util.Lsup(u) # initial error before iteration
      converged=(err<1.e-12)
      while (not converged) and (iterate<iter_max):
         #if iterate>iter_max:
         #   raise RuntimeError,"Convergence for Newton-Raphson failed after %s steps."%(iter_max)
         iterate+=1
         self.__domain.setX(x_safe+u)
         self.__upde.setValue(A=update_s,X=-update_stress+p,r=escript.Data())
         #residual=util.L2(self.__pde.getRightHandSide())
         du=self.__upde.getSolution()
         u+=du
         l,d=util.L2(u),util.L2(du)
         err=d/l # displacement error, alternatively using force error 'residual'
         converged=(err<rtol)
         if err>rtol**3: # only update DEM parts when error is large enough
            self.__domain.setX(x_safe)
            D=util.grad(u)
            update_stress,update_s,update_scenes=self.applyStrain_getStressTangentDEM(st=D)
      """reset domain geometry to original until global convergence"""
      self.__domain.setX(x_safe)
      return u,D,update_stress,update_s,update_scenes
   
   def solve(self, globalIter=10, solidIter=50):
      """
      solve the coupled PDE using fixed-stress split method,
      call solveSolid to get displacement
      """
      rtol=self.__rtol
      x_safe=self.__domain.getX()
      k=util.kronecker(self.__domain)
      kdr=util.inner(k,util.tensor_mult(self.__S,k))/4.
      n=self.getEquivalentPorosity()
      perm=self.__permeability*n**3/(1.-n)**2*k
      kf=self.__bulkFluid
      dt=self.__dt
      self.__ppde.setValue(A=perm,D=(n/kf+1./kdr)/dt,Y=(n/kf+1./kdr)/dt*self.__pgauss-self.__meanStressRate/kdr)
      p_iter_old=self.__ppde.getSolution()
      p_iter_gauss=util.interpolate(p_iter_old,escript.Function(self.__domain))
      u_old,D,sig,s,scene=self.solveSolid(p_iter_gauss=p_iter_gauss,iter_max=solidIter)
      converge=False
      iterate=0
      while (not converge) and (iterate<globalIter):
         iterate += 1
         self.__ppde.setValue(Y=n/kf/dt*self.__pgauss+1./kdr/dt*p_iter_gauss-util.trace(D)/dt)
         p_iter=self.__ppde.getSolution()
         p_err=util.L2(p_iter-p_iter_old)/util.L2(p_iter)
         p_iter_old=p_iter
         p_iter_gauss=util.interpolate(p_iter,escript.Function(self.__domain))
         u,D,sig,s,scene=self.solveSolid(p_iter_gauss=p_iter_gauss,iter_max=solidIter)
         u_err=util.L2(u-u_old)/util.L2(u)
         u_old=u
         converge=(u_err<=rtol and p_err <= rtol*.1)
      self.__meanStressRate=(util.trace(sig-self.__stress)/2.-p_iter_gauss+self.__pgauss)/dt
      self.__pore=p_iter_old
      self.__pgauss=p_iter_gauss
      self.__domain.setX(x_safe+u_old)
      self.__strain+=D
      self.__stress=sig
      self.__S=s
      self.__scenes=scene
      return u_old

   """
   apply strain to DEM packing,
   get stress and tangent operator
   """
   def applyStrain_getStressTangentDEM(self,st=escript.Data()):
      st = st.toListOfTuples()
      st = numpy.array(st).reshape(-1,4)
      stress = escript.Tensor(0,escript.Function(self.__domain))
      S = escript.Tensor4(0,escript.Function(self.__domain))
      scenes = self.__pool.map(shear2D,zip(self.__scenes,st))
      ST = self.__pool.map(getStressAndTangent2D,scenes)
      for i in xrange(self.__numGaussPoints):
         stress.setValueOfDataPoint(i,ST[i][0])
         S.setValueOfDataPoint(i,ST[i][1])
      return stress,S,scenes
      
