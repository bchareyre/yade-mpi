#include "Cylinder.hpp"
#include<yade/pkg-common/Sphere.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/pkg-common/Aabb.hpp>

Cylinder::~Cylinder(){}
ChainedCylinder::~ChainedCylinder(){}
ChainedState::~ChainedState(){}
CylScGeom::~CylScGeom(){}


YADE_PLUGIN((Cylinder)(ChainedCylinder)(ChainedState)(CylScGeom)(Ig2_Sphere_ChainedCylinder_CylScGeom)(Ig2_ChainedCylinder_ChainedCylinder_ScGeom)(Gl1_Cylinder)(Bo1_Cylinder_Aabb)/*(Bo1_ChainedCylinder_Aabb)*/);
YADE_REQUIRE_FEATURE(OPENGL)

vector<vector<int> > ChainedState::chains;
unsigned int ChainedState::currentChain=0;

//!##################	IG FUNCTORS   #####################

#ifdef YADE_DEVIRT_FUNCTORS
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw runtime_error("Do not call Ig2_Sphere_ChainedCylinder_CylScGeom::go, use getStaticFunctorPtr and call that function instead."); }
bool Ig2_Sphere_ChainedCylinder_CylScGeom::goStatic(InteractionGeometryFunctor* _self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	const Ig2_Sphere_ChainedCylinder_CylScGeom* self=static_cast<Ig2_Sphere_ChainedCylinder_CylScGeom*>(_self);
	const Real& interactionDetectionFactor=self->interactionDetectionFactor;
#else
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(	const shared_ptr<Shape>& cm1,
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
		if(isNew) scm->prevNormal=normal;
		else scm->prevNormal=scm->normal;
		scm->normal=normal;
		scm->penetrationDepth=penetrationDepth;
		scm->radius1=s1->radius;
		scm->radius2=s2->radius;
#ifdef IGCACHE
		if (scene->isPeriodic) {
			Vector3r shiftVel = scene->cell->velGrad*scene->cell->Hsize*c->cellDist.cast<Real>();
 			scm->precompute(state1,state2,scene->dt,shiftVel,true);}
 		else scm->precompute(state1,state2,scene->dt,true);
#endif
		return true;
	}
	return false;
}


bool Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,state2,state1,-shift2,force,c);
}


#ifdef YADE_DEVIRT_FUNCTORS
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw runtime_error("Do not call Ig2_Sphere_ChainedCylinder_CylScGeom::go, use getStaticFunctorPtr and call that function instead."); }
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::goStatic(InteractionGeometryFunctor* _self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	const Ig2_ChainedCylinder_ChainedCylinder_ScGeom* self=static_cast<Ig2_ChainedCylinder_ChainedCylinder_ScGeom*>(_self);
	const Real& interactionDetectionFactor=self->interactionDetectionFactor;
#else
bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
#endif
	const ChainedState *pChain1, *pChain2;
	pChain1=YADE_CAST<const ChainedState*>(&state1);
	pChain2=YADE_CAST<const ChainedState*>(&state2);
	const ChainedState& chain1 = *pChain1;
	const ChainedState& chain2 = *pChain2;
	if (!pChain1 || !pChain2) {
		cerr <<"cast failed8567"<<endl;
	}
	bool revert = (pChain2->rank-chain1.rank == -1);

	if (pChain2->chainNumber!=pChain1->chainNumber) {cerr<<"PROBLEM0124"<<endl; return false;}
	else if (chain2.rank-chain1.rank != 1 && pChain2->rank-pChain1->rank != -1) {/*cerr<<"PROBLEM0123"<<endl;*/ return false;}
	const ChainedState* pChain = revert ? pChain2 : pChain1;

	ChainedCylinder *s1=static_cast<ChainedCylinder*>(cm1.get()), *s2=static_cast<ChainedCylinder*>(cm2.get());
	ChainedCylinder *s = revert ? s2 : s1;
	shared_ptr<ScGeom> scm;
	bool isNew = !c->interactionGeometry;
	if(!isNew) scm=YADE_PTR_CAST<ScGeom>(c->interactionGeometry);
	else { scm=shared_ptr<ScGeom>(new ScGeom()); c->interactionGeometry=scm; }
	Real length=abs((pChain->ori*Vector3r::UnitZ()).dot(chain2.pos-chain1.pos));
 	Vector3r segment =pChain->ori*Vector3r::UnitZ()*length;
	
	if (revert) segment = -segment; 
	if(isNew) {scm->normal=scm->prevNormal=segment/length;s->initLength=length;}
	else {scm->prevNormal=scm->normal; scm->normal=segment/length;}

	if (!revert) {
		scm->radius1=s->initLength;
		scm->radius2=0;}
	else {
		scm->radius1=0;
		scm->radius2=s->initLength;}
		
	s->length=length;
	scm->penetrationDepth=s->initLength-length;
	scm->contactPoint=pChain->pos+pChain->ori*Vector3r::UnitZ()*length;
#ifdef IGCACHE
	if (scene->isPeriodic) {
		Vector3r shiftVel = scene->cell->velGrad*scene->cell->Hsize*c->cellDist.cast<Real>();
		scm->precompute(state1,state2,scene->dt,shiftVel,true);}
	else {
		scm->precompute(state1,state2,scene->dt,true);
	}
#else
	cerr<<"this is not supposed to work without #define IGCACHE"<<endl;
#endif
	//Set values that will be considered in Ip2 functor, geometry (precomputed) is really defined with values above
	scm->radius1=s->initLength*0.5;
	scm->radius2=s->initLength*0.5;
	return true;
}


bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

//!##################	RENDERING   #####################

bool Gl1_Cylinder::wire;
bool Gl1_Cylinder::glutNormalize;
int  Gl1_Cylinder::glutSlices;
int  Gl1_Cylinder::glutStacks;
int Gl1_Cylinder::glCylinderList=-1;

void Gl1_Cylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<Cylinder*>(cm.get()))->radius;
	Real length=(static_cast<Cylinder*>(cm.get()))->length;
	//glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	if(glutNormalize)	glPushAttrib(GL_NORMALIZE); // as per http://lists.apple.com/archives/Mac-opengl/2002/Jul/msg00085.html
// 	glPushMatrix();
	if (wire || wire2) drawCylinder(true, r,length);
	else drawCylinder(false, r,length);
	if(glutNormalize) glPopAttrib();
// 	glPopMatrix();
	return;
}


void Gl1_Cylinder::drawCylinder(bool wire, Real radius, Real length) const
{
//    GLERROR;
/*	if (glCylinderList<0) {
		glCylinderList = glGenLists(1);
		glNewList(glCylinderList,GL_COMPILE);*/
   glPushMatrix();
   GLUquadricObj *quadObj = gluNewQuadric();
   gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
   gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
   gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
//     glTranslatef(0.0,0.0,-length*0.5);
   //scaling needs to adapt spheres or they will be elipsoids. They actually seem to disappear when commented glList code is uncommented, the cylinders are displayed correclty.
//    glScalef(1,length,1);
   gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
   gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
   glutSolidSphere(radius,glutSlices,glutStacks);
   glTranslatef(0.0,0.0,length);
//    glRotatef(180,0.0,1.0,0.0);
   glutSolidSphere(radius,glutSlices,glutStacks);
//    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
   gluDeleteQuadric(quadObj);
   glPopMatrix();
//    GLERROR;
	
// 	glEndList();}
// 	glCallList(glCylinderList);

}

//!##################	BOUNDS FUNCTOR   #####################

void Bo1_Cylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = se3.position+se3.orientation*cylinder->segment;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
	// adjust box size along axes so that cylinder doesn't stick out of the box even if sheared (i.e. parallelepiped)
// 	if(scene->cell->hasShear()) {
// 		Vector3r refHalfSize(minkSize);
// 		const Vector3r& cos=scene->cell->getCos();
// 		for(int i=0; i<3; i++){
// 			//cerr<<"cos["<<i<<"]"<<cos[i]<<" ";
// 			int i1=(i+1)%3,i2=(i+2)%3;
// 			minkSize[i1]+=.5*refHalfSize[i1]*(1/cos[i]-1);
// 			minkSize[i2]+=.5*refHalfSize[i2]*(1/cos[i]-1);
// 		}
// 	}
// 	//cerr<<" || "<<halfSize<<endl;
// 	aabb->min = scene->cell->unshearPt(se3.position)-minkSize;
// 	aabb->max = scene->cell->unshearPt(se3.position)+minkSize;
}
/*
void Bo1_ChainedCylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position-se3.positioncylinder->segment*0.5;
		//In this case, segment will be updated by Ig2_ChainedCylinder_ChainedCylinder_ScGeom, no need to rotate it
		Vector3r O2 = se3.position+cylinder->segment*0.5;
// 		cerr << O<<" "<<O2<<endl;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
}*/




