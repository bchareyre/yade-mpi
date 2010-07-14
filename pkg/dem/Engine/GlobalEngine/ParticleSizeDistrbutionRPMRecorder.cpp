#include"ParticleSizeDistrbutionRPMRecorder.hpp"

YADE_PLUGIN((ParticleSizeDistrbutionRPMRecorder));
CREATE_LOGGER(ParticleSizeDistrbutionRPMRecorder);

void ParticleSizeDistrbutionRPMRecorder::action() {
	numberCohesiveContacts=0;
	int curBin = 1;				//Current bin number for PSD
	vector<identicalIds> arrayIdentIds;
	arrayIdentIds.clear();
	
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){			// Annulling specimenNumber
		if (!b) continue;
		YADE_PTR_CAST<RpmState>(b->state)->specimenNumber = 0;
		YADE_PTR_CAST<RpmState>(b->state)->specimenMass = 0;
	}
	
	//Check all interactions
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		if(!i->isReal()) continue;				//Check whether they are real
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->interactionPhysics);
		
		body_id_t id1 = i->getId1();			//Get bodies ids from interaction
		body_id_t id2 = i->getId2();
		
		const Sphere* sphere1 = dynamic_cast<Sphere*>(Body::byId(id1)->shape.get()); 
		const Sphere* sphere2 = dynamic_cast<Sphere*>(Body::byId(id2)->shape.get()); 
		
		if ((sphere1)&&(sphere2)) {				//This class is ONLY for spheres
			int& specimenNumberId1 = YADE_PTR_CAST<RpmState>(Body::byId(id1)->state)->specimenNumber;
			int& specimenNumberId2 = YADE_PTR_CAST<RpmState>(Body::byId(id2)->state)->specimenNumber;
			
			bool cohesiveState = contPhys->isCohesive;
			if (cohesiveState==true){
				if ((specimenNumberId1 == 0) and (specimenNumberId2 == 0)){					//Both bodies are "untouched"
					specimenNumberId1 = curBin;
					specimenNumberId2 = curBin;
					curBin++;
				} else if ((specimenNumberId1 != 0) and (specimenNumberId2 == 0)){	//On of bodies is 0, another one has number specimen
					specimenNumberId2 = specimenNumberId1;
				} else if ((specimenNumberId1 == 0) and (specimenNumberId2 != 0)){	//On of bodies is 0, another one has number specimen
					specimenNumberId1 = specimenNumberId2;
				} else if ((specimenNumberId1 != 0) and (specimenNumberId2 != 0) and (specimenNumberId1 != specimenNumberId2) ){		//Bodies have different specimen number, but they are cohesive! Update it
					int minIdR = std::min(specimenNumberId1, specimenNumberId2);
					int maxIdR = std::max(specimenNumberId1, specimenNumberId2);
					specimenNumberId1 = minIdR;
					specimenNumberId2 = minIdR;
					identicalIds tempVar(minIdR, maxIdR, 0);
					arrayIdentIds.push_back(tempVar);							//Put in the container, that 2 ids belong to the same spcimen!
				}
			}
			if (contPhys->isCohesive==true) {	//Check whether they are cohesive
				numberCohesiveContacts++;				//If yes - calculate them
			}
		}
	}
	
	//Clean dublicates
	for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
		for (unsigned int d=0; d<arrayIdentIds.size(); d++) {
			if ((i!=d)&&(arrayIdentIds[i].id1 == arrayIdentIds[d].id1)&&(arrayIdentIds[i].id2 == arrayIdentIds[d].id2)) {
				arrayIdentIds.erase(arrayIdentIds.begin()+d);
			}
		}
	}
	
	//Updating the container.
	bool flagChange=true;
	while (flagChange){
		flagChange=false;
		for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
			for (unsigned int d=0; d<arrayIdentIds.size(); d++) {
				if (i!=d){
					identicalIds& tempAr1 = arrayIdentIds[i];
					identicalIds& tempAr2 = arrayIdentIds[d];													//   Initial      New
					if ((tempAr1.id2 == tempAr2.id1) and (tempAr2.id1>tempAr1.id1)) { //   1 = 2   |=>  1 = 2
						tempAr2.id1 = tempAr1.id1;																			//   2 = 3   |=>  1 = 3
						flagChange=true;
					}
					if (tempAr1.id2 == tempAr2.id2) {																	//   Initial      New
						if (tempAr2.id1>tempAr1.id1) {																	//   1 = 3   |=>  1 = 3
							tempAr2.id2 = tempAr2.id1;																		//   2 = 3   |=>  1 = 2
							tempAr2.id1 = tempAr1.id1;
							flagChange=true;
						} else if (tempAr2.id1<tempAr1.id1) {														//   Initial      New
							tempAr1.id2 = tempAr1.id1;																		//   2 = 3   |=>  1 = 2
							tempAr1.id1 = tempAr2.id1;																		//   1 = 3   |=>  1 = 3
							flagChange=true;
						} else {																												//   Initial      New
							arrayIdentIds.erase(arrayIdentIds.begin()+d);									//   1 = 3   |=>  1 = 3
						}																																//   1 = 3   |=>  DELETE
					}
				}
			}
		}
	}
	
	//Update RpmState, specimenIds
	for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if (!b) continue;
			const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
			if (sphere) {
				int tempSpecimenNum = YADE_PTR_CAST<RpmState>(b->state)->specimenNumber;
				if ((tempSpecimenNum==arrayIdentIds[i].id1) or (tempSpecimenNum==arrayIdentIds[i].id2)){
					YADE_PTR_CAST<RpmState>(b->state)->specimenNumber=arrayIdentIds[i].id1;
				}
			}
		}
	}
	
	int maximalSpecimenId = curBin;
	arrayIdentIds.clear();
	Real totalMass = 0;
	//Calculate specimen masses, create vector for storing it
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere) {
			Real massTemp = b->state->mass;
			totalMass += massTemp;
			int specimenNumberId = YADE_PTR_CAST<RpmState>(b->state)->specimenNumber;
			
			if (specimenNumberId != 0) {					//Check, whether particle already belongs to any specimen
				bool foundItemInArray = false;			//If yes, search for suitable "bin"
				for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
					if (arrayIdentIds[i].id1 == specimenNumberId) {
						arrayIdentIds[i].mass+=massTemp;//If "bin" for particle with this specimenId found, put its mass there
						foundItemInArray = true;
					}
					if (foundItemInArray) break;
				}
				if (!foundItemInArray) {						//If "bin" for particle is not found, create a "bin" for it
					identicalIds tempVar(specimenNumberId, specimenNumberId+1, massTemp);
					arrayIdentIds.push_back(tempVar);
				}
			} else {									//If the particle was not in contact with other bodies, we give it maximal possible specimenId
				YADE_PTR_CAST<RpmState>(b->state)->specimenNumber = maximalSpecimenId;
				identicalIds tempVar(maximalSpecimenId, maximalSpecimenId+1, massTemp);
				arrayIdentIds.push_back(tempVar);
				maximalSpecimenId++;
			}
		}
	}
	
	//Update specimen masses
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere) {
			int specimenNumberId = YADE_PTR_CAST<RpmState>(b->state)->specimenNumber;
			for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
				if (arrayIdentIds[i].id1 == specimenNumberId) {
					YADE_PTR_CAST<RpmState>(b->state)->specimenMass = arrayIdentIds[i].mass;		//Each particle will contain now the mass of specimen, to which it belongs to
					break;
				}
			}
		}
	}
	
	std::sort (arrayIdentIds.begin(), arrayIdentIds.end(), identicalIds::sortArrayIdentIds);
	
	//Save data to a file
	out<<"**********\n";
	out<<"iter totalMass numSpecimen\n";
	out<<Omega::instance().getCurrentIteration()<<" "<<totalMass<<" "<<arrayIdentIds.size()<<"\n";
	out<<"id mass\n";
	for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
		out<<arrayIdentIds[i].id1<<" "<<arrayIdentIds[i].mass<<"\n";
	}
	out.close();
}
