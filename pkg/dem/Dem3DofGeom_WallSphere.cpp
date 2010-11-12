// © 2009 Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/pkg/dem/Dem3DofGeom_WallSphere.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Wall.hpp>
YADE_PLUGIN((Dem3DofGeom_WallSphere)
	#ifdef YADE_OPENGL
		(Gl1_Dem3DofGeom_WallSphere)
	#endif	
		(Ig2_Wall_Sphere_Dem3DofGeom));

CREATE_LOGGER(Dem3DofGeom_WallSphere);
Dem3DofGeom_WallSphere::~Dem3DofGeom_WallSphere(){}

void Dem3DofGeom_WallSphere::setTgPlanePts(const Vector3r& p1new, const Vector3r& p2new){
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
	cp1pt=p1new+contactPoint-se31.position;
	cp2rel=se32.orientation.conjugate()*Dem3DofGeom_SphereSphere::rollPlanePtToSphere(p2new,effR2,-normal);
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
}

void Dem3DofGeom_WallSphere::relocateContactPoints(const Vector3r& p1, const Vector3r& p2){
	//TRVAR2(p2.norm(),effR2);
	if(p2.squaredNorm()>pow(effR2,2)){
		setTgPlanePts(Vector3r::Zero(),p2-p1);
	}
}

Real Dem3DofGeom_WallSphere::slipToDisplacementTMax(Real displacementTMax){
	//FIXME: not yet tested
	// negative or zero: reset shear
	if(displacementTMax<=0.){ setTgPlanePts(Vector3r(0,0,0),Vector3r(0,0,0)); return displacementTMax;}
	// otherwise
	Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
	Real currDistSq=(p2-p1).squaredNorm();
	if(currDistSq<pow(displacementTMax,2)) return 0; // close enough, no slip needed
	//Vector3r diff=.5*(sqrt(currDistSq)/displacementTMax-1)*(p2-p1); setTgPlanePts(p1+diff,p2-diff);
	Real scale=displacementTMax/sqrt(currDistSq); setTgPlanePts(scale*p1,scale*p2);
	return (displacementTMax/scale)*(1-scale);
}

Vector3r Dem3DofGeom_WallSphere::scaleDisplacementT(Real multiplier){
	assert(multiplier>=0 && multiplier<=1);
	Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2();
	Vector3r diff=.5*(multiplier-1)*(p2-p1);
	setTgPlanePts(p1-diff,p2+diff);
	return diff*2.0;
}

CREATE_LOGGER(Ig2_Wall_Sphere_Dem3DofGeom);
bool Ig2_Wall_Sphere_Dem3DofGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	Wall* wall=static_cast<Wall*>(cm1.get());
	Real sphereRadius=static_cast<Sphere*>(cm2.get())->radius;

	Real dist=(state2.pos+shift2)[wall->axis]-state1.pos[wall->axis];
	if(!c->isReal() && abs(dist)>sphereRadius && !force){ /*LOG_DEBUG("dist="<<dist<<", returning false");*/ return false; } // wall and sphere too far from each other

	// contact point is sphere center projected onto the wall
	Vector3r contPt=state2.pos; contPt[wall->axis]=state1.pos[wall->axis];
	Vector3r normalGlob(0.,0.,0.);
	// wall interacting from both sides: normal depends on sphere's position
	assert(wall->sense==-1 || wall->sense==0 || wall->sense==1);
	if(wall->sense==0) normalGlob[wall->axis]=dist>0?1.:-1.;
	else normalGlob[wall->axis]=wall->sense==1?1.:-1;

	shared_ptr<Dem3DofGeom_WallSphere> ws;
	if(c->geom) ws=YADE_PTR_CAST<Dem3DofGeom_WallSphere>(c->geom);
	else {
		ws=shared_ptr<Dem3DofGeom_WallSphere>(new Dem3DofGeom_WallSphere());
		c->geom=ws;
		ws->effR2=abs(dist);
		ws->refR1=-1; ws->refR2=sphereRadius;
		ws->refLength=ws->effR2;
		ws->cp1pt=contPt-state1.pos; // initial contact point relative to wall position (orientation is global, since it is coincident with local for a wall)
		ws->cp2rel=Quaternionr::Identity();
		ws->cp2rel.setFromTwoVectors(Vector3r::UnitX(),state2.ori.conjugate()*(-normalGlob)); // initial sphere-local center-contactPt orientation WRT +x
		ws->cp2rel.normalize();
		//LOG_INFO(ws->cp1pt);
	}
	ws->se31=state1.se3; ws->se32=state2.se3; ws->se32.position+=shift2;
	ws->contactPoint=contPt;
	ws->normal=normalGlob;
	return true;
}
#ifdef YADE_OPENGL

	#include<yade/lib/opengl/OpenGLWrapper.hpp>
	#include<yade/lib/opengl/GLUtils.hpp>

	bool Gl1_Dem3DofGeom_WallSphere::normal=false;
	bool Gl1_Dem3DofGeom_WallSphere::rolledPoints=false;
	bool Gl1_Dem3DofGeom_WallSphere::unrolledPoints=false;
	bool Gl1_Dem3DofGeom_WallSphere::shear=false;
	bool Gl1_Dem3DofGeom_WallSphere::shearLabel=false;

	void Gl1_Dem3DofGeom_WallSphere::go(const shared_ptr<IGeom>& ig, const shared_ptr<Interaction>& ip, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		Dem3DofGeom_WallSphere* ws=static_cast<Dem3DofGeom_WallSphere*>(ig.get());
		const Se3r& se31=b1->state->se3,se32=b2->state->se3;
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
			GLUtils::GLDrawLine(pos2,pos2+(ori2*ws->cp2rel*Vector3r::UnitX()*ws->effR2),Vector3r(0,1,.5));
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
				if(shearLabel) GLUtils::GLDrawNum(ws->displacementT().norm(),contPt,Vector3r(1,1,1));
			}
		}
	}

#endif
