/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Law2_ScGeom_CapillaryPhys_Capillarity.hpp"
#include <pkg/common/ElastMat.hpp>
#include <pkg/dem/ScGeom.hpp>

#include <pkg/dem/CapillaryPhys.hpp>
#include <pkg/dem/HertzMindlin.hpp>
#include <core/Omega.hpp>
#include <core/Scene.hpp>
#include <lib/base/Math.hpp>

YADE_PLUGIN((Law2_ScGeom_CapillaryPhys_Capillarity));

void Law2_ScGeom_CapillaryPhys_Capillarity::postLoad(Law2_ScGeom_CapillaryPhys_Capillarity&){

  capillary = shared_ptr<capillarylaw>(new capillarylaw);
  capillary->fill(("M(r=1)"+suffCapFiles).c_str());
  capillary->fill(("M(r=1.1)"+suffCapFiles).c_str());
  capillary->fill(("M(r=1.25)"+suffCapFiles).c_str());
  capillary->fill(("M(r=1.5)"+suffCapFiles).c_str());
  capillary->fill(("M(r=1.75)"+suffCapFiles).c_str());
  capillary->fill(("M(r=2)"+suffCapFiles).c_str());
  capillary->fill(("M(r=3)"+suffCapFiles).c_str());
  capillary->fill(("M(r=4)"+suffCapFiles).c_str());
  capillary->fill(("M(r=5)"+suffCapFiles).c_str());
  capillary->fill(("M(r=10)"+suffCapFiles).c_str());
}


MeniscusParameters::MeniscusParameters()
{
        V = 0;
        F = 0;
        delta1 = 0;
        delta2 = 0;
        nn11 = 0;
        nn33 = 0;
}

MeniscusParameters::MeniscusParameters(const MeniscusParameters &source)
{
        V = source.V;
        F = source.F;
        delta1 = source.delta1;
        delta2 = source.delta2;
        nn11 = source.nn11;
        nn33 = source.nn33;
}

MeniscusParameters::~MeniscusParameters()
{}

