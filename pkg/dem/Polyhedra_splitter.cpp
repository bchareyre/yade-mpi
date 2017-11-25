// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

// NDEBUG causes crashes in CGAL sometimes. Anton
#ifdef NDEBUG
	#undef NDEBUG
#endif

#include <pkg/dem/Polyhedra_splitter.hpp>
#include <sys/stat.h>

YADE_PLUGIN((PolyhedraSplitter)(SplitPolyTauMax)(SplitPolyMohrCoulomb));
CREATE_LOGGER(PolyhedraSplitter);

using PSplitTwo = std::tuple<const shared_ptr<Body>, Vector3r, Vector3r>;
using PSplitOne = std::tuple<const shared_ptr<Body>, Vector3r>;

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
		const auto posB1 = Body::byId(I->getId1(),scene)->state->pos;
		const auto posB2 = Body::byId(I->getId2(),scene)->state->pos;
		bStresses[I->getId1()] -=f*((cP-posB1).transpose());
		bStresses[I->getId2()] +=f*((cP-posB2).transpose());
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
void SplitPolyhedraDouble(const PSplitTwo & split){
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

	vector<PSplitTwo> splitsV;
	vector<Matrix3r> bStresses (scene->bodies->size(), Matrix3r::Zero());
	getStressForEachBody(bStresses);

	for(const auto b : *(rb->bodies)) {
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
			const Vector3r dir1 = dirC.normalized() + dirT.normalized();
			const Vector3r dir2 = dirC.normalized() - dirT.normalized();
			//double sigma_t = -comp_stress/2.+ tens_stress;
			const Real sigma_t = pow((
				pow(I_valu(0,0)-I_valu(1,1),2)+
				pow(I_valu(0,0)-I_valu(2,2),2)+
				pow(I_valu(1,1)-I_valu(2,2),2))
				/2.,0.5)/p->GetVolume();
			if (sigma_t > getStrength(p->GetVolume(),m->GetStrength())) {
				splitsV.push_back(std::make_tuple(b, dir1.normalized(), dir2.normalized()));
			}
		}
	}
	std::for_each(splitsV.begin(), splitsV.end(), &SplitPolyhedraDouble);
}

//*********************************************************************************
/* Split if stress exceed strength */

void SplitPolyTauMax::action()
{
	const shared_ptr<Scene>& scene=Omega::instance().getScene();

	vector<PSplitOne> splitsV;
	vector<Matrix3r> bStresses (scene->bodies->size(), Matrix3r::Zero());
	getStressForEachBody(bStresses);

	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || !b->material || !b->shape) continue;
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		shared_ptr<PolyhedraMat> m=YADE_PTR_DYN_CAST<PolyhedraMat>(b->material);

		if(p && m->IsSplitable){
			//not real strees, to get real one, it has to be divided by body volume
			Matrix3r Sigma = bStresses[b->id];

			//get eigenstresses
			Symmetrize(Sigma);
			Eigen::SelfAdjointEigenSolver<Matrix3r> es(Sigma);

			const Matrix3r eVect = es.eigenvectors();
			const Vector3r eVals = es.eigenvalues()/p->GetVolume();

			unsigned int S1_i, S2_i, S3_i;
			Real         S1,   S2,   S3;
			std::set<unsigned int> inds {1,2,3};

			// Sigmas: indices and values
			S1 = eVals.maxCoeff(&S1_i); inds.erase(S1_i);
			S3 = eVals.minCoeff(&S3_i); inds.erase(S3_i);
			S2_i = *inds.begin(); S2 = eVals[S2_i];

			// Taus
			const Real T1 = 0.5 * std::abs(S2 - S3);
			const Real T2 = 0.5 * std::abs(S1 - S3);  // Max
			const Real T3 = 0.5 * std::abs(S1 - S2);

			if ((m->GetStrengthTau() > 0) && (T2 > (getStrength(p->GetVolume(),m->GetStrengthTau())*1000))) {
				//Split direction, tangential
				LOG_DEBUG("Split in tangential direction")
				const Vector3r SplitVector = (eVect.col(S1_i) + eVect.col(S3_i));
				splitsV.push_back(std::make_tuple(b, SplitVector.normalized()));
			} else {
				// Set absolute values for normal stresses
				Vector3r eValsAbs = Vector3r::Zero();
				for (unsigned short i=0; i < 3; i++) eValsAbs[i]=std::abs(eVals[i]);

				S1 = eValsAbs.maxCoeff(&S1_i);
				if (S1 > getStrength(p->GetVolume(),m->GetStrength()*1000)) {
					//Split direction, normal
					LOG_DEBUG("Split in normal direction")
					const Vector3r SplitVector = (eVect.col(S1_i));
					splitsV.push_back(std::make_tuple(b, SplitVector.normalized()));
				}
			}
		}
	}
	for (auto s : splitsV) {
		const auto b = get<0>(s);
		const auto vec = get<1>(s);
		Polyhedra* A = static_cast<Polyhedra*>(b->shape.get());
		//PrintPolyhedron(A->GetPolyhedron());
		shared_ptr<Body> B2 = SplitPolyhedra(b, vec, b->state->pos);
		A = static_cast<Polyhedra*>(b->shape.get());
		//PrintPolyhedron(A->GetPolyhedron());
		A = static_cast<Polyhedra*>(B2->shape.get());
		//PrintPolyhedron(A->GetPolyhedron());
	}
}

