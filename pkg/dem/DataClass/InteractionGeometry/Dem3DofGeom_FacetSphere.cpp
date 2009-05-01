#include "Dem3DofGeom_FacetSphere.hpp"
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>
YADE_PLUGIN("Dem3DofGeom_FacetSphere","GLDraw_Dem3DofGeom_FacetSphere","ef2_Facet_Sphere_Dem3DofGeom");

CREATE_LOGGER(Dem3DofGeom_FacetSphere);
Dem3DofGeom_FacetSphere::~Dem3DofGeom_FacetSphere(){}

void Dem3DofGeom_FacetSphere::setTgPlanePts(const Vector3r& p1new, const Vector3r& p2new){
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
	cp1pt=se31.orientation.Conjugate()*(turnPlanePt(p1new,normal,se31.orientation*localFacetNormal)+contactPoint-se31.position);
	cp2rel=se32.orientation.Conjugate()*Dem3DofGeom_SphereSphere::rollPlanePtToSphere(p2new,effR2,-normal);
	TRVAR3(cp1pt,cp2rel,contPtInTgPlane2()-contPtInTgPlane1());	
}

void Dem3DofGeom_FacetSphere::relocateContactPoints(const Vector3r& p1, const Vector3r& p2){
	//TRVAR2(p2.Length(),effR2);
	if(p2.SquaredLength()>pow(effR2,2)){
		setTgPlanePts(Vector3r::ZERO,p2-p1);
	}
}

Real Dem3DofGeom_FacetSphere::slipToDisplacementTMax(Real displacementTMax){
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

CREATE_LOGGER(ef2_Facet_Sphere_Dem3DofGeom);
bool ef2_Facet_Sphere_Dem3DofGeom::go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c){
	InteractingFacet* facet=static_cast<InteractingFacet*>(cm1.get());
	Real sphereRadius=static_cast<InteractingSphere*>(cm2.get())->radius;
	// begin facet-local coordinates 
		Vector3r contactLine=se31.orientation.Conjugate()*(se32.position-se31.position);
		Vector3r normal=facet->nf;
		Real L=normal.Dot(contactLine); // height/depth of sphere's center from facet's plane
		if(L<0){normal*=-1; L*=-1;}
		if(L>sphereRadius && !c->isReal) return false; // sphere too far away from the plane

		Vector3r contactPt=contactLine-L*normal; // projection of sphere's center to facet's plane (preliminary contact point)
		const Vector3r* edgeNormals=facet->ne; // array[3] of edge normals (in facet plane)
		int edgeMax=0; Real distMax=edgeNormals[0].Dot(contactPt);
		for(int i=1; i<3; i++){
			Real dist=edgeNormals[i].Dot(contactPt);
			if(distMax<dist){edgeMax=i; distMax=dist;}
		}
		//TRVAR2(distMax,edgeMax);
		// OK, what's the logic here? Copying from IF2IS4SCG…
		Real sphereRReduced=shrinkFactor*sphereRadius;
		Real inCircleR=facet->icr-sphereRReduced;
		Real penetrationDepth;
		if(inCircleR<0){inCircleR=facet->icr; sphereRReduced=0;}
		if(distMax<inCircleR){// contact with facet's surface
			penetrationDepth=sphereRadius-L;	
			normal.Normalize();
		} else { // contact with the edge
			contactPt+=edgeNormals[edgeMax]*(inCircleR-distMax);
			bool noVertexContact=false;
			//TRVAR3(edgeNormals[edgeMax],inCircleR,distMax);
			// contact with vertex no. edgeMax
			// FIXME: this is the original version, but why (edgeMax-1)%3? IN that case, edgeNormal to edgeMax would never be tried
			//    if     (contactPt.Dot(edgeNormals[        (edgeMax-1)%3])>inCircleR) contactPt=facet->vu[edgeMax]*(facet->vl[edgeMax]-sphereRReduced);
			if     (contactPt.Dot(edgeNormals[        edgeMax      ])>inCircleR) contactPt=facet->vu[edgeMax]*(facet->vl[edgeMax]-sphereRReduced);
			// contact with vertex no. edgeMax+1
			else if(contactPt.Dot(edgeNormals[edgeMax=(edgeMax+1)%3])>inCircleR) contactPt=facet->vu[edgeMax]*(facet->vl[edgeMax]-sphereRReduced);
			// contact with edge no. edgeMax
			else noVertexContact=true;
			normal=contactLine-contactPt;
			#ifdef FACET_TOPO
				if(noVertexContact && facet->edgeAdjIds[edgeMax]!=Body::ID_NONE){
					// find angle between our normal and the facet's normal (still local coords)
					Quaternionr q; q.Align(facet->nf,normal); Vector3r axis; Real angle; q.ToAxisAngle(axis,angle);
					assert(angle>=0 && angle<=Mathr::PI);
					if(edgeNormals[edgeMax].Dot(axis)<0) angle*=-1.;
					bool negFace=normal.Dot(facet->nf)<0; // contact in on the negative facet's face
					Real halfAngle=(negFace?-1.:1.)*facet->edgeAdjHalfAngle[edgeMax]; 
					if(halfAngle<0 && angle>halfAngle) return false; // on concave boundary, and if in the other facet's sector, no contact
					// otherwise the contact will be created
				}
			#endif
			//TRVAR4(contactLine,contactPt,normal,normal.Length());
			//TRVAR3(se31.orientation*contactLine,se31.position+se31.orientation*contactPt,se31.orientation*normal);
			penetrationDepth=sphereRadius-normal.Normalize();
			//TRVAR1(penetrationDepth);
		}
	// end facet-local coordinates

	if(penetrationDepth<0 && !c->isReal) return false;

	shared_ptr<Dem3DofGeom_FacetSphere> fs;
	Vector3r normalGlob=se31.orientation*normal;
	if(c->interactionGeometry) fs=YADE_PTR_CAST<Dem3DofGeom_FacetSphere>(c->interactionGeometry);
	else {
		fs=shared_ptr<Dem3DofGeom_FacetSphere>(new Dem3DofGeom_FacetSphere());
		c->interactionGeometry=fs;
		fs->effR2=sphereRadius-penetrationDepth;
		fs->refR1=-1; fs->refR2=sphereRadius;
		fs->refLength=fs->effR2;
		fs->cp1pt=contactPt; // facet-local intial contact point
		fs->localFacetNormal=normal;
		fs->cp2rel.Align(Vector3r::UNIT_X,se32.orientation.Conjugate()*(-normalGlob)); // initial sphere-local center-contactPt orientation WRT +x
		fs->cp2rel.Normalize();
	}
	fs->se31=se31; fs->se32=se32;
	fs->normal=normalGlob;
	fs->contactPoint=se32.position+(-normalGlob)*(sphereRadius-penetrationDepth);
	if(c->isNew){
		TRVAR1(penetrationDepth);
		TRVAR3(fs->refLength,fs->cp1pt,fs->localFacetNormal);
		TRVAR3(fs->effR2,fs->cp2rel,fs->normal);
		TRVAR2(fs->se31.orientation,fs->se32.orientation);
		TRVAR2(fs->contPtInTgPlane1(),fs->contPtInTgPlane2());
	}
	return true;
}

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-opengl/GLUtils.hpp>