void Law2_ScGeom_CapillaryPhys_Capillarity::action()
{
	if (!scene) cerr << "scene not defined!";
	if (!capillary) postLoad(*this);//when the script does not define arguments, postLoad is never called
	shared_ptr<BodyContainer>& bodies = scene->bodies;
	
	//check for contact model once (assuming that contact model does not change)
	if (!hertzInitialized){//NOTE: We are assuming that only one type is used in one simulation here
		FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
			if (I->isReal()) {
				if (CapillaryPhys::getClassIndexStatic()==I->phys->getClassIndex()) hertzOn=false;
				else if (MindlinCapillaryPhys::getClassIndexStatic()==I->phys->getClassIndex()) hertzOn=true;
				else LOG_ERROR("The capillary law is not implemented for interactions using "<<I->phys->getClassName());
				bodiesMenisciiList.initialized = false;//must be re-initialized after creation of first real contact in the model
				hertzInitialized = true;
				break;
			}
		}
	}
	
	if (fusionDetection && !bodiesMenisciiList.initialized) bodiesMenisciiList.prepare(scene,hertzOn);

	FOREACH(const shared_ptr<Interaction>& interaction, *scene->interactions){ // could be done in parallel as soon as OpenMPVector class (lib/base/openmp-accu.hpp) is extended See http://www.mail-archive.com/yade-dev@lists.launchpad.net/msg10842.html and msg11238.html
		/// interaction is real
		if (interaction->isReal()) {
			CapillaryPhys* cundallContactPhysics=NULL;
			MindlinCapillaryPhys* mindlinContactPhysics=NULL;

			/// contact physics depends on the contact law, that is used (either linear model or hertz model)
			if (!hertzOn) cundallContactPhysics = static_cast<CapillaryPhys*>(interaction->phys.get());//use CapillaryPhys for linear model
			else mindlinContactPhysics = static_cast<MindlinCapillaryPhys*>(interaction->phys.get());//use MindlinCapillaryPhys for hertz model
			
			unsigned int id1 = interaction->getId1();
			unsigned int id2 = interaction->getId2();

			/// interaction geometry search (this test is to compute capillarity only between spheres (probably a better way to do that)
			int geometryIndex1 = (*bodies)[id1]->shape->getClassIndex(); // !!!
			int geometryIndex2 = (*bodies)[id2]->shape->getClassIndex();
			if (!(geometryIndex1 == geometryIndex2)) continue;

			/// definition of interacting objects (not necessarily in contact)
			ScGeom* currentContactGeometry = static_cast<ScGeom*>(interaction->geom.get());

			/// Capillary components definition:
			Real liquidTension = surfaceTension;

			/// Interacting Grains:
			// If you want to define a ratio between YADE sphere size and real sphere size
			Real alpha=1;
			Real R1 = 0;
			R1=alpha*std::min(currentContactGeometry->radius2,currentContactGeometry->radius1) ;
			Real R2 = 0;
			R2=alpha*std::max(currentContactGeometry->radius2,currentContactGeometry->radius1) ;

			/// intergranular distance
			Real D = - alpha * currentContactGeometry->penetrationDepth;

			if ( D<0 || createDistantMeniscii) { //||(scene->iter < 1) ) // a simplified way to define meniscii everywhere
				D=max(0.,D); // defines fCap when spheres interpenetrate. D<0 leads to wrong interpolation as D<0 has no solution in the interpolation : this is not physically interpretable!! even if, interpenetration << grain radius.
				if (!hertzOn) {
					if (fusionDetection && !cundallContactPhysics->meniscus) bodiesMenisciiList.insert(interaction);
					cundallContactPhysics->meniscus=true;
				} else {
					if (fusionDetection && !mindlinContactPhysics->meniscus) bodiesMenisciiList.insert(interaction);
					mindlinContactPhysics->meniscus=true;
				}
			}
			Real Dinterpol = D/R2;

			/// Suction (Capillary pressure):
			Real Pinterpol = 0;
			if (!hertzOn) Pinterpol = cundallContactPhysics->isBroken ? 0 : capillaryPressure*(R2/liquidTension);
			else Pinterpol = mindlinContactPhysics->isBroken ? 0 : capillaryPressure*(R2/liquidTension);
			if (!hertzOn) cundallContactPhysics->capillaryPressure = capillaryPressure;
			else mindlinContactPhysics->capillaryPressure = capillaryPressure;

			/// Capillary solution finder:
			if ((Pinterpol>=0) && (hertzOn? mindlinContactPhysics->meniscus : cundallContactPhysics->meniscus)) {
				int* currentIndexes =  hertzOn? mindlinContactPhysics->currentIndexes : cundallContactPhysics->currentIndexes;
				//If P=0, we use null solution
				MeniscusParameters
				solution(Pinterpol? capillary->interpolate(R1,R2,Dinterpol, Pinterpol, currentIndexes) : MeniscusParameters());
				/// If no bridge, delete the interaction if necessary and escape
				if (!solution.V) {
					if ((fusionDetection) || (hertzOn ? mindlinContactPhysics->isBroken : cundallContactPhysics->isBroken)) bodiesMenisciiList.remove(interaction);
					if (D>0) {scene->interactions->requestErase(interaction); continue;}
					else if ((Pinterpol > 0) && (showError)) {
						LOG_ERROR("No meniscus found at a contact. capillaryPressure may be too large wrt. the loaded data files."); // V=0 at a contact reveals a problem if and only if uc* > 0
						showError = false;}//show error message once / avoid console spam	
				}
				/// capillary adhesion force
				Real Finterpol = solution.F;
				Vector3r fCap = - Finterpol*(2*Mathr::PI*(R2/alpha)*liquidTension)*currentContactGeometry->normal;
				if (!hertzOn) cundallContactPhysics->fCap = fCap;
				else mindlinContactPhysics->fCap = fCap;
				/// meniscus volume
				Real Vinterpol = solution.V * pow(R2/alpha,3);
				if (!hertzOn) {
					cundallContactPhysics->vMeniscus = Vinterpol;
					if (Vinterpol != 0) cundallContactPhysics->meniscus = true;
					else cundallContactPhysics->meniscus = false;
				} else {
					mindlinContactPhysics->vMeniscus = Vinterpol;
					if (Vinterpol != 0) mindlinContactPhysics->meniscus = true;
					else mindlinContactPhysics->meniscus = false;
				}
				
				/// wetting angles
				if (!hertzOn) {
					cundallContactPhysics->Delta1 = min(max(solution.delta1,solution.delta2),90.0); // undesired values greater than 90 degrees (by few degrees) may be present in the capillary files for high r (~ 10) and very low suction and distance
					cundallContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
				} else {
					mindlinContactPhysics->Delta1 = min(max(solution.delta1,solution.delta2),90.0);
					mindlinContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
				}
				// nn11 and nn33
				if (!hertzOn) {
					cundallContactPhysics->nn11 = pow(R2/alpha,2) * solution.nn11;
					cundallContactPhysics->nn33 = pow(R2/alpha,2) * solution.nn33;
				}
			}
		///interaction is not real	//If the interaction is not real, it should not be in the list
		} else if (fusionDetection) bodiesMenisciiList.remove(interaction);
	}
	if (fusionDetection) checkFusion();

        #ifdef YADE_OPENMP
        const long size=scene->interactions->size();
        #pragma omp parallel for schedule(guided) num_threads(ompThreads>0 ? min(ompThreads,omp_get_max_threads()) : omp_get_max_threads())
        for(long i=0; i<size; i++){
            const shared_ptr<Interaction>& interaction=(*scene->interactions)[i];
        #else
        FOREACH(const shared_ptr<Interaction>& interaction, *scene->interactions){
        #endif
		if (interaction->isReal()) {
			CapillaryPhys* cundallContactPhysics=NULL;
			MindlinCapillaryPhys* mindlinContactPhysics=NULL;
			if (!hertzOn) cundallContactPhysics = static_cast<CapillaryPhys*>(interaction->phys.get());//use CapillaryPhys for linear model
			else mindlinContactPhysics = static_cast<MindlinCapillaryPhys*>(interaction->phys.get());//use MindlinCapillaryPhys for hertz model

			if ((hertzOn && mindlinContactPhysics->meniscus) || (!hertzOn && cundallContactPhysics->meniscus)) {
				if (fusionDetection) {//version with effect of fusion
					//BINARY VERSION : if fusionNumber!=0 then no capillary force
					short int& fusionNumber = hertzOn?mindlinContactPhysics->fusionNumber:cundallContactPhysics->fusionNumber;
					if (binaryFusion) {
						if (fusionNumber!=0) {	//cerr << "fusion" << endl;
							hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap = Vector3r::Zero();
							continue;
						}
					}
					//LINEAR VERSION : capillary force is divided by (fusionNumber + 1) - NOTE : any decreasing function of fusionNumber can be considered in fact
					else if (fusionNumber !=0) hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap /= (fusionNumber+1.);
				}
				scene->forces.addForce(interaction->getId1(),-(hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap));
				scene->forces.addForce(interaction->getId2(),  hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap );
			}
		}
	}
}

