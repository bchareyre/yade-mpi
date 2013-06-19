#include"ViscoelasticPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/Sphere.hpp>

Real Law2_ScGeom_ViscElPhys_Basic::calculateCapillarForce(const ScGeom& geom, ViscElPhys& phys) {
  Real fC = 0.0;
  
  /* Capillar
    * Some equations have constants, which can be calculated only once per contact. 
    * No need to recalculate them at each step. 
    * It needs to be fixed.
    * 
    */
     
  if (phys.CapillarType  == "Weigert") {
      /* Capillar model from [Weigert1999]
       */
        Real R = phys.R;
        Real a = -geom.penetrationDepth;
        Real Ca = (1.0 + 6.0*a/(R*2.0));                                                          // [Weigert1999], equation (16)
        Real Ct = (1.0 + 1.1*sin(phys.theta));                                                    // [Weigert1999], equation (17)
        
        /*
        Real Eps = 0.36;                                                                          // Porosity
        Real fi = phys.Vb/(2.0*M_PI/6.0*pow(R*2.0,3.));                                           // [Weigert1999], equation (13)
        Real S = M_PI*(1-Eps)/(pow(Eps, 2.0))*fi;                                                 // [Weigert1999], equation (14)
        Real beta = asin(pow(((S/0.36)*(pow(Eps, 2.0)/(1-Eps))*(1.0/Ca)*(1.0/Ct)), 1.0/4.0));     // [Weigert1999], equation (19)
        */
        
        Real beta = asin(pow(phys.Vb/(0.12*Ca*Ct*pow(2.0*R, 3.0)), 1.0/4.0));                     // [Weigert1999], equation (15), against Vb
        
        Real r1 = (2.0*R*(1-cos(beta)) + a)/(2.0*cos(beta+phys.theta));                           // [Weigert1999], equation (5)
        Real r2 = R*sin(beta) + r1*(sin(beta+phys.theta)-1);                                      // [Weigert1999], equation (6)
        Real Pk = phys.gamma*(1/r1 - 1/r2);                                                       /* [Weigert1999], equation (22),
                                                                                                   * see also a sentence over the equation
                                                                                                   * "R1 was taken as positive and R2 was taken as negative"
                                                                                                   */ 

        //fC = M_PI*2.0*R*phys.gamma/(1+tan(0.5*beta));                                           // [Weigert1999], equation (23), [Fisher]
        
        fC = M_PI/4.0*pow((2.0*R),2.0)*pow(sin(beta),2.0)*Pk +
             phys.gamma*M_PI*2.0*R*sin(beta)*sin(beta+phys.theta);                                // [Weigert1999], equation (21)
        
      } else if (phys.CapillarType  == "Willett_numeric") {
      
        /* Capillar model from [Willett2000]
         */ 
        
        Real R = phys.R;
        Real s = -geom.penetrationDepth/2.0;
        Real Vb = phys.Vb;
        
        Real VbS = Vb/(R*R*R);
        Real Th1 = phys.theta;
        Real Th2 = phys.theta*phys.theta;
        Real Gamma = phys.gamma;
        
        /*
         * [Willett2000], equations in Anhang
        */
        Real f1 = (-0.44507 + 0.050832*Th1 - 1.1466*Th2) + 
                  (-0.1119 - 0.000411*Th1 - 0.1490*Th2) * log(VbS) +
                  (-0.012101 - 0.0036456*Th1 - 0.01255*Th2) *log(VbS)*log(VbS) +
                  (-0.0005 - 0.0003505*Th1 - 0.00029076*Th2) *log(VbS)*log(VbS)*log(VbS);
        
        Real f2 = (1.9222 - 0.57473*Th1 - 1.2918*Th2) +
                  (-0.0668 - 0.1201*Th1 - 0.22574*Th2) * log(VbS) +
                  (-0.0013375 - 0.0068988*Th1 - 0.01137*Th2) *log(VbS)*log(VbS);
                  
        Real f3 = (1.268 - 0.01396*Th1 - 0.23566*Th2) +
                  (0.198 + 0.092*Th1 - 0.06418*Th2) * log(VbS) +
                  (0.02232 + 0.02238*Th1 - 0.009853*Th2) *log(VbS)*log(VbS) +
                  (0.0008585 + 0.001318*Th1 - 0.00053*Th2) *log(VbS)*log(VbS)*log(VbS);
        
        Real f4 = (-0.010703 + 0.073776*Th1 - 0.34742*Th2) +
                  (0.03345 + 0.04543*Th1 - 0.09056*Th2) * log(VbS) +
                  (0.0018574 + 0.004456*Th1 - 0.006257*Th2) *log(VbS)*log(VbS);
  
        Real sPl = s/sqrt(Vb/R);
        
        Real lnFS = f1 - f2*exp(f3*log(sPl) + f4*log(sPl)*log(sPl));
        Real FS = exp(lnFS);
        
        fC = FS * 2.0 * M_PI* R * Gamma;
      } else if (phys.CapillarType  == "Willett_analytic") {
        /* Capillar model from Willet [Willett2000] (analytical solution), but 
         * used also in the work of Herminghaus [Herminghaus2005]
         */
         
        Real R = phys.R;
        Real Gamma = phys.gamma;
        Real s = -geom.penetrationDepth;
        Real Vb = phys.Vb;
                
        /*
        
        Real sPl = s/sqrt(Vb/R);                                                            // [Herminghaus2005], equation (sentence between (7) and (8))
        fC = 2.0 * M_PI* R * Gamma * cos(phys.theta)/(1 + 1.05*sPl + 2.5 *sPl * sPl);       // [Herminghaus2005], equation (7)
        
        */ 
        
        Real sPl = (s/2.0)/sqrt(Vb/R);                                                      // [Willett2000], equation (sentence after (11)), s - half-separation, so s*2.0
        Real f_star = cos(phys.theta)/(1 + 2.1*sPl + 10.0 * pow(sPl, 2.0));                 // [Willett2000], equation (12)
        fC = f_star * (2*M_PI*R*Gamma);                                                     // [Willett2000], equation (13), against F
        
      } else if (phys.CapillarType  == "Rabinovich") {
        /* Capillar model from Rabinovich [Rabinov2005]
         */
         
        Real R = phys.R;
        Real Gamma = phys.gamma;
        Real H = -geom.penetrationDepth;
        Real V = phys.Vb;
        
        Real alpha = 0.0;
        Real dsp = 0.0;
        if (H!=0.0) {
          alpha = sqrt(H/R*(-1+ sqrt(1 + 2.0*V/(M_PI*R*H*H))));                          // [Rabinov2005], equation (A3)
        
          dsp = H/2.0*(-1.0 + sqrt(1.0 + 2.0*V/(M_PI*R*H*H)));                           // [Rabinov2005], equation (20)
        
          fC = -(2*M_PI*R*Gamma*cos(phys.theta))/(1+(H/(2*dsp))) - 
              2*M_PI*R*Gamma*sin(alpha)*sin(phys.theta + alpha);                         // [Rabinov2005], equation (19)
        } else {
          fC = -(2*M_PI*R*Gamma*cos(phys.theta)) - 
              2*M_PI*R*Gamma*sin(alpha)*sin(phys.theta + alpha);                         // [Rabinov2005], equation (19)
        }
        
        fC *=-1;
        
      } else {
        throw runtime_error("CapillarType is unknown, please, use only Willett_numeric, Willett_analytic, Weigert or Rabinovich");
      }
  return fC;
}
