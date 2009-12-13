/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include<yade/pkg-common/ElasticMat.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>

#include<yade/pkg-dem/ElasticContactInteraction.hpp>

#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

#include <yade/pkg-common/InteractingSphere.hpp>

#include "MicroMacroAnalyser.hpp"

#include<yade/lib-triangulation/TesselationWrapper.h>
#include<yade/lib-triangulation/KinematicLocalisationAnalyser.hpp>
#include<yade/lib-triangulation/TriaxialState.h>

YADE_PLUGIN((MicroMacroAnalyser));
YADE_REQUIRE_FEATURE(CGAL)

CREATE_LOGGER(MicroMacroAnalyser);

MicroMacroAnalyser::MicroMacroAnalyser() : GlobalEngine()
{
	analyser = shared_ptr<CGT::KinematicLocalisationAnalyser> (new CGT::KinematicLocalisationAnalyser);
	analyser->SetConsecutive (true);
	analyser->SetNO_ZERO_ID (false);
	interval = 100;
	outputFile = "MicroMacroAnalysis";
	stateFileName = "./snapshots/state";
}

void MicroMacroAnalyser::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		bool file_exists = std::ifstream (outputFile.c_str()); //if file does not exist, we will write colums titles
		ofile.open(outputFile.c_str(), std::ios::app);
		if (!file_exists) ofile<<"iteration eps1w eps2w eps3w eps11g eps22g eps33g eps12g eps13g eps23g"<< endl;
	}
}

void MicroMacroAnalyser::action(Scene* ncb)
{
	//cerr << "MicroMacroAnalyser::action(Scene* ncb) (interval="<< interval <<", iteration="<< Omega::instance().getCurrentIteration()<<")" << endl;
	if (Omega::instance().getCurrentIteration() == 0) setState(ncb, 1);
	else if (Omega::instance().getCurrentIteration() % interval == 0) {
		setState(ncb, 2, true, true);
		analyser->ComputeParticlesDeformation();
		CGT::Tenseur_sym3 epsg ( analyser->grad_u_total );
		ofile << Omega::instance().getCurrentIteration() << analyser->Delta_epsilon(1,1)<<" "<<analyser->Delta_epsilon(2,2)<<" "<<analyser->Delta_epsilon(3,3)<<" "<<epsg(1,1)<<" "<<epsg(2,2)<< " "<<epsg(3,3)<<" "<<epsg(1,2)<<" "<<epsg(1,3)<<" "<<epsg(2,3)<<endl;
		analyser->SwitchStates();
	}
	//cerr << "ENDOF MicroMacro::action" << endl;
}



void MicroMacroAnalyser::setState ( Scene* ncb, unsigned int state, bool saveStates, bool computeIncrement )
{
	LOG_INFO ("MicroMacroAnalyser::setState");
	if ( !triaxialCompressionEngine )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
		for ( ;itFirst!=itLast; ++itFirst )
		{
			if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine" ) //|| (*itFirst)->getBaseClassName() == "TriaxialCompressionEngine")
			{
				LOG_DEBUG ( "stress controller engine found" );
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );
				//triaxialCompressionEngine = shared_ptr<TriaxialCompressionEngine> (static_cast<TriaxialCompressionEngine*> ( (*itFirst).get()));
			}
		}
		if ( !triaxialCompressionEngine ) LOG_ERROR ( "stress controller engine not found" );
	}

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	CGT::TriaxialState* ts;
	if ( state==1 ) ts = analyser->TS0;
	else if ( state==2 ) ts = analyser->TS1;
	else LOG_ERROR ( "state must be 1 or 2, instead of " << state );
	CGT::TriaxialState& TS = *ts;

	TS.reset();

	long Ng = bodies->size();
	//cerr << "Ng= " << Ng << endl;
	//Statefile >> Ng;
	//Real x, y, z, rad; //coordonn�es/rayon
	//Real tx, ty, tz;
	//Point pos;
	TS.mean_radius=0;
	//Vecteur trans, rot;
	//Real rad; //coordonn�es/rayon

	TS.grains.resize ( Ng );
	//cerr << "Ngrains =" << Ng << endl;
	//GrainIterator git= grains.begin();
