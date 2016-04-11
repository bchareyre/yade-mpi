// 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// 2012 © Kneib Francois <francois.kneib@irstea.fr>

#include "Cylinder.hpp"
#include<pkg/common/Sphere.hpp>
#ifdef YADE_OPENGL
	#include<lib/opengl/OpenGLWrapper.hpp>
#endif
#include<pkg/common/Aabb.hpp>

Cylinder::~Cylinder(){}
ChainedCylinder::~ChainedCylinder(){}
ChainedState::~ChainedState(){}
// Ig2_Sphere_ChainedCylinder_CylScGeom::~Ig2_Sphere_ChainedCylinder_CylScGeom() {}
// Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::~Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D() {}

YADE_PLUGIN((Cylinder)(ChainedCylinder)(ChainedState)(Ig2_Sphere_ChainedCylinder_CylScGeom)(Ig2_Sphere_ChainedCylinder_CylScGeom6D)(Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D)(Law2_CylScGeom6D_CohFrictPhys_CohesionMoment)(Law2_ChCylGeom6D_CohFrictPhys_CohesionMoment)(Law2_CylScGeom_FrictPhys_CundallStrack)
	#ifdef YADE_OPENGL
		(Gl1_Cylinder)(Gl1_ChainedCylinder)
	#endif
	(Bo1_Cylinder_Aabb)(Bo1_ChainedCylinder_Aabb)
);

vector<vector<int> > ChainedState::chains;
unsigned int ChainedState::currentChain=0;

//!##################	IG FUNCTORS   #####################


