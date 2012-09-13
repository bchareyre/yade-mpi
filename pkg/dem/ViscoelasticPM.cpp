// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
#include"ViscoelasticPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>


#ifdef FIXBUGINTRS
	#include<yade/pkg/common/Sphere.hpp>
	#include<yade/pkg/common/Facet.hpp>
#endif


YADE_PLUGIN((ViscElMat)(ViscElPhys)(Ip2_ViscElMat_ViscElMat_ViscElPhys)(Law2_ScGeom_ViscElPhys_Basic));

/* ViscElMat */
ViscElMat::~ViscElMat(){}

/* ViscElPhys */
ViscElPhys::~ViscElPhys(){}

/* Ip2_ViscElMat_ViscElMat_ViscElPhys */
void Ip2_ViscElMat_ViscElMat_ViscElPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
	// no updates of an existing contact 
	if(interaction->phys) return;
	ViscElMat* mat1 = static_cast<ViscElMat*>(b1.get());
	ViscElMat* mat2 = static_cast<ViscElMat*>(b2.get());
	const Real mass1 = Body::byId(interaction->getId1())->state->mass;
	const Real mass2 = Body::byId(interaction->getId2())->state->mass;
	const Real kn1 = mat1->kn*mass1; const Real cn1 = mat1->cn*mass1;
	const Real ks1 = mat1->ks*mass1; const Real cs1 = mat1->cs*mass1;
	const Real kn2 = mat2->kn*mass2; const Real cn2 = mat2->cn*mass2;
	const Real ks2 = mat2->ks*mass2; const Real cs2 = mat2->cs*mass2;
	ViscElPhys* phys = new ViscElPhys();
	
	if ((kn1>0) or (kn2>0)) {
		phys->kn = 1/( ((kn1>0)?1/kn1:0) + ((kn2>0)?1/kn2:0) );
	} else {
		phys->kn = 0;
	}
	if ((ks1>0) or (ks2>0)) {
		phys->ks = 1/( ((ks1>0)?1/ks1:0) + ((ks2>0)?1/ks2:0) );
	} else {
		phys->ks = 0;
	} 
	
	phys->cn = (cn1?1/cn1:0) + (cn2?1/cn2:0); phys->cn = phys->cn?1/phys->cn:0;
	phys->cs = (cs1?1/cs1:0) + (cs2?1/cs2:0); phys->cs = phys->cs?1/phys->cs:0;
	
	phys->tangensOfFrictionAngle = std::tan(std::min(mat1->frictionAngle, mat2->frictionAngle)); 
	phys->shearForce = Vector3r(0,0,0);
	//phys->prevNormal = Vector3r(0,0,0);
	interaction->phys = shared_ptr<ViscElPhys>(phys);
}

/* Law2_ScGeom_ViscElPhys_Basic */
void Law2_ScGeom_ViscElPhys_Basic::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){

	const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
	ViscElPhys& phys=*static_cast<ViscElPhys*>(_phys.get());

	const int id1 = I->getId1();
	const int id2 = I->getId2();
	
	if (geom.penetrationDepth<0) {
 		scene->interactions->requestErase(I);
		return;
	}

	const BodyContainer& bodies = *scene->bodies;

	const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
	const State& de2 = *static_cast<State*>(bodies[id2]->state.get());

	Vector3r& shearForce = phys.shearForce;
	if (I->isFresh(scene)) shearForce=Vector3r(0,0,0);
	const Real& dt = scene->dt;
	//Vector3r axis = phys.prevNormal.cross(geom.normal);
	//shearForce -= shearForce.cross(axis);
	//const Real angle = dt*0.5*geom.normal.dot(de1.angVel + de2.angVel);
	//axis = angle*geom.normal;
	//shearForce -= shearForce.cross(axis);
	shearForce = geom.rotate(shearForce);
	

	// Handle periodicity.
	const Vector3r shift2 = scene->isPeriodic ? scene->cell->intrShiftPos(I->cellDist): Vector3r::Zero(); 
	const Vector3r shiftVel = scene->isPeriodic ? scene->cell->intrShiftVel(I->cellDist): Vector3r::Zero(); 

	const Vector3r c1x = (geom.contactPoint - de1.pos);
	const Vector3r c2x = (geom.contactPoint - de2.pos - shift2);
	
	const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) + shiftVel;
	const Real normalVelocity	= geom.normal.dot(relativeVelocity);
	const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;

	// As Chiara Modenese suggest, we store the elastic part 
	// and then add the viscous part if we pass the Mohr-Coulomb criterion.
	// See http://www.mail-archive.com/yade-users@lists.launchpad.net/msg01391.html
	shearForce += phys.ks*dt*shearVelocity; // the elastic shear force have a history, but
	Vector3r shearForceVisc = Vector3r::Zero(); // the viscous shear damping haven't a history because it is a function of the instant velocity 

	phys.normalForce = ( phys.kn * geom.penetrationDepth + phys.cn * normalVelocity ) * geom.normal;
	//phys.prevNormal = geom.normal;

	const Real maxFs = phys.normalForce.squaredNorm() * std::pow(phys.tangensOfFrictionAngle,2);
	if( shearForce.squaredNorm() > maxFs )
	{
		// Then Mohr-Coulomb is violated (so, we slip), 
		// we have the max value of the shear force, so 
		// we consider only friction damping.
		const Real ratio = sqrt(maxFs) / shearForce.norm();
		shearForce *= ratio;
	} 
	else 
	{
		// Then no slip occurs we consider friction damping + viscous damping.
		shearForceVisc = phys.cs*shearVelocity; 
	}
  
  #ifdef FIXBUGINTRS
	if (bodies[id1]->checkIntrs) {
		updateInteractions(bodies[id1]);
	}
	
	if (bodies[id2]->checkIntrs) {
		updateInteractions(bodies[id2]);
	}
	#endif
	
	if (I->isActive) {
		const Vector3r f = phys.normalForce + shearForce + shearForceVisc;
		addForce (id1,-f,scene);
		addForce (id2, f,scene);
		addTorque(id1,-c1x.cross(f),scene);
		addTorque(id2, c2x.cross(f),scene);
  }
}