capillarylaw::capillarylaw()
{}

void capillarylaw::fill(const char* filename)
{
        data_complete.push_back(Tableau(filename));

}

void Law2_ScGeom_CapillaryPhys_Capillarity::checkFusion()
{
	//Reset fusion numbers
	FOREACH(const shared_ptr<Interaction>& interaction, *scene->interactions){ // same remark for parallel loops, the most problematic part ?
		if ( interaction->isReal()) {
			if (!hertzOn) static_cast<CapillaryPhys*>(interaction->phys.get())->fusionNumber=0;
			else static_cast<MindlinCapillaryPhys*>(interaction->phys.get())->fusionNumber=0;
		}
	}

	list< shared_ptr<Interaction> >::iterator firstMeniscus, lastMeniscus, currentMeniscus;
	Real angle1 = -1.0; Real angle2 = -1.0;
	
	for ( int i=0; i< bodiesMenisciiList.size(); ++i ) { // i is the index (or id) of the body being tested
		CapillaryPhys* cundallInteractionPhysics1=NULL;
		MindlinCapillaryPhys* mindlinInteractionPhysics1=NULL;
		CapillaryPhys* cundallInteractionPhysics2=NULL;
		MindlinCapillaryPhys* mindlinInteractionPhysics2=NULL;
		if ( !bodiesMenisciiList[i].empty() ) {
			lastMeniscus = bodiesMenisciiList[i].end();
			for ( firstMeniscus=bodiesMenisciiList[i].begin(); firstMeniscus!=lastMeniscus; ++firstMeniscus ) { //FOR EACH MENISCUS ON THIS BODY...
				currentMeniscus = firstMeniscus; ++currentMeniscus;
				if (!hertzOn) {
					cundallInteractionPhysics1 = YADE_CAST<CapillaryPhys*>((*firstMeniscus)->phys.get());
					if (i == (*firstMeniscus)->getId1()) angle1=cundallInteractionPhysics1->Delta1;//get angle of meniscus1 on body i
					else angle1=cundallInteractionPhysics1->Delta2;
				}
				else {
					mindlinInteractionPhysics1 = YADE_CAST<MindlinCapillaryPhys*>((*firstMeniscus)->phys.get());
					if (i == (*firstMeniscus)->getId1()) angle1=mindlinInteractionPhysics1->Delta1;//get angle of meniscus1 on body i
					else angle1=mindlinInteractionPhysics1->Delta2;
				}
				for ( ;currentMeniscus!= lastMeniscus; ++currentMeniscus) {//... CHECK FUSION WITH ALL OTHER MENISCII ON THE BODY
					if (!hertzOn) {
						cundallInteractionPhysics2 = YADE_CAST<CapillaryPhys*>((*currentMeniscus)->phys.get());
						if (i == (*currentMeniscus)->getId1()) angle2=cundallInteractionPhysics2->Delta1;//get angle of meniscus2 on body i
						else angle2=cundallInteractionPhysics2->Delta2;
					}
					else {
						mindlinInteractionPhysics2 = YADE_CAST<MindlinCapillaryPhys*>((*currentMeniscus)->phys.get());
						if (i == (*currentMeniscus)->getId1()) angle2=mindlinInteractionPhysics2->Delta1;//get angle of meniscus2 on body i
						else angle2=mindlinInteractionPhysics2->Delta2;
					}
					if (angle1==0 || angle2==0) cerr << "THIS SHOULD NOT HAPPEN!!"<< endl;

					//cerr << "angle1 = " << angle1 << " | angle2 = " << angle2 << endl;

					Vector3r normalFirstMeniscus = YADE_CAST<ScGeom*>((*firstMeniscus)->geom.get())->normal;
					Vector3r normalCurrentMeniscus = YADE_CAST<ScGeom*>((*currentMeniscus)->geom.get())->normal;

					Real normalDot = 0;
					if ((*firstMeniscus)->getId1() ==  (*currentMeniscus)->getId1() ||  (*firstMeniscus)->getId2()  == (*currentMeniscus)->getId2()) normalDot = normalFirstMeniscus.dot(normalCurrentMeniscus);
					else normalDot = - (normalFirstMeniscus.dot(normalCurrentMeniscus));

					Real normalAngle = 0;
					if (normalDot >= 0 ) normalAngle = Mathr::FastInvCos0(normalDot);
					else normalAngle = ((Mathr::PI) - Mathr::FastInvCos0(-(normalDot)));

					if ((angle1+angle2)*Mathr::DEG_TO_RAD > normalAngle) {
						if (!hertzOn) {++(cundallInteractionPhysics1->fusionNumber); ++(cundallInteractionPhysics2->fusionNumber);}//count +1 if 2 meniscii are overlaping
						else {++(mindlinInteractionPhysics1->fusionNumber); ++(mindlinInteractionPhysics2->fusionNumber);}
					}
				}
			}
		}
	}
}