//!Sphere-cylinder or cylinder-cylinder not implemented yet, see Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D and test/chained-cylinder-spring.py
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(	const shared_ptr<Shape>& cm1,
						const shared_ptr<Shape>& cm2,
						const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
						const shared_ptr<Interaction>& c)
{
	const State* sphereSt=YADE_CAST<const State*>(&state1);
	const ChainedState* cylinderSt=YADE_CAST<const ChainedState*>(&state2);
	ChainedCylinder *cylinder=YADE_CAST<ChainedCylinder*>(cm2.get());
	Sphere *sphere=YADE_CAST<Sphere*>(cm1.get());
	assert(sphereSt && cylinderSt && cylinder && sphere);
	bool isLast = (cylinderSt->chains[cylinderSt->chainNumber].size()==(cylinderSt->rank+1));
	bool isNew = !c->geom;
	shared_ptr<CylScGeom> scm;
	if (!isNew) {scm=YADE_PTR_CAST<CylScGeom>(c->geom);}
	
	bool betweenTwoCylinders = false;//defines whether the sphere's center is moving between two cylinders who have an angle>180°
	Vector3r segment, branch, direction;//informations about the current cylinder
	Real length, dist;
	shared_ptr<const ChainedState> statePrev;
	Vector3r segmentPrev=Vector3r(0,0,0),directionPrev=Vector3r(0,0,0),branchPrev=Vector3r(0,0,0);
	Real lengthPrev=0,distPrev=0;
	if (cylinderSt->rank>0){//informations about the previous cylinder
		statePrev = YADE_PTR_CAST<const ChainedState> (Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1],scene)->state);
		segmentPrev = cylinderSt->pos-statePrev->pos;
		lengthPrev = segmentPrev.norm();
		directionPrev = segmentPrev/lengthPrev;
		branchPrev = sphereSt->pos-statePrev->pos;
		distPrev = directionPrev.dot(branchPrev);
	}
	//FIXME : definition of segment in next line breaks periodicity
	shared_ptr<Body> cylinderNext;
	branch = sphereSt->pos-cylinderSt->pos-shift2;
	if (isLast){//handle the last node with length=0
		segment = Vector3r(0,0,0);
		length = 0;
		direction = Vector3r(0,1,0);
		dist = directionPrev.dot(branch);
		if (dist<0) {
			if (isNew) return false;
			else if (scm->isDuplicate) {
				 scm->isDuplicate=2;
				 scm->penetrationDepth=-1;
				 return true;
			}
		}
	}
	else {
		cylinderNext = Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1],scene);
		segment = cylinderNext->state->pos-cylinderSt->pos;
		length = segment.norm();
		direction = segment/length;
		dist = direction.dot(branch);
		if(cylinderSt->rank>0){
			if(distPrev>lengthPrev && dist<0){//the sphere is touching two cylinder who have an angle>180°
				betweenTwoCylinders = true;
			}
		}
		if (!betweenTwoCylinders && (cylinderSt->rank>0 or dist>0)) {			
			if ( segment.dot(branch) >= segment.dot(segment) or dist<0) {//position after or before the cylinder
			//FIXME : scm->penetrationDepth=-1 is defined to workaround interactions never being erased when scm->isDuplicate=2 on the true interaction.
				if (isNew) return false;
				else if (scm->isDuplicate) {
					scm->isDuplicate=2;
					scm->penetrationDepth=-1;
					return true;
				}
			}
		}
	}

	//Check sphere-cylinder distance
	Vector3r projectedP = cylinderSt->pos+shift2 + direction*dist;
	branch = projectedP-sphereSt->pos;
	if(isLast || (cylinderSt->rank==0 && dist<0)){branch = cylinderSt->pos - sphereSt->pos;}
	if (branch.norm() > sphere->radius+cylinder->radius) {
		if (isNew) return false;
		else if (scm->isDuplicate){
			scm->isDuplicate=2;
			scm->penetrationDepth=-1;
			return true;
		}
	}
	
	if (!isNew) scm->isDuplicate = false;//reset here at each step, and recompute below
	//if there is a contact with the previous element in the chain, consider this one a duplicate and push data to the new contact. Two interactions will share the same geometry and physics during a timestep.
	if (!betweenTwoCylinders && cylinderSt->rank>0 && dist<0) {
		if (!isNew) {
			const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1]);
			if(!intr) {cout<<"Skipping contact because collider didn't found the previous cylinder"<<endl;return false;}
			//we know there is a contact, so there should be at least a virtual interaction created by collider
			intr->geom = c->geom;
			intr->phys = c->phys;
			scm=YADE_PTR_CAST<CylScGeom>(c->geom);
			scm->isDuplicate = 2;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
			isNew = false;
			return true;
		} else scm->isDuplicate=true;
		scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
	}
	//similarly, make sure there is no contact with the next element in the chain
	//else if (!isLast && dist>length) {
	if (!betweenTwoCylinders && !isLast && dist>=length) {
		if (!isNew) {
			const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1]);
			if(!intr) {cout<<"Skipping contact because collider didn't found the next cylinder."<<endl;return false;}
			intr->geom = c->geom;
			intr->phys = c->phys;
			scm=YADE_PTR_CAST<CylScGeom>(c->geom);
			scm->isDuplicate = 2;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
			isNew = false;
			return true;
		} else scm->isDuplicate=true;
		scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	}

	//We didn't find any special case, do normal geometry definition
	if (isNew) { scm=shared_ptr<CylScGeom>(new CylScGeom()); c->geom=scm;}

	scm->radius1=sphere->radius;
	scm->radius2=cylinder->radius;
	if (!isLast) scm->id3=cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	scm->start=cylinderSt->pos+shift2; scm->end=scm->start+segment;

	//FIXME : there should be other checks without distanceFactor?
	if (dist<=0 || isLast) {//We have sphere-node contact
		Vector3r normal=(cylinderSt->pos+shift2)-sphereSt->pos;
		Real norm=normal.norm();
		normal /=norm;
		scm->relPos=0;
		scm->onNode=true; scm->relPos=0;
		scm->penetrationDepth=sphere->radius+cylinder->radius-norm;
		scm->contactPoint=sphereSt->pos+(sphere->radius-0.5*scm->penetrationDepth)*normal;
		scm->precompute(state1,state2,scene,c,normal,isNew,shift2,true);//use sphere-sphere precompute (a node is a sphere)
	} else {//we have sphere-cylinder contact
		scm->onNode=false;
		scm->relPos=dist/length;
		Real norm=branch.norm();
		Vector3r normal=branch/norm;
		scm->penetrationDepth= sphere->radius+cylinder->radius-norm;

		// define a virtual sphere at the projected center
		scm->fictiousState.pos = projectedP;
		scm->fictiousState.vel = (1-scm->relPos)*cylinderSt->vel + scm->relPos*cylinderNext->state->vel;
		scm->fictiousState.angVel =
			((1-scm->relPos)*cylinderSt->angVel + scm->relPos*cylinderNext->state->angVel).dot(direction)*direction //twist part : interpolated
			+ segment.cross(cylinderNext->state->vel - cylinderSt->vel);// non-twist part : defined from nodes velocities

		if (dist>length) {
			scm->penetrationDepth=sphere->radius+cylinder->radius-(cylinderSt->pos+segment-sphereSt->pos).norm();
			//FIXME : handle contact jump on next element
		}
		scm->contactPoint = sphereSt->pos+normal*(sphere->radius-0.5*scm->penetrationDepth);
		scm->precompute(state1,scm->fictiousState,scene,c,branch/norm,isNew,shift2,true);//use sphere-sphere precompute (with a virtual sphere)
	}
	return true;
}


bool Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
 	cerr<<"Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse"<<endl;
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

