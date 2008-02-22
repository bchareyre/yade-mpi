/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryCohesiveLaw.hpp"
#include <yade/pkg-dem/BodyMacroParameters.hpp>
#include <yade/pkg-dem/SpheresContactGeometry.hpp>

#include <yade/pkg-dem/CapillaryParameters.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>
#include <yade/pkg-common/Force.hpp>
#include <yade/pkg-common/Momentum.hpp>
#include <yade/core/PhysicalAction.hpp>
#include <Wm3Vector3.h>
#include <yade/lib-base/yadeWm3.hpp>

#include <yade/core/GeometricalModel.hpp>


#include <iostream>
#include <fstream>

using namespace std;

//int compteur1 = 0;
//int compteur2 = 0;

CapillaryCohesiveLaw::CapillaryCohesiveLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
        sdecGroupMask=1;

        capillary = shared_ptr<capillarylaw>(new capillarylaw); // ????????

        //importFilename 		= "."; // chemin vers M(r=...)

        capillary->fill("M(r=1)");
        capillary->fill("M(r=1.1)");
        capillary->fill("M(r=1.25)");
        capillary->fill("M(r=1.5)");
        capillary->fill("M(r=1.75)");
        capillary->fill("M(r=2)");
        capillary->fill("M(r=3)");
        capillary->fill("M(r=4)");
        capillary->fill("M(r=5)");
        capillary->fill("M(r=10)");

        CapillaryPressure=0;

}


void CapillaryCohesiveLaw::registerAttributes()
{
        InteractionSolver::registerAttributes();
        REGISTER_ATTRIBUTE(sdecGroupMask);
        REGISTER_ATTRIBUTE(CapillaryPressure);

}

Parameters::Parameters()
{
        V = 0;
        F = 0;
        delta1 = 0;
        delta2 = 0;
};

Parameters::Parameters(const Parameters &source)
{
        V = source.V;
        F = source.F;
        delta1 = source.delta1;
        delta2 = source.delta2;
}

Parameters::~Parameters()
{}
;

