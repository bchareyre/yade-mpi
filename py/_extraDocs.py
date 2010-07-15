# encoding: utf-8
# 2010 © Václav Šmilauer <eudoxos@arcig.cz>
#
# This module is imported at startup. It is meant to update
# docstrings of wrapper classes, which are not practical to document
# in the c++ source itself, due to the necessity of writing
# \n for newlines and having everything as "string".
#
# PLEASE:
#
# 1. provide at least brief description of the class
#    in the c++ code (for those who read it) and 
#
# 2. Add something like
#
#    "Full documentation of this class is in py/_extraDocs.py."
#
#    to the c++ documentation.

import wrapper

# Update docstring of your class/function like this:
#
#	wrapper.YourClass.__doc__="""
#		This class is documented from _extraDocs.py. Yay!
#
#		.. note::
#			The c++ documentation will be overwritten by this string.
#	"""


wrapper.Peri3dController.__doc__=r'''
Class for controlling independently strain and stress tensor components of the periodic cell. :yref:`goal<Peri3dController.goal>` are the goal values, while :yref:`stressMask<Peri3dController.stressMask>` determines which components prescribe stress and which prescribe strain.

Current stiffness and stress tensors are update at every step using formulas presented in [Kuhl2001]_:

.. math::
	:nowrap:

	\begin{align*}
		\tens{\sig}&=\frac{1}{V}\sum_c l^c\left[\tens{N}^c F_N^c+{\tens{T}^c}^T \otimes F_T^c \right] & (34) \\
		\tens{K}&=\frac{1}{V}\sum_c {l^c}^2\left[K_N^c \tens{N}^c\otimes\tens{N}^c+K_T^c{\tens{T}^c}^T\otimes\tens{T}^c\right] & (35)
	\end{align*}

where $c$ traverses all interactions within the :yref:`Cell` with volume $V$, $l$ the current distance between particle centers within the interaction, $F_N$ the normal force magnitude, $\vec{F_T}$ the shear force (in global coordinates), $K_N$ and $K_T$ are (current) normal and shear contact stiffnesses [N/m].

The projection tensors $\tens{N}$ and $\tens{T}$ are ($n$ is the interaction unit normal, $\delta$ is the Kronecker delta)

.. math::
	:nowrap:

	\begin{align*}
		\tens{N}&=\vec{n}\otimes\vec{n} \\
		\tens{N}\otimes\tens{N}&=\vec{n}\otimes\vec{n}\otimes\vec{n}\otimes\vec{n}=n_i n_j n_k n_l \\
		\tens{T}&=\vec{n}\otimes\tens{I}_{\rm sym}-\vec{n}\otimes\vec{n}\otimes\vec{n}\\
		T_{ijk}&=n_l\frac{1}{2}(\delta_{lj}\delta_{ik}+\delta_{lk}\delta_{ij})-n_i n_j n_k=n_j\delta_{ik}/2+n_k\delta_{ij}/2-n_i n_j n_k \\
		\tens{T}^T&=\tens{I}_{\rm sym}\otimes\vec{n}-\vec{n}\otimes\vec{n}\otimes\vec{n}\\
		T_{ijk}^T&=\frac{1}{2}(\delta_{ik}\delta_{jl}+\delta_{il}\delta_{jk})n_l-n_i n_j n_k=\delta_{ik}n_j/2+n_i\delta_{jk}/2-n_i n_j n_k\\
		\tens{T}^T\otimes\tens{T}=T^T_{ijm}T_{mkl}&=(\delta_{im}n_j/2+n_i\delta_{jm}/2-n_i n_j n_m)(n_k\delta_{ml}/2+n_l\delta_{mk}/2-n_m n_k n_l)= \\
			&=n_jn_k\delta_{im}\delta_{ml}/4+n_jn_l\delta_{im}\delta_{mk}/4-n_jn_kn_ln_m\delta_{im}/2+ \\
			&\quad+n_in_k\delta_{jm}\delta_{ml}/4+n_in_l\delta_{jm}\delta_{mk}/4-n_in_kn_ln_m\delta_{jm}/2-\\
			&\quad-n_jn_kn_ln_m\delta_{im}/2-n_in_kn_ln_m\delta_{jm}/2+n_in_jn_kn_ln_mn_m= \\
			&=n_jn_k\delta_{il}/4+n_jn_l\delta_{ik}/4+n_in_k\delta_{jl}/4+n_in_l\delta_{jk}/4- \\
			&\quad-n_in_jn_kn_l/2-n_in_jn_kn_l/2-n_in_jn_kn_l/2-n_in_jn_kn_l/2+n_in_jn_kn_ln_mn_m= \\
			&=n_jn_k\delta_{il}/4+n_jn_l\delta_{ik}/4+n_in_k\delta_{jl}/4+n_in_l\delta_{jk}/4-2n_in_jn_kn_l+n_in_jn_kn_ln_mn_m \\
			&=n_jn_k\delta_{il}/4+n_jn_l\delta_{ik}/4+n_in_k\delta_{jl}/4+n_in_l\delta_{jk}/4-n_in_jn_kn_l \quad\hbox{(using \$n_m n_m=1\$)} %%% \$ must be escaped, since it would be dumbly replaced by :math:`...`
	\end{align*}

The global elastic stiffness tensor then reads (omitting the $c$ indices of per-contact symbols in the summation for readability)

.. math::
	
	K_{ijkl}=\frac{1}{V}\sum_{c}{l}^2\left[K_N n_i n_j n_k n_l+K_T\left(\frac{n_j n_k \delta_{il}+n_j n_l\delta_{ik}+n_i n_k\delta_{jl}+n_i n_l\delta_{jk}}{4}-n_i n_j n_kn_l\right)\right].

Global strain tensor $\tens{\eps}$ is symmetric and has 6 degrees of freedom, whereas the current transformation matrix $\mathcal{T}$ of the cell (:yref:`Cell.trsf`) has 9 independent components -- it has 3 additional for arbitrary rotation, which induces no strain. We perform `polar decomposition <http://en.wikipedia.org/wiki/Polar_decomposition#Matrix_polar_decomposition>`_ $\mathcal{T}=\mat{U}\mat{P}$, where $\mat{U}$ is unitary (representing rotation) and $\mat{P}$ is deformation in the proper sense; after converting $\mat{P}$ to strain, it is rotated back by $\mat{U}$ so that it is in global coordinates. To have `true strain <http://en.wikipedia.org/wiki/True_strain#True_strain>`_, the strain matrix is obtained via

.. math:: \mat{\eps}=\mat{U}\log \mat{P}

computed (see `wikipedia <http://en.wikipedia.org/wiki/Logarithm_of_a_matrix#Calculating_the_logarithm_of_a_diagonalizable_matrix>`_ and [Lu1998]_) by spectral decomposition

.. math::
	:nowrap:
	
	\begin{align*}
		\mat{\eps}'&=\mat{V}^{-1}\mat{\eps}\mat{V} \\
		\log(\mat{\eps})&=\mat{V}\log(\mat{\eps}')\mat{V}^{-1}
	\end{align*}

where $\mat{\eps}'$ is diagonal and $\log(\mat{\eps}')$ is a diagonal matrix with $\log(\mat{\eps}')_{ii}=\log(\mat{\eps}'_{ii})$. $\mat{V}$ is orthonormal, therefore $\mat{V}^{-1}=\mat{V}^T$.

Once current values of $\tens{\sigma}$, $\tens{K}$ and $\tens{\eps}$ are known, the control algorithm comes. These tensors are converted to 6-vectors using the `Voigt notation <http://en.wikiversity.org/wiki/Introduction_to_Elasticity/Constitutive_relations#Voigt_notation>`_. Since for each component $i\in\{0\dots5\}$ either $\vec{\sig}_i$ of $\vec{\eps}_i$ is prescribed (determined by :yref:`stressMask<Peri3dController.stressMask>`), the equation $\vec{\sig}=\mat{K}\vec{\eps}$; assuming large strains, we take the rate form $\vec{\dot\sig}=\mat{K}\vec{\dot\eps}$ is decomposed in 2 parts:

* $p$ denotes the part where strain component $\vec{\dot\eps}_i$ is *prescribed* (via $\frac{\vec{\eps}^g_i-\vec{\eps}_i}{\Dt}$, where $\vec{\eps}^g_i$ is the corresponding :yref:`goal<Peri3dController.goal>` strain component value);
* $u$ denotes the part where strain component $\vec{\dot\eps}_i$ is *unprescribed* (and $\vec{\sig}_i$ is prescribed, again via $\frac{\vec{\sig}^g_i-\vec{\sig}_i}{\Dt}$).

The equation then is re-arranged as

.. math:: 

	\begin{Bmatrix}\vec{\dot\sig}_u \\ \vec{\dot\sig}_p\end{Bmatrix}=\begin{bmatrix}\mat{K}_{uu} & \mat{K}_{up} \\ \mat{K}_{pu} & \mat{K}_{pp}\end{bmatrix}\begin{Bmatrix}\vec{\dot\eps}_u \\ \vec{\dot\eps}_p\end{Bmatrix}

Since we can only apply strain (via the :yref:`velocity gradient<Cell.velGrad>`), we must evaluate 

.. math :: \vec{\dot\eps}_u=\mat{K}_{uu}^{-1}(\vec{\dot\sig}_u-\mat{K}_{up}\vec{\dot\eps}_p)

The :yref:`velocity gradient <Cell.velGrad>` is then assembled from $\vec{\eps}_u$ and $\vec{\eps}_p$. If magnitude of any component of $\nabla\vec{v}$ exceeds :yref:`maxStrainRate<Peri3dController.maxStrainRate>`, the whole tensor is scaled accordingly.

'''


