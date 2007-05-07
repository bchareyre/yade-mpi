// © 2007 Václav Šmilauer <eudoxos@arcig.cz>

#include"Tetra.hpp"

char* yadePluginClasses[]={
	// self-contained in hpp:
	"TetraMold",
	"TetraBang", 
	"Tetrahedron2TetraMold",
	"TetraAABB", 
	// some code in cpp (this file):
	"TetraLaw",	 
	"Tetra2TetraBang",
	"TetraDraw",
	NULL /*sentinel*/
};

#include <boost/shared_ptr.hpp>

#include <yade/core/Interaction.hpp>
#include <yade/core/PhysicalAction.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>

#include <yade/lib-opengl/OpenGLWrapper.hpp>

#include <yade/pkg-common/AABB.hpp>
#include <yade/pkg-common/Tetrahedron.hpp>
#include <yade/pkg-common/ElasticBodyParameters.hpp>
#include <yade/pkg-common/SimpleElasticInteraction.hpp>

#include<Wm3Tetrahedron3.h>
#include<Wm3IntrTetrahedron3Tetrahedron3.h> // not necessary since the cpp includes it as well
//#include"Intersection/Wm3IntrTetrahedron3Tetrahedron3.cpp"
//#include"Intersection/Wm3Intersector.cpp"


/*! Calculate configuration of TetraMold - TetraMold intersection.
 *
 * Wildmagick's functions are used here: intersection is returned as a set of tetrahedra (may be empty, inwhich case there is no real intersection).
 * Then we calcualte volumetric proeprties of this intersection volume: inertia, centroid, volume.
 *
 * Contact normal (the direction in which repulsive force will act) coincides with the direction of least inertia,
 * since that is the gradient that maximizes the drop of elastic deformation energy and will reach minimum fastest.
 *
 * Equivalent cross section of the penetrating volume (as if it were a cuboid with the same inertia) and equivalent penetration depth are calculated;
 * Equivalent solid size in the dimension of normal serves as reference for strain calculation and is different for solids A and B.
 *
 * Strain will be then approximated by equivalentPenetrationDepth/.5*(maxPenetrationDepthA+maxPenetrationDepthB) (the average of A and B)
 *
 * All the relevant results are fed into TetraBang which is passed to TetraLaw later that makes actual use of all this.
 *
 * @todo thoroughly test this for numerical correctness.
 *
 */
