This folder illustrates the use of Law2_ScGeom_CapillaryPhys_Capillarity engine to describe capillary interaction between particle pairs in YADE (see also the wiki page https://www.yade-dem.org/wiki/CapillaryTriaxialTest)


I. Simulation scripts examples
------------------------------

Two examples of simulations using Law2_ScGeom_CapillaryPhys_Capillarity are herein provided:

- CapillaryPhys-example.py illustrates the mutual attraction due to capillary interaction in a simple packing
- capillaryBridge.py defines and let evolve a capillary bridge between two particles only


II. Capillary files scripts
---------------------------

As explained on the wiki page (see above) and in Law2_ScGeom_CapillaryPhys_Capillarity doc, so-called capillary files are required in order to use Law2_ScGeom_CapillaryPhys_Capillarity engine. These capillary files include capillary bridges configurations data (for positiv capillary pressure values). A version of capillary files can be downloaded from the wiki page, they consider a zero contact (wetting) angle and some given particle radius ratio.

Also, three .m files are herein provided in order to enable users to build their own capillary files for any contact angle or particle radius ratio, or to study capillary bridges as such (solveLiqBridge.m). A capillary files generation requires launching writesCapFile() (in writesCapFile.m) only, after providing a couple of parameters therein (see description of writesCapFile.m below). The two other files (solveLiqBridge.m and solveLaplace_uc.m) define functions used by writesCapFile() and do not require any user input. Specifically, the roles of the three .m files are as follows:


- solveLiqBridge.m solves the Laplace-Young equation for one given bridge, defined in terms of the input attributes of the solveLiqBridge function (see therein). The solveLiqBridge function is usually called by other files (see below) during capillary files generation, however it can also be executed on its own in order to study (e.g. plot) capillary bridge profile.

Code comments include references to:
 * Duriez2016: J. Duriez and R. Wan, Contact angle mechanical influence for wet granular soils, currently under Review in Acta Geotechnica
 * Lian1993: G. Lian and C. Thornton and M. J. Adams, A Theoretical Study of the Liquid Bridge Forces between Two Rigid Spherical Bodies, Journal of Colloid and Interface Science, 161(1), 1993
 * Scholtes2008 (french): L. Scholtes, Modelisation Micro-Mecanique des Milieux Granulaires Partiellement Satures, PhD Thesis from Institut polytechnique de Grenoble, 2008
 * Soulie2005 (french): F. Soulie, Cohesion par capillarite et comportement mecanique de milieux granulaires, PhD Thesis from Universite Montpellier II, 2005
 * Soulie2006: F. Soulie and F. Cherblanc and M. S. El Youssoufi and C. Saix, Influence of liquid bridges on the mechanical behaviour of polydisperse granular materials, International Journal for Numerical and Analytical Methods in Geomechanics, 30(3), 2006


- solveLaplace_uc.m calls several times solveLiqBridge in order to compute all possible bridges configurations for given contact angle, capillary pressure, and particle radius ratio. It is usually called by writesCapFile (see below) during capillary files generation, however it can also be executed on its own. In particular, it may output text files including bridges data for one given capillary pressure.


- writesCapFile.m, finally, is the conductor during the capillary files generation, and is the only one that actually requires the user's attention for such task. Parameters are to be defined by the user directly in the .m file (from l. 10 to 30) before launching.
  Generating a complete set of 10 capillary files typically requires few days in terms of computation time with MATLAB and much (~ 7*) more with Octave. The .m files makes wide use of iterative procedures and a greater performance for MATLAB is indeed expected. In case Octave is still chosen for generation, you may want to uncomment some Octave specific command ('fflush(..)') in writesCapFile.m in order for the terminal to update you on the progress.
