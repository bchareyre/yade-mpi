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
	}
	
	//Check all interactions
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		if(!i->isReal()) continue;				//Check whether they are real
		const shared_ptr<RpmPhys>& contPhys = YADE_PTR_CAST<RpmPhys>(i->interactionPhysics);
		
		body_id_t id1 = i->getId1();			//Get bodies ids from interaction
		body_id_t id2 = i->getId2();
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
				identicalIds tempVar(minIdR, maxIdR);
				arrayIdentIds.push_back(tempVar);
			}
		}
		
		
		for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
			for (unsigned int d=0; d<arrayIdentIds.size(); d++) {
				if (i!=d){
					identicalIds& tempAr1 = arrayIdentIds[i];
					identicalIds& tempAr2 = arrayIdentIds[d];
					if (tempAr1.id2 == tempAr2.id1) {
						tempAr2.id1 = tempAr1.id1;
					}
				}
			}
		}
		
		for (unsigned int i=0; i<arrayIdentIds.size(); i++) {
			FOREACH(const shared_ptr<Body>& b, *scene->bodies){
				if (!b) continue;
				if ((YADE_PTR_CAST<RpmState>(b->state)->specimenNumber)==arrayIdentIds[i].id2) {
					YADE_PTR_CAST<RpmState>(b->state)->specimenNumber=arrayIdentIds[i].id1;
				}
			}
		}

		if (contPhys->isCohesive==true) {	//Check whether they are cohesive
			numberCohesiveContacts++;				//If yes - calculate them
		}
	}
	//Save data to a file
	out<<Omega::instance().getCurrentIteration()<<" "<<curBin<<"\n";
	out.close();
}