bool Tetra2TetraBang::go(const shared_ptr<InteractingGeometry>& cm1,const shared_ptr<InteractingGeometry>& cm2,const Se3r& se31,const Se3r& se32,const shared_ptr<Interaction>& interaction){
	TetraMold* A = static_cast<TetraMold*>(cm1.get());
	TetraMold* B = static_cast<TetraMold*>(cm2.get());
	//return false;
	
	shared_ptr<TetraBang> bang;
	// depending whether it's a new interaction: create new one, or use the existing one.
	if (interaction->isNew) bang=shared_ptr<TetraBang>(new TetraBang());
	else bang=YADE_PTR_CAST<TetraBang>(interaction->interactionGeometry);	
	interaction->interactingGeometry=bang;

	// transform to global coordinates, build Tetrahedron3r objects to make wm3 happy
	Tetrahedron3r tA(se31.orientation*A->v[0]+se31.position,se31.orientation*A->v[1]+se31.position,se31.orientation*A->v[2]+se31.position,se31.orientation*A->v[3]+se31.position);
	Tetrahedron3r tB(se32.orientation*B->v[0]+se32.position,se32.orientation*B->v[1]+se32.position,se32.orientation*B->v[2]+se32.position,se32.orientation*B->v[3]+se32.position);

	IntrTetrahedron3Tetrahedron3r iAB(tA,tB);
	bool found=iAB.Find();  //calculates the intersection volume as a composition of 0 or more tetrahedra

	if(!found) return false; // no intersecting volume

	Real V(0); // volume of intersection (cummulative)
	Vector3r Sg(0,0,0); // static moment of intersection
	vector<vector<Vector3r> > tAB;

	Wm3::TArray<Wm3::Tetrahedron3d> iABinfo(iAB.GetIntersection()); // retrieve the array of 4hedra
	for(int i=0; i<iABinfo.GetQuantity(); i++){
		iABinfo[i];  // has i-th tehtrahedron as Tetrahedron3r&
		#define v0 iABinfo[i].V[0]
		#define v1 iABinfo[i].V[1]
		#define v2 iABinfo[i].V[2]
		#define v3 iABinfo[i].V[3]
		Real dV=fabs(Vector3r(v1-v0).Dot((v2-v).Cross(v3-v0)))/6.;
		M+=dV;
		Sg+=dV*(v0+v1+v2+v3)*.25;
		vector<Vector3r> t; t.push_back(v0); t.push_back(v1); t.push_back(v2); t.push_back(v3);
		tAB.push_back(t);
		#undef v0
		#undef v1
		#undef v2
		#undef v3
	}

	Vector3r centroid=Sg/V;
	Matrix3r I(true); // zero matrix initially
	// get total 
	for(size_t i=0; i<tAB.size(); i++){
		tAB[i][0]-=centroid; tAB[i][1]-=centroid; tAB[i][2]-=centroid; tAB[i][3]-=centroid;
		I+=TetrahedronInertiaTensor(tAB[i]);
	}
	
	/* Now, we have the collision volumetrically described by intersection volume (V), its inertia tensor (I) and centroid (centroid; contact point).
	 * The inertia tensor is in global coordinates; by eigendecomposition, we find principal axes, which will give us
	 *  1. normal, the direction of the lest inertia; this is the gradient of penetration energy
	 *  	it may have either direction mathematically, but since 4hedra are convex, 
	 *  	normal will be always the direction pointing more towards the centroid of the other 4hedron
	 *  2. tangent?! hopefully not neede at all. */

	Matrix3r I_princ, R;
	I.EigenDecomposition(R,Ip);
	// according to the documentation in Wm3 header, diagonal entries are in ascending order: d0<=d1<=d2;
	// but keep it algorithmic for now and just assert that.
	int ix=(Ip(0,0)<Ip(1,1) && Ip(0,0)<Ip(2,2))?0:( (Ip(1,1)<Ip(0,0) && Ip(1,1)<Ip(2,2))?1:2); // index of the minimum moment of inertia
	// the other two indices, modulated by 3, since they are ∈ {0,1,2}
	int ixx=(ix+1)%3, ixxx=(ix+2)%3;
	assert(ix==0);
	Vector3r base(0,0,0); base[ix]=1;
	Vector3r normal=R*base; normal.Normalize();

	// centroid of B
	Vector3r Bcent=se31.orientation*((B->v[0]+B->v[1]+B->v[2]+B->v[3])*.25)+se31.position;
	// reverse direction if projection of the (contact_point-centroid_of_B) vector onto the normal is negative (i.e. the normal points more towards A)
	if((Bcent-centroid).Dot(normal)<0) normal*=-1;

	/* now estimate the area of the solid that is perpendicular to the normal. This will be needed to estimate elastic force based on Young's modulus.
	 * Suppose we have cuboid, with edgesof lengths x,y,z in the direction of respective axes.
	 * It's inertia are Ix=(V/12)*(y^2+z^2), Iy=(V/12)*(x^2+z^2), Iz=(V/12)*(x^2+y^2) and suppose Iz is minimal; Ix, Iy and Iz are known (from decomposition above).
	 * Then the area perpendicular to z (normal direction) is given by x*y=V/z, where V is known.
	 * Ix+Iy-Iz=(V/12)*(y^2+z^2+x^2+z^2-x^2-y^2)=(V*z^2)/6, z=√(6*(Ix+Iy-Iz)/V)
	 * Az=V/z=√(V^3/(6*(Ix+Iy+Iz))).
	 *
	 * In our case, the least inertia is along ix, the other coordinates are (ix+1)%3 and (ix+2)%3. equivalentPenetrationDepth means what was z.
	 */
	Real equivalentPenetrationDepth=sqrt(6*(-Ip(ix,ix)+Ip(ixx,ixx)+Ip(ixxx,ixxx))/V);
	Real equivalentCrossSection=V/equivalentPenetrationDepth;

	/* Now rotate the whole inertia tensors of A and B and estimate maxPenetrationDepth -- the length of the body in the direction of the contact normal.
	 * This will be used to calculate relative deformation, which is needed for elastic response. */
	const shared_ptr<BodyMacroParameters>& physA=YADE_PTR_CAST<BodyMacroParameters>(Body::byId(interaction->id1)->physicalParameters);
	const shared_ptr<BodyMacroParameters>& physB=YADE_PTR_CAST<BodyMacroParameters>(Body::byId(interaction->id2)->physicalParameters);
	Matrix3r IA(physA->inertia); Matrix3r IB(physB->inertia);
	// see Clump::inertiaTensorRotate for references
	IA=R.Transpose()*IA*R; IB=R.Transpose()*IB*R;
	Real maxPenetrationDepthA=sqrt(6*(-IA(ix,ix)+IA(ixx,ixx)+IA(ixxx,ixxx))/V);
	Real maxPenetrationDepthB=sqrt(6*(-IB(ix,ix)+IB(ixx,ixx)+IB(ixxx,ixxx))/V);

	/* store calculated stuff in bang; some is redundant */
	bang->normal=normal;
	bang->equivalentCrossSection=equivalentCrossSection;
	bang->contactPoint=centroid;
	bang->penetrationVolume=V;

	bang->equiavelntPenetrationDepth=equivalentPenetrationDepth;
	bang->maxPenetrationDepthA=maxPenetrationDepthA;
	bang->maxPenetrationDepthB=maxPenetrationDepthB;

	return true;
}

