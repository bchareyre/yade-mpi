/*************************************************************************
*  Copyright (C) 2004 by Andrea Cortis & Bruno Chareyre                  *
*  acortis@lbl.gov,   bruno.chareyre@hmg.inpg.fr                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "HydraulicForceEngine.hpp"
#include <yade/pkg-common/ParticleParameters.hpp>
#include <yade/core/MetaBody.hpp>
#include <yade/core/Body.hpp>
#include <yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include <vector>
#include "HydraulicForceEngine.hpp"
#include <yade/pkg-common/ParticleParameters.hpp>
#include <yade/core/MetaBody.hpp>
#include <yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include <vector>
#include <yade/extra/Shop.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

vector<Real> initialPositions;

HydraulicForceEngine::HydraulicForceEngine() : gravity(Vector3r::ZERO), isActivated(true)
{
	dummyParameter = false;
	HFinverted = false;
	savePositions = 1;
	outputFile = "positions.dat";
	inputFile = "forces.dat";
	HFinversion_counter = 1000;
	forceFactor = 1;


}


HydraulicForceEngine::~HydraulicForceEngine()
{
}



void HydraulicForceEngine::applyCondition(MetaBody* ncb)
{

    if (isActivated)
    {
        shared_ptr<BodyContainer>& bodies = ncb->bodies;
        
        
        ///This commented is to artificially remove eroded (i.e. bond-breaked) grains by applying a force moving them away
//cerr << "HFinverted " << HFinverted << " Omega::instance().getTimeStep() " << Omega::instance().getCurrentIteration() << endl;
//         if (HFinversion_counter < (Omega::instance().getCurrentIteration() - 1500))
//         {
//         //cerr << "HFinverted " << HFinverted << " HFinversion_counter " << HFinversion_counter << endl;
//             HFinversion_counter = Omega::instance().getCurrentIteration();
//             HFinverted = !HFinverted;
// //             initialPositions.clear();
// //             for (int i =0; i < initialPositions.size(); ++i)
// //             {
// //             	initialPositions[i-6] = (static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->se3.position[1];
// //             	
// //             }
//             
//             
//         }
//         

        if (HFinverted)
        {
            BodyContainer::iterator bi    = bodies->begin();
            BodyContainer::iterator biEnd = bodies->end();
            for ( ; bi!=biEnd ; ++bi )
            {
                shared_ptr<Body> b = *bi;
                if (b->geometricalModel && b->interactingGeometry && b->geometricalModel->getClassName()=="Sphere")
                {
                    //cerr << "translate it" << endl;
                    if ((static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->isBroken == true)
                    {
								ncb->bex.addForce(b->getId(),Vector3r(0,5,0));
                    }
                    // else  b->geometricalModel->diffuseColor= Vector3r(0.5,0.9,0.3);
                }
            }
        }
        else
        {
            long Nspheres;
            long id;
	    Real temp;
	    Real realID;
            Real fx, fy, fz, mx, my, mz;
            ifstream file (inputFile.c_str());
	    ofstream ofile (outputFile.c_str());
            
            //cerr << "r = " << R << endl;
		#define USELINENUMBERSFORID
            if (file.is_open())
            {
		file >> realID >> temp >> temp>> temp>> temp>> temp>> temp;
		Nspheres = realID; 
		cerr << "Nspheres" << Nspheres << endl;
		initialPositions.resize(Nspheres);
		
		for (long i=6; i<Nspheres+6; ++i)
		{
			file >> realID >> fx >> fy >> fz >> mx >> my >> mz;
			id = realID;
		#ifdef USELINENUMBERSFORID
                	id = i;
		#endif
                

                    Vector3r f (fx,fy,fz);
                    Vector3r t (mx,my,mz);
		    f*=forceFactor;
		    t*=forceFactor;
// 		    cerr << "f=" << f << " on " << id << endl;
                    //f /= -10000;
                    //t *= 0;
						  	ncb->bex.addForce(id,f);
							ncb->bex.addTorque(id,t);
                }
		file.close();
            }
            else cerr << "problem opening the file with hydraulic forces" << endl;
            
	    if (savePositions)
	    #ifdef USELINENUMBERSFORID
	    	{Shop::saveSpheresToFile(outputFile.c_str()); savePositions = false;}//only once
	    #else
	    {
		    const shared_ptr<MetaBody>& rootBody=Omega::instance().getRootBody();
		    ofstream f(outputFile.c_str());
		    if(!f.good()) throw runtime_error("Unable to open file `"+outputFile+"'");
		    FOREACH(shared_ptr<Body> b, *rootBody->bodies){
			    if (!b->isDynamic) continue;
			    shared_ptr<InteractingSphere>	intSph=dynamic_pointer_cast<InteractingSphere>(b->interactingGeometry);
			    if(!intSph) continue;
			    const Vector3r& pos=b->physicalParameters->se3.position;
			    f<< b->getId()<<" "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<intSph->radius<<endl; // <<" "<<1<<" "<<1<<endl;
		    }
		    f.close();
	    }
	    #endif

        }

//         BodyContainer::iterator bi    = bodies->begin();
//         BodyContainer::iterator biEnd = bodies->end();
//         for ( ; bi!=biEnd ; ++bi )
//         {
//             shared_ptr<Body> b = *bi;
//             /* skip bodies that are within a clump;
//              * even if they are marked isDynamic==false, forces applied to them are passed to the clump, which is dynamic;
//              * and since clump is a body with mass equal to the sum of masses of its components, it would have gravity applied twice.
//              *
//              * The choice is to skip (b->isClumpMember()) or (b->isClump()). We rather skip members,
//              * since that will apply smaller number of forces and (theoretically) improve numerical stability ;-) */
//             if (b->isClumpMember()) continue;
//
//           //  cerr << "particle id = "<< b->getId() << endl;
//          //   cout << "cout version" << endl;
//
//             ParticleParameters* p = dynamic_cast<ParticleParameters*>(b->physicalParameters.get());
//             //if (p)
// //             if (b->getId() == 67)
// //             {
// //             	static_cast<Force*>( ncb->physicalActions->find( b->getId() , actionParameterForce->getClassIndex() ).get() )->force += Vector3r((Real) -100000,(Real) 0,(Real) 0);
// //             }
//             //static_cast<Force*>( ncb->physicalActions->find( b->getId() , actionParameterForce->getClassIndex() ).get() )->force += gravity * p->mass;
//         }
    }
}

YADE_PLUGIN();
