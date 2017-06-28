/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<pkg/dem/KinemSimpleShearBox.hpp>

class KinemCTDEngine : public KinemSimpleShearBox
{
	private :
		Real current_sigma		// Computed in kPa
			;

		int		temoin;

	public :
		virtual ~KinemCTDEngine() {};

		void action();
// 			;

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCTDEngine,KinemSimpleShearBox,
			"To compress a simple shear sample by moving the upper box in a vertical way only, so that the tangential displacement (defined by the horizontal gap between the upper and lower boxes) remains constant (thus, the CTD = Constant Tangential Displacement).\n \t The lateral boxes move also to keep always contact. All that until this box is submitted to a given stress (:yref:`targetSigma<KinemCTDEngine.targetSigma>`). Moreover saves are executed at each value of stresses stored in the vector :yref:`sigma_save<KinemCTDEngine.sigma_save>`, and at :yref:`targetSigma<KinemCTDEngine.targetSigma>`",
			((Real,compSpeed,0.0,,"(vertical) speed of the upper box : >0 for real compression, <0 for unloading [$m/s$]"))
			((std::vector<Real>,sigma_save,,,"vector with the values of sigma at which a save of the simulation should be performed [$kPa$]"))
			((Real,targetSigma,0.0,,"the value of sigma at which the compression should stop [$kPa$]")),
			temoin=0;
						)
	

};

REGISTER_SERIALIZABLE(KinemCTDEngine);

class KinemCNDEngine : public KinemSimpleShearBox
{
	private :
		int temoinfin;
	public :
		void action();

	protected :

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNDEngine,KinemSimpleShearBox,
			"To apply a Constant Normal Displacement (CND) shear for a parallelogram box\n\n \tThis engine, designed for simulations implying a simple shear box (:yref:`SimpleShear` Preprocessor or scripts/simpleShear.py), allows one to perform a constant normal displacement shear, by translating horizontally the upper plate, while the lateral ones rotate so that they always keep contact with the lower and upper walls.",
			((Real,shearSpeed,0.0,,"the speed at which the shear is performed : speed of the upper plate [m/s]"))
			((Real,gammalim,0.0,,"the value of the tangential displacement at wich the displacement is stopped [m]"))
			((Real,gamma,0.0,,"the current value of the tangential displacement"))
			((std::vector<Real>,gamma_save,,,"vector with the values of gamma at which a save of the simulation is performed [m]")),
			temoinfin=0;
						);
};

REGISTER_SERIALIZABLE(KinemCNDEngine);

class KinemCNLEngine : public KinemSimpleShearBox
{
	private :

		int	temoin,// utile pour savoir ou on en est
			it_stop
			;

	public :
		void	action()
			;

	
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNLEngine,KinemSimpleShearBox,
				 "To apply a constant normal stress shear (i.e. Constant Normal Load : CNL) for a parallelogram box (simple shear box : :yref:`SimpleShear` Preprocessor or scripts/simpleShear.py)\n\nThis engine allows one to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls.\n\nIn fact the upper plate can move not only horizontally but also vertically, so that the normal stress acting on it remains constant (this constant value is not chosen by the user but is the one that exists at the beginning of the simulation)\n\nThe right vertical displacements which will be allowed are computed from the rigidity Kn of the sample over the wall (so to cancel a deltaSigma, a normal dplt deltaSigma*S/(Kn) is set)\n\nThe movement is moreover controlled by the user via a :yref:`shearSpeed<KinemCNLEngine.shearSpeed>` which will be the speed of the upper wall, and by a maximum value of horizontal displacement :yref:`gammalim<KinemCNLEngine.gammalim>`, after which the shear stops.\n\n.. note::\n\tNot only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements.\n\n.. warning::\n\tBecause of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them 'by hand' in the .xml.\n",
				 ((Real,shearSpeed,0.0,,"the speed at wich the shearing is performed : speed of the upper plate [m/s]"))
				 ((Real,gammalim,0.0,,"the value of tangential displacement (of upper plate) at wich the shearing is stopped [m]"))
				 ((Real,gamma,0.0,,"current value of tangential displacement [m]"))
				 ((std::vector<Real>,gamma_save,,,"vector with the values of gamma at which a save of the simulation is performed [m]")),
				temoin=0;
				it_stop=0;
				 );
};

REGISTER_SERIALIZABLE(KinemCNLEngine);

class KinemCNSEngine : public KinemSimpleShearBox
{
	private :
		int	temoin,it_stop
			;
	public :
		void 	action()
			;
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCNSEngine,KinemSimpleShearBox,
		"To apply a Constant Normal Stifness (CNS) shear for a parallelogram box (simple shear)\n\nThis engine, useable in simulations implying one deformable parallelepipedic box, allows one to translate horizontally the upper plate while the lateral ones rotate so that they always keep contact with the lower and upper walls. The upper plate can move not only horizontally but also vertically, so that the normal rigidity defined by DeltaF(upper plate)/DeltaU(upper plate) = constant (= :yref:`KnC<KinemCNSEngine.KnC>` defined by the user).\n\nThe movement is moreover controlled by the user via a :yref:`shearSpeed<KinemCNLEngine.shearSpeed>` which is the horizontal speed of the upper wall, and by a maximum value of horizontal displacement :yref:`gammalim<KinemCNLEngine.gammalim>` (of the upper plate), after which the shear stops.\n\n.. note::\n\t not only the positions of walls are updated but also their speeds, which is all but useless considering the fact that in the contact laws these velocities of bodies are used to compute values of tangential relative displacements.\n\n.. warning::\n\tBut, because of this last point, if you want to use later saves of simulations executed with this Engine, but without that stopMovement was executed, your boxes will keep their speeds => you will have to cancel them by hand in the .xml",
		((Real,shearSpeed,0.0,,"the speed at wich the shearing is performed : speed of the upper plate [m/s]"))
		((Real,gammalim,0.0,,"the value of tangential displacement (of upper plate) at wich the shearing is stopped [m]"))
		((Real,gamma,0.0,,"current value of tangential displacement [m]"))
		((Real,KnC,10.0e6,,"the normal rigidity chosen by the user [MPa/mm] - the conversion in Pa/m will be made")),
		temoin=0;
		it_stop=0;
	);

};

REGISTER_SERIALIZABLE(KinemCNSEngine);
