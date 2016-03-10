/*****************************************************************************
*  Copyright (C) 2015 by Anna Effeindzourou   anna.effeindzourou@gmail.com   *
*  Copyright (C) 2015 by Bruno Chareyre       bruno.chareyre@hmg.inpg.fr     *
*  Copyright (C) 2015 by Klaus Thoeni         klaus.thoeni@gmail.com         *
*  This program is free software; it is licensed under the terms of the      *
*  GNU General Public License v2 or later. See file LICENSE for details.     *
******************************************************************************/
#include "PFacet.hpp"
#ifdef YADE_OPENGL
	#include<lib/opengl/OpenGLWrapper.hpp>
#endif

//!##################	IGeom Functors   #####################
// Function used in order to calculate the projection of the sphere on the PFacet element. The function returns if the the porjection is on the inside the triangle and the barycentric coordinates of the projection P on PFacet the element.
boost::tuple<Vector3r,bool, double, double,double> Ig2_Sphere_PFacet_ScGridCoGeom::projection(
						const shared_ptr<Shape>& cm2, const State& state1)
{
	const State* sphereSt = YADE_CAST<const State*>(&state1);
	PFacet* Pfacet = YADE_CAST<PFacet*>(cm2.get());

	vector<Vector3r> vertices;
	vertices.push_back(Pfacet->node1->state->pos);
	vertices.push_back(Pfacet->node2->state->pos);
	vertices.push_back(Pfacet->node3->state->pos);
	Vector3r center=vertices[0]+((vertices[2]-vertices[0])*(vertices[1]-vertices[0]).norm()+(vertices[1]-vertices[0])*(vertices[2]-vertices[0]).norm())/((vertices[1]-vertices[0]).norm()+(vertices[2]-vertices[1]).norm()+(vertices[0]-vertices[2]).norm());

	
	Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
	Vector3r normal = e[0].cross(e[1])/((e[0].cross(e[1])).norm());

// 	Vector3r centerS=sphereSt->pos+shift2;//FIXME: periodicity?
	const Vector3r& centerS=sphereSt->pos;
	Vector3r cl=centerS-center;	
	 
	Real dist=normal.dot(cl);

	if (dist<0) {normal=-normal; dist=-dist;}
	
	Vector3r P =center+(cl - dist*normal);
	
	Vector3r v0 = vertices[1] - vertices[0];
	Vector3r v1 = vertices[2] - vertices[0];
	Vector3r v2 = P - vertices[0];
	
	// Compute dot products
	Real dot00 = v0.dot(v0);
	Real dot01 = v0.dot(v1);
	Real dot02 = v0.dot(v2);
	Real dot11 = v1.dot(v1);
	Real dot12 = v1.dot(v2);
	
	// Compute the barycentric coordinates of the projection P
	Real  invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	Real p1 = (dot11 * dot02 - dot01 * dot12) * invDenom;
	Real p2 = (dot00 * dot12 - dot01 * dot02) * invDenom;
	Real p3 = 1-p1-p2;

	// Check if P is in triangle
	bool isintriangle= (p1 > 0) && (p2 > 0) && (p1 + p2 < 1);
	return boost::make_tuple(P,isintriangle,p1,p2,p3);
}