MeniscusParameters capillarylaw::interpolate(Real R1, Real R2, Real D, Real P, int* index)
{
        if (R1 > R2) {
                Real R3 = R1;
                R1 = R2;
                R2 = R3;
        }

        Real R = R2/R1;
        //cerr << "R = " << R << endl;

        MeniscusParameters result_inf;
        MeniscusParameters result_sup;
        MeniscusParameters result;
        int i = 0;

        for ( ; i < (NB_R_VALUES); i++)
        {
                Real data_R = data_complete[i].R;
                //cerr << "i = " << i << endl;

                if (data_R > R)	// Attention a l'ordre ds lequel vont etre ranges les tableau R (croissant)
                {
                        Tableau& tab_inf=data_complete[i-1];
                        Tableau& tab_sup=data_complete[i];

                        Real r=(R-tab_inf.R)/(tab_sup.R-tab_inf.R);

                        result_inf = tab_inf.Interpolate2(D,P,index[0], index[1]);
                        result_sup = tab_sup.Interpolate2(D,P,index[2], index[3]);

                        result.V = result_inf.V*(1-r) + r*result_sup.V;
                        result.F = result_inf.F*(1-r) + r*result_sup.F;
                        result.delta1 = result_inf.delta1*(1-r) + r*result_sup.delta1;
                        result.delta2 = result_inf.delta2*(1-r) + r*result_sup.delta2;
                        result.nn11 = result_inf.nn11*(1-r) + r*result_sup.nn11;
                        result.nn33 = result_inf.nn33*(1-r) + r*result_sup.nn33;

                        i=NB_R_VALUES;
                        //cerr << "i = " << i << endl;

                }
                else if (data_complete[i].R == R)
                {
                        result = data_complete[i].Interpolate2(D,P, index[0], index[1]);
                        i=NB_R_VALUES;
                }
        }
        return result;
}

