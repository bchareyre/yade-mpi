/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ForceSnapshot.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;



ForceSnapshot::ForceSnapshot () : DataRecorder()
{
	outputFile = "";
	list_id.resize(0);
	i=0;
}


ForceSnapshot::~ForceSnapshot ()
{

}


void ForceSnapshot::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	}
}



bool ForceSnapshot::isActivated(MetaBody*)
{
	vector<int>::const_iterator ii = list_id.begin();
	vector<int>::const_iterator iiEnd = list_id.end();
	bool booleen = false;	// booleen qui sera renvoyé a la fin de la methode. Sera mis a vrai si necessaire
	for(;ii!=iiEnd;++ii)
		if(*ii==Omega::instance().getCurrentIteration()) // check if current Id is one of them for which a snapshot is required
			booleen=true;
	if(booleen)
		i++;

	return (booleen);
}



void ForceSnapshot::action(MetaBody * ncb)
{
// 	copie de outputFile dans ce qui sera utilise pour le nom des fichiers resultats
	nom_int=outputFile;
	//ouverture d'un fichier correspondant au Snapshot courant
	myfile.open(((nom_int.append("_")).append(lexical_cast<string>(i))).c_str());
	if(!(myfile.is_open()))
		cerr << "PROBLEME D OUVERTURE DU FICHIER RESULTAT\n";
		
	//first line of ouptut file :
	myfile << "Id(body1) Id(body2) Fn R1 x1 y1 z1 R2 x2 y2 z2  a l'it " << Omega::instance().getCurrentIteration() << endl;
// 	that is to say : the ids of the two interacting bodies, the norm of the normal force, and the radii and the position of the centers of the two spheres, and finally the number of the corresponding iteration

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	Vector3r pos(Vector3r::ZERO);	// which will be the position (of the center) of the considered body

	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();

	for( ;ii!=iiEnd;++ii)
	{
		if ((*ii)->isReal())
		{
			const shared_ptr<Interaction>& contact = *ii;
			body_id_t id1 = contact->getId1();
			body_id_t id2 = contact->getId2();

			shared_ptr<Body> b1=(*bodies)[id1]; // the first body in contact of this interaction...
			shared_ptr<Body> b2=(*bodies)[id2]; // the second one...
			
			if( (b1->isDynamic)&&(b2->isDynamic) ) // to not take into account the contacts sphere-wall
			{
				Vector3r pos1=(b1->physicalParameters.get())->se3.position;	// the position of centers
				Vector3r pos2=(b2->physicalParameters.get())->se3.position;

				Real R1 = YADE_CAST<InteractingSphere*>(b1->interactingGeometry.get())->radius;
				Real R2 = YADE_CAST<InteractingSphere*>(b2->interactingGeometry.get())->radius;

				Vector3r Fn=(YADE_CAST<ElasticContactInteraction*>(contact->interactionPhysics.get()))->normalForce;
				Real Fnl=Fn.Length();

				myfile<<lexical_cast<string>(id1)<<" "<<lexical_cast<string>(id2)<<" "<<lexical_cast<string>(Fnl)<<" "<<lexical_cast<string>(R1);
				myfile<<" "<<lexical_cast<string>(pos1[0])<<" "<<lexical_cast<string>(pos1[1])<<" "<< lexical_cast<string>(pos1[2]) <<" "<<lexical_cast<string>(R2);
				myfile<<" "<<lexical_cast<string>(pos2[0])<<" "<<lexical_cast<string>(pos2[1])<<" "<< lexical_cast<string>(pos2[2]) << endl;
			}
		}
	}
// 	fermeture du fichier
	myfile.close();

}



YADE_REQUIRE_FEATURE(PHYSPAR);

