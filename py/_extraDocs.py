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
Class for controlling independently strain and stress tensor components of the periodic cell.

:yref:`goal<Peri3dController.goal>` are the goal values, while :yref:`stressMask<Peri3dController.stressMask` determines which components prescribe stress and which prescribe strain.

Current stiffness and stress tensors are update at every step using formulas presented in [Kuhl2001]_:

.. math::

	\begin{align}
		\tens{K}&=\frac{1}{V}\sum_c {d_0^c}^2\left[K_N^c \vec{N}^c\otimes\vec{N}^c+K_T^c{\vec{T}^c}^T\cdot\vec{T}^c












'''
