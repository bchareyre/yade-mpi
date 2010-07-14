// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2007 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"Ig2_Sphere_Sphere_ScGeom.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/Scene.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/core/Omega.hpp>

#ifdef YADE_DEVIRT_FUNCTORS
bool Ig2_Sphere_Sphere_ScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw runtime_error("Do not call Ig2_Sphere_Sphere_ScGeom::go, use getStaticFunctorPtr and call that function instead."); }
bool Ig2_Sphere_Sphere_ScGeom::goStatic(InteractionGeometryFunctor* _self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	const Ig2_Sphere_Sphere_ScGeom* self=static_cast<Ig2_Sphere_Sphere_ScGeom*>(_self);
	const Real& interactionDetectionFactor=self->interactionDetectionFactor;
#else
bool Ig2_Sphere_Sphere_ScGeom::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
#endif
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;

	Sphere *s1=static_cast<Sphere*>(cm1.get()), *s2=static_cast<Sphere*>(cm2.get());
	Vector3r normal=(se32.position+shift2)-se31.position;
	Real penetrationDepthSq=pow(interactionDetectionFactor*(s1->radius+s2->radius),2) - normal.squaredNorm();
	if (penetrationDepthSq>0 || c->isReal() || force){
		shared_ptr<ScGeom> scm;
		bool isNew = !c->interactionGeometry;
		if(!isNew) scm=YADE_PTR_CAST<ScGeom>(c->interactionGeometry);
		else { scm=shared_ptr<ScGeom>(new ScGeom()); c->interactionGeometry=scm; }

		Real norm=normal.norm(); normal/=norm; // normal is unit vector now
		Real penetrationDepth=s1->radius+s2->radius-norm;
		scm->contactPoint=se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->penetrationDepth=penetrationDepth;
		scm->radius1=s1->radius;
		scm->radius2=s2->radius;
		scm->precompute(state1,state2,scene,c,normal,isNew,true);
		return true;
	}
	return false;
}


bool Ig2_Sphere_Sphere_ScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,state2,state1,-shift2,force,c);
}

YADE_PLUGIN((Ig2_Sphere_Sphere_ScGeom));