Tableau::Tableau()
{}

Tableau::Tableau(const char* filename)

{
        ifstream file (filename);
        file >> R;
        int n_D;	//number of D values
        file >> n_D;

        if (!file.is_open())
	{
		static bool first=true;
		if(first)
		{
	                cout << "WARNING: cannot open files used for capillary law, all forces will be null. Instructions on how to download and install them is found here : https://yade-dem.org/wiki/CapillaryTriaxialTest." << endl;
			first=false;
		}
		return;
	}
        for (int i=0; i<n_D; i++)
                full_data.push_back(TableauD(file));
        file.close();

}

Tableau::~Tableau()
{}

MeniscusParameters Tableau::Interpolate2(Real D, Real P, int& index1, int& index2)

{	//cerr << "interpolate2" << endl;
        MeniscusParameters result;
        MeniscusParameters result_inf;
        MeniscusParameters result_sup;

        for ( unsigned int i=0; i < full_data.size(); ++i) // loop over D values
        {
                if (full_data[i].D > D )	// ok si D rang�s ds l'ordre croissant

                {
                        Real rD = (D-full_data[i-1].D)/(full_data[i].D-full_data[i-1].D);

                        result_inf = full_data[i-1].Interpolate3(P, index1);
                        result_sup = full_data[i].Interpolate3(P, index2);

                        result.V = result_inf.V*(1-rD) + rD*result_sup.V;
                        result.F = result_inf.F*(1-rD) + rD*result_sup.F;
                        result.delta1 = result_inf.delta1*(1-rD) + rD*result_sup.delta1;
                        result.delta2 = result_inf.delta2*(1-rD) + rD*result_sup.delta2;
                        result.nn11 = result_inf.nn11*(1-rD) + rD*result_sup.nn11;
                        result.nn33 = result_inf.nn33*(1-rD) + rD*result_sup.nn33;

                        i = full_data.size();
                }
                else if (full_data[i].D == D)
                {
                        result=full_data[i].Interpolate3(P, index1);

                        i=full_data.size();
                }

        }
        return result;
}

TableauD::TableauD()
{}