//FIXME : remove bool first !!!!!
void CapillaryCohesiveLaw::action(Body* body)
{

//compteur1 = 0;
//compteur2 = 0;
        // 	cerr << "CapillaryCohesiveLaw::action" << endl;

        MetaBody * ncb = static_cast<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;


        /// Non Permanents Links ///

        //InteractionContainer::iterator ii    = ncb->persistentInteractions->begin();
        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        //InteractionContainer::iterator iiEnd = ncb->persistentInteractions->end();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();

        /// initialisation du volume avant calcul
        //Real Vtotal = 0;

        for(  ; ii!=iiEnd ; ++ii ) {

                if ((*ii)->isReal) {

                        const shared_ptr<Interaction>& interaction = *ii;
                        unsigned int id1 = interaction->getId1();
                        unsigned int id2 = interaction->getId2();

                        if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
                                continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword

                        /// interaction geometry search

                        int geometryIndex1 = (*bodies)[id1]->geometricalModel->getClassIndex(); // !!!
                        //cerr << "geo1 =" << geometryIndex1 << endl;
                        int geometryIndex2 = (*bodies)[id2]->geometricalModel->getClassIndex();
                        //cerr << "geo2 =" << geometryIndex2 << endl;

                        if (!(geometryIndex1 == geometryIndex2))
                                continue;

                        /// definition of interacting objects (not in contact)

                        BodyMacroParameters* de1 		=
                                static_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
                        BodyMacroParameters* de2 		=
                                static_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());

                        SpheresContactGeometry* currentContactGeometry 	=
                                static_cast<SpheresContactGeometry*>(interaction->interactionGeometry.get());

                        //SDECLinkGeometry* currentContactGeometry	= static_cast<SDECLinkGeometry*>(interaction->interactionGeometry.get());

                        CapillaryParameters* currentContactPhysics 	=
                                static_cast<CapillaryParameters*>(interaction->interactionPhysics.get());

                        //SDECLinkPhysics* currentContactPhysics	= static_cast<SDECLinkPhysics*>(interaction->interactionPhysics.get());

                        //                         CapillaryParameters* meniscusParameters
                        // = static_cast<CapillaryParameters*>(interaction->interactionPhysics.get());

                        /// Capillary components definition:

                        Real liquidTension = 0.073; 	// superficial water tension N/m (20°C)

                        //Real teta = 0;		// mouillage parfait (eau pure/billes de verre)

                        /// Interacting Grains:
                        // definition du rapport tailleReelle/TailleYADE

                        Real alpha=1; // OK si pas de gravite!!!

                        Real R1 = 0;
                        R1=alpha*std::min(currentContactGeometry->radius2,currentContactGeometry->
                                          radius1 ) ;
                        Real R2 = 0;
                        R2=alpha*std::max(currentContactGeometry->radius2,currentContactGeometry->
                                          radius1 ) ;
                        //cerr << "R1 = " << R1 << " R2 = "<< R2 << endl;

                        /// intergranular distance

                        //Real intergranularDistance = currentContactGeometry->penetrationDepth;

                        Real D =
                                alpha*(de2->se3.position-de1->se3.position).Length()-alpha*(
                                        currentContactGeometry->radius1+ currentContactGeometry->radius2);
                     

                        if
                        ((currentContactGeometry->penetrationDepth>=0)||(D<=0)||CapillaryPressure<=300||
                                        (Omega::instance().getCurrentIteration() < 2) ) 
			{ 
                                D = 0;	// def Fcap when spheres interpenetrate
                                currentContactPhysics->meniscus=true;
                                //meniscusParameters->meniscus=true;

                        }


                        Real Dinterpol = D/R2;

                        /// Suction (Capillary pressure):

                        Real Pinterpol =CapillaryPressure*(R2/liquidTension);
                        currentContactPhysics->CapillaryPressure = CapillaryPressure;

                        //Real r = R2/R1;

                        /// Capillary solution finder:

                        //cerr << "solution finder " << endl;

                        if ((Pinterpol>=0) && (currentContactPhysics->meniscus==true))

                        {	//cerr << "Pinterpol = "<< Pinterpol << endl;

                                Parameters
                                solution(capillary->Interpolate(R1,R2,
                                                                Dinterpol, Pinterpol, currentContactPhysics->currentIndexes));

                                /// capillary adhesion force

                                Real Finterpol = solution.F;
                                Vector3r Fcap =
                                        Finterpol*(2*Mathr::PI*(R2/alpha)*liquidTension)*currentContactGeometry->
                                        normal; /// unitï¿½s !!!

                                currentContactPhysics->Fcap = Fcap;

                                /// meniscus volume

                                Real Vinterpol = solution.V;
                                currentContactPhysics->Vmeniscus =
                                        Vinterpol*(R2*R2*R2)/(alpha*alpha*alpha);
                                
				if (currentContactPhysics->Vmeniscus != 0)
                                {
                                        currentContactPhysics->meniscus = true;
                                } else
                                {
                                        currentContactPhysics->meniscus = false;
                                }

                                /// wetting angles
                                currentContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                                currentContactPhysics->Delta2 = min(solution.delta1,solution.delta2);

                                static_cast<Force*>   (ncb->physicalActions->find( id1 , actionForce  ->getClassIndex()).get())->force    += Fcap;
                                static_cast<Force*>   (ncb->physicalActions->find( id2 , actionForce  ->getClassIndex()).get())->force    -= Fcap;

                                currentContactPhysics->prevNormal = currentContactGeometry->normal;

                        }
                }
        }
//cerr << "compteur1=" << compteur1 << "; compteur2=" << compteur2 << endl;
}

capillarylaw::capillarylaw()
{}

void capillarylaw::fill(const char* filename)

{
        data_complete.push_back(Tableau(filename));

}

Parameters capillarylaw::Interpolate(Real R1, Real R2, Real D, Real P, int* index)

