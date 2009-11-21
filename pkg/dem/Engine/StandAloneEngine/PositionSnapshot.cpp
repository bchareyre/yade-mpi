/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PositionSnapshot.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;



PositionSnapshot::PositionSnapshot () : DataRecorder()
{
	outputFile = "";
	list_id.resize(0);
	i=0;
}


PositionSnapshot::~PositionSnapshot ()
{

}


void PositionSnapshot::postProcessAttributes(bool deserializing)
{

}



bool PositionSnapshot::isActivated(MetaBody*)
{
	vector<int>::const_iterator ii = list_id.begin();
	vector<int>::const_iterator iiEnd = list_id.end();
	bool booleen = false;	// booleen qui sera renvoyé a la fin de la methode. Sera mis a vrai si necessaire
	for(;ii!=iiEnd;++ii)
		if(*ii==Omega::instance().getCurrentIteration())  // check if current Id is one of them for which a snapshot is required
			booleen=true;
	if(booleen)
		i++;

	return (booleen);
}



void PositionSnapshot::action(MetaBody * ncb)
{
// 	copie de outputFile dans ce qui sera utilise pour le nom des fichiers resultats
	nom_int=outputFile;
	//ouverture d'un fichier correspondant au Snapshot courant
	myfile.open(((nom_int.append("_")).append(lexical_cast<string>(i))).c_str());
	if(!(myfile.is_open()))
		cerr << "PROBLEME D OUVERTURE DU FICHIER RESULTAT\n";
		
	// first line of output file
	myfile << "Id Rayon Xc Yc Zc   a l'it " << Omega::instance().getCurrentIteration() << endl;

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	Vector3r pos(Vector3r::ZERO);	// which will be the position (of the center) of the considered body
	body_id_t Id;			// its identity

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->isDynamic)
		{
			Id=b->getId();
			pos=(b->physicalParameters.get())->se3.position;
			Real rayon = YADE_CAST<InteractingSphere*>(b->interactingGeometry.get())->radius;
			myfile << lexical_cast<string>(Id) << " " << lexical_cast<string>(rayon) << " " << lexical_cast<string>(pos[0]) << " " << lexical_cast<string>(pos[1]) << " " << lexical_cast<string>(pos[2]) << endl;
		}
	}
// 	fermeture du fichier
	myfile.close();

}



YADE_REQUIRE_FEATURE(PHYSPAR);