bool Ig2_Sphere_PFacet_ScGridCoGeom::go(	const shared_ptr<Shape>& cm1,
						const shared_ptr<Shape>& cm2,
						const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
						const shared_ptr<Interaction>& c)
{
	TIMING_DELTAS_START();

	Sphere* sphere = YADE_CAST<Sphere*>(cm1.get());
	PFacet* Pfacet = YADE_CAST<PFacet*>(cm2.get());
	
	const State* sphereSt = YADE_CAST<const State*>(&state1);
	
	Real sphereRadius = sphere->radius;
	Real PFacetradius=Pfacet->radius;

	vector<Vector3r> vertices;
	vertices.push_back(Pfacet->node1->state->pos);
	vertices.push_back(Pfacet->node2->state->pos);
	vertices.push_back(Pfacet->node3->state->pos);
	Vector3r center=vertices[0]+((vertices[2]-vertices[0])*(vertices[1]-vertices[0]).norm()+(vertices[1]-vertices[0])*(vertices[2]-vertices[0]).norm())/((vertices[1]-vertices[0]).norm()+(vertices[2]-vertices[1]).norm()+(vertices[0]-vertices[2]).norm());

	
	Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
	Vector3r normal = e[0].cross(e[1])/((e[0].cross(e[1])).norm());

// 	Vector3r centerS=sphereSt->pos+shift2;//FIXME: periodicity?
	const Vector3r& centerS=sphereSt->pos;
	Vector3r cl=centerS-center;	
	Real dist=normal.dot(cl);
	
	shared_ptr<ScGridCoGeom> scm;
	bool isNew = !(c->geom);

	if (c->geom)
		scm = YADE_PTR_CAST<ScGridCoGeom>(c->geom);
	else
		scm = shared_ptr<ScGridCoGeom>(new ScGridCoGeom());
	
	if(scm->isDuplicate==2 && scm->trueInt!=c->id2)return true;	//the contact will be deleted into the Law, no need to compute here.
	scm->isDuplicate=0;
	scm->trueInt=-1;
	
	
	if (std::abs(dist)>(PFacetradius+sphereRadius) && !c->isReal() && !force) { // no contact, but only if there was no previous contact; ortherwise, the constitutive law is responsible for setting Interaction::isReal=false
		TIMING_DELTAS_CHECKPOINT("Ig2_Sphere_PFacet_ScGridCoGeom");
		return false;
	}
	if (dist<0) {normal=-normal; dist=-dist;}
	
	
	boost::tuple <Vector3r,bool, double, double,double> projectionres = projection(cm2, state1);
	Vector3r P = boost::get<0>(projectionres);
	bool isintriangle = boost::get<1>(projectionres);
	Real p1 = boost::get<2>(projectionres);
	Real p2 = boost::get<3>(projectionres);
	Real p3 = boost::get<4>(projectionres);
	
	
	shared_ptr<Body> GridList[3]={Pfacet->conn1,Pfacet->conn2,Pfacet->conn3};

	// Check if the projection of the contact point is inside the triangle 
	bool isconn1=((p1 > 0) && (p2 <= 0) && (p1 + p2 < 1))||((p1 > 0) && (p2 <= 0) && (p1 + p2 >= 1));
	bool isconn2=((p1 > 0) && (p2 > 0) && (p1 + p2 >= 1))||((p1 <= 0) && (p2 > 0) && (p1 + p2 >= 1));
	bool isconn3=((p1 <= 0) && (p2 > 0) && (p1 + p2 < 1))||((p1 <= 0) && (p2 <= 0) && (p1 + p2 < 1));
	
	Real penetrationDepth=0;
	int connnum=-1;

	GridNode* GridNodeList[3]={ YADE_CAST<GridNode*>(Pfacet->node1->shape.get()), YADE_CAST<GridNode*>(Pfacet->node2->shape.get()),YADE_CAST<GridNode*>(Pfacet->node3->shape.get())};
// 	If the contact projection is in the triangle, a search for an old contact is performed to export the information regarding the contact (phys)
	if (isintriangle){ 
		if(isNew){
			for (int unsigned i=0; i<3;i++){
				for(int unsigned j=0;j<GridNodeList[i]->pfacetList.size();j++){
					if(GridNodeList[i]->pfacetList[j]->getId()!=c->id2){
						boost::tuple <Vector3r,bool, double, double,double> projectionPrev = projection(GridNodeList[i]->pfacetList[j]->shape,state1);
						bool isintrianglePrev = boost::get<1>(projectionPrev);
						if(!isintrianglePrev){	//if(!isintrianglePrev)
							const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GridNodeList[i]->pfacetList[j]->getId());
							if( intr && intr->isReal() ){// if an interaction exist between the sphere and the previous pfacet, import parameters.
// 								cout<<"Copying contact with pfacet geom and phys from "<<intr->id1<<"-"<<intr->id2<<" to here ("<<c->id1<<"-"<<c->id2<<")"<<endl;
								scm=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
								c->geom=scm;
								c->phys=intr->phys;
								c->iterMadeReal=intr->iterMadeReal;// iteration from the time when the contact became real
								scm->trueInt=c->id2;
								scm->isDuplicate=2;	//command the old contact deletion.
								isNew=0;
								break;
							}
						}
					}	
				}	  
			}
		}
	}
	else{	
// 		identification of the cylinder possibly in contact with the sphere
		if (isconn1) connnum=0;
		if (isconn2) connnum=1;
		if (isconn3) connnum=2;
// 		check if the identified cylinder  previously is in contact with the sphere
		if (connnum!=-1){
			//verify if there is a contact between the a neighbouring PFacet, avoid double contacts
			for (int unsigned i=0; i<3;i++){
				for(int unsigned j=0;j<GridNodeList[i]->pfacetList.size();j++){
					if(GridNodeList[i]->pfacetList[j]->getId()!=c->id2){
						boost::tuple <Vector3r,bool, double, double,double> projectionPrev = projection(GridNodeList[i]->pfacetList[j]->shape,state1);
						bool isintrianglePrev = boost::get<1>(projectionPrev);
						if(isintrianglePrev){
							const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GridNodeList[i]->pfacetList[j]->getId());
							if( intr && intr->isReal() ){// if an interaction exist between the sphere and the previous pfacet, import parameters.  
								if (isNew){
									return false;}
								else {
									scm->isDuplicate=1 ;
									scm->trueInt=-1 ;
									return true;
								}	
							}							
						
						}
					}	
				}	  
			}
			//SPhere-cylinder contact
			const State*    sphereSt  = YADE_CAST<const State*>(&state1);
			GridConnection* gridCo    = YADE_CAST<GridConnection*>(GridList[connnum]->shape.get());
			GridNode*       gridNo1   = YADE_CAST<GridNode*>(gridCo->node1->shape.get());
			GridNode*       gridNo2   = YADE_CAST<GridNode*>(gridCo->node2->shape.get());
			State*          gridNo1St = YADE_CAST<State*>(gridCo->node1->state.get());
			State*          gridNo2St = YADE_CAST<State*>(gridCo->node2->state.get());

			Vector3r segt = gridCo->getSegment();
			Real len = gridCo->getLength();
			
			Vector3r spherePos = sphereSt->pos;
			Vector3r branch = spherePos - gridNo1St->pos;
			Vector3r branchN = spherePos - gridNo2St->pos;
			for(int i=0;i<3;i++){
				if(std::abs(branch[i])<1e-14) branch[i]=0.0;
				if(std::abs(branchN[i])<1e-14) branchN[i]=0.0;
			}
			Real relPos = branch.dot(segt)/(len*len);			
			bool SGr=true;
			if(relPos<=0){	// if the sphere projection is BEFORE the segment ...
				if(gridNo1->ConnList.size()>1){//	if the node is not an extremity of the Grid (only one connection)
					for(int unsigned i=0;i<gridNo1->ConnList.size();i++){	// ... loop on all the Connections of the same Node ...
						GridConnection* GC = (GridConnection*)gridNo1->ConnList[i]->shape.get();
						if(GC==gridCo)continue;//	self comparison.
						Vector3r segtCandidate1 = GC->node1->state->pos - gridNo1St->pos; // (be sure of the direction of segtPrev to compare relPosPrev.)
						Vector3r segtCandidate2 = GC->node2->state->pos - gridNo1St->pos;
						Vector3r segtPrev = segtCandidate1.norm()>segtCandidate2.norm() ? segtCandidate1:segtCandidate2;
						for(int j=0;j<3;j++){
							if(std::abs(segtPrev[j])<1e-14) segtPrev[j]=0.0;
						}
						Real relPosPrev = (branch.dot(segtPrev))/(segtPrev.norm()*segtPrev.norm());
						// ... and check whether the sphere projection is before the neighbours connections too.
						if(relPosPrev<=0){//if the sphere projection is outside both the current Connection AND this neighbouring connection, then create the interaction if the neighbour did not already do it before.
							for(int unsigned j=0;j<GC->pfacetList.size();j++){
								if(GC->pfacetList[j]->getId()!=c->id2){
									const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GC->pfacetList[j]->getId());
									
									if(intr && intr->isReal()){
										shared_ptr<ScGridCoGeom> intrGeom=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
										if(!(intrGeom->isDuplicate==1)){ //skip contact.
											if (isNew) {SGr=false;}
											else {scm->isDuplicate=1;}/*cout<<"Declare "<<c->id1<<"-"<<c->id2<<" as duplicated."<<endl;*/
										}
									}
								}
							}
						}
						else{//the sphere projection is outside the current Connection but inside the previous neighbour. The contact has to be handled by the Prev GridConnection, not here.
							if (isNew) {SGr=false;}
							else {	
								//cout<<"The contact "<<c->id1<<"-"<<c->id2<<" HAVE to be copied and deleted NOW."<<endl ;
								scm->isDuplicate=1;
								scm->trueInt=-1; //trueInt id de l'objet avec lequel il y a un contact -1 = rien faire
								return true;
							}
						}		
					}
				}
			}
			
			//Exactly the same but in the case the sphere projection is AFTER the segment.
			else if(relPos>=1){
				if(gridNo2->ConnList.size()>1){
					for(int unsigned i=0;i<gridNo2->ConnList.size();i++){
						GridConnection* GC = (GridConnection*)gridNo2->ConnList[i]->shape.get();
						if(GC==gridCo)continue;//	self comparison.
						Vector3r segtCandidate1 = GC->node1->state->pos - gridNo2St->pos;
						Vector3r segtCandidate2 = GC->node2->state->pos - gridNo2St->pos;
						Vector3r segtNext = segtCandidate1.norm()>segtCandidate2.norm() ? segtCandidate1:segtCandidate2;
						for(int j=0;j<3;j++){
							if(std::abs(segtNext[j])<1e-14) segtNext[j]=0.0;
						}
						
						Real relPosNext = (branchN.dot(segtNext))/(segtNext.norm()*segtNext.norm());
						if(relPosNext<=0){ //if the sphere projection is outside both the current Connection AND this neighbouring connection, then create the interaction if the neighbour did not already do it before.
							for(int unsigned j=0;j<GC->pfacetList.size();j++){
								if(GC->pfacetList[j]->getId()!=c->id2){
									const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GC->pfacetList[j]->getId());
									if(intr && intr->isReal()){
										shared_ptr<ScGridCoGeom> intrGeom=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
										if(!(intrGeom->isDuplicate==1)){
											if (isNew) SGr=false;
											else scm->isDuplicate=1;/*cout<<"Declare "<<c->id1<<"-"<<c->id2<<" as duplicated."<<endl;*/
										}
									}
								}
							}
						}
						else{//the sphere projection is outside the current Connection but inside the previous neighbour. The contact has to be handled by the Prev GridConnection, not here.
							if (isNew)SGr=false;
							else {//cout<<"The contact "<<c->id1<<"-"<<c->id2<<" HAVE to be copied and deleted NOW."<<endl ;
								scm->isDuplicate=1 ;
								scm->trueInt=-1 ;
								return true; 
							}
						}
					}
				}
			}
			else if (relPos<=0.5){
				if(gridNo1->ConnList.size()>1){//	if the node is not an extremity of the Grid (only one connection)
					for(int unsigned i=0;i<gridNo1->ConnList.size();i++){	// ... loop on all the Connections of the same Node ...
						GridConnection* GC = (GridConnection*)gridNo1->ConnList[i]->shape.get();
						if(GC==gridCo)continue;//	self comparison.

						Vector3r segtCandidate1 = GC->node1->state->pos - gridNo1St->pos; // (be sure of the direction of segtPrev to compare relPosPrev.)
						Vector3r segtCandidate2 = GC->node2->state->pos - gridNo1St->pos;
						Vector3r segtPrev = segtCandidate1.norm()>segtCandidate2.norm() ? segtCandidate1:segtCandidate2;
						for(int j=0;j<3;j++){
							if(std::abs(segtPrev[j])<1e-14) segtPrev[j]=0.0;
						}
						Real relPosPrev = (branch.dot(segtPrev))/(segtPrev.norm()*segtPrev.norm());
						if(relPosPrev<=0){ //the sphere projection is inside the current Connection and outide this neighbour connection.
							for(int unsigned j=0;j<GC->pfacetList.size();j++){
								if(GC->pfacetList[j]->getId()!=c->id2){
									const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GC->pfacetList[j]->getId());
									if( intr && intr->isReal() ){// if an ineraction exist between the sphere and the previous connection, import parameters.
	// 									cout<<"1Copying contact geom and phys from "<<intr->id1<<"-"<<intr->id2<<" to here ("<<c->id1<<"-"<<c->id2<<")"<<endl;
										scm=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
										if(isNew){
											c->geom=scm;
											c->phys=intr->phys;
											c->iterMadeReal=intr->iterMadeReal;
										}
										scm->trueInt=c->id2;
										scm->isDuplicate=2;	//command the old contact deletion.
										isNew=0;
										break;
									}
								}
							}	
						}
					}
				}
				
			}
			
			else if (relPos>0.5){
				if(gridNo2->ConnList.size()>1){
					for(int unsigned i=0;i<gridNo2->ConnList.size();i++){
						GridConnection* GC = (GridConnection*)gridNo2->ConnList[i]->shape.get();
						if(GC==gridCo)continue;//	self comparison.
						Vector3r segtCandidate1 = GC->node1->state->pos - gridNo2St->pos;
						Vector3r segtCandidate2 = GC->node2->state->pos - gridNo2St->pos;
						Vector3r segtNext = segtCandidate1.norm()>segtCandidate2.norm() ? segtCandidate1:segtCandidate2;
						for(int j=0;j<3;j++){
							if(std::abs(segtNext[j])<1e-14) segtNext[j]=0.0;
						}
						Real relPosNext = (branchN.dot(segtNext))/(segtNext.norm()*segtNext.norm());
						if(relPosNext<=0){ //the sphere projection is inside the current Connection and outside this neighbour connection.
							for(int unsigned j=0;j<GC->pfacetList.size();j++){
								if(GC->pfacetList[j]->getId()!=c->id2){
									const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GC->pfacetList[j]->getId());
									if( intr && intr->isReal() ){// if an ineraction exist between the sphere and the previous connection, import parameters.
										scm=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
										if(isNew){
											c->geom=scm;
											c->phys=intr->phys;
											c->iterMadeReal=intr->iterMadeReal;
										}
										scm->trueInt=c->id2;
										scm->isDuplicate=2;	//command the old contact deletion.
										break;
									}
								}
							}						
						}
					}
				}
			}
			if(SGr){
				relPos=relPos<0?0:relPos;	//min value of relPos : 0
				relPos=relPos>1?1:relPos;	//max value of relPos : 1
				Vector3r fictiousPos=gridNo1St->pos+relPos*segt;
				Vector3r branchF = fictiousPos - spherePos;

				Real dist = branchF.norm();
				bool SG= !(isNew && (dist > (sphere->radius + gridCo->radius)));
				if(SG){
				  //	Create the geometry :
					if(isNew) c->geom=scm;
					scm->radius1=sphere->radius;
					scm->radius2=gridCo->radius;
					scm->id3=gridCo->node1->getId();
					scm->id4=gridCo->node2->getId();
			
					scm->relPos=relPos;
					Vector3r normal=branchF/dist;
					scm->penetrationDepth = sphere->radius+gridCo->radius-dist;
					scm->fictiousState.pos = fictiousPos;
					scm->contactPoint = spherePos + normal*(scm->radius1 - 0.5*scm->penetrationDepth);
					scm->fictiousState.vel = (1-relPos)*gridNo1St->vel + relPos*gridNo2St->vel;
					scm->fictiousState.angVel =
						((1-relPos)*gridNo1St->angVel + relPos*gridNo2St->angVel).dot(segt/len)*segt/len //twist part : interpolated
						+ segt.cross(gridNo2St->vel - gridNo1St->vel);// non-twist part : defined from nodes velocities
					scm->precompute(state1,scm->fictiousState,scene,c,normal,isNew,shift2,true);//use sphere-sphere precompute (with a virtual sphere)
					return true;
				}

			}
		}
		else{//taking into account the shadow zone
			for (int unsigned i=0; i<3;i++){
				for(int unsigned j=0;j<GridNodeList[i]->pfacetList.size();j++){
					if(GridNodeList[i]->pfacetList[j]->getId()!=c->id2){
						boost::tuple <Vector3r,bool, double, double,double> projectionPrev = projection(GridNodeList[i]->pfacetList[j]->shape,state1);
						bool isintrianglePrev = boost::get<1>(projectionPrev);
						if(!isintrianglePrev){	
						//if the sphere projection is outside both the current PFacet AND this neighbouring PFacet, then create the interaction if the neighbour did not already do it before.
							const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,GridNodeList[i]->pfacetList[j]->getId());
							if(intr && intr->isReal()){
								shared_ptr<ScGridCoGeom> intrGeom=YADE_PTR_CAST<ScGridCoGeom>(intr->geom);
								if(!(intrGeom->isDuplicate==1)){ //skip contact.
									if (isNew) {
									  return false;}
									else {
										scm->isDuplicate=1;
									}/*cout<<"Declare "<<c->id1<<"-"<<c->id2<<" as duplicated."<<endl;*/
								  }
							}
						}	 
						else{//the sphere projection is outside the current PFacet but inside the previous neighbour. The contact has to be handled by the Prev GridConnection, not here.
							if (isNew) {return false;}
							else {	
	    // 						cout<<"The contact "<<c->id1<<"-"<<c->id2<<" HAVE to be copied and deleted NOW."<<endl ;
								scm->isDuplicate=1; //scm->isDuplicate=1;
								scm->trueInt=-1; //trueInt id de l'objet avec lequel il y a un contact -1 = rien faire
								return true;
							}
						}
					}
						  
				}
			}
		}
	}
	if(isintriangle){
		penetrationDepth = sphereRadius + PFacetradius - std::abs(dist);
		normal.normalize();
		if (penetrationDepth>0 || c->isReal() ){
			if(isNew) c->geom=scm;
			scm->radius1=sphereRadius;
			scm->radius2=PFacetradius;
			scm->id3=Pfacet->node1->getId();
			scm->id4=Pfacet->node2->getId();
			scm->id5=Pfacet->node3->getId();
			scm->weight[0]=p1;
			scm->weight[1]=p2;
			scm->weight[2]=p3;
			scm->penetrationDepth = penetrationDepth;
			scm->fictiousState.pos = P;	
			scm->contactPoint = centerS - (PFacetradius-0.5*penetrationDepth)*normal;
			scm->fictiousState.vel = (p1*Pfacet->node1->state->vel + p2*Pfacet->node2->state->vel +p3*Pfacet->node3->state->vel);
			scm->fictiousState.angVel =
				(p1*Pfacet->node1->state->angVel + p2*Pfacet->node2->state->angVel+ p3*Pfacet->node3->state->angVel);
			scm->precompute(state1,scm->fictiousState,scene,c,-normal,isNew,shift2,true);//use sphere-sphere precompute (with a virtual sphere)
			TIMING_DELTAS_CHECKPOINT("Ig2_Sphere_PFacet_ScGridCoGeom");
			return true;	
	
		}
	}
	TIMING_DELTAS_CHECKPOINT("Ig2_Sphere_PFacet_ScGridCoGeom");
	return false;  
}
bool Ig2_Sphere_PFacet_ScGridCoGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}
YADE_PLUGIN((Ig2_Sphere_PFacet_ScGridCoGeom));

