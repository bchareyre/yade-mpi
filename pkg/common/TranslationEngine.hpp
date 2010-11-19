/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib/base/Math.hpp>

class TranslationEngine : public PartialEngine {
	public:
		virtual void action();
		void postLoad(TranslationEngine&){ translationAxis.normalize(); } // should be called whenever translationAxis is changed

	YADE_CLASS_BASE_DOC_ATTRS(TranslationEngine,PartialEngine,"This engine is the base class for different engines, which require any kind of motion.",
		((Real,velocity,,,"Velocity [m/s]"))
		((Vector3r,translationAxis,,,"Direction [Vector3]"))
	);
};

class HarmonicMotionEngine : public TranslationEngine {
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(HarmonicMotionEngine,TranslationEngine,"This engine implements the harmonic oscillation of bodies. http://en.wikipedia.org/wiki/Simple_harmonic_motion#Dynamics_of_simple_harmonic_motion ; please, set dynamic=False for bodies, droven by this engine, otherwise amplitude will be 2x more, than awaited.",
		((Vector3r,A,Vector3r::Zero(),,"Amplitude [m]"))
		((Vector3r,f,Vector3r::Zero(),,"Frequency [hertz]"))
		((Vector3r,fi,Vector3r(Mathr::PI/2.0, Mathr::PI/2.0, Mathr::PI/2.0),,"Initial phase [radians]. By default, the body oscillates around initial position."))
	);
};

REGISTER_SERIALIZABLE(TranslationEngine);
REGISTER_SERIALIZABLE(HarmonicMotionEngine);