TableauD::TableauD(ifstream& file)
{
        int i=0;
        Real x;
        int n_lines;	//pb: n_lines is real!!!
        file >> n_lines;

        file.ignore(200, '\n'); // saute les caract�res (200 au maximum) jusque au caract�re \n (fin de ligne)*_

        if (n_lines!=0)
                for (; i<n_lines; ++i) {
                        data.push_back(vector<Real> ());
                        for (int j=0; j < 8; ++j)	// [D,P,V,F,delta1,delta2,nn11,nn33]
                        {
                                file >> x;
                                data[i].push_back(x);
                        }
                }
        else
                LOG_ERROR("Problem regarding the capillary file structure (e.g. n_D is not consistent with the actual data), and/or mismatch with the expected structure by the code ! Will segfault");
        D = data[i-1][0];
}

MeniscusParameters TableauD::Interpolate3(Real P, int& index)
{	//cerr << "interpolate3" << endl;
        MeniscusParameters result;
        int dataSize = data.size();

        if (index < dataSize && index>0)
        {
        	if (data[index][1] >= P && data[index-1][1] < P)
        	{
        		//compteur1+=1;
        		Real Pinf=data[index-1][1];
                        Real Finf=data[index-1][3];
                        Real Vinf=data[index-1][2];
                        Real Delta1inf=data[index-1][4];
                        Real Delta2inf=data[index-1][5];
                        Real nn11inf = data[index-1][6];
                        Real nn33inf = data[index-1][7];

                        Real Psup=data[index][1];
                        Real Fsup=data[index][3];
                        Real Vsup=data[index][2];
                        Real Delta1sup=data[index][4];
                        Real Delta2sup=data[index][5];
                        Real nn11sup = data[index][6];
                        Real nn33sup = data[index][7];

                        result.V = Vinf+((Vsup-Vinf)/(Psup-Pinf))*(P-Pinf);
                        result.F = Finf+((Fsup-Finf)/(Psup-Pinf))*(P-Pinf);
                        result.delta1 = Delta1inf+((Delta1sup-Delta1inf)/(Psup-Pinf))*(P-Pinf);
                        result.delta2 = Delta2inf+((Delta2sup-Delta2inf)/(Psup-Pinf))*(P-Pinf);
                        result.nn11 = nn11inf + (nn11sup - nn11inf) / (Psup-Pinf) * (P-Pinf);
                        result.nn33 = nn33inf + (nn33sup - nn33inf) / (Psup-Pinf) * (P-Pinf);
                        return result;

        	}
        }
	//compteur2+=1;
        for (int k=1; k < dataSize; ++k) 	// Length(data) ??

        {
                if ( data[k][1] > P) 	// OK si P rangés ds l'ordre croissant

                {
                        Real Pinf=data[k-1][1];
                        Real Finf=data[k-1][3];
                        Real Vinf=data[k-1][2];
                        Real Delta1inf=data[k-1][4];
                        Real Delta2inf=data[k-1][5];
                        Real nn11inf = data[k-1][6];
                        Real nn33inf = data[k-1][7];

                        Real Psup=data[k][1];
                        Real Fsup=data[k][3];
                        Real Vsup=data[k][2];
                        Real Delta1sup=data[k][4];
                        Real Delta2sup=data[k][5];
                        Real nn11sup = data[k][6];
                        Real nn33sup = data[k][7];

                        result.V = Vinf+((Vsup-Vinf)/(Psup-Pinf))*(P-Pinf);
                        result.F = Finf+((Fsup-Finf)/(Psup-Pinf))*(P-Pinf);
                        result.delta1 = Delta1inf+((Delta1sup-Delta1inf)/(Psup-Pinf))*(P-Pinf);
                        result.delta2 = Delta2inf+((Delta2sup-Delta2inf)/(Psup-Pinf))*(P-Pinf);
                        result.nn11 = nn11inf + (nn11sup - nn11inf) / (Psup-Pinf) * (P-Pinf);
                        result.nn33 = nn33inf + (nn33sup - nn33inf) / (Psup-Pinf) * (P-Pinf);
                        index = k;

                        k=dataSize;
                }
                else if (data[k][1] == P)

                {
                        result.V = data[k][2];
                        result.F = data[k][3];
                        result.delta1 = data[k][4];
                        result.delta2 = data[k][5];
                        result.nn11= data[k][6];
                        result.nn33= data[k][7];
                        index = k;

                        k=dataSize;
                }

        }
        return result;
}