//  git->id=0;
//  git->sphere = Sphere(CGAL::ORIGIN, 0);
//  git->translation = CGAL::NULL_VECTOR;
//  git->rotation = CGAL::NULL_VECTOR;

	BodyContainer::iterator biBegin    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	BodyContainer::iterator bi = biBegin;
	Ng = 0;

	
	for ( ; bi!=biEnd ; ++bi )
	{
		body_id_t Idg = ( *bi )->getId();
		TS.grains[Idg].id = Idg;
		
		if ( !( *bi )->isDynamic ) TS.grains[Idg].isSphere = false;
		else {//then it is a sphere (not a wall)
			++Ng;
			const InteractingSphere* s = YADE_CAST<InteractingSphere*> ( ( *bi )->shape.get() );
			//const GranularMat* p = YADE_CAST<GranularMat*> ( ( *bi )->material.get() );
			const Vector3r& pos = (*bi)->state->pos;
			Real rad = s->radius;

			TS.grains[Idg].sphere = CGT::Sphere ( CGT::Point ( pos[0],pos[1],pos[2] ),  rad );
//    TS.grains[Idg].translation = trans;
//    grains[Idg].rotation = rot;
			TS.box.base = CGT::Point ( min ( TS.box.base.x(), pos.X()-rad ),
								  min ( TS.box.base.y(), pos.Y()-rad ),
								  min ( TS.box.base.z(), pos.Z()-rad ) );
			TS.box.sommet = CGT::Point ( max ( TS.box.sommet.x(), pos.X() +rad ),
									max ( TS.box.sommet.y(), pos.Y() +rad ),
									max ( TS.box.sommet.z(), pos.Z() +rad ) );
			TS.mean_radius += TS.grains[Idg].sphere.weight();


		}
	}

	TS.mean_radius /= Ng;//rayon moyen
	LOG_INFO ( " loaded : " << Ng << " grains with mean radius = " << TS.mean_radius );

	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for ( ; ii!=iiEnd ; ++ii )
	{
		if ( ( *ii )->isReal() )
		{
			CGT::TriaxialState::Contact *c = new CGT::TriaxialState::Contact;
			TS.contacts.push_back ( c );
			CGT::TriaxialState::VectorGrain& grains = TS.grains;
			body_id_t id1 = ( *ii )->getId1();
			body_id_t id2 = ( *ii )->getId2();

			c->grain1 = & ( TS.grains[id1] );
			c->grain2 = & ( TS.grains[id2] );
			grains[id1].contacts.push_back ( c );
			grains[id2].contacts.push_back ( c );
			c->normal = CGT::Vecteur ( 
					 ( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->normal.X(),
					 ( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->normal.Y(),
					 ( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->normal.Z() );
// 			c->normal = ( grains[id2].sphere.point()-grains[id1].sphere.point() );
// 			c->normal = c->normal/sqrt ( pow ( c->normal.x(),2 ) +pow ( c->normal.y(),2 ) +pow ( c->normal.z(),2 ) );
			c->position = CGT::Vecteur ( 
					( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->contactPoint.X(),
					( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->contactPoint.Y(),
					( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->contactPoint.Z() );
// 			c->position = 0.5* ( ( grains[id1].sphere.point()-CGAL::ORIGIN ) +
// 								 ( grains[id1].sphere.weight() *c->normal ) +
// 								 ( grains[id2].sphere.point()-CGAL::ORIGIN ) -
// 								 ( grains[id2].sphere.weight() *c->normal ) );


			c->fn = YADE_CAST<ElasticContactInteraction*> ( ( ( *ii )->interactionPhysics.get() ) )->normalForce.Dot ( ( YADE_CAST<SpheresContactGeometry*> ( ( *ii )->interactionGeometry.get() ) )->normal );
			Vector3r fs = YADE_CAST<ElasticContactInteraction*> ( ( *ii )->interactionPhysics.get() )->shearForce;
			c->fs = CGT::Vecteur ( fs.X(),fs.Y(),fs.Z() );
			c->old_fn = c->fn;
			c->old_fs = c->fs;
			c->frictional_work = 0;
		}
	}

	//Load various parameters

	//rfric = find_parameter("rfric=", Statefile);// � remettre quand les fichiers n'auront plus l'espace de trop...
//  Eyn = find_parameter("Eyn=", Statefile);
//  Eys = find_parameter("Eys=", Statefile);
	TS.wszzh = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1];//find_parameter("wszzh=", Statefile);
	TS.wsxxd = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0];//find_parameter("wsxxd=", Statefile);
	TS.wsyyfa = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2];//find_parameter("wsyyfa=", Statefile);
	TS.eps3 = triaxialCompressionEngine->strain[2];//find_parameter("eps3=", Statefile);
	TS.eps1 = triaxialCompressionEngine->strain[0];//find_parameter("eps1=", Statefile);
	TS.eps2 = triaxialCompressionEngine->strain[1];//find_parameter("eps2=", Statefile);

	TS.haut = triaxialCompressionEngine->height;//find_parameter("haut=", Statefile);
	TS.larg = triaxialCompressionEngine->width;//find_parameter("larg=", Statefile);
	TS.prof = triaxialCompressionEngine->depth;//find_parameter("prof=", Statefile);
	TS.porom = analyser->ComputeMacroPorosity();//find_parameter("porom=", Statefile);
	TS.ratio_f = triaxialCompressionEngine-> ComputeUnbalancedForce ( ncb );//find_parameter("ratio_f=", Statefile);
//  vit = find_parameter("vit=", Statefile);

	
	if ( state == 2 && computeIncrement )
	{
		analyser->SetForceIncrements();
		analyser->SetDisplacementIncrements();
	}
	if ( saveStates ) 
	{
		ostringstream oss;
		oss<<stateFileName<<"_"<<Omega::instance().getCurrentIteration();
		TS.to_file ( oss.str().c_str() );
	}
	cerr << "ENDOF MicroMacroAnalyser::setState" << endl;
}