bool Ig2_Sphere_ChainedCylinder_CylScGeom6D::go(	const shared_ptr<Shape>& cm1,
        const shared_ptr<Shape>& cm2,
        const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
        const shared_ptr<Interaction>& c)
{
	const State* sphereSt=YADE_CAST<const State*>(&state1);
	const ChainedState* cylinderSt=YADE_CAST<const ChainedState*>(&state2);
	ChainedCylinder *cylinder=YADE_CAST<ChainedCylinder*>(cm2.get());
	Sphere *sphere=YADE_CAST<Sphere*>(cm1.get());
	assert(sphereSt && cylinderSt && cylinder && sphere);
	bool isLast = (cylinderSt->chains[cylinderSt->chainNumber].size()==(cylinderSt->rank+1));
	bool isNew = !c->geom;
	shared_ptr<CylScGeom6D> scm;
	if (!isNew) {scm=YADE_PTR_CAST<CylScGeom6D>(c->geom);}
	
	bool betweenTwoCylinders = false;//defines whether the sphere's center is moving between two cylinders who have an angle>180°
	Vector3r segment, branch, direction;//informations about the current cylinder
	Real length, dist;
	shared_ptr<const ChainedState> statePrev;
	Vector3r segmentPrev=Vector3r(0,0,0),directionPrev=Vector3r(0,0,0),branchPrev=Vector3r(0,0,0);
	Real lengthPrev=0,distPrev=0;
	if (cylinderSt->rank>0){//informations about the previous cylinder
		statePrev = YADE_PTR_CAST<const ChainedState> (Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1],scene)->state);
		segmentPrev = cylinderSt->pos-statePrev->pos;
		lengthPrev = segmentPrev.norm();
		directionPrev = segmentPrev/lengthPrev;
		branchPrev = sphereSt->pos-statePrev->pos;
		distPrev = directionPrev.dot(branchPrev);
	}
	
	//FIXME : definition of segment in next line breaks periodicity
	shared_ptr<Body> cylinderNext;
	branch = sphereSt->pos-cylinderSt->pos-shift2;
	if (isLast){//handle the last node with length=0
		segment = Vector3r(0,0,0);
		length = 0;
		direction = Vector3r(0,1,0);
		dist = directionPrev.dot(branch);
		if (dist<0) {
			if (isNew) return false;
			else if (scm->isDuplicate) {
				 scm->isDuplicate=2;
				 return true;
			}
		}
	}
	else {
		cylinderNext = Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1],scene);
		segment = cylinderNext->state->pos-cylinderSt->pos;
		length = segment.norm();
		direction = segment/length;
		dist = direction.dot(branch);
		if(cylinderSt->rank>0){
			if(distPrev>lengthPrev && dist<0){//the sphere is touching two cylinder who have an angle>180°
				betweenTwoCylinders = true;
			}
		}
		if (!betweenTwoCylinders && (cylinderSt->rank>0 or dist>0)) {			
			if ( segment.dot(branch) >= segment.dot(segment) or dist<0) {//position after or before the cylinder
			//FIXME : scm->penetrationDepth=-1 is defined to workaround interactions never being erased when scm->isDuplicate=2 on the true interaction.
				if (isNew) return false;
				else if (scm->isDuplicate) {
					scm->isDuplicate=2;
					return true;
				}
			}
		}
	}

	//Check sphere-cylinder distance
	Vector3r projectedP = cylinderSt->pos+shift2 + direction*dist;
	branch = projectedP-sphereSt->pos;
	if(isLast || (cylinderSt->rank==0 && dist<0)){branch = cylinderSt->pos - sphereSt->pos;}
	if (branch.norm() > sphere->radius+cylinder->radius) {
		if (isNew) return false;
		else if (scm->isDuplicate){
			scm->isDuplicate=2;
			return true;
		}
	}
	
	if (!isNew) scm->isDuplicate = false;//reset here at each step, and recompute below
	//if there is a contact with the previous element in the chain, consider this one a duplicate and push data to the new contact. Two interactions will share the same geometry and physics during a timestep.
	if (!betweenTwoCylinders && cylinderSt->rank>0 && dist<0) {
		if (!isNew) {
			const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1]);
			if(!intr) {cout<<"Skipping contact because collider didn't found the previous cylinder"<<endl;return false;}
			//we know there is a contact, so there should be at least a virtual interaction created by collider
			intr->geom = c->geom;
			intr->phys = c->phys;
			scm=YADE_PTR_CAST<CylScGeom6D>(c->geom);
			scm->isDuplicate = 2;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
			isNew = false;
			return true;
		} else scm->isDuplicate=true;
		scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
	}
	//similarly, make sure there is no contact with the next element in the chain
	//else if (!isLast && dist>length) {
	if (!betweenTwoCylinders && !isLast && dist>=length) {
		if (!isNew) {
			const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1]);
			if(!intr) {cout<<"Skipping contact because collider didn't found the next cylinder."<<endl;return false;}
			intr->geom = c->geom;
			intr->phys = c->phys;
			scm=YADE_PTR_CAST<CylScGeom6D>(c->geom);
			scm->isDuplicate = 2;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
			isNew = false;
			return true;
		} else scm->isDuplicate=true;
		scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	}

	//We didn't find any special case, do normal geometry definition
	if (isNew) { scm=shared_ptr<CylScGeom6D>(new CylScGeom6D()); c->geom=scm;}

	scm->radius1=sphere->radius;
	scm->radius2=cylinder->radius;
	if (!isLast && !scm->id3) scm->id3=cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	scm->start=cylinderSt->pos+shift2; scm->end=scm->start+segment;

	//FIXME : there should be other checks without distanceFactor?
	if (dist<=0 || isLast) {//We have sphere-node contact
		Vector3r normal=(cylinderSt->pos+shift2)-sphereSt->pos;
		Real norm=normal.norm();
		normal /=norm;
		scm->relPos=0;
		scm->onNode=true; scm->relPos=0;
		scm->penetrationDepth=sphere->radius+cylinder->radius-norm;
		scm->contactPoint=sphereSt->pos+(sphere->radius-0.5*scm->penetrationDepth)*normal;
		scm->precompute(state1,state2,scene,c,normal,isNew,shift2,true);//use sphere-sphere precompute (a node is a sphere)
	} else {//we have sphere-cylinder contact
		scm->onNode=false;
		scm->relPos=dist/length;
		Real norm=branch.norm();
		Vector3r normal=branch/norm;
		scm->penetrationDepth= sphere->radius+cylinder->radius-norm;

		// define a virtual sphere at the projected center
		scm->fictiousState.pos = projectedP;
		scm->fictiousState.vel = (1-scm->relPos)*cylinderSt->vel + scm->relPos*cylinderNext->state->vel;
		scm->fictiousState.angVel =
			((1-scm->relPos)*cylinderSt->angVel + scm->relPos*cylinderNext->state->angVel).dot(direction)*direction //twist part : interpolated
			+ segment.cross(cylinderNext->state->vel - cylinderSt->vel);// non-twist part : defined from nodes velocities

		if (dist>length) {
			scm->penetrationDepth=sphere->radius+cylinder->radius-(cylinderSt->pos+segment-sphereSt->pos).norm();
			//FIXME : handle contact jump on next element
		}
		scm->contactPoint = sphereSt->pos+normal*(sphere->radius-0.5*scm->penetrationDepth);
		scm->precompute(state1,scm->fictiousState,scene,c,branch/norm,isNew,shift2,true);//use sphere-sphere precompute (with a virtual sphere)
	}
	return true;
}



