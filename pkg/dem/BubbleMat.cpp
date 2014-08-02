#include "BubbleMat.hpp"



YADE_PLUGIN((BubbleMat)(Ip2_BubbleMat_BubbleMat_BubblePhys)(BubblePhys)(Law2_ScGeom_BubblePhys_Bubble));


/********************** Ip2_BubbleMat_BubbleMat_BubblePhys ****************************/
CREATE_LOGGER(Ip2_BubbleMat_BubbleMat_BubblePhys);
void Ip2_BubbleMat_BubbleMat_BubblePhys::go(const shared_ptr<Material>& m1, const shared_ptr<Material>& m2, const shared_ptr<Interaction>& interaction){
	// phys already exists
	if (interaction->phys) return;

	shared_ptr<BubblePhys> phys(new BubblePhys());
	interaction->phys = phys;
}

/********************** BubblePhys ****************************/
CREATE_LOGGER(BubblePhys);
void BubblePhys::computeCoeffs(Real pctMaxForce, Real surfaceTension, Real c1)
{
	    Real Fmax = pctMaxForce*c1*rAvg;
	    Real logPct = log(pctMaxForce/4);
	    Dmax = pctMaxForce*rAvg*logPct;
	    Real dfdd = c1/(logPct+1);
	    coeffB = dfdd/Fmax;
	    coeffA = Fmax/exp(coeffB*Dmax);
	    fN = 0.1*Fmax;
}

Real BubblePhys::computeForce(Real separation, Real surfaceTension, Real rAvg, int newtonIter, Real newtonTol, Real c1, Real fN, BubblePhys* phys) {

	if (separation >= phys->Dmax) {
	  Real f,df,g,retOld,residual;
	  Real c2 = 1./(4*c1*rAvg);
	  Real ret = fN;
	  int i = 0;
	  do {				//[Chan2011], Loop solves modified form of equation (25) using Newton-Raphson method
		  retOld = ret;
		  g = log(ret*c2);
		  f = separation*c1 - ret*g;
		  df = g+1;
		  ret += f/df;
		  if(ret <= 0.0){	//Need to make sure ret > 0, otherwise the next iteration will try to evaluate the natural logarithm of a negative number, which results in NaN
		    ret = 0.9*fabs(ret);
		    residual = newtonTol*2;	//Also, if, by chance, retOld = 0.9*ret it would cause the loop to exit based on the boolean residual > newtonTol, so we force the next iteration by setting residual = newtonTol*2
		  }	
		  else {residual = fabs(ret - retOld)/retOld;}
		  if (i++ > newtonIter) {
			  throw runtime_error("BubblePhys::computeForce: no convergence\n");
		  }
	  } while (residual > newtonTol);
	  return ret;
	}
	else {				//Artificial Extension of [Chan2011] equation 25 to approximiate behaviour outside the valid regime (large penetration cases)
	  return phys->coeffA*exp(phys->coeffB*separation);
	}
}

/********************** Law2_ScGeom_BubblePhys_Bubble ****************************/
CREATE_LOGGER(Law2_ScGeom_BubblePhys_Bubble);

bool Law2_ScGeom_BubblePhys_Bubble::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
	ScGeom* geom=static_cast<ScGeom*>(_geom.get());
	BubblePhys* phys=static_cast<BubblePhys*>(_phys.get());
	
	if(geom->penetrationDepth <= 0.0) {
		return false;
	}
	
	if (I->isFresh(scene)) {
		c1 = 2*Mathr::PI*surfaceTension;
		phys->rAvg = .5*(geom->refR1 + geom->refR2);
		phys->computeCoeffs(pctMaxForce,surfaceTension,c1);
	}

	Real &fN = phys->fN;
	fN = phys->computeForce(-geom->penetrationDepth, surfaceTension, phys->rAvg, phys->newtonIter, phys->newtonTol, c1, fN, phys);
	phys->fN = fN;
	Vector3r &normalForce = phys->normalForce;
	normalForce = fN*geom->normal;

	int id1 = I->getId1(), id2 = I->getId2();
	if (!scene->isPeriodic) {
		State* b1 = Body::byId(id1,scene)->state.get();
		State* b2 = Body::byId(id2,scene)->state.get();
		applyForceAtContactPoint(-normalForce, geom->contactPoint , id1, b1->se3.position, id2, b2->se3.position);
	} else {
		scene->forces.addForce(id1,-normalForce);
		scene->forces.addForce(id2,normalForce);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(normalForce));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(normalForce));
	}
	return true;
}