wrapper.TriaxialTest.__doc__='''
Create a scene for triaxal test.

**Introduction**
	Yade includes tools to simulate triaxial tests on particles assemblies. This pre-processor (and variants like e.g. :yref:`CapillaryTriaxialTest`) illustrate how to use them. It generates a scene which will - by default - go through the following steps :

	* generate random loose packings in a parallelepiped.
	* compress the packing isotropicaly, either squeezing the packing between moving rigid boxes or expanding the particles while boxes are fixed (depending on flag :yref:`internalCompaction<TriaxialTest.internalCompaction>`). The confining pressure in this stage is defined via :yref:`sigmaIsoCompaction<TriaxialTest.sigmaIsoCompaction>`.
	* when the packing is dense and stable, simulate a loading path and get the mechanical response as a result. 

	The default loading path corresponds to a constant lateral stress (:yref:`sigmaLateralConfinement<TriaxialTest.sigmaLateralConfinement>`) in 2 directions and constant strain rate on the third direction. This default loading path is performed when the flag :yref:`autoCompressionActivation<TriaxialTest.autoCompressionActivation>` it ``True``, otherwise the simulation stops after isotropic compression.

	Different loading paths might be performed. In order to define them, the user can modify the flags found in engine :yref:`TriaxialStressController` at any point in the simulation (in c++). If ``TriaxialStressController.wall_X_activated`` is ``true`` boundary X is moved automatically to maintain the defined stress level *sigmaN* (see axis conventions below). If ``false`` the boundary is not controlled by the engine at all. In that case the user is free to prescribe fixed position, constant velocity, or more complex conditions.

	.. note:: *Axis conventions.* Boundaries perpendicular to the *x* axis are called "left" and "right", *y* corresponds to "top" and "bottom", and axis *z* to "front" and "back". In the default loading path, strain rate is assigned along *y*, and constant stresses are assigned on *x* and *z*.

**Essential engines**
	#. The :yref:`TrixaialCompressionEngine` is used for controlling the state of the sample and simulating loading paths. :yref:`TriaxialCompressionEngine` inherits from :yref:`TriaxialStressController`, which computes stress- and strain-like quantities in the packing and maintain a constant level of stress at each boundary. :yref:`TriaxialCompressionEngine` has few more members in order to impose constant strain rate and control the transition between isotropic compression and triaxial test. Transitions are defined by changing some flags of the :yref:`TriaxialStressController`, switching from/to imposed strain rate to/from imposed stress.
	#. The class :yref:`TriaxialStateRecorder` is used to write to a file the history of stresses and strains.
	#. :yref:`TriaxialTest` is using :yref:`GlobalStiffnessTimeStepper` to compute an appropriate $\Dt$ for the numerical scheme. 

	.. note:: ``TriaxialStressController::ComputeUnbalancedForce`` returns a value that can be useful for evaluating the stability of the packing. It is defined as (mean force on particles)/(mean contact force), so that it tends to 0 in a stable packing. This parameter is checked by :yref:`TriaxialCompressionEngine` to switch from one stage of the simulation to the next one (e.g. stop isotropic confinment and start axial loading)

.. admonition:: Frequently Asked Questions

	#. How is generated the packing? How to change particles sizes distribution? Why do I have a message "Exceeded 3000 tries to insert non-overlapping sphere?
		The initial positioning of spheres is done by generating random (x,y,z) in a box and checking if a sphere of radius R (R also randomly generated with respect to a uniform distribution between mean*(1-std_dev) and mean*(1+std_dev) can be inserted at this location without overlaping with others.

		If the sphere overlaps, new (x,y,z)'s are generated until a free position for the new sphere is found. This explains the message you have: after 3000 trial-and-error, the sphere couldn't be placed, and the algorithm stops.
		
		You get the message above if you try to generate an initialy dense packing, which is not possible with this algorithm. It can only generate clouds. You should keep the default value of porosity (n~0.7), or even increase if it is still to low in some cases. The dense state will be obtained in the second step (compaction, see below).
	
	#. How is the compaction done, what are the parameters :yref:`maxWallVelocity<TriaxialTest.maxWallVelocity>` and :yref:`finalMaxMultiplier<TriaxialTest.finalMaxMultiplier>`?
		Compaction is done
			#. by moving rigid boxes or
			#. by increasing the sizes of the particles (decided using the option :yref:`internalCompaction<TriaxialTest.internalCompaction>` ⇒ size increase).
		Both algorithm needs numerical parameters to prevent instabilities. For instance, with the method (1) :yref:`maxWallVelocity<TriaxialTest.maxWallVelocity>` is the maximum wall velocity, with method (2) :yref:`finalMaxMultiplier<TriaxialTest.finalMaxMultiplier>` is the max value of the multiplier applied on sizes at each iteration (always something like 1.00001).

	#. During the simulation of triaxial compression test, the wall in one direction moves with an increment of strain while the stresses in other two directions are adjusted to :yref:`sigma_iso<TriaxialStressController.sigma_iso>`. How the stresses in other directions are maintained constant to :yref:`sigma_iso<TriaxialStressController.sigma_iso>`? What is the mechanism? Where is it implemented in Yade?
		The control of stress on a boundary is based on the total stiffness *K* of all contacts between the packing and this boundary. In short, at each step, displacement=stress_error/K. This algorithm is implemented in :yref:`TriaxialStressController`, and the control itself is in ``TriaxialStressController::ControlExternalStress``. The control can be turned off independently for each boundary, using the flags ``wall_XXX_activated``, with *XXX*\ ∈{*top*, *bottom*, *left*, *right*, *back*, *front*}. The imposed sress is a unique value (:yref:`sigma_iso<TriaxialStressController.sigma_iso>`) for all directions if :yref:`TriaxialStressController.isAxisymetric`, or 3 independent values :yref:`sigma1<TriaxialStressController.sigma1>`, :yref:`sigma2<TriaxialStressController.sigma2>`, :yref:`sigma3<TriaxialStressController.sigma3>`.

	#. Which value of friction angle do you use during the compaction phase of the Triaxial Test?
		The friction during the compaction (whether you are using the expansion method or the compression one for the specimen generation) can be anything between 0 and the final value used during the Triaxial phase. Note that higher friction than the final one would result in volumetric collapse at the beginning of the test. The purpose of using a different value of friction during this phase is related to the fact that the final porosity you get at the end of the sample generation essentially depends on it as well as on the assumed Particle Size Distribution. Changing the initial value of friction will get to a different value of the final porosity.

	#. Which is the aim of the ``bool isRadiusControlIteration``?
		This internal variable (updated automatically) is true each *N* timesteps (with *N*\ =\ :yref:`radiusControlInterval<TriaxialTest.radiusControlInterval>`). For other timesteps, there is no expansion. Cycling without expanding is just a way to speed up the simulation, based on the idea that 1% increase each 10 iterations needs less operations than 0.1% at each iteration, but will give similar results.

	#. How comes the unbalanced force reaches a low value only after many timesteps in the compaction phase?
		The value of unbalanced force (dimensionless) is expected to reach low value (i.e. identifying a static-equilibrium condition for the specimen) only at the end of the compaction phase. The code is not aiming at simulating a quasistatic isotropic compaction process, it is only giving a stable packing at the end of it.
'''