bool Ig2_Sphere_ChainedCylinder_CylScGeom6D::goReverse( const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
    return go(cm1,cm2,state2,state1,-shift2,force,c);
}

bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
	const ChainedState *pChain1, *pChain2;
	pChain1=YADE_CAST<const ChainedState*>(&state1);
	pChain2=YADE_CAST<const ChainedState*>(&state2);
	unsigned int sizeChain1=pChain1->chains[pChain1->chainNumber].size();
	unsigned int sizeChain2=pChain2->chains[pChain2->chainNumber].size();
	if (!pChain1 || !pChain2) {
		cerr <<"cast failed8567"<<endl;
	}
	const bool revert = ((int) pChain2->rank- (int) pChain1->rank == -1);
	const ChainedState& bchain1 = revert? *pChain2 : *YADE_CAST<const ChainedState*>(&state1);
	const ChainedState& bchain2 = revert? *pChain1 : *pChain2;
	ChainedCylinder *bs1=static_cast<ChainedCylinder*>(revert? cm2.get():cm1.get());
	bool isLast = bchain1.chains[bchain1.chainNumber].size()==(bchain1.rank+1) || bchain2.chains[bchain2.chainNumber].size()==(bchain2.rank+1);
	bool isNew = !c->geom;
	if (pChain2->chainNumber!=pChain1->chainNumber) {
		shared_ptr<ChCylGeom6D> scm;
		if(isLast){ return false; }
		shared_ptr<Body> cylinderNext1=Body::byId(pChain1->chains[pChain1->chainNumber][pChain1->rank+1],scene);
		shared_ptr<Body> cylinderNext2=Body::byId(pChain2->chains[pChain2->chainNumber][pChain2->rank+1],scene);
		//cout<<c->id1<<"  "<<c->id2<<endl;
		bool colinearVectors=0, insideCyl1=1, insideCyl2=1;	//insideCyl1&2 are used to determine whether the contact is inside each cylinder's segment
		Real dist=NaN,k=0,m=0;	//k and m are the parameters of the fictious states on the cylinders.
		Vector3r A=pChain1->pos, a=cylinderNext1->state->pos-A , B=pChain2->pos , b=cylinderNext2->state->pos-B;
		Vector3r N=a.cross(b);
		Vector3r normal;
		if(N.norm()>1e-14){
			dist=std::abs(N.dot(B-A)/(N.norm()));	//distance between the two LINES.
			//But we need to check that the intersection point is inside the two SEGMENTS ...
			//Projection of B to have a common plan between the two segments.
			Vector3r projB1=B+dist*(N/(N.norm())) , projB2=B-dist*(N/(N.norm()));
			Real distB1A=(projB1-A).norm() , distB2A=(projB2-A).norm() ;
			Vector3r projB=(distB1A<=distB2A)*projB1 + (distB1A>distB2A)*projB2;
			int b1=0, b2=1; //base vectors used to compute the segment intersection (if N is aligned with an axis, we can't use this axis)
			if(std::abs(N[1])<1e-14 && std::abs(N[2])<1e-14){b1=1;b2=2;}
			if(std::abs(N[0])<1e-14 && std::abs(N[2])<1e-14){b1=0;b2=2;}
			Real det=a[b1]*b[b2]-a[b2]*b[b1];
			if(std::abs(det)>1e-14){	//Check if the two segments are intersected (using k and m)
				k = (b[b2]*(projB[b1]-A[b1])+b[b1]*(A[b2]-projB[b2]))/det;
				m = (a[b1]*(-projB[b2]+A[b2])+a[b2]*(projB[b1]-A[b1]))/det;
				if( k<0.0 || k>=1.0 || m<0.0 || m>=1.0 ) {	//so they are not intersected
					dist=NaN;
					if(k>=1){k=1;
						if(!(pChain1->rank>=sizeChain1-2))insideCyl1=0;
					}
					if(k<0){k=0;
						if(!(pChain1->rank==0))insideCyl1=0;
					}
					if(m>=1){m=1;
						if(!(pChain2->rank>=sizeChain2-2))insideCyl2=0;
					}
					if(m<0){m=0;
						if(!(pChain2->rank==0))insideCyl2=0;
					}
				}
			}
			else cout<<"Error in Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D : det==0 !!!"<<endl;//should not happen
		}
		else {
			//Special case for parallel cylinders.
			//FIXME : this is an approximation, but it seems very complicated to do something else.
			//FIXME : contact following have to be done for parallel cylinders.
			colinearVectors=1;
			insideCyl1=1 ; insideCyl2=1;
			Real PA=(A-B).dot(b)/(b.norm()*b.norm()); PA=min((Real)1.0,max((Real)0.0,PA));
			Real Pa=(A+a-B).dot(b)/(b.norm()*b.norm()); Pa=min((Real)1.0,max((Real)0.0,Pa));
			Real PB=(B-A).dot(a)/(a.norm()*a.norm()); PB=min((Real)1.0,max((Real)0.0,PB));
			Real Pb=(B+b-A).dot(a)/(a.norm()*a.norm()); Pb=min((Real)1.0,max((Real)0.0,Pb));
			
			Real h1=(A+0.5*a-B).dot(b)/(b.norm()*b.norm());	//Projection parameter of center of a on b
			Real h2=(B+0.5*b-A).dot(a)/(a.norm()*a.norm());	//Projection parameter of center of b on a
			k=(PB+Pb)/2. ; m=(PA+Pa)/2.; dist= (A+k*a - (B+m*b)).norm();
			bool edgeEdgeContact = (h1>1 && pChain2->rank>=sizeChain2-2) || (h1<0 && pChain2->rank==0) || (h2>1 && pChain1->rank>=sizeChain1-2) || (h2<0 && pChain1->rank==0);
			if(edgeEdgeContact)colinearVectors=0;
			if( (0<=h1 and 1>h1) or (0<=h2 and 1>h2) or edgeEdgeContact){;	//Do a perfectly flat contact
			}
			else return false;//Parallel lines but edge-edge contact
		}
		
		ChainedCylinder *cc1=static_cast<ChainedCylinder*>(cm1.get());
		ChainedCylinder *cc2=static_cast<ChainedCylinder*>(cm2.get());
		if(std::isnan(dist)){ //now if we didn't found a suitable distance because the segments don't cross each other, we try to find a sphere-cylinder distance.
			Vector3r pointsToCheck[4]={A,A+a,B,B+b}; Real resultDist=dist, resultProj=dist ; int whichCaseIsCloser=-1 ;
			for (int i=0;i<4;i++){  //loop on the 4 cylinder's extremities and look at the extremity-cylinder distance
				Vector3r S=pointsToCheck[i], C=(i<2)?B:A, vec=(i<2)?b:a; Vector3r CS=S-C;
				Real d=CS.dot(vec)/(vec.norm());
				if(d<0.) resultDist=CS.norm();
				else if(d>vec.norm()) resultDist=(C+vec-S).norm();
				else resultDist=(CS.cross(vec)).norm()/(vec.norm());
				if(dist>resultDist or std::isnan(dist)){dist=resultDist ; whichCaseIsCloser=i; resultProj=d;}
			}
			//we know which extremity may be in contact (i), so k and m are computed to generate the right fictiousStates.
			insideCyl1=1 ; insideCyl2=1;
			
			//FIXME:NATCHOS ! this should be reformulated
			if(whichCaseIsCloser==0 || whichCaseIsCloser==1){
				k=whichCaseIsCloser==0?0:1;
				if(resultProj<=0){
					m=0;
					if(!(pChain2->rank==0))insideCyl2=0;}
				else if(resultProj>b.norm()){
					m=1;
					if(!(pChain2->rank>=sizeChain2-2))insideCyl2=0;}
				else m=resultProj/(b.norm());
				if(isNew && whichCaseIsCloser==1 && !(pChain1->rank>=sizeChain1-2)) return false;
			}
			else{
				m=whichCaseIsCloser==2?0:1;
				if(resultProj<=0){
					k=0;
					if(!(pChain1->rank==0))insideCyl1=0;}
				else if(resultProj>a.norm()){
					k=1;
					if(!(pChain1->rank>=sizeChain2-2))insideCyl1=0;}
				else k=resultProj/(a.norm());
				if(isNew && whichCaseIsCloser==3 && !(pChain2->rank>=sizeChain2-2)) return false;
			}
		}
		if(isNew && dist>=cc1->radius + cc2->radius) return false;	//if the contact had not yet occured, return false.
		//FIXME:the next line sometimes causes an error in the terminal, because instead of returning false here the contact should be correctly erased.
		if(insideCyl1==0 || insideCyl2==0) return false;	//the contact may be duplicated ...
		else{	//else create the geometry.
			if(!isNew) scm=YADE_PTR_CAST<ChCylGeom6D>(c->geom);
			else { scm=shared_ptr<ChCylGeom6D>(new ChCylGeom6D()); c->geom=scm; }
			scm->relPos1=colinearVectors?0.5:k ; scm->relPos2=colinearVectors?0.5:m;
			scm->fictiousState1.pos=A + k*a;
			scm->fictiousState2.pos=B + m*b;
			scm->fictiousState1.vel = (1-k)*pChain1->vel + k*cylinderNext1->state->vel;
			scm->fictiousState2.vel = (1-m)*pChain2->vel + m*cylinderNext2->state->vel;
			Vector3r direction = a/(a.norm());
			scm->fictiousState1.angVel = ((1-k)*pChain1->angVel + k*cylinderNext1->state->angVel).dot(direction)*direction //twist part : interpolated
			+ a.cross(cylinderNext1->state->vel - pChain1->vel);// non-twist part : defined from nodes velocities
			direction = b/(b.norm());
			scm->fictiousState2.angVel = ((1-m)*pChain2->angVel + m*cylinderNext2->state->angVel).dot(direction)*direction //twist part : interpolated
			+ b.cross(cylinderNext2->state->vel - pChain2->vel);// non-twist part : defined from nodes velocities
			scm->contactPoint = 0.5*(scm->fictiousState1.pos+scm->fictiousState2.pos);
			normal= scm->fictiousState2.pos - scm->fictiousState1.pos;
			normal=normal/(normal.norm());
			scm->penetrationDepth=cc1->radius+cc2->radius-dist;
			scm->radius1=cc1->radius;
			scm->radius2=cc2->radius;
			scm->precompute(scm->fictiousState1,scm->fictiousState2,scene,c,normal,isNew,shift2,true);
			return true;
		}
	}
	else if (bchain2.rank-bchain1.rank != 1) {/*cerr<<"Mutual contacts in same chain between not adjacent elements, not handled*/ return false;}
	else{	//contact between two Cylinders within the same chain.
		shared_ptr<ScGeom6D> scm;
		if(!isNew) scm=YADE_PTR_CAST<ScGeom6D>(c->geom);
		else { scm=shared_ptr<ScGeom6D>(new ScGeom6D()); c->geom=scm; }
		Real length=(bchain2.pos-bchain1.pos).norm();
		Vector3r segt =pChain2->pos-pChain1->pos;
		if(isNew) {/*scm->normal=scm->prevNormal=segt/length;*/bs1->initLength=length;}
		if (!halfLengthContacts){
		scm->radius1=revert ? 0:bs1->initLength;
		scm->radius2=revert ? bs1->initLength:0;
		scm->contactPoint=bchain2.pos;
	} else {
		scm->radius1=scm->radius2=0.5*bs1->initLength;
		scm->contactPoint=0.5*(bchain2.pos+bchain1.pos);
	}
	scm->penetrationDepth=bs1->initLength-length;
		//bs1->segment used for fast BBs and projections + display
		bs1->segment= bchain2.pos-bchain1.pos;
#ifdef YADE_OPENGL
		bs1->length=length;
		bs1->chainedOrientation.setFromTwoVectors(Vector3r::UnitZ(),bchain1.ori.conjugate()*segt);
#endif
		scm->precompute(state1,state2,scene,c,segt/length,isNew,shift2,true);
		scm->precomputeRotations(state1,state2,isNew,false);
		//Set values that will be considered in Ip2 functor, geometry (precomputed) is really defined with values above
		scm->radius1 = scm->radius2 = bs1->initLength*0.5;
		return true;
	}
}

bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

#ifdef YADE_OPENGL
//!##################	RENDERING   #####################

bool Gl1_Cylinder::wire;
bool Gl1_Cylinder::glutNormalize;
int  Gl1_Cylinder::glutSlices;
int  Gl1_Cylinder::glutStacks;
int Gl1_Cylinder::glCylinderList=-1;

void Gl1_Cylinder::out( Quaternionr q )
{
	AngleAxisr aa(q);
	std::cout << " axis: " <<  aa.axis()[0] << " " << aa.axis()[1] << " " << aa.axis()[2] << ", angle: " << aa.angle() << " | ";
}

void Gl1_Cylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<Cylinder*>(cm.get()))->radius;
	Real length=(static_cast<Cylinder*>(cm.get()))->length;
	//glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
// 	glPushMatrix();
	Quaternionr shift = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
// 	glPopMatrix();
	return;
}

void Gl1_ChainedCylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& state,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<ChainedCylinder*>(cm.get()))->radius;
	Real length=(static_cast<ChainedCylinder*>(cm.get()))->length;
	Quaternionr shift;// = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	shift.setFromTwoVectors(Vector3r::UnitZ(),state->ori.conjugate()*(static_cast<ChainedCylinder*>(cm.get()))->segment);
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
	return;
}