TableauD::~TableauD()
{}

std::ostream& operator<<(std::ostream& os, Tableau& T)
{
        os << "Tableau : R=" << T.R << endl;
        for (unsigned int i=0; i<T.full_data.size(); i++) {
                os << "TableauD : D=" << T.full_data[i].D << endl;
                for (unsigned int j=0; j<T.full_data[i].data.size();j++) {
                        for (unsigned int k=0; k<T.full_data[i].data[j].size(); k++)
                                os << T.full_data[i].data[j][k] << " ";
                        os << endl;
                }
        }
        os << endl;
        return os;
}

//what is this function for? it is never called...
//TODO: remove?
BodiesMenisciiList::BodiesMenisciiList(Scene * scene, bool hertzOn)
{
	initialized=false;
	prepare(scene, hertzOn);
}


bool BodiesMenisciiList::prepare(Scene * scene, bool hertzOn)
{
	//cerr << "preparing bodiesInteractionsList" << endl;
	interactionsOnBody.clear();
	shared_ptr<BodyContainer>& bodies = scene->bodies;

	Body::id_t MaxId = -1;
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		MaxId=max(MaxId, (*bi)->getId());
	}
	interactionsOnBody.resize(MaxId+1);
	for ( unsigned int i=0; i<interactionsOnBody.size(); ++i )
	{
		interactionsOnBody[i].clear();
	}
	
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){ // parallel version using Engine::ompThreads variable not accessible, this function is not one of the Engine..
                if (I->isReal()) {
			if (!hertzOn) {if (static_cast<CapillaryPhys*>(I->phys.get())->meniscus) insert(I);}
			else {if (static_cast<MindlinCapillaryPhys*>(I->phys.get())->meniscus) insert(I);}
                }
        }

	return initialized=true;
}

bool BodiesMenisciiList::insert(const shared_ptr< Interaction >& interaction)
{
  checkLengthBuffer(interaction);
	interactionsOnBody[interaction->getId1()].push_back(interaction);
	interactionsOnBody[interaction->getId2()].push_back(interaction);
	return true;
}


bool BodiesMenisciiList::remove(const shared_ptr< Interaction >& interaction)
{
  checkLengthBuffer(interaction);
	interactionsOnBody[interaction->getId1()].remove(interaction);
	interactionsOnBody[interaction->getId2()].remove(interaction);
	return true;
}

void BodiesMenisciiList::checkLengthBuffer(const shared_ptr<Interaction>& interaction) {
	Body::id_t maxBodyId = std::max(interaction->getId1(), interaction->getId2());
	if (unsigned(maxBodyId) >= interactionsOnBody.size()) {
		interactionsOnBody.resize(maxBodyId+1);
	}
}

list< shared_ptr<Interaction> >&  BodiesMenisciiList::operator[] (int index)
{
	return interactionsOnBody[index];
}

int BodiesMenisciiList::size()
{
	return interactionsOnBody.size();
}

void BodiesMenisciiList::display()
{
	list< shared_ptr<Interaction> >::iterator firstMeniscus;
	list< shared_ptr<Interaction> >::iterator lastMeniscus;
	for ( unsigned int i=0; i<interactionsOnBody.size(); ++i )
	{
		if ( !interactionsOnBody[i].empty() )
		{
			lastMeniscus = interactionsOnBody[i].end();
			//cerr << "size = "<<interactionsOnBody[i].size() << " empty="<<interactionsOnBody[i].empty() <<endl;
			for ( firstMeniscus=interactionsOnBody[i].begin(); firstMeniscus!=lastMeniscus; ++firstMeniscus )
			{
				if ( *firstMeniscus ){
					if ( firstMeniscus->get() )
						cerr << "(" << ( *firstMeniscus )->getId1() << ", " << ( *firstMeniscus )->getId2() <<") ";
					else cerr << "(void)";
				}
			}
			cerr << endl;
		}
		else cerr << "empty" << endl;
	}
}

BodiesMenisciiList::BodiesMenisciiList()
{
	initialized=false;
}