bool GLDraw_Dem3DofGeom_FacetSphere::normal=false;
bool GLDraw_Dem3DofGeom_FacetSphere::rolledPoints=false;
bool GLDraw_Dem3DofGeom_FacetSphere::unrolledPoints=false;
bool GLDraw_Dem3DofGeom_FacetSphere::shear=false;
bool GLDraw_Dem3DofGeom_FacetSphere::shearLabel=false;

void GLDraw_Dem3DofGeom_FacetSphere::go(const shared_ptr<InteractionGeometry>& ig, const shared_ptr<Interaction>& ip, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
	Dem3DofGeom_FacetSphere* fs = static_cast<Dem3DofGeom_FacetSphere*>(ig.get());
	//const Se3r& se31=b1->physicalParameters->dispSe3,se32=b2->physicalParameters->dispSe3;
	const Se3r& se31=b1->physicalParameters->se3,se32=b2->physicalParameters->se3;
	const Vector3r& pos1=se31.position; const Vector3r& pos2=se32.position;
	const Quaternionr& ori1=se31.orientation; const Quaternionr& ori2=se32.orientation;
	const Vector3r& contPt=fs->contactPoint;
	
	if(normal){
		GLUtils::GLDrawArrow(contPt,contPt+fs->refLength*fs->normal); // normal of the contact
	}
	// sphere center to point on the sphere
	if(rolledPoints){
		//cerr<<pos1<<" "<<pos1+ori1*fs->cp1pt<<" "<<contPt<<endl;
		GLUtils::GLDrawLine(pos1+ori1*fs->cp1pt,contPt,Vector3r(0,.5,1));
		GLUtils::GLDrawLine(pos2,pos2+(ori2*fs->cp2rel*Vector3r::UNIT_X*fs->effR2),Vector3r(0,1,.5));
	}
	// contact point to projected points
	if(unrolledPoints||shear){
		Vector3r ptTg1=fs->contPtInTgPlane1(), ptTg2=fs->contPtInTgPlane2();
		if(unrolledPoints){
			//TRVAR3(ptTg1,ptTg2,ss->normal)
			GLUtils::GLDrawLine(contPt,contPt+ptTg1,Vector3r(0,.5,1));
			GLUtils::GLDrawLine(contPt,contPt+ptTg2,Vector3r(0,1,.5)); GLUtils::GLDrawLine(pos2,contPt+ptTg2,Vector3r(0,1,.5));
		}
		if(shear){
			GLUtils::GLDrawLine(contPt+ptTg1,contPt+ptTg2,Vector3r(1,1,1));
			if(shearLabel) GLUtils::GLDrawNum(fs->displacementT().Length(),contPt,Vector3r(1,1,1));
		}
	}
}