bool Tetra2TetraBang::goReverse(const shared_ptr<InteractingGeometry>& cm1,const shared_ptr<InteractingGeometry>& cm2,const Se3r& se31,const Se3r& se32,const shared_ptr<Interaction>& interaction){
	// reverse only normal direction, otherwise use the inverse contact
	bool isInteracting = go(cm2,cm1,se32,se31,interaction);
	if(isInteracting){
		TetraBang* bang=static_cast<TetraBang*>(c->interactionGeometry.get());
		bang->normal*=-1;
	}
	return isInteracting;
}

/*! Apply forces on tetrahedra in collision based on geometric configuration provided by Tetra2TetraBang.
 *
 * Comments on functionality limitations are in the code. It has not been tested at all!!! */
void TetraLaw::action(Body* body)
{
 	MetaBody* rootBody = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;

	for(InteractionContainer::iterator contactI=rootBody->transientInteractions->begin(); contactI!=rootBody->transientInteractions->end(); ++contactI){
		if (!(*contactI)->isReal) continue; // Tetra2TetraBang::go returned false for this interaction, skip it

		const shared_ptr<Body>& A=Body::byId((*contactI)->getId1());
		const shared_ptr<Body>& B=Body::byId((*contactI)->getId2());
			
		if(!(A->getGroupMask()&B->getGroupMask()&sdecGroupMask)) continue; // no bits overlap in masks, skip this one

		const shared_ptr<ElasticBodyParameters>& physA(dynamic_pointer_cast<ElasticBodyParameters>(A->physicalParameters));
		const shared_ptr<ElasticBodyParameters>& physB(dynamic_pointer_cast<ElasticBodyParameters>(B->physicalParameters));
		
		const shared_prt<TetraBang>& contactGeom(dynamic_pointer_cast<TetraBang*>((*contactI)->interactionGeometry));
		const shared_prt<SimpleElasticInteraction>& contactPhys(dynamic_pointer_cast<SimpleElasticInteraction*>((*contactI)->interactionPhysics));


		/* Cross-section is volumetrically equivalent to the penetration configuration */
		Real averageStrain=contactGeom->equivalentPenetrationDepth/(.5*(contactGeom->maxPenetrationDepthA+contactGeom->maxPenetrationDepthB));

		/* Do not use SimpleElasticInteraction::kn (as calculated by ElasticBodySimpleRelationship).
		 * SimpleElasticInteraction::kn is not Young's modulus, it is calculated by MacroMicroElasticRelationships. So perhaps
		 * a new InteractionPhysicsEngineUnit will be needed that will just pass the average Young's modulus here?
		 * For now, just go back to Young's moduli directly here. */
		Real young=.5*(physA->young+physB->young);
		// F=σA=εEA
		Vector3r F=averageStrain*contactPhys->kn*contactGeom->equivalentCrossSection;

		static_pointer_cast<Force*>(ncb->physicalActions->find(idA,actionForce->getClassIndex()))->force-=F;
		static_pointer_cast<Force*>(ncb->physicalActions->find(idB,actionForce->getClassIndex()))->force+=F;
		static_pointer_cast<Momentum*>(ncb->physicalActions->find(idA,actionMomentum->getClassIndex()))->momentum-=(physA->se3.position-contactGeom-contactPoint).Cross(F);
		static_pointer_cast<Momentum*>(ncb->physicalActions->find(idB,actionMomentum->getClassIndex()))->momentum+=(physB->se3.position-contactGeom-contactPoint).Cross(F);
	}
}


