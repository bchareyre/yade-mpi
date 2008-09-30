// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2007 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Omega.hpp>


InteractingSphere2InteractingSphere4SpheresContactGeometry::InteractingSphere2InteractingSphere4SpheresContactGeometry()
{
	interactionDetectionFactor = 1;
	hasShear=false;
}

void InteractingSphere2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
	REGISTER_ATTRIBUTE(interactionDetectionFactor);
	REGISTER_ATTRIBUTE(hasShear);
}

bool InteractingSphere2InteractingSphere4SpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingSphere *s1=static_cast<InteractingSphere*>(cm1.get()), *s2=static_cast<InteractingSphere*>(cm2.get());
	Vector3r normal=se32.position-se31.position;
	Real penetrationDepthSq=pow(interactionDetectionFactor*(s1->radius+s2->radius),2) - normal.SquaredLength();
	if (penetrationDepthSq>0 || c->isReal){
		shared_ptr<SpheresContactGeometry> scm;
		if(c->interactionGeometry) scm=YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
		else { scm=shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry()); c->interactionGeometry=scm; }
		//cerr<<"Assigned scm "<<scm.get()<<"=="<<c->interactionGeometry.get()<<endl;

		Real penetrationDepth=s1->radius+s2->radius-normal.Normalize(); /* Normalize() works in-place and returns length before normalization; from here, normal is unit vector */
		scm->contactPoint=se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal=normal;
		scm->penetrationDepth=penetrationDepth;
		scm->radius1=s1->radius;
		scm->radius2=s2->radius;
		if(hasShear){
			scm->hasShear=true;
			scm->pos1=se31.position; scm->pos2=se32.position;
			scm->ori1=se31.orientation; scm->ori2=se32.orientation;
			if(c->isNew){
				// contact constants
				scm->d0=(se32.position-se31.position).Length();
				scm->d1=s1->radius-penetrationDepth; scm->d2=s2->radius-penetrationDepth;
				// quasi-constants
				scm->cp1rel.Align(Vector3r::UNIT_X,se31.orientation.Conjugate()*normal);
				scm->cp2rel.Align(Vector3r::UNIT_X,se32.orientation.Conjugate()*(-normal));
				scm->cp1rel.Normalize(); scm->cp2rel.Normalize();
			}
			// testing only
			#if 0
			if((Omega::instance().getCurrentIteration()%10000)==0) scm->relocateContactPoints();
			if((Omega::instance().getCurrentIteration()%10000)==0) {
				Real slip=scm->slipToEpsNMax(1.); if(slip>0) cerr<<"SLIP by Δε_N "<<slip<<" → ε_N="<<scm->epsN()<<endl;
			}
			#endif
		}
		return true;
	}
	return false;
}


bool InteractingSphere2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();