//*********************************************************************************
/* Split if stress exceed strength */

inline bool isPolyhedraBroken (const Real & Sigma0, const Real & Sigma, const Real & V0, const Real & V, unsigned int m, const Real & P) {
	// [Gladk2017], eq. (6)
	const Real failureProbability = 1 - exp(-V/V0 * (std::pow((Sigma/Sigma0),m)));
	if (failureProbability > P) {
		return true;
	} else {
		return false;
	}
}

void SplitPolyMohrCoulomb::action() {
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	vector<PSplitOne> splitsV;
	vector<Matrix3r> bStresses (scene->bodies->size(), Matrix3r::Zero());
	getStressForEachBody(bStresses);

	ofstream fileS;
	struct stat buffer;
	if (stat (fileName.c_str(), &buffer) != 0) {
		fileS.open (fileName, ios::out);
		fileS << "id\ttime\titer\tV\tmass\tS1\tS3\tSigmaCD\tSigmaCZ\tSigmaT\tSigmaV\tSigma0\tWei_m\tWei_V0\tP\tpGen\tVersagen\tS2"<<std::endl;
		fileS.close();
	}

	fileS.open (fileName, ios::out | ios::app);

	for(const auto b : *(scene->bodies)) {
		if(!b || !b->material || !b->shape) continue;
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		shared_ptr<PolyhedraMat> m=YADE_PTR_DYN_CAST<PolyhedraMat>(b->material);

		if(p && m->IsSplitable){
			//not real strees, to get real one, it has to be divided by body volume
			Matrix3r Sigma = bStresses[b->id];

			//get eigenstresses
			Symmetrize(Sigma);
			Eigen::SelfAdjointEigenSolver<Matrix3r> es(Sigma);

			const Matrix3r eVect = es.eigenvectors();
			const Vector3r eVals = es.eigenvalues()/p->GetVolume();

			const auto V0 = m->GetWeiV0();
			const auto V = p->GetVolume();
			const auto M = m->GetWeiM();
			const auto P = m->GetP();

			unsigned int S1_i = 0, S2_i = 0, S3_i = 0;
			Real         S1 = 0.,   S2 = 0.,   S3 = 0.;
			std::set<unsigned int> inds {0,1,2};
			// Sigmas: indices and values
			S1 = eVals.maxCoeff(&S1_i); inds.erase(S1_i);
			S3 = eVals.minCoeff(&S3_i); inds.erase(S3_i);
			S2_i = *inds.begin(); S2 = eVals[S2_i];

			if ((m->GetStrengthSigmaCZ() > 0) && (m->GetStrengthSigmaCD() > 0) ) {
				//Split direction, tangential
				const Real SigmaCZ = m->GetStrengthSigmaCZ();
				const Real SigmaCD = m->GetStrengthSigmaCD();
				bool BodyBroken = false;
				Real SigmaV = 0.;

				//==================================
				if (S1 <= 0 and S3 <= 0) {
					if (P>0) {
						BodyBroken = isPolyhedraBroken(SigmaCD, -S3, V0, V, M, P);
					} else {
						if (S3 < -SigmaCD) {
							BodyBroken = true;
						}
					}
					SigmaV = -S3;
				} else if (S1 > 0 and S3 > 0) {
					if (P>0) {
						BodyBroken = isPolyhedraBroken(std::abs(SigmaCZ), S1, V0, V, M, P);
					} else {
						if (S1 > SigmaCZ) {
							BodyBroken = true;
						 }
					}
					SigmaV = S1;
				} else {
					const Real SigmaT = S1 - SigmaCZ/SigmaCD*S3;
					if (P>0) {
						BodyBroken = isPolyhedraBroken(std::abs(SigmaCZ), std::abs(SigmaT), V0, V, M, P);
					} else {
						if (SigmaT >= SigmaCZ) {
							BodyBroken = true;
						}
					}
					SigmaV = std::abs(SigmaT);
				}
				//==================================
				fileS	<<b->id<<"\t"
							<<scene->time<<"\t"
							<<scene->iter<<"\t"
							<<V<<"\t"
							<<b->state->mass<<"\t"
							<<S1<<"\t"
							<<S3<<"\t"
							<<SigmaCD<<"\t"
							<<SigmaCZ<<"\t"
							<<(S1 - SigmaCZ/SigmaCD*S3)<<"\t"
							<<SigmaV<<"\t"
							<<m->GetWeiS0()<<"\t"
							<<M<<"\t"
							<<V0<<"\t"
							<<m->GetP()<<"\t"
							;
				fileS	<<"-1\t";

				fileS	<<BodyBroken<<"\t";
				fileS	<<S2<<"\n";

				if (BodyBroken) {
					const Vector3r SplitVector = (eVect.col(S1_i) + eVect.col(S3_i));
					splitsV.push_back(std::make_tuple(b, SplitVector.normalized()));
				}
			}
		}
	}

	fileS.close();

	for (auto s : splitsV) {
		const auto b = get<0>(s);
		const auto vec = get<1>(s);
		shared_ptr<Body> B2 = SplitPolyhedra(b, vec, b->state->pos);
	}
}

#endif // YADE_CGAL