void Gl1_Cylinder::drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift) const
{
   glPushMatrix();
   GLUquadricObj *quadObj = gluNewQuadric();
   gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
   gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
   gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
   AngleAxisr aa(shift);
   glRotatef(aa.angle()*180.0/Mathr::PI,aa.axis()[0],aa.axis()[1],aa.axis()[2]);
   gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
   gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
   glutSolidSphere(radius,glutSlices,glutStacks);
   glTranslatef(0.0,0.0,length);

   glutSolidSphere(radius,glutSlices,glutStacks);
//    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
   gluDeleteQuadric(quadObj);
   glPopMatrix();
}


//!##################	BOUNDS FUNCTOR   #####################

#endif

void Bo1_Cylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
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
}

void Bo1_ChainedCylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	ChainedCylinder* cylinder = static_cast<ChainedCylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = O+cylinder->segment;
		//cout<<"O="<<O<<" O2="<<O2<<endl;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(O[k],O2[k])-cylinder->radius;
			aabb->max[k]=max(O[k],O2[k])+cylinder->radius;
		}
		return;
	}
}

bool Law2_CylScGeom_FrictPhys_CundallStrack::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	int id1 = contact->getId1(), id2 = contact->getId2();

	CylScGeom* geom= static_cast<CylScGeom*>(ig.get());
	FrictPhys* phys = static_cast<FrictPhys*>(ip.get());
	if(geom->penetrationDepth <0){
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();}
		else return false;}
	if (geom->isDuplicate) {
		if (id2!=geom->trueInt) {
			//cerr<<"skip duplicate "<<id1<<" "<<id2<<endl;
			if (geom->isDuplicate==2) return false;}
	}
	Real& un=geom->penetrationDepth;
	phys->normalForce=phys->kn*std::max(un,(Real) 0)*geom->normal;

	Vector3r& shearForce = geom->rotate(phys->shearForce);
	const Vector3r& shearDisp = geom->shearIncrement();
	shearForce -= phys->ks*shearDisp;
	Real maxFs = phys->normalForce.squaredNorm()*std::pow(phys->tangensOfFrictionAngle,2);

	if (!scene->trackEnergy){//Update force but don't compute energy terms (see below))
		// PFC3d SlipModel, is using friction angle. CoulombCriterion
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = sqrt(maxFs) / shearForce.norm();
			shearForce *= ratio;}
	} else {
		//almost the same with additional Vector3r instanciated for energy tracing, duplicated block to make sure there is no cost for the instanciation of the vector when traceEnergy==false
		if(shearForce.squaredNorm() > maxFs){
			Real ratio = sqrt(maxFs) / shearForce.norm();
			Vector3r trialForce=shearForce;//store prev force for definition of plastic slip
			//define the plastic work input and increment the total plastic energy dissipated
			shearForce *= ratio;
			Real dissip=((1/phys->ks)*(trialForce-shearForce))/*plastic disp*/ .dot(shearForce)/*active force*/;
			if(dissip>0) scene->energy->add(dissip,"plastDissip",plastDissipIx,/*reset*/false);
		}
		// compute elastic energy as well
		scene->energy->add(0.5*(phys->normalForce.squaredNorm()/phys->kn+phys->shearForce.squaredNorm()/phys->ks),"elastPotential",elastPotentialIx,/*reset at every timestep*/true);
	}
	if (!scene->isPeriodic) {
		Vector3r force = -phys->normalForce-shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		//FIXME : include moment due to axis-contact distance in forces on node
		Vector3r twist = (geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force);
		scene->forces.addForce(id2,(geom->relPos-1)*force);
		scene->forces.addTorque(id2,(1-geom->relPos)*twist);
		if (geom->relPos) { //else we are on node (or on last node - and id3 is junk)
			scene->forces.addForce(geom->id3,(-geom->relPos)*force);
			scene->forces.addTorque(geom->id3,geom->relPos*twist);}
	}