bool Ig2_GridConnection_PFacet_ScGeom::go( const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	GridConnection* gridCo    = YADE_CAST<GridConnection*>(cm1.get());
	PFacet* Pfacet = YADE_CAST<PFacet*>(cm2.get());
	
	if(gridCo->node1==Pfacet->node1 || gridCo->node1==Pfacet->node2|| gridCo->node1==Pfacet->node3 || gridCo->node2==Pfacet->node1 || gridCo->node2==Pfacet->node2|| gridCo->node2==Pfacet->node3){return false;}
	
	Body::id_t idNode1=gridCo->node1->getId();
	Body::id_t idNode2=gridCo->node2->getId();
	Body::id_t ids2[3]={Pfacet->conn1->getId(),Pfacet->conn2->getId(),Pfacet->conn3->getId()};
	Body::id_t id1=c->id1;
	Body::id_t id2=c->id2;
	if (!scene->interactions->found(idNode1,id2)){ 
		shared_ptr<Interaction> scm1 (new Interaction(idNode1,id2));
		scene->interactions->insert(scm1);
	}

	if (!scene->interactions->found(idNode2,id2)){ 
		shared_ptr<Interaction> scm2 (new Interaction(idNode2,id2));
		scene->interactions->insert(scm2);
	}
	
	for (int i=0; i<3; i++){ 
		int entier=i;
		ostringstream oss;
		string chaine = "scm";
		oss << chaine << entier;
		string chaine1=oss.str();
		if (!scene->interactions->found(id1,ids2[i])){ 
			shared_ptr<Interaction> chaine1 (new Interaction(id1,ids2[i]));
			scene->interactions->insert(chaine1);
		}
	}

	return(false);
}