#ifdef FIXBUGINTRS

bool sortIterChain(sortIters a, sortIters b ) { 
	return a.chain < b.chain;
}

bool sortIterActive(sortIters a, sortIters b ) { 
	if ((a.active > b.active) and (a.chain == b.chain)) {
		return true;
	} else {
		return false;
	}
}

bool sortIterBorn(sortIters a, sortIters b ) { 
	if ((a.iterBorn < b.iterBorn) and (a.chain == b.chain) and (a.active == b.active)) {
		return true;
	} else {
		return false;
	}
}

void updateInteractions(shared_ptr <Body> b) {
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	std::cerr<<"The body" << b->id<<" is checked"<<std::endl;
	if (not ((b->intrs.size()<2))) {  //The body collides with more than 1 body
		std::cerr<<"Number of intrs " << b->intrs.size()<<std::endl;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());  
			if (sphere){  //The body is a sphere
				std::cerr<<"It is a sphere"<<std::endl;
				std::vector<shared_ptr<Body> > bodyFacetsContactList;   //List of contacted facets
				
				std::vector<sortIters> arrayItrs;
				
				for(Body::MapId2IntrT::iterator it=b->intrs.begin(),end=b->intrs.end(); it!=end; ++it) {  //Iterate over all bodie's interactions
					Body::id_t bID = (*it).first;
					shared_ptr <Body> b = Body::byId(bID,scene);
					if (b) {
						const Facet* facetTMP = dynamic_cast<Facet*>(b->shape.get());
						if (facetTMP) { //If the current sphere contacts with facets, check it.
							std::cerr<<bID<<std::endl;
							bodyFacetsContactList.push_back(b);
							
							sortIters tempIter((*it).second, (*it).second->iterBorn, b->chain, (*it).second->isActive);
							arrayItrs.push_back(tempIter);
							
						} else {
							std::cerr<<bID<<" is a sphere"<<std::endl;
						}
					}
				}
				
				if (arrayItrs.size() > 1) {
					for (unsigned int i=0; i<arrayItrs.size(); i++) {
						sortIters& tempAr = arrayItrs[i];
						std::cerr<<tempAr.iterBorn<<" " << tempAr.chain<<" " << tempAr.I->isActive<<"\n";
					}
					std::cerr<<"The sphere contacts with more than 1 facet!"<<std::endl;
					std::sort(arrayItrs.begin(), arrayItrs.end(), sortIterChain);       //Sort on chains
					std::sort(arrayItrs.begin(), arrayItrs.end(), sortIterActive);      //Sort on Active/Unactive contacts
					std::sort(arrayItrs.begin(), arrayItrs.end(), sortIterBorn);        //Sort on BornIter
					for (unsigned int i=0; i<arrayItrs.size(); i++) {
						if (i==0) {
							arrayItrs[i].I->isActive=true;
						} else if (arrayItrs[i-1].chain==arrayItrs[i].chain) {
							arrayItrs[i].I->isActive = false;
						} else if (arrayItrs[i-1].chain!=arrayItrs[i].chain) {
							arrayItrs[i].I->isActive = true;
						}
					}
					for (unsigned int i=0; i<arrayItrs.size(); i++) {
						sortIters& tempAr = arrayItrs[i];
						std::cerr<<tempAr.iterBorn<<" " << tempAr.chain<<" " << tempAr.I->isActive<<"\n";
					}
				}
				
			}
	}	
	std::cerr<<std::endl;
	b->checkIntrs = false;
	}
#endif
