.. _FEMxDEM:

##############################################################################################################################
Parallel hierarchical multiscale modeling of granular media by coupling FEM and DEM with open-source codes Escript and YADE
##############################################################################################################################
Authors: Ning Guo and Jidong Zhao

Institution: Hong Kong University of Science and Technology

Escript download page: https://launchpad.net/escript-finley

mpi4py download page (optional, require MPI): https://bitbucket.org/mpi4py/mpi4py

Tested platforms: Desktop with Ubuntu 10.04, 32 bit; Server with Ubuntu 12.04, 14.04, 64 bit; Cluster with Centos 6.2, 6.5, 64 bit;

Introduction
^^^^^^^^^^^^^^^^
The code is built upon two open source packages: Yade for DEM modules and Escript for FEM modules. It implements the hierarchical multiscale model (FEMxDEM) for simulating the boundary value problem (BVP) of granular media. FEM is used to discretize the problem domain. Each Gauss point of the FEM mesh is embedded a representative volume element (RVE) packing simulated by DEM which returns local material constitutive responses to FEM. Typically, hundreds to thousands of RVEs are involved in a medium-sized problem which is critically time consuming. Hence parallelization is achieved in the code through either multiprocessing on a supercomputer or mpi4py on a HPC cluster (require MPICH or Open MPI). The MPI implementation in the code is quite experimental. The "mpipool.py" is contributed by Lisandro Dalcin, the author of mpi4py package. Please refer to the examples for the usage of the code.

Work on the YADE side
^^^^^^^^^^^^^^^^^^^^^^^^
The version of YADE should be at least rev3682 in which Bruno added the stringToScene function. Before installation, I added some functions to the source code (in "yade" subfolder). But only one function ("Shop::getStressAndTangent" in "./pkg/dem/Shop.cpp") is necessary for the FEMxDEM coupling, which returns the stress tensor and the tangent operator of a discrete packing. The former is homogenized using the Love's formula and the latter is homogenized as the elastic modulus. After installation and we get the executable file: yade-versionNo. We then generate a .py file linked to the executable file by "ln yade-versionNo yadeimport.py". This .py file will serve as a wrapped library of YADE. Later on, we will import all YADE functions into the python script through "from yadeimport import \*" (see simDEM.py file).

Open a python terminal. Make sure you can run ::

	import sys
	sys.path.append('where you put yadeimport.py')
	from yadeimport import *
	Omega().load('your initial RVE packing, e.g. 0.yade.gz')

If you are successful, you should also be able to run :: 
   
	from simDEM import *

Work on the Escript side
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
No particular requirement. But make sure the modules are callable in python, which means the main folder of Escript should be in your PYTHONPATH and LD_LIBRARY_PATH. The modules are wrapped as a class in msFEM\*.py.

Open a python terminal. Make sure you can run::

	from esys.escript import *
	from esys.escript.linearPDEs import LinearPDE
	from esys.finley import Rectangle

(Note: Escript is used for the current implementation. It can be replaced by any other FEM package provided with python bindings, e.g. FEniCS (http://fenicsproject.org). But the interface files "msFEM\*.py" need to be modified.)

Example tests
^^^^^^^^^^^^^^^^

After Steps 1 & 2, one should be able to run all the scripts for the multiscale analysis. The initial RVE packing (default name "0.yade.gz") should be provided by the user (e.g. using YADE to prepare a consolidated packing), which will be loaded by simDEM.py when the problem is initialized. The sample is initially uniform as long as the same RVE packing is assigned to all the Gauss points in the problem domain. It is also possible for the user to specify different RVEs at different Gauss points to generate an inherently inhomogeneous sample.

While simDEM.py is always required, only one msFEM\*.py is needed for a single test. For example, in a 2D (3D) dry test, msFEM2D.py (msFEM3D.py) is needed; similarly for a coupled hydro-mechanical problem (2D only, saturated), msFEMup.py is used which incorporates the u-p formulation. Multiprocessing is used by default. To try MPI parallelization, please set useMPI=True when constructing the problem in the main script. Example tests given in the "example" subfolder are listed below.
Note: The initial RVE packing (named 0.yade.gz by default) needs to be generated, e.g. using prepareRVE.py in "example" subfolder for a 2D packing (similarly for 3D).

#.	**2D drained biaxial compression test on dry dense sand** (biaxialSmooth.py)
	*Note*: Test description and result were presented in [Guo2014]_ and [Guo2014c]_.
#.	**2D passive failure under translational mode of dry sand retained by a rigid and frictionless wall** (retainingSmooth.py)
	*Note:* Rolling resistance model (CohFrictMat) is used in the RVE packing. Test description and result were presented in [Guo2015]_.
#.	**2D half domain footing settlement problem with mesh generated by Gmsh** (footing.py, footing.msh)
	*Note:* Rolling resistance model (CohFrictMat) is used in the RVE packing. Six-node triangle element is generated by Gmsh with three Gauss points each. Test description and result were presented in [Guo2015]_.
#.	**3D drained conventional triaxial compression test on dry dense sand using MPI parallelism** (triaxialRough.py)
	*Note 1:* The simulation is very time consuming. It costs ~4.5 days on one node using multiprocessing (16 processes, 2.0 GHz CPU). When useMPI is switched to True (as in the example script) and four nodes are used (80 processes, 2.2 GHz CPU), the simulation costs less than 24 hours. The speedup is about 4.4 in our test.
	*Note 2:* When MPI is used, mpi4py is required to be installed. The MPI implementation can be either MPICH or Open MPI. The file "mpipool.py" should also be placed in the main folder. Our test is based on openmpi-1.6.5. This is an on-going work. Test description and result will be presented later.
#.	**2D globally undrained biaxial compression test on saturated dense sand with changing permeability using MPI parallelism** (undrained.py)
	*Note:* This is an on-going work. Test description and result will be presented later.

Disclaim
^^^^^^^^^^^^
This work extensively utilizes and relies on some third-party packages as mentioned above. Their contributions are acknowledged. Feel free to use and redistribute the code. But there is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