{	//cerr << "interpolate" << endl;
        if (R1 > R2) {
                Real R3 = R1;
                R1 = R2;
                R2 = R3;
        }

        Real R = R2/R1;
        //cerr << "R = " << R << endl;

        Parameters result_inf;
        Parameters result_sup;
        Parameters result;
        int i = 0;

        for ( ; i < (NB_R_VALUES); i++)
        {
                Real data_R = data_complete[i].R;
                //cerr << "i = " << i << endl;

                if (data_R > R)	// Attention ï¿½ l'ordre ds lequel
                        //vont ï¿½tre rangï¿½s les tableau R (croissant)

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

                        i=NB_R_VALUES;
                        //cerr << "i = " << i << endl;

                }
                else if (data_complete[i].R == R)
                {
                        result = data_complete[i].Interpolate2(D,P, index[0], index[1]);
                        i=NB_R_VALUES;
                        //cerr << "i = " << i << endl;
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
        //cerr << "r = " << R << endl;
        int n_D;	//number of D values
        file >> n_D;

        if (!file.is_open())
	{
		static bool first=true;
		if(first)
		{
	                cout << "WARNING: cannot open file used for capillary law, in TriaxalTestWater" << endl;
			first=false;
		}
		return;
	}
        for (int i=0; i<n_D; i++)
                full_data.push_back(TableauD(file));
        file.close();
        //cerr << *this;	// exemple d'utilisation de la fonction d'ï¿½criture (this est le pointeur vers l'objet courant)
}

Tableau::~Tableau()
{}

Parameters Tableau::Interpolate2(Real D, Real P, int& index1, int& index2)

{	//cerr << "interpolate2" << endl;
        Parameters result;
        Parameters result_inf;
        Parameters result_sup;

        for ( unsigned int i=0; i < full_data.size(); ++i)
        {
                if (full_data[i].D > D )	// ok si D rangï¿½s ds l'ordre croissant

                {
                        Real rD = (D-full_data[i-1].D)/(full_data[i].D-full_data[i-1].D);

                        result_inf = full_data[i-1].Interpolate3(P, index1);
                        result_sup = full_data[i].Interpolate3(P, index2);

                        result.V = result_inf.V*(1-rD) + rD*result_sup.V;
                        result.F = result_inf.F*(1-rD) + rD*result_sup.F;
                        result.delta1 = result_inf.delta1*(1-rD) + rD*result_sup.delta1;
                        result.delta2 = result_inf.delta2*(1-rD) + rD*result_sup.delta2;

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
{} // ?? constructeur

TableauD::TableauD(ifstream& file)

{
        int i=0;
        Real x;
        int n_lines;	//pb: n_lines is real!!!
        file >> n_lines;
        //cout << n_lines << endl;

        file.ignore(200, '\n'); // saute les caractï¿½res (200 au maximum) jusque au caractï¿½re \n (fin de ligne)*_

        if (n_lines!=0)
                for (; i<n_lines; ++i) {
                        data.push_back(vector<Real> ());
                        for (int j=0; j < 6; ++j)	// [D,P,V,F,delta1,delta2]
                        {
                                file >> x;
                                data[i].push_back(x);
                        }
                }
        D = data[i-1][0];
}

Parameters TableauD::Interpolate3(Real P, int& index)

{	//cerr << "interpolate3" << endl;
        Parameters result;
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

                        Real Psup=data[index][1];
                        Real Fsup=data[index][3];
                        Real Vsup=data[index][2];
                        Real Delta1sup=data[index][4];
                        Real Delta2sup=data[index][5];

                        result.V = Vinf+((Vsup-Vinf)/(Psup-Pinf))*(P-Pinf);
                        result.F = Finf+((Fsup-Finf)/(Psup-Pinf))*(P-Pinf);
                        result.delta1 = Delta1inf+((Delta1sup-Delta1inf)/(Psup-Pinf))*(P-Pinf);
                        result.delta2 = Delta2inf+((Delta2sup-Delta2inf)/(Psup-Pinf))*(P-Pinf);
                        return result;
        		
        	}
        }
	//compteur2+=1;	
        for (int k=1; k < dataSize; ++k) 	// Length(data) ??

        {	//cerr << "k = " << k << endl;
                if ( data[k][1] > P) 	// OK si P rangÃ©s ds l'ordre croissant

                {	//cerr << "if" << endl;
                        Real Pinf=data[k-1][1];
                        Real Finf=data[k-1][3];
                        Real Vinf=data[k-1][2];
                        Real Delta1inf=data[k-1][4];
                        Real Delta2inf=data[k-1][5];

                        Real Psup=data[k][1];
                        Real Fsup=data[k][3];
                        Real Vsup=data[k][2];
                        Real Delta1sup=data[k][4];
                        Real Delta2sup=data[k][5];

                        result.V = Vinf+((Vsup-Vinf)/(Psup-Pinf))*(P-Pinf);
                        result.F = Finf+((Fsup-Finf)/(Psup-Pinf))*(P-Pinf);
                        result.delta1 = Delta1inf+((Delta1sup-Delta1inf)/(Psup-Pinf))*(P-Pinf);
                        result.delta2 = Delta2inf+((Delta2sup-Delta2inf)/(Psup-Pinf))*(P-Pinf);
                        index = k;

                        k=dataSize;
                }
                else if (data[k][1] == P)

                {	//cerr << "elseif" << endl;
                        result.V = data[k][2];
                        result.F = data[k][3];
                        result.delta1 = data[k][4];
                        result.delta2 = data[k][5];
                        index = k;

                        k=dataSize;
                }

        }
        return result;
}

TableauD::~TableauD()
{} // ?? destructeur

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
YADE_PLUGIN();
