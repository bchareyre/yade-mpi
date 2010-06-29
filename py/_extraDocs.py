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
