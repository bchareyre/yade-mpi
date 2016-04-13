// 2016 © Jan Stránský <jan.stransky@fsv.cvut.cz> 
#include"MortarMat.hpp"


YADE_PLUGIN((MortarMat)(Ip2_MortarMat_MortarMat_MortarPhys)(MortarPhys)(Law2_ScGeom_MortarPhys_Lourenco))


CREATE_LOGGER(Ip2_MortarMat_MortarMat_MortarPhys);
void Ip2_MortarMat_MortarMat_MortarPhys::go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction){
	if (interaction->phys) return;
	if (scene->iter >= cohesiveThresholdIter) return;
	shared_ptr<MortarPhys> phys(new MortarPhys());
	interaction->phys = phys;
	MortarMat* mat1 = YADE_CAST<MortarMat*>(material1.get());
	MortarMat* mat2 = YADE_CAST<MortarMat*>(material2.get());

	if (mat1->id>=0 && mat1->id == mat2->id) {
		#define _CPATTR(a) phys->a=mat1->a
			_CPATTR(tensileStrength);
			_CPATTR(compressiveStrength);
			_CPATTR(cohesion);
			_CPATTR(ellAspect);
		#undef _CPATTR
		phys->tangensOfFrictionAngle = std::tan(mat1->frictionAngle);
	} else {
		// averaging over both materials
		#define _MINATTR(a) phys->a=std::min(mat1->a,mat2->a)
		#define _AVGATTR(a) phys->a=.5*(mat1->a+mat2->a)
			_MINATTR(tensileStrength);
			_MINATTR(compressiveStrength);
			_MINATTR(cohesion);
			_AVGATTR(ellAspect);
		#undef _AVGATTR
		#undef _MINATTR
		phys->tangensOfFrictionAngle = std::tan(.5*(mat1->frictionAngle+mat2->frictionAngle));
		// E, G, kn, ks, crosssection, refPD, refLength to be computed in Law2
	}
}




CREATE_LOGGER(MortarPhys);
MortarPhys::~MortarPhys(){};







/********************** Law2_ScGeom_MortarPhys_Lourenco ****************************/
CREATE_LOGGER(Law2_ScGeom_MortarPhys_Lourenco);
bool Law2_ScGeom_MortarPhys_Lourenco::go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction){
	ScGeom* geom=static_cast<ScGeom*>(iGeom.get());
	MortarPhys* phys=static_cast<MortarPhys*>(iPhys.get());

	Body::id_t id1 = interaction->getId1();
 	Body::id_t id2 = interaction->getId2();
	const shared_ptr<Body> b1 = Body::byId(id1,scene);
	const shared_ptr<Body> b2 = Body::byId(id2,scene);

	/* just the first time */
	if (interaction->isFresh(scene)) {
		const Vector3r& pos1 = b1->state->pos;
		const Vector3r& pos2 = b2->state->pos;
		const Real& r1 = geom->refR1;
		const Real& r2 = geom->refR2;
		Vector3r shift2 = scene->isPeriodic? Vector3r(scene->cell->hSize*interaction->cellDist.cast<Real>()) : Vector3r::Zero();
		phys->refLength = (pos2 - pos1 + shift2).norm();
		Real minRad = r1 <= 0 ? r2 : r2 <= 0 ? r1 : std::min(r1,r2);
		phys->crossSection = std::pow(minRad,2);
		phys->refPD = geom->refR1 + geom->refR2 - phys->refLength;
		const shared_ptr<MortarMat> mat1 = YADE_PTR_CAST<MortarMat>(b1->material);
		const shared_ptr<MortarMat> mat2 = YADE_PTR_CAST<MortarMat>(b2->material);
		const Real& E1(mat1->young);
		const Real& E2(mat2->young);
		const Real& n1(mat1->poisson);
		const Real& n2(mat2->poisson);
		phys->kn = 2*E1*r1*E2*r2/(E1*r1+E2*r2);
		phys->ks = 2*E1*r1*n1*E2*r2*n2/(E1*r1*n1+E2*r2*n2);
		phys->E = phys->kn * phys->refLength / phys->crossSection;
		phys->G = phys->ks * phys->refLength / phys->crossSection;
	}
	
	/* shorthands */
	Real& epsN(phys->epsN);
	Vector3r& epsT(phys->epsT);
	const Real& E(phys->E); \
	const Real& G(phys->G);
	const Real& crossSection(phys->crossSection);
	Real& sigmaN(phys->sigmaN);
	Vector3r& sigmaT(phys->sigmaT);

	epsN = - (-phys->refPD + geom->penetrationDepth) / phys->refLength;
	geom->rotate(epsT);
	epsT += geom->shearIncrement() / phys->refLength;

	/* constitutive law */
	sigmaN = E*epsN;
	sigmaT = G*epsT;

	Real st = sigmaT.norm();
	bool cond1 = sigmaN - phys->tensileStrength > 0;
	bool cond2 = st + sigmaN*phys->tangensOfFrictionAngle - phys->cohesion > 0;
	bool cond3 = std::pow(sigmaN,2) + std::pow(phys->ellAspect*st,2) - std::pow(phys->compressiveStrength,2) > 0;
	if (cond1 || cond2 || cond3) {
		return false;
	}
   
	phys->normalForce = -sigmaN*crossSection*geom->normal;
	phys->shearForce = -sigmaT*crossSection;

	State* s1 = b1->state.get();
	State* s2 = b2->state.get();	

	Vector3r f = -phys->normalForce - phys->shearForce;
	if (!scene->isPeriodic) {
		applyForceAtContactPoint(f, geom->contactPoint , id1, s1->se3.position, id2, s2->se3.position);
	} else {
		scene->forces.addForce(id1,f);
		scene->forces.addForce(id2,-f);
		scene->forces.addTorque(id1,(geom->radius1+.5*(phys->refPD-geom->penetrationDepth))*geom->normal.cross(f));
		scene->forces.addTorque(id2,(geom->radius2+.5*(phys->refPD-geom->penetrationDepth))*geom->normal.cross(f));
	}
	return true;
}