bool Ig2_GridConnection_PFacet_ScGeom::goReverse( const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,state2,state1,-shift2,force,c);
}
YADE_PLUGIN((Ig2_GridConnection_PFacet_ScGeom));



bool Ig2_PFacet_PFacet_ScGeom::go( const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	PFacet* Pfacet1 = YADE_CAST<PFacet*>(cm1.get());
	PFacet* Pfacet2 = YADE_CAST<PFacet*>(cm2.get());
	Body::id_t id1=c->id1;
	Body::id_t id2=c->id2;
	
	if(Pfacet1->node1==Pfacet2->node1 || Pfacet1->node1==Pfacet2->node2|| Pfacet1->node1==Pfacet2->node3
	  || Pfacet1->node2==Pfacet2->node1 || Pfacet1->node2==Pfacet2->node2|| Pfacet1->node2==Pfacet2->node3
	  || Pfacet1->node3==Pfacet2->node1 || Pfacet1->node3==Pfacet2->node2|| Pfacet1->node3==Pfacet2->node3 ){ return false;}
	
	
	boost::tuple <Vector3r,bool, double, double,double> p1 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm1,* Pfacet2->node1->state);
	boost::tuple <Vector3r,bool, double, double,double> p2 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm1,* Pfacet2->node2->state);
	boost::tuple <Vector3r,bool, double, double,double> p3 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm1,* Pfacet2->node3->state);

	boost::tuple <Vector3r,bool, double, double,double> p4 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm2,* Pfacet1->node1->state);
	boost::tuple <Vector3r,bool, double, double,double> p5 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm2,* Pfacet1->node2->state);
	boost::tuple <Vector3r,bool, double, double,double> p6 = Ig2_Sphere_PFacet_ScGridCoGeom::projection(cm2,* Pfacet1->node3->state);
	
	bool isintriangle1 = boost::get<1>(p1);
	bool isintriangle2 = boost::get<1>(p2);
	bool isintriangle3 = boost::get<1>(p3);
	

	
	bool isintriangle4 = boost::get<1>(p4);
	bool isintriangle5 = boost::get<1>(p5);
	bool isintriangle6 = boost::get<1>(p6);
	
	
	bool istriangleNodes2P1 = (isintriangle1==true) && (isintriangle2==true)  && (isintriangle3==true);
	bool istriangleNodes1P2 = (isintriangle4==true) && (isintriangle5==true)  && (isintriangle6==true);
	
	
	if(istriangleNodes1P2 ){
		Body::id_t idNode11=Pfacet1->node1->getId();
		Body::id_t idNode12=Pfacet1->node2->getId();
		Body::id_t idNode13=Pfacet1->node3->getId();


		if (!scene->interactions->found(idNode11,id2)){ 
			shared_ptr<Interaction> scm1 (new Interaction(idNode11,id2));
			scene->interactions->insert(scm1);
		}

		if (!scene->interactions->found(idNode12,id2)){ 
			shared_ptr<Interaction> scm2 (new Interaction(idNode12,id2));
			scene->interactions->insert(scm2);
		}
		
		if (!scene->interactions->found(idNode13,id2)){ 
			shared_ptr<Interaction> scm3 (new Interaction(idNode13,id2));
			scene->interactions->insert(scm3);
		}
	}
	else if(istriangleNodes2P1 ){
		Body::id_t idNode21=Pfacet2->node1->getId();
		Body::id_t idNode22=Pfacet2->node2->getId();
		Body::id_t idNode23=Pfacet2->node3->getId();

		

		if (!scene->interactions->found(idNode21,id1)){ 
			shared_ptr<Interaction> scm1 (new Interaction(idNode21,id1));
			scene->interactions->insert(scm1);
		}

		if (!scene->interactions->found(idNode22,id1)){ 
			shared_ptr<Interaction> scm2 (new Interaction(idNode22,id1));
			scene->interactions->insert(scm2);
		}
		
		if (!scene->interactions->found(idNode23,id1)){ 
			shared_ptr<Interaction> scm3 (new Interaction(idNode23,id1));
			scene->interactions->insert(scm3);
		}
	}
	else{

		vector<Vector3r> vertices1={Pfacet1->node1->state->pos,Pfacet1->node2->state->pos,Pfacet1->node3->state->pos};
		vector<Vector3r> vertices2={Pfacet2->node1->state->pos,Pfacet2->node2->state->pos,Pfacet2->node3->state->pos};

	
		
		GridConnection* gridCo1P1    = (GridConnection*) Pfacet1->conn1->shape.get();
		GridConnection* gridCo2P1    = (GridConnection*) Pfacet1->conn2->shape.get();
		GridConnection* gridCo3P1    = (GridConnection*) Pfacet1->conn3->shape.get();
		
		GridConnection* gridCo1P2    = (GridConnection*) Pfacet2->conn1->shape.get();
		GridConnection* gridCo2P2    = (GridConnection*) Pfacet2->conn2->shape.get();
		GridConnection* gridCo3P2    = (GridConnection*) Pfacet2->conn3->shape.get();
		

		State*          gridNo1StgridCo1P1 = YADE_CAST<State*>(gridCo1P1->node1->state.get());
		State*          gridNo2StgridCo1P1 = YADE_CAST<State*>(gridCo1P1->node2->state.get());
		
		State*          gridNo1StgridCo2P1 = YADE_CAST<State*>(gridCo2P1->node1->state.get());
		State*          gridNo2StgridCo2P1 = YADE_CAST<State*>(gridCo2P1->node2->state.get());
		
		State*          gridNo1StgridCo3P1 = YADE_CAST<State*>(gridCo3P1->node1->state.get());
		State*          gridNo2StgridCo3P1 = YADE_CAST<State*>(gridCo3P1->node2->state.get());
		
		State*          gridNo1StgridCo1P2 = YADE_CAST<State*>(gridCo1P2->node1->state.get());
		State*          gridNo2StgridCo1P2 = YADE_CAST<State*>(gridCo1P2->node2->state.get());
		
		State*          gridNo1StgridCo2P2 = YADE_CAST<State*>(gridCo2P2->node1->state.get());
		State*          gridNo2StgridCo2P2 = YADE_CAST<State*>(gridCo2P2->node2->state.get());
		
		State*          gridNo1StgridCo3P2 = YADE_CAST<State*>(gridCo3P2->node1->state.get());
		State*          gridNo2StgridCo3P2 = YADE_CAST<State*>(gridCo3P2->node2->state.get());
		
		
		Vector3r seg11=(gridNo1StgridCo1P1->pos - gridNo2StgridCo1P1->pos)/(gridNo1StgridCo1P1->pos - gridNo2StgridCo1P1->pos).norm();
		Vector3r seg12=(gridNo1StgridCo2P1->pos - gridNo2StgridCo2P1->pos)/(gridNo1StgridCo2P1->pos - gridNo2StgridCo2P1->pos).norm() ;
		Vector3r seg13=(gridNo1StgridCo3P1->pos - gridNo2StgridCo3P1->pos)/(gridNo1StgridCo3P1->pos - gridNo2StgridCo3P1->pos).norm();
		
		
		Vector3r seg21=(gridNo1StgridCo1P2->pos - gridNo2StgridCo1P2->pos)/(gridNo1StgridCo1P2->pos - gridNo2StgridCo1P2->pos).norm();
		Vector3r seg22=(gridNo1StgridCo2P2->pos - gridNo2StgridCo2P2->pos)/(gridNo1StgridCo2P2->pos - gridNo2StgridCo2P2->pos).norm();
		Vector3r seg23=(gridNo1StgridCo3P2->pos - gridNo2StgridCo3P2->pos)/(gridNo1StgridCo3P2->pos - gridNo2StgridCo3P2->pos).norm();
		
		
		Real normal1seg21seg11= seg21.dot(seg11);
		Real normal1seg21seg12= seg21.dot(seg12);
		Real normal1seg21seg13= seg21.dot(seg13);
		
		Real normal1seg22seg11= seg22.dot(seg11);
		Real normal1seg22seg12= seg22.dot(seg12);
		Real normal1seg22seg13= seg22.dot(seg13);
		
		Real normal1seg23seg11= seg23.dot(seg11);
		Real normal1seg23seg12= seg23.dot(seg12);
		Real normal1seg23seg13= seg23.dot(seg13);

		Body::id_t ids1[3]={Pfacet1->conn1->getId(),Pfacet1->conn2->getId(),Pfacet1->conn3->getId()};
		Body::id_t ids2[3]={Pfacet2->conn1->getId(),Pfacet2->conn2->getId(),Pfacet2->conn3->getId()};
		
		Body::id_t c1 =-1;
		Body::id_t c2 =-1;	
	  
		
		if(std::abs(normal1seg21seg11)==1){c1=ids2[0];c2=ids1[0];}
		if(std::abs(normal1seg21seg12)==1){c1=ids2[0];c2=ids1[1];}
		if(std::abs(normal1seg21seg13)==1){c1=ids2[0];c2=ids1[2];}
		
		if(std::abs(normal1seg22seg11)==1){c1=ids2[1];c2=ids1[0];}
		if(std::abs(normal1seg22seg12)==1){c1=ids2[1];c2=ids1[1];}
		if(std::abs(normal1seg22seg13)==1){c1=ids2[1];c2=ids1[2];}
		
		if(std::abs(normal1seg23seg11)==1){c1=ids2[2];c2=ids1[0];}
		if(std::abs(normal1seg23seg12)==1){c1=ids2[2];c2=ids1[1];}
		if(std::abs(normal1seg23seg13)==1){c1=ids2[2];c2=ids1[2];}

		if(	(isintriangle1==false) && (isintriangle2==false)  && (isintriangle3==false) && (isintriangle4==false) && (isintriangle5==false)  && (isintriangle6==false)){
			if((c1!=-1)and (c2!=-1) and(Body::byId(c1)->getGroupMask()!=0)and(Body::byId(c2)->getGroupMask()!=0)){
				ostringstream oss;
				string chaine = "scm";
				if (!scene->interactions->found(c1,c2)){ 
					shared_ptr<Interaction> chaine (new Interaction(c1,c2));
					scene->interactions->insert(chaine);
				}
			}
			else{			
				for (int i=0; i<3; i++){ 
					for (int j=0; j<3; j++){ 
						int entier = j+i*3;
						ostringstream oss;
						string chaine = "scm";
						oss << chaine << entier;
						string chaine1=oss.str();
						if (!scene->interactions->found(ids1[i],ids2[j])){
							if((Body::byId(ids1[i])->getGroupMask()!=0)and(Body::byId(ids2[j])->getGroupMask()!=0)){
								shared_ptr<Interaction> chaine1 (new Interaction(ids1[i],ids2[j]));
								scene->interactions->insert(chaine1);
							}
						}
					}
				}
			  
			}
		  
		}
		else{
			Body::id_t idNode11=Pfacet1->node1->getId();
			Body::id_t idNode12=Pfacet1->node2->getId();
			Body::id_t idNode13=Pfacet1->node3->getId();



			if (!scene->interactions->found(idNode11,id2)){ 
				shared_ptr<Interaction> scm1 (new Interaction(idNode11,id2));
				scene->interactions->insert(scm1);
			}

			if (!scene->interactions->found(idNode12,id2)){ 
				shared_ptr<Interaction> scm2 (new Interaction(idNode12,id2));
				scene->interactions->insert(scm2);
			}
			
			if (!scene->interactions->found(idNode13,id2)){ 
				shared_ptr<Interaction> scm3 (new Interaction(idNode13,id2));
				scene->interactions->insert(scm3);
			}
			
			Body::id_t idNode21=Pfacet2->node1->getId();
			Body::id_t idNode22=Pfacet2->node2->getId();
			Body::id_t idNode23=Pfacet2->node3->getId();

			

			if (!scene->interactions->found(idNode21,id1)){ 
				shared_ptr<Interaction> scm1 (new Interaction(idNode21,id1));
				scene->interactions->insert(scm1);
			}

			if (!scene->interactions->found(idNode22,id1)){ 
				shared_ptr<Interaction> scm2 (new Interaction(idNode22,id1));
				scene->interactions->insert(scm2);
			}
			
			if (!scene->interactions->found(idNode23,id1)){ 
				shared_ptr<Interaction> scm3 (new Interaction(idNode23,id1));
				scene->interactions->insert(scm3);
			}

			Body::id_t ids1[3]={Pfacet1->conn1->getId(),Pfacet1->conn2->getId(),Pfacet1->conn3->getId()};
			Body::id_t ids2[3]={Pfacet2->conn1->getId(),Pfacet2->conn2->getId(),Pfacet2->conn3->getId()};
			
			for (int i=0; i<3; i++){ 
				for (int j=0; j<3; j++){ 
					int entier = j+i*3;
					ostringstream oss;
					string chaine = "scm";
					oss << chaine << entier;
					string chaine1=oss.str();
					if (!scene->interactions->found(ids1[i],ids2[j])){ 
						if((Body::byId(ids1[i])->getGroupMask()!=0)and(Body::byId(ids2[j])->getGroupMask()!=0)){
							shared_ptr<Interaction> chaine1 (new Interaction(ids1[i],ids2[j]));
							scene->interactions->insert(chaine1);
						}
					}
				}
			}
		  
		  
		}
	
	}
	
	return(false);
}

