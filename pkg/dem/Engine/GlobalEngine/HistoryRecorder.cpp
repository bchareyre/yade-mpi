/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  Vincent.Richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "HistoryRecorder.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include <boost/lexical_cast.hpp>

#include <yade/pkg-common/Sphere.hpp>
#include <yade/pkg-common/Box.hpp>

#include <yade/pkg-dem/ScGeom.hpp>
#include <yade/pkg-common/NormalShearInteractions.hpp>

HistoryRecorder::HistoryRecorder () : DataRecorder()
{
  outputBase = "spl_nwk_";
  interval   = 50;
  stateId    = 0;
}


HistoryRecorder::~HistoryRecorder ()
{

}


void HistoryRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		
	}
}



bool HistoryRecorder::isActivated(Scene*)
{
	return ((Omega::instance().getCurrentIteration() % interval == 0));
}



void HistoryRecorder::action(Scene * ncb)
{
  shared_ptr<BodyContainer>& bodies = ncb->bodies;

  std::string outputFile = outputBase + lexical_cast<string>(stateId++) + ".his";
  std::ofstream ofs(outputFile.c_str());
  cout << "Saving " << outputFile << "..." << endl;
   ofs << "Sample{" << endl;

   BodyContainer::iterator bi    = bodies->begin();
   BodyContainer::iterator biEnd = bodies->end();
   for( ; bi != biEnd ; ++bi)
   {
     const shared_ptr<Body>& b = *bi;
	 if (b->isClump()) continue;
	 
     const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>(b->physicalParameters.get());
	 const Shape* gm   = YADE_CAST<Shape*>(b->shape.get());

	if ( typeid(*gm) == typeid(Sphere) )
     {
		ofs << "sphere " << b->groupMask << " " << YADE_CAST<Sphere*>(b->shape.get())->radius
		<< " " << p->se3.position[0] << " " << p->se3.position[1] << " " << p->se3.position[2]
		<< " 0 0 0" 
		<< " " << p->velocity[0] << " " << p->velocity[1] << " " << p->velocity[2]
		<< " " << p->angularVelocity[0] << " " << p->angularVelocity[1] << " " << p->angularVelocity[2]
		<< endl << flush;
	 }

	if ( typeid(*gm) == typeid(Box) )
	{
	  ofs << "box " << b->groupMask << " " << YADE_CAST<Box*>(b->shape.get())->extents
	  << " " << p->se3.position[0] << " " << p->se3.position[1] << " " << p->se3.position[2]
	  << " 0 0 0"
	  << " " << p->velocity[0] << " " << p->velocity[1] << " " << p->velocity[2]
	  << " " << p->angularVelocity[0] << " " << p->angularVelocity[1] << " " << p->angularVelocity[2]
	  << endl << flush;
	}

   } 
   ofs << "}" << endl << endl;
   
   ofs << "Network{" << endl;
   
   InteractionContainer::iterator ii    = ncb->interactions->begin();
   InteractionContainer::iterator iiEnd = ncb->interactions->end();
   
   for( ;ii!=iiEnd;++ii)
   {
	 if ((*ii)->isReal()) // to be claryfied...
	 {
	   const shared_ptr<Interaction>& contact = *ii;

	   const InteractionGeometry* ig = YADE_CAST<InteractionGeometry*>(contact->interactionGeometry.get());
	   
	   body_id_t id1 = contact->getId1();
	   body_id_t id2 = contact->getId2();
	   
	   shared_ptr<Body> b1=(*bodies)[id1];
	   shared_ptr<Body> b2=(*bodies)[id2];

	   if( typeid(*ig) == typeid(ScGeom) )
	   {
		 
		 if (typeid(*(b1->shape.get())) == typeid(Sphere)
		  && typeid(*(b2->shape.get())) == typeid(Sphere))
		 {
		   const NormalShearInteraction* nsi = YADE_CAST<NormalShearInteraction*>(contact->interactionPhysics.get());
		   Vector3r n = nsi->normalForce;
		   Real fn = n.Length();

		   Vector3r t = nsi->shearForce;
		   Real ft = t.Length();
		   
		   // remark: This is not exactly the format used in gdm-tk
		   ofs << "spsp " << id1 << " " << id2 << " " << n << " " << t << " "
			   << fn << " " << ft << " 0 0 0 0"
		       << endl;
		 }

	   }
	 }
   }

  ofs << "}" << endl << endl;
   
}





YADE_REQUIRE_FEATURE(PHYSPAR);

