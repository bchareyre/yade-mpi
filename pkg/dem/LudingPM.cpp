#include"LudingPM.hpp"
#include<core/State.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<core/Omega.hpp>
#include<core/Scene.hpp>
#include<pkg/common/Sphere.hpp>

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
  const Real ks1 = mat1->ks; const Real ks2 = mat2->ks;
  const Real G01 = mat1->G0; const Real G02 = mat2->G0;
  const Real PhiF1 = mat1->PhiF; const Real PhiF2 = mat2->PhiF;

  LudingPhys* phys = new LudingPhys();

  phys->k1 = this->reduced(k11, k12);
  phys->kp = this->reduced(kp1, kp2);
  phys->kc = this->reduced(kc1, kc2);
  phys->ks = this->reduced(ks1, ks2);
  phys->PhiF = this->reduced(PhiF1, PhiF2);
  phys->k2 = 0.0;
  phys->G0 = this->reduced(G01, G02);
  
  
  Real a1 = 0.0;
  Real a2 = 0.0;
  Sphere* s1=dynamic_cast<Sphere*>(Body::byId(interaction->getId1())->shape.get());
  Sphere* s2=dynamic_cast<Sphere*>(Body::byId(interaction->getId2())->shape.get());

  Real a1dR = 0.;
  Real a2dR = 0.;

#ifdef YADE_DEFORM
  State* de1 = dynamic_cast<State*>(Body::byId(interaction->getId1())->state.get());
  State* de2 = dynamic_cast<State*>(Body::byId(interaction->getId2())->state.get());

  if (de1 and de2) {
      a1dR = de1->dR ;
      a2dR = de2->dR ;
  }
  else if (de1 and not(de2)) {
      a1dR = de1->dR ;
  }
  else {
      a2dR = de2->dR ;
  }
#endif

  if (s1 and s2) {
    a1 = s1->radius + a1dR;
    a2 = s2->radius + a2dR;
  } else if (s1 and not(s2)) {
    a1 = s1->radius + a1dR;
  } else {
    a2 = s2->radius + a2dR;
  }
    
  if (phys->k1 >= phys->kp) {
    throw runtime_error("k1 have to be less as kp!");     // [Luding2008], sentence after equation (6); kp = k2^
                                                          // [Singh2013], sentence after equation (6)
  }
  
  
  
  phys->tangensOfFrictionAngle = std::tan(std::min(mat1->frictionAngle, mat2->frictionAngle)); 
  
  phys->shearForce = Vector3r(0,0,0);
  phys->DeltMax = 0.0;
  phys->DeltNull = 0.0;
  phys->DeltPMax = phys->kp/(phys->kp-phys->k1)*phys->PhiF*2*a1*a2/(a1+a2);   // [Luding2008], equation (7)
                                                                              // [Singh2013], equation (11)
  phys->DeltPNull = phys->PhiF*2*a1*a2/(a1+a2);                               // [Singh2013], equation (12)
  phys->DeltPrev = 0.0;
  phys->DeltMin = 0.0;
  interaction->phys = shared_ptr<LudingPhys>(phys);
}

Real Ip2_LudingMat_LudingMat_LudingPhys::reduced(Real a1, Real a2){
  Real a = (a1?1/a1:0) + (a2?1/a2:0); a = a?1/a:0;
  return 2.0*a;
}

bool Law2_ScGeom_LudingPhys_Basic::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
  
  const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
  LudingPhys& phys=*static_cast<LudingPhys*>(_phys.get());

  const int id1 = I->getId1();
  const int id2 = I->getId2();

  const BodyContainer& bodies = *scene->bodies;
  const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
  const State& de2 = *static_cast<State*>(bodies[id2]->state.get());
  Real addDR = 0. ;

#ifdef YADE_DEFORM
  addDR = de1.dR + de2.dR;
#endif

  const Real Delt = geom.penetrationDepth + addDR;
  if (Delt  < 0 ) return false;

  Real forceHys = 0.0;
  
  if (phys.DeltMax/phys.DeltPMax >= 1.0) {                           // [Luding2008], equation (8)
    phys.k2 = phys.kp;                                               // [Singh2013], equation (10)
  } else {
    phys.k2 = phys.k1 + (phys.kp - phys.k1)*phys.DeltMax/phys.DeltPMax;
  }
  
  if (phys.k2>phys.kp) { 
    phys.k2 = phys.kp;
  }
  
  if (phys.k1>phys.k2) { 
    phys.k1 = phys.k2;
  }
  
  phys.DeltMin = (phys.k2- phys.k1)/(phys.k2 + phys.kc);
    
  if (Delt > phys.DeltMax) {
    phys.DeltMax  = Delt;
    phys.DeltNull  = std::min((1.0 - phys.k1/phys.k2)*phys.DeltMax, phys.DeltPNull);  // [Luding2008], equation over Fig 1
                                                                                      // [Singh2013], equation (8)
  }
  
  Real k2DeltTtmp = phys.k2*(Delt - phys.DeltNull);                  // [Luding2008], equation (6)
                                                                     // [Singh2013], equation (6)
  
  if ( k2DeltTtmp >= phys.k1*Delt) {
    if (Delt<phys.DeltPMax){
      forceHys = phys.k1*Delt;                                        
    } else {
      forceHys = k2DeltTtmp;                                        
    }
  } else if (k2DeltTtmp > -phys.kc*Delt and k2DeltTtmp < phys.k1*Delt) {
    forceHys = k2DeltTtmp;
  } else if (k2DeltTtmp<=-phys.kc*Delt) {
    if ((Delt - phys.DeltPrev) < 0) {
      forceHys = -phys.kc*Delt;
      phys.DeltMax = Delt*(phys.k2 + phys.kc)/(phys.k2 - phys.k1);                     // [Singh2013], equation (9)
      phys.DeltNull  = std::min((1.0 - phys.k1/phys.k2)*phys.DeltMax, phys.DeltPNull);  // [Luding2008], equation over Fig 1
                                                                                        // [Singh2013], equation (8)
    } else {
      forceHys = k2DeltTtmp;
    }
  }
  
  phys.DeltPrev = Delt;
  
  //===================================================================
  //===================================================================
  //===================================================================
  // Copy-paste from ViscoElasticPM
  

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
  return true;
  
}
