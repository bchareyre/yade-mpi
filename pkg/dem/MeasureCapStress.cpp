#include <pkg/dem/MeasureCapStress.hpp>
#include <core/Interaction.hpp>
#include <pkg/dem/CapillaryPhys.hpp>
#include <pkg/dem/ScGeom.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/dem/Shop.hpp> // for direct use of aabbExtrema


YADE_PLUGIN((MeasureCapStress));

void MeasureCapStress::action() {
  shared_ptr<BodyContainer>& bodies = scene->bodies;

  muSsw = Matrix3r::Zero();
  muGamma = Matrix3r::Zero();
  muSnw = Matrix3r::Zero();
    
  vW= 0; // was the case at the creation of the Engine, but has to be reset at each execution...
  
  FOREACH(const shared_ptr<Interaction>& interaction, *scene->interactions){ // not possible or meaningfull to use parallel loops here.. (http://www.mail-archive.com/yade-dev@lists.launchpad.net/msg11018.html)
    if ( !interaction->isReal()) continue;
    const shared_ptr<CapillaryPhys> phys = YADE_PTR_CAST<CapillaryPhys>(interaction->phys);
    const shared_ptr<ScGeom> geom = YADE_PTR_CAST<ScGeom>(interaction->geom);
    if (phys->meniscus) {

      vW += phys->vMeniscus;
      
      Body* b1 = (*bodies)[interaction->getId1()].get();
      Body* b2 = (*bodies)[interaction->getId2()].get();
      
      Real rB1 = YADE_PTR_DYN_CAST<Sphere>( b1->shape )->radius;
      Real rB2 = YADE_PTR_DYN_CAST<Sphere>( b2->shape )->radius;
      
      Real deltaB1,deltaB2;
      Vector3r vecN = geom->normal; // from body1 to body2
      Vector3r vecSmallToBig = Vector3r::Zero(); // is this one useful ???
      if(rB1 > rB2){ // body1 is the biggest, i.e. the one with Delta2
	deltaB1 = phys->Delta2;
	deltaB2 = phys->Delta1;
        vecSmallToBig = - vecN;
      }
      else {
	deltaB1 = phys->Delta1;
	deltaB2 = phys->Delta2;
        vecSmallToBig = vecN;
      }
      
      // Body 1 consideration: vecN = z axis with respect to Fig 3.18 [Khosravani2014]
      muSsw += matA_BodyGlob( deltaB1 * Mathr::DEG_TO_RAD,rB1,vecN);
      muGamma += matBp_BodyGlob( deltaB1 * Mathr::DEG_TO_RAD,wettAngle,rB1,vecN);
      
      // Body 2 consideration: vecN = - z axis with respect to Fig 3.18 [Khosravani2014]
      muSsw += matA_BodyGlob( deltaB2 * Mathr::DEG_TO_RAD , rB2 , -vecN);
      muGamma += matBp_BodyGlob(deltaB2 * Mathr::DEG_TO_RAD,wettAngle,rB2,-vecN);
      
      // liq-gas interface term
      muSnw += matLG_bridgeGlob(phys->nn11,phys->nn33,vecSmallToBig);
      }
  }

  // last microstructural tensor:
  muVw = vW * Matrix3r::Identity();

  Real volume = 0;
  if (scene->isPeriodic)
    volume = scene->cell->hSize.determinant();
  else {
	  vector<Vector3r> extrema = Shop::aabbExtrema();
	  volume = (extrema[1][0] - extrema[0][0])*(extrema[1][1] - extrema[0][1])*(extrema[1][2] - extrema[0][2]);
  }
  if (debug) cout << "c++ : volume = " << volume << endl;
  if (volume ==0) LOG_ERROR("Could not get a non-zero volume value");
//   else { // error: ‘else’ without a previous ‘if’ ?????????!!!!!##########

  sigmaCap = 1 / volume * (  capillaryPressure*(muVw + muSsw) + surfaceTension*(muGamma + muSnw) );
//   }
}


Matrix3r MeasureCapStress::matA_BodyGlob(Real alpha,Real radius,Vector3r vecN){
    Matrix3r A_BodyGlob ;
    
    A_BodyGlob << pow( 1 - cos(alpha),2) * (2 + cos(alpha)) , 0 , 0 ,
                  0 , pow( 1 - cos(alpha),2) * (2 + cos(alpha)) , 0 ,
                  0 , 0 , 2 * ( 1-pow(cos(alpha),3) );
                  
    A_BodyGlob *= Mathr::PI * pow(radius,3.0)/3.0;
    Matrix3r globToLocRet = matGlobToLoc(vecN);
    return globToLocRet * A_BodyGlob * globToLocRet.transpose() ;
}

Matrix3r MeasureCapStress::matLG_bridgeGlob(Real nn11,Real nn33, Vector3r vecN){
        Matrix3r LG_bridgeGlob ;
        
        LG_bridgeGlob << nn11 + nn33 , 0 , 0 , // useless to write lgArea - nn11 = 2*nn11 + nn33 - nn11
        0 , nn11 + nn33 , 0 ,
        0 , 0 , 2*nn11; // trace = 2*(2*nn11 + nn33) = 2*lgArea
        
        Matrix3r globToLocRet = matGlobToLoc(vecN);
        return globToLocRet * LG_bridgeGlob * globToLocRet.transpose() ;
}

Matrix3r MeasureCapStress::matBp_BodyGlob(Real alpha,Real wettAngle, Real radius,Vector3r vecN){ // matrix B prime (the surface tension coefficient excepted), defined at body scale (see (3.49) p.65), expressed in global framework
    Matrix3r Bp_BodyGlob ;
    Bp_BodyGlob << - pow(sin(alpha),2) * cos(alpha+wettAngle) , 0 , 0 ,
                   0 , - pow(sin(alpha),2) * cos(alpha+wettAngle) , 0 ,
                   0 , 0 , sin(2*alpha) * sin(alpha+wettAngle);
    Bp_BodyGlob *= Mathr::PI * pow(radius,2.0);
    Matrix3r globToLocRet = matGlobToLoc(vecN);
    return globToLocRet * Bp_BodyGlob * globToLocRet.transpose() ;    
}



Matrix3r MeasureCapStress::matGlobToLoc(Vector3r vecN){
    Real phi; // the angle between x-axis and vecN
    Real theta = acos(vecN[2]); // in [0,pi] according to http://www.cplusplus.com/reference/cmath/acos/. The same in std according to http://en.cppreference.com/w/cpp/numeric/math/acos ?
    if (fabs(vecN[2]) == 1) // hence vecN = +/- Z, sin(theta = 0), and phi value does not matter
        phi = 0;
    else {
        Real cosPhi;
        cosPhi = vecN[0]/sin(theta);
        if (cosPhi > 1) cosPhi = 1; // might occur. Because of numeric precision ?
        else if (cosPhi < -1) cosPhi = -1;
        if (vecN[1] > 0) // <=> here sinPhi > 0 <=> phi in ]0,pi[
        {
            phi = acos(cosPhi);
        }
        else
        {
            phi = 2*Mathr::PI - acos(cosPhi);
        }
    }
    // Change of basis matrix from global (X,Y,Z), to local (x=eTheta,y=ePhi,vecN=eR)
    Matrix3r globToLoc = Matrix3r::Zero();
    globToLoc << cos(theta)*cos(phi), -sin(phi), sin(theta)*cos(phi),
                 cos(theta)*sin(phi), cos(phi) , sin(theta)*sin(phi),
                 - sin(theta)    ,     0    ,     cos(theta);
    return globToLoc;
}