bool Ig2_PFacet_PFacet_ScGeom::goReverse( const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,state2,state1,-shift2,force,c);
}
YADE_PLUGIN((Ig2_PFacet_PFacet_ScGeom));


/********* Wall + Sphere **********/

bool Ig2_Wall_PFacet_ScGeom::go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
	
	PFacet* Pfacet = YADE_CAST<PFacet*>(cm2.get());

	Body::id_t idNode1=Pfacet->node1->getId();
	Body::id_t idNode2=Pfacet->node2->getId();
	Body::id_t idNode3=Pfacet->node3->getId();
	
	Body::id_t id1=c->id1;

	if (!scene->interactions->found(id1,idNode1)){ 
		shared_ptr<Interaction> scm1 (new Interaction(id1,idNode1));
		scene->interactions->insert(scm1);
	}

	if (!scene->interactions->found(id1,idNode2)){ 
		shared_ptr<Interaction> scm2 (new Interaction(id1,idNode2));
		scene->interactions->insert(scm2);
	}
	
	if (!scene->interactions->found(id1,idNode3)){ 
		shared_ptr<Interaction> scm3 (new Interaction(id1,idNode3));
		scene->interactions->insert(scm3);
	}
	
	return(false);
	
}

YADE_PLUGIN((Ig2_Wall_PFacet_ScGeom));
//!##################	Bounds   #####################

void Bo1_PFacet_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b)
{	
	PFacet* Pfacet = YADE_CAST<PFacet*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Vector3r O = Pfacet->node1->state->pos;
	Vector3r O2 =Pfacet->node2->state->pos;
	Vector3r O3 =Pfacet->node3->state->pos;

 	if(!scene->isPeriodic){
		for (int k=0;k<3;k++){
			aabb->min[k] = min(min(O[k],O2[k]),O3[k]) - Pfacet->radius;
			aabb->max[k] = max(max(O[k],O2[k]),O3[k]) + Pfacet->radius;
		}
		return;
 	}
 	else{
		O = scene->cell->unshearPt(O);
		O2 = scene->cell->unshearPt(O2);
		O3= scene->cell->unshearPt(O3);
		
		Vector3r T=scene->cell->hSize*Pfacet->cellDist.cast<Real>();
		O = O + T;
		O2 = O2 + T;
		O3 = O3 + T;
		for (int k=0;k<3;k++){
			aabb->min[k] = min(min(O[k],O2[k]),O3[k]) - Pfacet->radius;
			aabb->max[k] = max(max(O[k],O2[k]),O3[k]) + Pfacet->radius;
		}
	}
}

YADE_PLUGIN((Bo1_PFacet_Aabb));