// 		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	else {//FIXME : periodicity not implemented here :
		Vector3r force = -phys->normalForce-shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	}
	return true;
}


bool Law2_CylScGeom6D_CohFrictPhys_CohesionMoment::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact) {

    int id1 = contact->getId1(), id2 = contact->getId2();

    CylScGeom6D* geom= YADE_CAST<CylScGeom6D*>(ig.get());
    CohFrictPhys* currentContactPhysics = YADE_CAST<CohFrictPhys*>(ip.get());

    Vector3r& shearForce    = currentContactPhysics->shearForce;			//force tangentielle
    if (contact->isFresh(scene)) shearForce   = Vector3r::Zero(); 			//contact nouveau => force tengentielle = 0,0,0
    Real un     = geom->penetrationDepth;				//un : interpenetration
    Real Fn    = currentContactPhysics->kn*(un-currentContactPhysics->unp);		//Fn : force normale
    if (geom->isDuplicate) {
		if (id2!=geom->trueInt) {
 			//cerr<<"skip duplicate "<<id1<<" "<<id2<<endl;
			if (geom->isDuplicate==2) return false;}
	}

    if (currentContactPhysics->fragile && (-Fn)> currentContactPhysics->normalAdhesion) {
        // BREAK due to tension
        return false;
    } else {
        if ((-Fn)> currentContactPhysics->normalAdhesion) {//normal plasticity
            Fn=-currentContactPhysics->normalAdhesion;
            currentContactPhysics->unp = un+currentContactPhysics->normalAdhesion/currentContactPhysics->kn;
            if (currentContactPhysics->unpMax && currentContactPhysics->unp<currentContactPhysics->unpMax) return false;
        }
        currentContactPhysics->normalForce = Fn*geom->normal;
        Vector3r& shearForce = geom->rotate(currentContactPhysics->shearForce);
        const Vector3r& dus = geom->shearIncrement();

        //Linear elasticity giving "trial" shear force
        shearForce -= currentContactPhysics->ks*dus;

        Real Fs = currentContactPhysics->shearForce.norm();
        Real maxFs = currentContactPhysics->shearAdhesion;
        if (!currentContactPhysics->cohesionDisablesFriction || maxFs==0)
            maxFs += Fn*currentContactPhysics->tangensOfFrictionAngle;
        maxFs = std::max((Real) 0, maxFs);
        if (Fs  > maxFs) {//Plasticity condition on shear force
            if (currentContactPhysics->fragile && !currentContactPhysics->cohesionBroken) {
                currentContactPhysics->SetBreakingState();
                maxFs = max((Real) 0, Fn*currentContactPhysics->tangensOfFrictionAngle);
            }
            maxFs = maxFs / Fs;
            shearForce *= maxFs;
            if (Fn<0)  currentContactPhysics->normalForce = Vector3r::Zero();//Vector3r::Zero()
        }
        Vector3r force = -currentContactPhysics->normalForce-shearForce;
	if (!scene->isPeriodic) {
		scene->forces.addForce(id1,force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		//FIXME : include moment due to axis-contact distance in forces on node
		Vector3r twist = (geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force);
		scene->forces.addForce(id2,(geom->relPos-1)*force);
		scene->forces.addTorque(id2,(1-geom->relPos)*twist);
		if (geom->relPos) { //else we are on node (or on last node - and id3 is junk)
			scene->forces.addForce(geom->id3,(-geom->relPos)*force);
			scene->forces.addTorque(geom->id3,geom->relPos*twist);}
	}
// 		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	else {//FIXME : periodicity not implemented here :
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	}
        //applyForceAtContactPoint(-currentContactPhysics->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
    }
    return true;
}



