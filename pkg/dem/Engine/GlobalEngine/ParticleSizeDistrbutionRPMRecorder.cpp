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
		YADE_PTR_CAST<RpmState>(b->state)->maxDiametrParticle = 0;
	}
	
	//Check all interactions
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		if(!i->isReal()) continue;				//Check whether they are real
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->interactionPhysics);
		
		Body::id_t id1 = i->getId1();			//Get bodies ids from interaction
		Body::id_t id2 = i->getId2();
		
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
	
	
	//Find maximal distance between spheres of one specimen
	FOREACH(const shared_ptr<Body>& b1, *scene->bodies){
		if (!b1) continue;
		FOREACH(const shared_ptr<Body>& b2, *scene->bodies){
			if (!b2) continue;
			const Sphere* sphere1 = dynamic_cast<Sphere*>(b1->shape.get());							//Check, whether it is a sphere
			const Sphere* sphere2 = dynamic_cast<Sphere*>(b2->shape.get());
			int specimenNumberId1 = YADE_PTR_CAST<RpmState>(b1->state)->specimenNumber;	//Get specimenNumberId
			int specimenNumberId2 = YADE_PTR_CAST<RpmState>(b2->state)->specimenNumber;
			
			if (((sphere1)&&(sphere2))&&(b1 != b2)&&(specimenNumberId1==specimenNumberId2)) {
				Real distBetweenSpheres = (b1->state->pos - b2->state->pos).norm() + sphere1->radius + sphere2->radius;
				for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
					if ((arrayIdentIds[i].id1 == specimenNumberId1) or (arrayIdentIds[i].id1 == specimenNumberId2)) {
						if (arrayIdentIds[i].maxDistanceBetweenSpheres<distBetweenSpheres) {
							arrayIdentIds[i].maxDistanceBetweenSpheres = distBetweenSpheres;
							break;
						}
					}
				}
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
					if (arrayIdentIds[i].maxDistanceBetweenSpheres==0) {
						arrayIdentIds[i].maxDistanceBetweenSpheres=sphere->radius;
					}
					YADE_PTR_CAST<RpmState>(b->state)->maxDiametrParticle = arrayIdentIds[i].maxDistanceBetweenSpheres;		//Each particle will contain now the maximal diametr of the specimen, to which it belongs to
					break;
				}
			}
		}
	}
	std::sort (arrayIdentIds.begin(), arrayIdentIds.end(), identicalIds::sortArrayIdentIds);
	
	//Material Analyze===============================================================================================
	vector<materialAnalyze> materialAnalyzeIds;
	materialAnalyzeIds.clear();
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere) {
			int specimenNumberId = YADE_PTR_CAST<RpmState>(b->state)->specimenNumber;
			int materialId = b->material->id;
			
			//Check, whether this specimenId is in array
			bool foundSuitableRecording = false;
			for (unsigned int i=0; i<materialAnalyzeIds.size(); i++) {
				if ((materialAnalyzeIds[i].specId == specimenNumberId) and (materialAnalyzeIds[i].matId == materialId)) {
					materialAnalyzeIds[i].particleNumber++;
					materialAnalyzeIds[i].mass+=YADE_PTR_CAST<RpmState>(b->state)->specimenMass;
					foundSuitableRecording = true;
					break;
				}
			}
			//If not found the recording, create one
			if (!foundSuitableRecording) {
				materialAnalyze tempVar (materialId, specimenNumberId, 1, YADE_PTR_CAST<RpmState>(b->state)->specimenMass);
				materialAnalyzeIds.push_back(tempVar);
			}
		}
	}
	std::sort (materialAnalyzeIds.begin(), materialAnalyzeIds.end(), materialAnalyze::sortMaterialAnalyze);
	
	for (unsigned int i=0; i<materialAnalyzeIds.size(); i++) {
		std::cout<<materialAnalyzeIds[i].matId<<" "<<materialAnalyzeIds[i].specId<<" "<<materialAnalyzeIds[i].mass<<" "<<materialAnalyzeIds[i].particleNumber<<"\n";
	}
	std::cout<<"\n";
	
	//Define, how many material columns we need:
	vector<int> materialCount;
	materialCount.clear();
	for (unsigned int i=0; i<materialAnalyzeIds.size(); i++) {
		bool foundItem = false;
		for (unsigned int w=0; w<materialCount.size(); w++) {
			if (materialCount[w]==materialAnalyzeIds[i].matId) {
				foundItem = true;
				break;
			}
		}
		if (foundItem==false) {materialCount.push_back(materialAnalyzeIds[i].matId);}
	}
	for (unsigned int w=0; w<materialCount.size(); w++) { std::cout<<materialCount[w]<<" ";}
	std::cout<<"\n";
		
	//=================================================================================================================
	//Save data to a file
	out<<"**********\n";
	out<<"iter totalMass numSpecimen\n";
	out<<scene->iter<<" "<<totalMass<<" "<<arrayIdentIds.size()<<"\n";
	out<<"id mass maxDistanceBetweenSph ";
	
	for (unsigned int w=0; w<materialCount.size(); w++) { out<<"mat_"<<materialCount[w]<<" partN_"<<materialCount[w]<<" ";}
	out<<"\n";
	
	for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
		out<<arrayIdentIds[i].id1<<" "<<arrayIdentIds[i].mass<<" "<<arrayIdentIds[i].maxDistanceBetweenSpheres<<" ";
		//Find Material Info
		for (unsigned int w=0; w<materialCount.size(); w++) {
			bool findItem=false;
			for (unsigned int l=0; l<materialAnalyzeIds.size(); l++) {
				if ((materialAnalyzeIds[l].matId==materialCount[w])&&(materialAnalyzeIds[l].specId==arrayIdentIds[i].id1)) {
					out<<materialAnalyzeIds[l].mass<<" "<<materialAnalyzeIds[l].particleNumber<<" ";
					findItem=true;
				}
			}
			if (!findItem) {
				out<<"0 0 ";
			}
		}
		
		out<<"\n";
	}
	out.close();
}
