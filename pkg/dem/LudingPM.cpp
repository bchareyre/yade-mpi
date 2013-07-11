#include"LudingPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/Sphere.hpp>

YADE_PLUGIN((LudingMat)(LudingPhys)(Ip2_LudingMat_LudingMat_LudingPhys)(Law2_ScGeom_LudingPhys_Basic));

LudingMat::~LudingMat(){}

LudingPhys::~LudingPhys(){}

void Ip2_LudingMat_LudingMat_LudingPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
  if(interaction->phys) return;
  
  LudingMat* mat1 = static_cast<LudingMat*>(b1.get());
  LudingMat* mat2 = static_cast<LudingMat*>(b2.get());
  
  const Real k11 = mat1->k1; const Real k12 = mat2->k1;
  const Real kp1 = mat1->kp; const Real kp2 = mat2->kp; 
  const Real kc1 = mat1->kc; const Real kc2 = mat2->kc;
  const Real G01 = mat1->G0; const Real G02 = mat2->G0;
  const Real PhiF1 = mat1->PhiF; const Real PhiF2 = mat2->PhiF;

  LudingPhys* phys = new LudingPhys();

  phys->k1 = this->reduced(k11, k12);
  phys->kp = this->reduced(kp1, kp2);
  phys->kc = this->reduced(kc1, kc2);
  phys->PhiF = this->reduced(PhiF1, PhiF2);
  phys->k2 = 0.0;
  phys->G0 = this->reduced(G01, G02);
  
  
  Real a1 = 0.0;
  Real a2 = 0.0;
  Sphere* s1=dynamic_cast<Sphere*>(Body::byId(interaction->getId1())->shape.get());
  Sphere* s2=dynamic_cast<Sphere*>(Body::byId(interaction->getId2())->shape.get());
  if (s1 and s2) {
    a1 = s1->radius;
    a2 = s2->radius;
  } else if (s1 and not(s2)) {
    a1 = s1->radius;
  } else {
    a2 = s2->radius;
  }
    
  if (phys->k1 >= phys->kp) {
    throw runtime_error("k1 have to be less as kp!");     // [Luding2008], sentence after equation (6); kp = k2^
                                                          // [Singh2013], sentence after equation (6)
  }
  
  
  
  phys->tangensOfFrictionAngle = std::tan(std::min(mat1->frictionAngle, mat2->frictionAngle)); 
  
  phys->shearForce = Vector3r(0,0,0);
  phys->ThetMax = 0.0;
  phys->ThetNull = 0.0;
  phys->ThetPMax = phys->kp/(phys->kp-phys->k1)*phys->PhiF*2*a1*a2/(a1+a2);   // [Luding2008], equation (7)
                                                                              // [Singh2013], equation (11)
  
  interaction->phys = shared_ptr<LudingPhys>(phys);
}

Real Ip2_LudingMat_LudingMat_LudingPhys::reduced(Real a1, Real a2){
  Real a = (a1?1/a1:0) + (a2?1/a2:0); a = a?1/a:0;
  return a;
}

void Law2_ScGeom_LudingPhys_Basic::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
  
  const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
  LudingPhys& phys=*static_cast<LudingPhys*>(_phys.get());

  const int id1 = I->getId1();
  const int id2 = I->getId2();
  
  const Real Theta = geom.penetrationDepth;
  
  if (Theta<0) {
    scene->interactions->requestErase(I);
    return;
  };

  const BodyContainer& bodies = *scene->bodies;
  
  
  
  
  Real forceHys = 0.0;
  
  if (phys.ThetMax/phys.ThetPMax >= 1.0) {                           // [Luding2008], equation (8)
    phys.k2 = phys.kp;                                               // [Singh2013], equation (10)
  } else {
    phys.k2 = phys.k1 + (phys.kp - phys.k1)*phys.ThetMax/phys.ThetPMax;
  }
    
  if (Theta > phys.ThetMax) {
    phys.ThetMax  = Theta;
    phys.ThetNull  = (1.0 - phys.k1/phys.k2)*phys.ThetMax;           // [Luding2008], equation over Fig 1
                                                                     // [Singh2013], equation (8)
  }
  
  Real k2DeltaTtmp = phys.k2*(Theta - phys.ThetNull);                
  if ( k2DeltaTtmp >= phys.k1*Theta) {                               // [Luding2008], equation (6)
    forceHys = phys.k1*Theta;                                        // [Singh2013], equation (6)
  } else if (k2DeltaTtmp > -phys.kc*Theta and k2DeltaTtmp < phys.k1*Theta) {
    forceHys = phys.k2*(Theta-phys.ThetNull);
  } else if (k2DeltaTtmp<=-phys.kc*Theta) {
    forceHys = -phys.kc*Theta;
  }
  
  
  
  //===================================================================
  //===================================================================
  //===================================================================
  // Copy-paste from ViscoElasticPM
  
  const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
  const State& de2 = *static_cast<State*>(bodies[id2]->state.get());

  Vector3r& shearForce = phys.shearForce;
  if (I->isFresh(scene)) shearForce=Vector3r(0,0,0);
  const Real& dt = scene->dt;
  shearForce = geom.rotate(shearForce);


  // Handle periodicity.
  
  const Vector3r shift2 = scene->isPeriodic ? scene->cell->intrShiftPos(I->cellDist): Vector3r::Zero(); 
  const Vector3r shiftVel = scene->isPeriodic ? scene->cell->intrShiftVel(I->cellDist): Vector3r::Zero(); 
  
  
  const Vector3r c1x = (geom.contactPoint - de1.pos);
  const Vector3r c2x = (geom.contactPoint - de2.pos - shift2);
  
  
  const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) + shiftVel;
  const Real normalVelocity	= geom.normal.dot(relativeVelocity);
  const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;

  shearForce += phys.ks*dt*shearVelocity;     // the elastic shear force have a history, but
  Vector3r shearForceVisc = Vector3r::Zero(); // the viscous shear damping haven't a history because it is a function of the instant velocity 

  
  phys.normalForce = (forceHys + phys.G0 * normalVelocity)*geom.normal;
  
  
  const Real maxFs = phys.normalForce.squaredNorm() * std::pow(phys.tangensOfFrictionAngle,2);
  if( shearForce.squaredNorm() > maxFs )
  {
    const Real ratio = sqrt(maxFs) / shearForce.norm();
    shearForce *= ratio;
  } 
  else 
  {
    // shearForceVisc = phys.cs*shearVelocity; //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       shearForceVisc = phys.G0*shearVelocity; 
  }
  //===================================================================
  //===================================================================
  //===================================================================
  

   if (I->isActive) {
    const Vector3r f = phys.normalForce + shearForce + shearForceVisc;
    addForce (id1,-f,scene);
    addForce (id2, f,scene);
    addTorque(id1,-c1x.cross(f),scene);
    addTorque(id2, c2x.cross(f),scene);
  }
  
  
}
