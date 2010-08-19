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
		YADE_PTR_CAST<RpmState>(b->state)->specimenVol = 0;
		YADE_PTR_CAST<RpmState>(b->state)->specimenMaxDiam = 0;
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
					identicalIds tempVar(minIdR, maxIdR, 0, 0);
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
	Real totalVol = 0;
	Real const constForVol = 4.0/3.0;
	//Calculate specimen masses, create vector for storing it
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
		if (sphere) {
			Real massTemp = b->state->mass;
			Real volTemp = constForVol*Mathr::PI*pow ( sphere->radius, 3 );
			totalMass += massTemp;
			totalVol += volTemp;
			int specimenNumberId = YADE_PTR_CAST<RpmState>(b->state)->specimenNumber;
			
			if (specimenNumberId != 0) {					//Check, whether particle already belongs to any specimen
				bool foundItemInArray = false;			//If yes, search for suitable "bin"
				for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
					if (arrayIdentIds[i].id1 == specimenNumberId) {
						arrayIdentIds[i].mass+=massTemp;//If "bin" for particle with this specimenId found, put its mass there
						arrayIdentIds[i].vol+=volTemp;//If "bin" for particle with this specimenId found, put its volume there
						foundItemInArray = true;
					}
					if (foundItemInArray) break;
				}
				if (!foundItemInArray) {						//If "bin" for particle is not found, create a "bin" for it
					identicalIds tempVar(specimenNumberId, specimenNumberId+1, massTemp, volTemp);
					arrayIdentIds.push_back(tempVar);
				}
			} else {									//If the particle was not in contact with other bodies, we give it maximal possible specimenId
				YADE_PTR_CAST<RpmState>(b->state)->specimenNumber = maximalSpecimenId;
				identicalIds tempVar(maximalSpecimenId, maximalSpecimenId+1, massTemp, volTemp);
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
	
	//Update specimen masses and volumes
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
					YADE_PTR_CAST<RpmState>(b->state)->specimenMaxDiam = arrayIdentIds[i].maxDistanceBetweenSpheres;		//Each particle will contain now the maximal diametr of the specimen, to which it belongs to
					YADE_PTR_CAST<RpmState>(b->state)->specimenVol = arrayIdentIds[i].vol;		//Each particle will contain now the volume of the specimen, to which it belongs to
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
					materialAnalyzeIds[i].mass+=b->state->mass;
					materialAnalyzeIds[i].vol+=constForVol*Mathr::PI*pow ( sphere->radius, 3 );
					foundSuitableRecording = true;
					break;
				}
			}
			//If not found the recording, create one
			if (!foundSuitableRecording) {
				Real spheresVolume = constForVol*Mathr::PI*pow ( sphere->radius, 3 );
				materialAnalyze tempVar (materialId, specimenNumberId, 1, b->state->mass, spheresVolume);
				materialAnalyzeIds.push_back(tempVar);
			}
		}
	}
	std::sort (materialAnalyzeIds.begin(), materialAnalyzeIds.end(), materialAnalyze::sortMaterialAnalyze);
	
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
		
	//=================================================================================================================
	//Save data to a file
	out<<"**********\n";
	out<<"iter\ttotalMass\ttotalVol\tnumSpec\tmatNum\n";
	out<<scene->iter<<"\t"<<totalMass<<"\t"<<totalVol<<"\t"<<arrayIdentIds.size()<<"\t"<<materialCount.size()<<"\n\n";
	out<<"id\tmassSpec\tvolSpec \tmaxDiamSpec\t";
	
	if (materialCount.size() > 1) {
		for (unsigned int w=0; w<materialCount.size(); w++) { out<<"mat_"<<materialCount[w]<<"_Mass\tmat_"<<materialCount[w]<<"_Vol\tmat_"<<materialCount[w]<<"_PartNum\t";}
	}
	out<<"\n";
	
	for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
		out<<arrayIdentIds[i].id1<<"\t"<<arrayIdentIds[i].mass<<"\t"<<arrayIdentIds[i].vol<<"\t"<<arrayIdentIds[i].maxDistanceBetweenSpheres<<"\t";
		if (materialCount.size() > 1) {
			//Find Material Info
			for (unsigned int w=0; w<materialCount.size(); w++) {
				bool findItem=false;
				vector<int> indexToDelete;
				for (unsigned int l=0; l<materialAnalyzeIds.size(); l++) {
					if ((materialAnalyzeIds[l].matId==materialCount[w])&&(materialAnalyzeIds[l].specId==arrayIdentIds[i].id1)) {
						out<<materialAnalyzeIds[l].mass<<"\t"<<materialAnalyzeIds[l].vol<<"\t"<<materialAnalyzeIds[l].particleNumber<<"\t";
						findItem=true;
					}
				}
				if (!findItem) {
					out<<"0\t0\t0\t";
				}
			}
		}
		out<<"\n";
	}
	
	out.close();
}
