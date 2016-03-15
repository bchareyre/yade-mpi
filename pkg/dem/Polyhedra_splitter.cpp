// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include <pkg/dem/Polyhedra_splitter.hpp>

YADE_PLUGIN((PolyhedraSplitter));
CREATE_LOGGER(PolyhedraSplitter);

using PSplitT = std::tuple<const shared_ptr<Body>&, Vector3r, Vector3r>;

//*********************************************************************************
/* Evaluate tensorial stress estimation in polyhedras */

void getStressForEachBody(vector<Matrix3r>& bStresses){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		PolyhedraGeom* geom=YADE_CAST<PolyhedraGeom*>(I->geom.get());
		PolyhedraPhys* phys=YADE_CAST<PolyhedraPhys*>(I->phys.get());
		if(!geom || !phys) continue;
		Vector3r f=phys->normalForce+phys->shearForce;
		//Sum f_i*l_j for each contact of each particle
		const auto cP = geom->contactPoint;
		bStresses[I->getId1()] -=f*((cP-Body::byId(I->getId1(),scene)->state->pos).transpose());
		bStresses[I->getId2()] +=f*((cP-Body::byId(I->getId2(),scene)->state->pos).transpose());
	}
}

//*********************************************************************************
/* Size dependent strength */

Real PolyhedraSplitter::getStrength(const Real & volume, const Real & strength) const {
	//equvalent radius
	const Real r_eq = pow(volume*3./4./Mathr::PI,1./3.);
	//r should be in milimeters
	return strength/(r_eq/1000.);
}

//*********************************************************************************
/* Symmetrization of stress tensor */

void PolyhedraSplitter::Symmetrize(Matrix3r & bStress){
	bStress(0,1) = (bStress(0,1) + bStress(1,0))/2.;
	bStress(0,2) = (bStress(0,2) + bStress(2,0))/2.;
	bStress(1,2) = (bStress(1,2) + bStress(2,1))/2.;
	bStress(1,0) = bStress(0,1);
	bStress(2,0) = bStress(0,2);
	bStress(2,1) = bStress(1,2);
}

//**********************************************************************************
//split polyhedra
void SplitPolyhedraDouble(const PSplitT & split){
	const auto & b =(get<0>(split));
	const auto & d1=(get<1>(split));
	const auto & d2=(get<2>(split));
	const Se3r& se3=b->state->se3;
	const Vector3r pnt = se3.position;

	shared_ptr<Body> B2 = SplitPolyhedra(b, d1, pnt);
	shared_ptr<Body> B3 = SplitPolyhedra(B2, d2, pnt);
	shared_ptr<Body> B4 = SplitPolyhedra(b, d2, pnt);
}

//*********************************************************************************
/* Split if stress exceed strength */

void PolyhedraSplitter::action()
{
	const shared_ptr<Scene> _rb=shared_ptr<Scene>();
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());

	vector<PSplitT> splitsV;
	vector<Matrix3r> bStresses (scene->bodies->size(), Matrix3r::Zero());
	getStressForEachBody(bStresses);

	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->material || !b->shape) continue;
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		shared_ptr<PolyhedraMat> m=YADE_PTR_DYN_CAST<PolyhedraMat>(b->material);
	
		if(p && m->IsSplitable){
			//not real strees, to get real one, it has to be divided by body volume
			Matrix3r stress = bStresses[b->id];

			//get eigenstresses
			Symmetrize(stress);
			Matrix3r I_vect(Matrix3r::Zero()), I_valu(Matrix3r::Zero()); 
			matrixEigenDecomposition(stress,I_vect,I_valu);

			Eigen::Matrix3f::Index min_i, max_i;
			I_valu.diagonal().minCoeff(&min_i);
			I_valu.diagonal().maxCoeff(&max_i);

			//division of stress by volume
			const Vector3r dirC = I_vect.col(max_i);
			const Vector3r dirT = I_vect.col(min_i);
			const Vector3r dir1  = dirC.normalized() + dirT.normalized();
			const Vector3r dir2  = dirC.normalized() - dirT.normalized();
			//double sigma_t = -comp_stress/2.+ tens_stress;
			const Real sigma_t = pow((pow(I_valu(0,0)-I_valu(1,1),2)+pow(I_valu(0,0)-I_valu(2,2),2)+pow(I_valu(1,1)-I_valu(2,2),2))/2.,0.5)/p->GetVolume();
			if (sigma_t > getStrength(p->GetVolume(),m->GetStrength())) {
				splitsV.push_back(std::make_tuple(b, dir1.normalized(), dir2.normalized()));
			}
		}
	}
	std::for_each(splitsV.begin(), splitsV.end(), &SplitPolyhedraDouble);
}

#endif // YADE_CGAL