void TetraDraw::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>&,bool)
{
  	glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,Vector3f(cm->diffuseColor[0],cm->diffuseColor[1],cm->diffuseColor[2]));
	glColor3v(cm->diffuseColor);
	TetraMold* t=static_cast<TetraMold*>(cm.get());
	if (0) { // wireframe, as for Tetrahedron
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			#define __ONEWIRE(a,b) glVertex3dv(t->v[a]);glVertex3dv(t->v[b])
				__ONEWIRE(0,1);__ONEWIRE(0,2);__ONEWIRE(0,3);__ONEWIRE(1,2);__ONEWIRE(1,3);__ONEWIRE(2,3);
			#undef __ONEWIRE
		glEnd();
	}
	else
	{
		Vector3r center = (t->v[0]+t->v[1]+t->v[2]+t->v[3])*.25, faceCenter, n;
		glDisable(GL_CULL_FACE); glEnable(GL_LIGHTING);
		glBegin(GL_TRIANGLES);
			#define __ONEFACE(a,b,c) n=(t->v[b]-t->v[a]).UnitCross(t->v[c]-t->v[a]); faceCenter=(t->v[a]+t->v[b]+t->v[c])/3.; if((faceCenter-center).Dot(n)<0)n=-n; glNormal3dv(n); glVertex3dv(t->v[a]); glVertex3dv(t->v[b]); glVertex3dv(t->v[c]);
				__ONEFACE(3,0,1);
				__ONEFACE(0,1,2);
				__ONEFACE(1,2,3);
				__ONEFACE(2,3,0);
			#undef __ONEFACE
		glEnd();
	}
	
}

/*! calculate terahedron's volume */
Real TetrahedronVolume(const vector<Vector3r>& v){
	assert(v.size()==4);
	return fabs(Vector3r(v[1]-v[0]).Dot(Vector3r(v[2]-v[0]).Cross(v[3]-v[0])))/6.;
}

/*! calculates tetrahedron inertia relative to the origin (0,0,0), with unit density (scales linearly)
See article F. Tonon, "Explicit Exact Formulas for the 3-D Tetrahedron Inertia Tensor in Terms of its Vertex Coordinates", http://www.scipub.org/fulltext/jms2/jms2118-11.pdf

Numerical example to check:

vertices:
	(8.33220, 11.86875, 0.93355)
	(0.75523 ,5.00000, 16.37072)
	(52.61236, 5.00000, 5.38580)
	(2.00000, 5.00000, 3.00000)
centroid:
	(15.92492, 0.78281, 3.72962)
intertia/density WRT centroid:
	a/μ = 43520.33257 m⁵
	b/μ = 194711.28938 m⁵
	c/μ = 191168.76173 m⁵
	a’/μ= 4417.66150 m⁵
	b’/μ=-46343.16662 m⁵
	c’/μ= 11996.20119 m⁵

@fixme: failing numerical testcase (in TetraTestGen::generate) ?! centroid is correct, for inertia we get:

63509.2
193465
191169
4417.66
-52950.8
-11971.3

I checked "a" charcter by character and it is correct; it the author wrong (doubtful)?

*/
Matrix3r TetrahedronInertiaTensor(const vector<Vector3r>& v){
	#define x1 v[0][0]
	#define y1 v[0][1]
	#define z1 v[0][2]
	#define x2 v[1][0]
	#define y2 v[1][1]
	#define z2 v[1][2]
	#define x3 v[2][0]
	#define y3 v[2][1]
	#define z3 v[2][2]
	#define x4 v[3][0]
	#define y4 v[3][1]
	#define z4 v[3][2]

	assert(v.size()==4);

	// Jacobian of transformation to the reference 4hedron
	double detJ=(x2-x1)*(y3-y1)*(z4-z1)+(x3-x1)*(y4-y1)*(z2-z1)+(x4-x1)*(y2-y1)*(z3-z1)
		-(x2-x1)*(y4-y1)*(z3-z1)-(x3-x1)*(y2-y1)*(z4-z1)-(x4-x1)*(y3-y1)*(z2-z1);
	detJ=fabs(detJ);
	double a=detJ*(y1*y1-y1*y2+y2*y2+y1*y3+y2*y3+
		y3*y3+y1*y4+y2*y4+y3*y4+y4*y4+z1*z1+z1*z2+
		z2*z2+z1*z3+z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	double b=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+
		x1*x4+x2*x4+x3*x4+x4*x4+z1*z1+z1*z2+z2*z2+z1*z3+
		z2*z3+z3*z3+z1*z4+z2*z4+z4*z4)/60.;
	double c=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+x1*x4+
		x2*x4+x3*x4+x4*x4+y1*y1+y1*y2+y2*y2+y1*y3+
		y2*y3+y3*y3+y1*y4+y2*y4+y3*y4+y4*y4)/60.;
	// a' in the article etc.
	double a__=detJ*(2*y1*z1+y2*z1+y3*z1+y4*z1+y1*z2+
		2*y2*z2+y3*z2+y4*z2+y1*z3+y2*z3+2*y3*z3+
		y4*z3+y1*z4+y2*z4+y3*z4+2*y4*z4)/120.;
	double b__=detJ*(2*z1*z1+x2*z1+x3*z1+x4*z1+x1*z2+
		2*x2*z2+x3*z2+x4*z2+z1*z3+x2*z3+2*x3*z3+
		x4*z3+x1*z4+x2*z4+x3*z4+2*x4*z4)/120.;
	double c__=detJ*(2*x1*y1+x2*y2+x3*y1+x4*y1+x1*y2+
		2*x2*y2+x3*y2+x4*y2+x1*y3+x2*y3+2*x3*y3+
		x4*y3+x1*y4+x2*y4+x3*y4+2*x4*y4)/120.;

	return Matrix3r(
		a   , -b__, -c__,
		-b__, b   , -a__,
		-c__, -a__, c    );

	#undef x1
	#undef y1
	#undef z1
	#undef x2
	#undef y2
	#undef z2
	#undef x3
	#undef y3
	#undef z3
	#undef x4
	#undef y4
	#undef z4
}

