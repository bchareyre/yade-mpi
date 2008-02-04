/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "HydraulicForceEngine.hpp"
#include <yade/pkg-common/ParticleParameters.hpp>
#include <yade/pkg-common/Force.hpp>
#include <yade/pkg-common/Momentum.hpp>
#include<yade/core/MetaBody.hpp>
#include <yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include <vector>

long int HFinversion_counter = 0;
bool HFinverted = false;
vector<Real> initialPositions;

HydraulicForceEngine::HydraulicForceEngine() : actionParameterForce(new Force), actionParameterMomentum(new Momentum), gravity(Vector3r::ZERO), isActivated(false)
{
dummyParameter = false;
}


HydraulicForceEngine::~HydraulicForceEngine()
{
}


void HydraulicForceEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(isActivated);
	REGISTER_ATTRIBUTE(dummyParameter);
}


void HydraulicForceEngine::applyCondition(Body* body)
{

    if (isActivated)
    {
        MetaBody * ncb = YADE_CAST<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;
        
//cerr << "HFinverted " << HFinverted << " Omega::instance().getTimeStep() " << Omega::instance().getCurrentIteration() << endl;
        if (HFinversion_counter < (Omega::instance().getCurrentIteration() - 1500))
        {
        //cerr << "HFinverted " << HFinverted << " HFinversion_counter " << HFinversion_counter << endl;
            HFinversion_counter = Omega::instance().getCurrentIteration();
            HFinverted = !HFinverted;
//             initialPositions.clear();
//             for (int i =0; i < initialPositions.size(); ++i)
//             {
//             	initialPositions[i-6] = (static_cast<CohesiveFrictionalBodyParameters*> (b->physicalParameters.get()))->se3.position[1];
//             	
//             }
            
            
        }
        

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
                        static_cast<Force*>( ncb->physicalActions->find( b->getId() , actionParameterForce->getClassIndex() ).get() )->force += Vector3r(0,5,0);
                    }
                    // else  b->geometricalModel->diffuseColor= Vector3r(0.5,0.9,0.3);
                }
            }
        }
        else
        {
            long Nspheres;
            long id;
            Real fx, fy, fz, mx, my, mz;
            ifstream file ("/home/bruno/YADE/data/hydraulic_actions.dat");
            file >> Nspheres;
            initialPositions.resize(Nspheres);
            //cerr << "r = " << R << endl;
            if (file.is_open())
            {
                for (long i=1; i<Nspheres; ++i)
                {

                    file >> id >> fx >> fy >> fz >> mx >> my >> mz;

                    Vector3r f (fx,fy,fz);
                    Vector3r t (mx,my,mz);
                    //f /= -10000;
                    //t *= 0;
                    static_cast<Force*>( ncb->physicalActions->find( id , actionParameterForce->getClassIndex() ).get() )->force += f;
                    //cerr << "added force = " << f << endl;
                    static_cast<Momentum*>( ncb->physicalActions->find( id , actionParameterMomentum->getClassIndex() ).get() )->momentum += t;

                }
            }
            else cerr << "problem opening the file with hydraulic forces" << endl;
            file.close();

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
