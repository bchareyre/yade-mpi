/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/FunctorWrapper.hpp>

class TimingDeltas;
class Scene;

class Functor : public Serializable
{
	public: virtual vector<std::string> getFunctorTypes(){throw;}
	shared_ptr<TimingDeltas> timingDeltas;
	//! updated before every dispatch loop by the dispatcher; DO NOT ABUSE access to scene, except for getting global variables like scene->dt.
	Scene* scene;
	// label to be able to retrieve an engine unit by its label
	string label; 
	REGISTER_CLASS_AND_BASE(Functor,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(label));
};
REGISTER_SERIALIZABLE(Functor);



template
<
	class ReturnType,
	class AttributesType
>
class Functor1D : 	public Functor,
			public FunctorWrapper<ReturnType, AttributesType>
{
	public:
		#define FUNCTOR1D(type1) public: std::string get1DFunctorType1(void){return string(#type1);}
		virtual std::string get1DFunctorType1(void){throw runtime_error("Class "+this->getClassName()+" did not use FUNCTOR1D to declare its argument type?"); }
		virtual vector<string> getFunctorTypes(void){vector<string> ret; ret.push_back(get1DFunctorType1()); return ret;};
	REGISTER_CLASS_AND_BASE(Functor1D,Functor FunctorWrapper);
	/* do not REGISTER_ATTRIBUTES here, since we are template; derived classes should call REGISTER_ATTRIBUTES(Functor,(their)(own)(attributes)), bypassing Functor1D */
};


template
<
	class ReturnType,
	class AttributesType
>
class Functor2D :	public Functor,
			public FunctorWrapper<ReturnType, AttributesType>
{
	public:
		#define FUNCTOR2D(type1,type2) public: std::string get2DFunctorType1(void){return string(#type1);}; std::string get2DFunctorType2(void){return string(#type2);};
		virtual std::string get2DFunctorType1(void){throw runtime_error("Class "+this->getClassName()+" did not use FUNCTOR2D to declare its argument types?");}
		virtual std::string get2DFunctorType2(void){throw runtime_error("Class "+this->getClassName()+" did not use FUNCTOR2D to declare its argument types?");}
		virtual vector<string> getFunctorTypes(){vector<string> ret; ret.push_back(get2DFunctorType1()); ret.push_back(get2DFunctorType2()); return ret;};
	REGISTER_CLASS_AND_BASE(Functor2D,Functor FunctorWrapper);
	/* do not REGISTER_ATTRIBUTES here, since we are template; derived classes should call REGISTER_ATTRIBUTES(Functor,(their)(own)(attributes)), bypassing Functor2D */
};



