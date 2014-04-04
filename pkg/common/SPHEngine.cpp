#ifdef YADE_SPH
#include"SPHEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/ViscoelasticPM.hpp>
#include<yade/pkg/common/Sphere.hpp>

#include<yade/core/State.hpp>
#include<yade/core/Omega.hpp>

void SPHEngine::action(){
  YADE_PARALLEL_FOREACH_BODY_BEGIN(const shared_ptr<Body>& b, scene->bodies){
    if(mask>0 && (b->groupMask & mask)==0) continue;
    this->calculateSPHRho(b);
    b->press=k*(b->rho - b->rho0);
  } YADE_PARALLEL_FOREACH_BODY_END();
}

void SPHEngine::calculateSPHRho(const shared_ptr<Body>& b) {
  if (b->rho0<0) {
    b->rho0 = rho0;
  }
  Real rho = 0;
  for(Body::MapId2IntrT::iterator it=b->intrs.begin(),end=b->intrs.end(); it!=end; ++it) {
    const shared_ptr<Body> b2 = Body::byId((*it).first,scene);
    Sphere* s=dynamic_cast<Sphere*>(b->shape.get());
    if(!s) continue;
    
    if (((*it).second)->geom and ((*it).second)->phys) {
      const ScGeom geom = *(YADE_PTR_CAST<ScGeom>(((*it).second)->geom));
      const ViscElPhys phys=*(YADE_PTR_CAST<ViscElPhys>(((*it).second)->phys));
      
      if((b2->groupMask & mask)==0)  continue;
      
      Real Mass = b2->state->mass;
      if (Mass == 0) Mass = b->state->mass;
      
      const Real SmoothDist = -geom.penetrationDepth + phys.h;
      
      rho += b2->state->mass*smoothkernelPoly6(SmoothDist, phys.h);
    }
    rho += b->state->mass*smoothkernelPoly6(0.0, s->radius);
  }
  b->rho = rho;
}

Real smoothkernelPoly6(const double & rrj, const double & h) {
  if (rrj<=h) {
    return 315/(64*M_PI*pow(h,9))*pow((h*h - rrj*rrj), 3);   // [Mueller2003], (20)
  } else {
    return 0;
  }
}

Real smoothkernelSpiky(const double & rrj, const double & h) {
  if (rrj<=h) {
    return 15/(M_PI*pow(h,6))*pow((h - rrj), 3);             // [Mueller2003], (21)
  } else {
    return 0;
  }
}

Real smoothkernelVisco(const double & rrj, const double & h) {
  if (rrj<=h) {
    return 15/(2*M_PI*pow(h,3))*(-rrj*rrj*rrj/(2*h*h*h) + rrj*rrj/(h*h) + h/(2*rrj) - 1);   // [Mueller2003], (21)
  } else {
    return 0;
  }
}

Real smoothkernelViscoLapl(const double & rrj, const double & h) {
  if (rrj<=h) {
    return 45/(M_PI*pow(h,6))*(h - rrj);                     // [Mueller2003], (22+)
  } else {
    return 0;
  }
}

void computeForceSPH(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I, Vector3r & force) {
  
  const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
  Scene* scene=Omega::instance().getScene().get();
  ViscElPhys& phys=*static_cast<ViscElPhys*>(_phys.get());
  
  const int id1 = I->getId1();
  const int id2 = I->getId2();
  
  const BodyContainer& bodies = *scene->bodies;
  
  //////////////////////////////////////////////////////////////////
  // Copy-paste
  
  const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
  const State& de2 = *static_cast<State*>(bodies[id2]->state.get());
  
    // Handle periodicity.
  const Vector3r shift2 = scene->isPeriodic ? scene->cell->intrShiftPos(I->cellDist): Vector3r::Zero(); 
  const Vector3r shiftVel = scene->isPeriodic ? scene->cell->intrShiftVel(I->cellDist): Vector3r::Zero(); 

  const Vector3r c1x = (geom.contactPoint - de1.pos);
  const Vector3r c2x = (geom.contactPoint - de2.pos - shift2);
  
  const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) + shiftVel;
  const Real normalVelocity	= geom.normal.dot(relativeVelocity);
  const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;
  
  // Copy-paste
  //////////////////////////////////////////////////////////////////
  
  if (phys.h<0) {
    Sphere* s1=dynamic_cast<Sphere*>(bodies[id1]->shape.get());
    Sphere* s2=dynamic_cast<Sphere*>(bodies[id2]->shape.get());
    if (s1 and s2) {
      phys.h = s1->radius + s2->radius;
    } else if (s1 and not(s2)) {
      phys.h = s1->radius;
    } else {
      phys.h = s2->radius;
    }
  }
  
  Real Mass = bodies[id2]->state->mass;
  if (Mass==0.0 and bodies[id1]->state->mass!= 0.0) {
    Mass = bodies[id1]->state->mass;
  }
  
  Real Rho = bodies[id2]->rho;
  if (Rho==0.0 and bodies[id1]->rho!=0.0) {
    Rho = bodies[id1]->rho!=0.0;
  }
  
  const Vector3r xixj = (-geom.penetrationDepth + phys.h)*geom.normal;
  
  if (xixj.norm() < phys.h) {
    Real fpressure = 0.0;
    if (Rho!=0.0) {
      fpressure = Mass * 
                  (bodies[id1]->press + bodies[id2]->press)/(2.0*Rho) *
                  smoothkernelSpiky(xixj.norm(), phys.h);
    }
    
    Vector3r fvisc = Vector3r::Zero();
    if (Rho!=0.0) {
      fvisc = phys.mu * Mass * 
                  normalVelocity*geom.normal/Rho *
                  smoothkernelViscoLapl(xixj.norm(), phys.h);
    }
    force = fpressure*geom.normal - fvisc;
    return;
  } else {
    return;
  }
}
YADE_PLUGIN((SPHEngine));
#endif