bool Law2_ChCylGeom6D_CohFrictPhys_CohesionMoment::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
  int id1 = contact->getId1(), id2 = contact->getId2();

    ChCylGeom6D* geom= YADE_CAST<ChCylGeom6D*>(ig.get());
    CohFrictPhys* currentContactPhysics = YADE_CAST<CohFrictPhys*>(ip.get()); 
    
    /*
    shared_ptr<const ChainedState> state1 = YADE_PTR_CAST<const ChainedState> (Body::byId(id1,scene)->state);
    const shared_ptr<Interaction> intr = scene->interactions->find(id1,id2+1);
    if(!intr) {cout<<"Skipping contact because collider didn't found the next cylinder."<<endl;return false;}
    intr->geom = c->geom;
    intr->phys = c->phys;
    */
    
    Vector3r& shearForce    = currentContactPhysics->shearForce;			//force tangentielle
    if (contact->isFresh(scene)) shearForce   = Vector3r::Zero(); 			//contact nouveau => force tengentielle = 0,0,0
    Real un     = geom->penetrationDepth;				//un : interpenetration
    Real Fn    = currentContactPhysics->kn*(un-currentContactPhysics->unp);		//Fn : force normale
    
    if (currentContactPhysics->fragile && (-Fn)> currentContactPhysics->normalAdhesion) return false; // BREAK due to tension
    else {
        if ((-Fn)> currentContactPhysics->normalAdhesion) {//normal plasticity
            Fn=-currentContactPhysics->normalAdhesion;
            currentContactPhysics->unp = un+currentContactPhysics->normalAdhesion/currentContactPhysics->kn;
            if (currentContactPhysics->unpMax && currentContactPhysics->unp<currentContactPhysics->unpMax)
                return false;
	}
    
        
        currentContactPhysics->normalForce = Fn*geom->normal;
        Vector3r& shearForce = geom->rotate(currentContactPhysics->shearForce);
        const Vector3r& dus = geom->shearIncrement();

        //Linear elasticity giving "trial" shear force
        shearForce -= currentContactPhysics->ks*dus;
 
        Real Fs = currentContactPhysics->shearForce.norm();
        Real maxFs = currentContactPhysics->shearAdhesion;
        if (!currentContactPhysics->cohesionDisablesFriction || maxFs==0)
            maxFs += Fn*currentContactPhysics->tangensOfFrictionAngle;
        maxFs = std::max((Real) 0, maxFs);
        if (Fs  > maxFs) {//Plasticity condition on shear force
            if (currentContactPhysics->fragile && !currentContactPhysics->cohesionBroken) {
                currentContactPhysics->SetBreakingState();
                maxFs = max((Real) 0, Fn*currentContactPhysics->tangensOfFrictionAngle);
            }
            maxFs = maxFs / Fs;
            shearForce *= maxFs;
            if (Fn<0)  currentContactPhysics->normalForce = Vector3r::Zero();//Vector3r::Zero()
        }
        

        
        Vector3r force = -currentContactPhysics->normalForce-shearForce;
	//cout<<"id1="<<contact->getId1()<<" id2="<<contact->getId2()<<" normalForce="<<currentContactPhysics->normalForce<<" shearForce="<<shearForce<<endl;
	if (!scene->isPeriodic) {
		Vector3r twist1 = (geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force);
		Vector3r twist2 = (geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force);
		scene->forces.addForce(id1,(1-geom->relPos1)*force);
		scene->forces.addTorque(id1,(1-geom->relPos1)*twist1);
		
		scene->forces.addForce(id2,-(1-geom->relPos2)*force);
		scene->forces.addTorque(id2,(1-geom->relPos2)*twist2);
		
		scene->forces.addForce(id1+1,geom->relPos1*force);
		scene->forces.addTorque(id1+1,geom->relPos1*twist1);
		
		scene->forces.addForce(id2+1,-geom->relPos2*force);
		scene->forces.addTorque(id2+1,geom->relPos2*twist2);
	}
// 		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	else {//FIXME : periodicity not implemented here :
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	}
    }
    return true;
}




