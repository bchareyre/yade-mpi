// © 2009 Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/pkg-dem/Dem3DofGeom_WallSphere.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/Wall.hpp>
YADE_PLUGIN((Dem3DofGeom_WallSphere)
	#ifdef YADE_OPENGL
		(Gl1_Dem3DofGeom_WallSphere)
	#endif	
		(ef2_Wall_Sphere_Dem3DofGeom));

CREATE_LOGGER(Dem3DofGeom_WallSphere);
Dem3DofGeom_WallSphere::~Dem3DofGeom_WallSphere(){}

void Dem3DofGeom_WallSphere::setTgPlanePts(const Vector3r& p1new, const Vector3r& p2new){
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
	cp1pt=p1new+contactPoint-se31.position;
	cp2rel=se32.orientation.Conjugate()*Dem3DofGeom_SphereSphere::rollPlanePtToSphere(p2new,effR2,-normal);
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
}

void Dem3DofGeom_WallSphere::relocateContactPoints(const Vector3r& p1, const Vector3r& p2){
	//TRVAR2(p2.Length(),effR2);
	if(p2.SquaredLength()>pow(effR2,2)){
		setTgPlanePts(Vector3r::ZERO,p2-p1);
	}
}

Real Dem3DofGeom_WallSphere::slipToDisplacementTMax(Real displacementTMax){
	//FIXME: not yet tested
	// negative or zero: reset shear
	if(displacementTMax<=0.){ setTgPlanePts(Vector3r(0,0,0),Vector3r(0,0,0)); return displacementTMax;}
	// otherwise
	Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
	Real currDistSq=(p2-p1).SquaredLength();
	if(currDistSq<pow(displacementTMax,2)) return 0; // close enough, no slip needed
	//Vector3r diff=.5*(sqrt(currDistSq)/displacementTMax-1)*(p2-p1); setTgPlanePts(p1+diff,p2-diff);
	Real scale=displacementTMax/sqrt(currDistSq); setTgPlanePts(scale*p1,scale*p2);
	return (displacementTMax/scale)*(1-scale);
}

CREATE_LOGGER(ef2_Wall_Sphere_Dem3DofGeom);
bool ef2_Wall_Sphere_Dem3DofGeom::go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c){
	Wall* wall=static_cast<Wall*>(cm1.get());
	Real sphereRadius=static_cast<InteractingSphere*>(cm2.get())->radius;

	Real dist=se32.position[wall->axis]-se31.position[wall->axis];
	if(!c->isReal() && abs(dist)>sphereRadius){ /*LOG_DEBUG("dist="<<dist<<", returning false");*/ return false; } // wall and sphere too far from each other

	// contact point is sphere center projected onto the wall
	Vector3r contPt=se32.position; contPt[wall->axis]=se31.position[wall->axis];
	Vector3r normalGlob(0.,0.,0.);
	// wall interacting from both sides: normal depends on sphere's position
	assert(wall->sense==-1 || wall->sense==0 || wall->sense==1);
	if(wall->sense==0) normalGlob[wall->axis]=dist>0?1.:-1.;
	else normalGlob[wall->axis]=wall->sense==1?1.:-1;

	shared_ptr<Dem3DofGeom_WallSphere> ws;
	if(c->interactionGeometry) ws=YADE_PTR_CAST<Dem3DofGeom_WallSphere>(c->interactionGeometry);
	else {
		ws=shared_ptr<Dem3DofGeom_WallSphere>(new Dem3DofGeom_WallSphere());
		c->interactionGeometry=ws;
		ws->effR2=abs(dist);
		ws->refR1=-1; ws->refR2=sphereRadius;
		ws->refLength=ws->effR2;
		ws->cp1pt=contPt-se31.position; // initial contact point relative to wall position (orientation is global, since it is coincident with local for a wall)
		ws->cp2rel=Quaternionr::IDENTITY;
		ws->cp2rel.Align(Vector3r::UNIT_X,se32.orientation.Conjugate()*(-normalGlob)); // initial sphere-local center-contactPt orientation WRT +x
		ws->cp2rel.Normalize();
		//LOG_INFO(ws->cp1pt);
	}
	ws->se31=se31; ws->se32=se32;
	ws->contactPoint=contPt;
	ws->normal=normalGlob;
	return true;
}
#ifdef YADE_OPENGL

	#include<yade/lib-opengl/OpenGLWrapper.hpp>
	#include<yade/lib-opengl/GLUtils.hpp>

	bool Gl1_Dem3DofGeom_WallSphere::normal=false;
	bool Gl1_Dem3DofGeom_WallSphere::rolledPoints=false;
	bool Gl1_Dem3DofGeom_WallSphere::unrolledPoints=false;
	bool Gl1_Dem3DofGeom_WallSphere::shear=false;
	bool Gl1_Dem3DofGeom_WallSphere::shearLabel=false;

	void Gl1_Dem3DofGeom_WallSphere::go(const shared_ptr<InteractionGeometry>& ig, const shared_ptr<Interaction>& ip, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		Dem3DofGeom_WallSphere* ws=static_cast<Dem3DofGeom_WallSphere*>(ig.get());
		const Se3r& se31=b1->physicalParameters->se3,se32=b2->physicalParameters->se3;
		const Vector3r& pos1=se31.position; const Vector3r& pos2=se32.position;
		//const Quaternionr& ori1=se31.orientation;
		const Quaternionr& ori2=se32.orientation;
		const Vector3r& contPt=ws->contactPoint;
		
		if(normal){
			GLUtils::GLDrawArrow(contPt,contPt+ws->refLength*ws->normal); // normal of the contact
		}
		// sphere center to point on the sphere
		if(rolledPoints){
			GLUtils::GLDrawLine(pos1+ws->cp1pt,contPt,Vector3r(0,.5,1));
			GLUtils::GLDrawLine(pos2,pos2+(ori2*ws->cp2rel*Vector3r::UNIT_X*ws->effR2),Vector3r(0,1,.5));
		}
		// contact point to projected points
		if(unrolledPoints||shear){
			Vector3r ptTg1=ws->contPtInTgPlane1(), ptTg2=ws->contPtInTgPlane2();
			if(unrolledPoints){
				GLUtils::GLDrawLine(contPt,contPt+ptTg1,Vector3r(0,.5,1));
				GLUtils::GLDrawLine(contPt,contPt+ptTg2,Vector3r(0,1,.5)); GLUtils::GLDrawLine(pos2,contPt+ptTg2,Vector3r(0,1,.5));
			}
			if(shear){
				GLUtils::GLDrawLine(contPt+ptTg1,contPt+ptTg2,Vector3r(1,1,1));
				if(shearLabel) GLUtils::GLDrawNum(ws->displacementT().Length(),contPt,Vector3r(1,1,1));
			}
		}
	}

#endif

YADE_REQUIRE_FEATURE(PHYSPAR);