/*! Caluclate tetrahedron's central inertia tensor */
Matrix3r TetrahedronCentralInertiaTensor(const vector<Vector3r>& v){
	assert(v.size()==4);
	vector<Vector3r> vv;
	Vector3r cg=(v[0]+v[1]+v[2]+v[3])*.25;
	vv.push_back(v[0]-cg); vv.push_back(v[1]-cg); vv.push_back(v[2]-cg); vv.push_back(v[3]-cg);
	return TetrahedronInertiaTensor(vv);
}

/*! Rotate and translate terahedron body so that its local axes are principal, keeping global position by updating vertex positions as well.
 * Updates all body parameters as need.
 *
 * @returns rotation that was done as Wm3::Quaternionr.
 * @todo check for geometrical correctness...
 * */
Quaternionr TetrahedronWithLocalAxesPrincipal(shared_ptr<Body>& tetraBody){
	const shared_ptr<Tetrahedron>& tShape(YADE_PTR_CAST<Tetrahedron>(tetraBody->geometricalModel));
	const shared_ptr<RigidBodyParameters>& rbp(YADE_PTR_CAST<RigidBodyParameters>(tetraBody->physicalParameters));
	const shared_ptr<TetraMold>& tMold(dynamic_pointer_cast<TetraMold>(tetraBody->interactingGeometry));

	#define v0 tShape->v[0]
	#define v1 tShape->v[1]
	#define v2 tShape->v[2]
	#define v3 tShape->v[3]

	// adjust position (origin to centroid)
	Vector3r cg=(v0+v1+v2+v3)*.25;
	v0-=cg; v1-=cg; v2-=cg; v3-=cg;
	//tMold->v[0]=v0; tMold->v[1]=v1; tMold->v[2]=v2; tMold->v[3]=v3;
	rbp->se3.position+=cg;

	// adjust orientation (local axes to principal axes)
	Matrix3r I_old=TetrahedronInertiaTensor(tShape->v); //≡TetrahedronCentralInertiaTensor
	Matrix3r I_rot(true), I_new(true); 
	I_old.EigenDecomposition(I_rot,I_new);
	Quaternionr I_Qrot; I_Qrot.FromRotationMatrix(I_rot);
	//! @fixme from right to left: rotate by I_rot, then add original rotation (?!!)
	rbp->se3.orientation=rbp->se3.orientation*I_Qrot;
	for(size_t i=0; i<4; i++){
		tShape->v[i]=I_Qrot.Conjugate()*tShape->v[i];
		if(tMold) tMold->v[i]=tShape->v[i]; // this may have failed...
	}

	// set inertia
	rbp->inertia=Vector3r(I_new(0,0),I_new(1,1),I_new(2,2));

	return I_Qrot;
	#undef v0
	#undef v1
	#undef v2
	#undef v3
}
