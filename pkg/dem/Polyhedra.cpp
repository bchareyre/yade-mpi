// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include "Polyhedra.hpp"

YADE_PLUGIN(/* self-contained in hpp: */ (Polyhedra) (PolyhedraGeom) (Bo1_Polyhedra_Aabb) (PolyhedraPhys) (PolyhedraMat) (Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys) (Ip2_FrictMat_PolyhedraMat_FrictPhys) (Law2_PolyhedraGeom_PolyhedraPhys_Volumetric)
	/* some code in cpp (this file): */ 
	#ifdef YADE_OPENGL
		(Gl1_Polyhedra) (Gl1_PolyhedraGeom) (Gl1_PolyhedraPhys)
	#endif
	);

//*********************************************************************************
/* Polyhedra Constructor */

void Polyhedra::Initialize(){
	if (init) return;
	bool isRandom = false;
	
	//get vertices
	int N = (int) v.size();	
	if (N==0) {
		//generate randomly
		while ((int) v.size()<4) GenerateRandomGeometry();
		N = (int) v.size();
		isRandom = true;
	}

	//compute convex hull of vertices	
	std::vector<CGALpoint> points;
	points.resize(v.size());
	for(int i=0;i<N;i++) {
		points[i] = CGALpoint(v[i][0],v[i][1],v[i][2]);
	}

	CGAL::convex_hull_3(points.begin(), points.end(), P);
	
	//connect triagular facets if possible
	std::transform(P.facets_begin(), P.facets_end(), P.planes_begin(),Plane_equation());
	P = Simplify(P, 1E-9);

	//modify order of v according to CGAl polyhedron 
	int i = 0;
	v.clear();
	for (Polyhedron::Vertex_iterator vIter = P.vertices_begin(); vIter != P.vertices_end(); ++vIter, i++){
		v.push_back(Vector3r(vIter->point().x(),vIter->point().y(),vIter->point().z()));
	}

	//list surface triangles for plotting
	faceTri.clear();
	std::transform(P.facets_begin(), P.facets_end(), P.planes_begin(),Plane_equation());
	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter != P.facets_end(); fIter++){
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		int n = fIter->facet_degree();
		hfc0 = fIter->facet_begin();		
		int a = std::distance(P.vertices_begin(), hfc0->vertex());
		for (int i=2; i<n; i++){
			++hfc0;
			faceTri.push_back(a);
			faceTri.push_back(std::distance(P.vertices_begin(), hfc0->vertex()));
			faceTri.push_back(std::distance(P.vertices_begin(), hfc0->next()->vertex()));
		}
	}

	//compute centroid and volume
	P_volume_centroid(P, &volume, &centroid);
	//check vierd behavior of CGAL in tessalation
	if(isRandom && volume*1.75<4./3.*3.14*size[0]/2.*size[1]/2.*size[2]/2.) {
		v.clear();
		seed = rand();
		Initialize();
	}
	Vector3r translation((-1)*centroid);
	
	//set centroid to be [0,0,0]
	for(int i=0;i<N;i++) {
		v[i] = v[i]-centroid;
	}
	if(isRandom) centroid = Vector3r::Zero();

	Vector3r origin(0,0,0);

	//move and rotate also the CGAL structure Polyhedron
	Transformation t_trans(1.,0.,0.,translation[0],0.,1.,0.,translation[1],0.,0.,1.,translation[2],1.);
	std::transform( P.points_begin(), P.points_end(), P.points_begin(), t_trans);

	//compute inertia
	Real vtet;
	Vector3r ctet;
	Matrix3r Itet1, Itet2;
	Matrix3r inertia_tensor(Matrix3r::Zero());
	for(int i=0; i<(int) faceTri.size(); i+=3){
		vtet = std::abs((origin-v[faceTri[i+2]]).dot((v[faceTri[i]]-v[faceTri[i+2]]).cross(v[faceTri[i+1]]-v[faceTri[i+2]]))/6.);		
		ctet = (origin+v[faceTri[i]]+v[faceTri[i+1]]+v[faceTri[i+2]]) / 4.;
		Itet1 = TetraInertiaTensor(origin-ctet, v[faceTri[i]]-ctet, v[faceTri[i+1]]-ctet, v[faceTri[i+2]]-ctet);
		ctet = ctet-origin;
		Itet2<<
			ctet[1]*ctet[1]+ctet[2]*ctet[2], -ctet[0]*ctet[1], -ctet[0]*ctet[2],
			-ctet[0]*ctet[1], ctet[0]*ctet[0]+ctet[2]*ctet[2], -ctet[2]*ctet[1],
			-ctet[0]*ctet[2], -ctet[2]*ctet[1], ctet[1]*ctet[1]+ctet[0]*ctet[0];
		inertia_tensor = inertia_tensor + Itet1 + Itet2*vtet; 
	}

	if(std::abs(inertia_tensor(0,1))+std::abs(inertia_tensor(0,2))+std::abs(inertia_tensor(1,2)) < 1E-13){
		// no need to rotate, inertia already diagonal
		orientation = Quaternionr::Identity();
		inertia = Vector3r(inertia_tensor(0,0),inertia_tensor(1,1),inertia_tensor(2,2));
	}else{
		// calculate eigenvectors of I
		Vector3r rot;
		Matrix3r I_rot(Matrix3r::Zero()), I_new(Matrix3r::Zero()); 
		matrixEigenDecomposition(inertia_tensor,I_rot,I_new);
		// I_rot = eigenvectors of inertia_tensors in columns
		// I_new = eigenvalues on diagonal
		// set positove direction of vectors - otherwise reloading does not work
		Matrix3r sign(Matrix3r::Zero()); 
		Real max_v_signed = I_rot(0,0);
		Real max_v = std::abs(I_rot(0,0));
		if (max_v < std::abs(I_rot(1,0))) {max_v_signed = I_rot(1,0); max_v = std::abs(I_rot(1,0));} 
		if (max_v < std::abs(I_rot(2,0))) {max_v_signed = I_rot(2,0); max_v = std::abs(I_rot(2,0));} 
		sign(0,0) = max_v_signed/max_v;
		max_v_signed = I_rot(0,1);
		max_v = std::abs(I_rot(0,1));
		if (max_v < std::abs(I_rot(1,1))) {max_v_signed = I_rot(1,1); max_v = std::abs(I_rot(1,1));} 
		if (max_v < std::abs(I_rot(2,1))) {max_v_signed = I_rot(2,1); max_v = std::abs(I_rot(2,1));} 
		sign(1,1) = max_v_signed/max_v;
		sign(2,2) = 1.;
		I_rot = I_rot*sign;
		// force the eigenvectors to be right-hand oriented
		Vector3r third = (I_rot.col(0)).cross(I_rot.col(1));
		I_rot(0,2) = third[0];
		I_rot(1,2) = third[1];
		I_rot(2,2) = third[2];
		
		inertia = Vector3r(I_new(0,0),I_new(1,1),I_new(2,2));
		orientation = Quaternionr(I_rot); 
		//rotate the voronoi cell so that x - is maximal inertia axis and z - is minimal inertia axis
		//orientation.normalize();  //not needed
		for(int i=0; i< (int) v.size();i++) {
			v[i] =  orientation.conjugate()*v[i];
		}
		
		//rotate also the CGAL structure Polyhedron
		Matrix3r rot_mat = (orientation.conjugate()).toRotationMatrix();
		Transformation t_rot(
			rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),
			rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),
			rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),1.);
		std::transform( P.points_begin(), P.points_end(), P.points_begin(), t_rot);
	}
	//initialization done
	init = 1;
}

void Polyhedra::setVertices(const std::vector<Vector3r>& v) {
	init = false;
	this->v = v;
	Initialize();
}

void Polyhedra::setVertices4(const Vector3r& v0, const Vector3r& v1,const Vector3r& v2,const Vector3r& v3) {
	init = false;
	v.resize(4);
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
	v[3] = v3;
	Initialize();
}

//**************************************************************************
/* Generator of randomly shaped polyhedron based on Voronoi tessellation*/

void Polyhedra::GenerateRandomGeometry(){
	srand(seed);

	vector<CGALpoint> nuclei;
	nuclei.push_back(CGALpoint(5.,5.,5.));
	CGALpoint trial;
	unsigned int iter = 0; 
	bool isOK;
	//fill box 5x5x5 with randomly located nuclei with restricted minimal mutual distance 0.75 which gives approximate mean mutual distance 1;
	Real dist_min2 = 0.75*0.75;
	while(iter<500){
		isOK = true;
		iter++;
		trial = CGALpoint(Real(rand())/RAND_MAX*5.+2.5,Real(rand())/RAND_MAX*5.+2.5,Real(rand())/RAND_MAX*5.+2.5);
		for(int i=0;i< (int) nuclei.size();i++) {
			isOK = pow(nuclei[i].x()-trial.x(),2)+pow(nuclei[i].y()-trial.y(),2)+pow(nuclei[i].z()-trial.z(),2) > dist_min2;
			if (!isOK) break;
		}
		if(isOK){
			iter = 0;
			nuclei.push_back(trial);
		}
	}
	
	//perform Voronoi tessellation
	nuclei.erase(nuclei.begin());
	Triangulation dt(nuclei.begin(), nuclei.end());
	Triangulation::Vertex_handle zero_point = dt.insert(CGALpoint(5.,5.,5.));
	v.clear();
	std::vector<Triangulation::Cell_handle>  ch_cells;
 	dt.incident_cells(zero_point,std::back_inserter(ch_cells));
	for(auto ci = ch_cells.begin(); ci !=ch_cells.end(); ++ci){
		v.push_back(FromCGALPoint(dt.dual(*ci))-Vector3r(5.,5.,5.));
	}

	//resize and rotate the voronoi cell
	Quaternionr Rot(Real(rand())/RAND_MAX,Real(rand())/RAND_MAX,Real(rand())/RAND_MAX,Real(rand())/RAND_MAX);
	Rot.normalize();
	for(int i=0; i< (int) v.size();i++) {
		v[i] = Rot*(Vector3r(v[i][0]*size[0],v[i][1]*size[1],v[i][2]*size[2]));
	}
	
	//to avoid patological cases (that should not be present, but CGAL works somehow unpredicable)
	if (v.size() < 8) {
		cout << "wrong " << v.size() << endl;
		v.clear();
		seed = rand();
		GenerateRandomGeometry();
	}
}

//**************************************************************************
/* Get polyhdral surfaces */
vector<vector<int>> Polyhedra::GetSurfaces() const {
	vector<vector<int>> ret(P.size_of_facets());
	int i=0;
	for (Polyhedron::Facet_const_iterator f = P.facets_begin(); f != P.facets_end(); f++, i++){
		Polyhedron::Halfedge_around_facet_const_circulator h=f->facet_begin();
		do {
			ret[i].push_back(std::distance(P.vertices_begin(), h->vertex()));
		} while (++h != f->facet_begin());
	}
	return ret;
}


//****************************************************************************************
/* Destructor */

Polyhedra::~Polyhedra(){}

//****************************************************************************************
/* Destructor */

PolyhedraGeom::~PolyhedraGeom(){}

//****************************************************************************************
/* AaBb overlap checker  */

void Bo1_Polyhedra_Aabb::go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){

	Polyhedra* t=static_cast<Polyhedra*>(ig.get());
	if (!t->IsInitialized()) t->Initialize();
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	//Quaternionr invRot=se3.orientation.conjugate();
	int N = (int) t->v.size();
	Vector3r v_g, mincoords(0.,0.,0.), maxcoords(0.,0.,0.);
	for(int i=0; i<N; i++) {
		v_g=se3.orientation*t->v[i]; // vertices in global coordinates
		mincoords = Vector3r(min(mincoords[0],v_g[0]),min(mincoords[1],v_g[1]),min(mincoords[2],v_g[2]));
		maxcoords = Vector3r(max(maxcoords[0],v_g[0]),max(maxcoords[1],v_g[1]),max(maxcoords[2],v_g[2]));
	}
	if (aabbEnlargeFactor>0) {
		mincoords *= aabbEnlargeFactor;
		maxcoords *= aabbEnlargeFactor;
	}
	aabb->min=se3.position+mincoords;
	aabb->max=se3.position+maxcoords;
}

//**********************************************************************************
/* Plotting */

#ifdef YADE_OPENGL
	#include<lib/opengl/OpenGLWrapper.hpp>
	bool Gl1_Polyhedra::wire;
	void Gl1_Polyhedra::go(const shared_ptr<Shape>& cm, const shared_ptr<State>&,bool wire2,const GLViewInfo&)
	{
		glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,Vector3r(cm->color[0],cm->color[1],cm->color[2]));
		glColor3v(cm->color);
		Polyhedra* t=static_cast<Polyhedra*>(cm.get());
		vector<int> faceTri = t->GetSurfaceTriangulation();

		if (wire || wire2) { 
			glDisable(GL_LIGHTING);
			glBegin(GL_LINES);
				#define __ONEWIRE(a,b) glVertex3v(t->v[a]);glVertex3v(t->v[b])
					for(int tri=0; tri < (int) faceTri.size(); tri+=3) {__ONEWIRE(faceTri[tri],faceTri[tri+1]); __ONEWIRE(faceTri[tri],faceTri[tri+2]); __ONEWIRE(faceTri[tri+1],faceTri[tri+2]);}
				#undef __ONEWIRE
			glEnd();
		}
		else
		{
			Vector3r centroid = t->GetCentroid();
			Vector3r faceCenter, n;
			glDisable(GL_CULL_FACE); glEnable(GL_LIGHTING);
			glBegin(GL_TRIANGLES);
				#define __ONEFACE(a,b,c) n=(t->v[b]-t->v[a]).cross(t->v[c]-t->v[a]); n.normalize(); faceCenter=(t->v[a]+t->v[b]+t->v[c])/3.; if((faceCenter-centroid).dot(n)<0)n=-n; glNormal3v(n); glVertex3v(t->v[a]); glVertex3v(t->v[b]); glVertex3v(t->v[c]);
					for(int tri=0; tri < (int) faceTri.size(); tri+=3) {__ONEFACE(faceTri[tri],faceTri[tri+1],faceTri[tri+2]);}
				#undef __ONEFACE
			glEnd();
		}
	}

	void Gl1_PolyhedraGeom::go(const shared_ptr<IGeom>& ig, const shared_ptr<Interaction>&,
		const shared_ptr<Body>&, const shared_ptr<Body>&, bool) {draw(ig);}

	void Gl1_PolyhedraGeom::draw(const shared_ptr<IGeom>& ig){};

	GLUquadric* Gl1_PolyhedraPhys::gluQuadric=NULL;
	Real Gl1_PolyhedraPhys::maxFn;
	Real Gl1_PolyhedraPhys::refRadius;
	Real Gl1_PolyhedraPhys::maxRadius;
	int Gl1_PolyhedraPhys::signFilter;
	int Gl1_PolyhedraPhys::slices;
	int Gl1_PolyhedraPhys::stacks;

	void Gl1_PolyhedraPhys::go(const shared_ptr<IPhys>& ip, const shared_ptr<Interaction>& i,
		const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		if(!gluQuadric){
			gluQuadric=gluNewQuadric();
			if(!gluQuadric) throw runtime_error("Gl1_PolyhedraPhys::go unable to allocate new GLUquadric object (out of memory?).");
		}
		PolyhedraPhys* np=static_cast<PolyhedraPhys*>(ip.get());
		shared_ptr<IGeom> ig(i->geom); if(!ig) return; // changed meanwhile?
		PolyhedraGeom* geom=YADE_CAST<PolyhedraGeom*>(ig.get());
		Real fnNorm=np->normalForce.dot(geom->normal);
		if((signFilter>0 && fnNorm<0) || (signFilter<0 && fnNorm>0)) return;
		int fnSign=fnNorm>0?1:-1;
		fnNorm=std::abs(fnNorm);
		Real radiusScale=1.;
		maxFn=max(fnNorm,maxFn);
		Real realMaxRadius;
		if(maxRadius<0){
			refRadius=min(0.03,refRadius);
			realMaxRadius=refRadius;
		}
		else realMaxRadius=maxRadius;
		Real radius=radiusScale*realMaxRadius*(fnNorm/maxFn); 
		if (radius<=0.) radius = 1E-8;
		Vector3r color=Shop::scalarOnColorScale(fnNorm*fnSign,-maxFn,maxFn);
		
		Vector3r p1=b1->state->pos, p2=b2->state->pos;
		Vector3r relPos;
		relPos=p2-p1;
		Real dist=relPos.norm();
		
		glDisable(GL_CULL_FACE); 
		glPushMatrix();
			glTranslatef(p1[0],p1[1],p1[2]);
			Quaternionr q(Quaternionr().setFromTwoVectors(Vector3r(0,0,1),relPos/dist /* normalized */));
			// using Transform with OpenGL: http://eigen.tuxfamily.org/dox/TutorialGeometry.html
			//glMultMatrixd(Eigen::Affine3d(q).data());
			glMultMatrix(Eigen::Transform<Real,3,Eigen::Affine>(q).data());
			glColor3v(color);
			gluCylinder(gluQuadric,radius,radius,dist,slices,stacks);
		glPopMatrix();
	}
#endif

//**********************************************************************************
//!Precompute data needed for rotating tangent vectors attached to the interaction

void PolyhedraGeom::precompute(const State& rbp1, const State& rbp2, const Scene* scene,
	const shared_ptr<Interaction>& c, const Vector3r& currentNormal, bool isNew, const Vector3r& shift2) {
	
	if(!isNew) {
		orthonormal_axis = normal.cross(currentNormal);
		Real angle = scene->dt*0.5*normal.dot(rbp1.angVel + rbp2.angVel);
		twist_axis = angle*normal;}
	else twist_axis=orthonormal_axis=Vector3r::Zero(); 
	//Update contact normal
	normal=currentNormal;
	//Precompute shear increment
	Vector3r c1x = (contactPoint - rbp1.pos);
	Vector3r c2x = (contactPoint - rbp2.pos + shift2);
	Vector3r relativeVelocity = (rbp2.vel+rbp2.angVel.cross(c2x)) - (rbp1.vel+rbp1.angVel.cross(c1x));
	//keep the shear part only
	relativeVelocity = relativeVelocity-normal.dot(relativeVelocity)*normal;
	shearInc = relativeVelocity*scene->dt;
}

//**********************************************************************************
Vector3r& PolyhedraGeom::rotate(Vector3r& shearForce) const {
	// approximated rotations
	shearForce -= shearForce.cross(orthonormal_axis);
	shearForce -= shearForce.cross(twist_axis);
	//NOTE : make sure it is in the tangent plane? It's never been done before. Is it not adding rounding errors at the same time in fact?...
	shearForce -= normal.dot(shearForce)*normal;
	return shearForce;
}

//**********************************************************************************
/* Material law, physics */

void Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys::go( const shared_ptr<Material>& b1
					, const shared_ptr<Material>& b2
					, const shared_ptr<Interaction>& interaction) {
	if(interaction->phys) return;
	const shared_ptr<PolyhedraMat>& mat1 = YADE_PTR_CAST<PolyhedraMat>(b1);
	const shared_ptr<PolyhedraMat>& mat2 = YADE_PTR_CAST<PolyhedraMat>(b2);
	interaction->phys = shared_ptr<PolyhedraPhys>(new PolyhedraPhys());
	const shared_ptr<PolyhedraPhys>& contactPhysics = YADE_PTR_CAST<PolyhedraPhys>(interaction->phys);
	Real Kna 	= mat1->young;
	Real Knb 	= mat2->young;
	Real Ksa 	= mat1->young*mat1->poisson;
	Real Ksb 	= mat2->young*mat2->poisson;
	Real frictionAngle = std::min(mat1->frictionAngle,mat2->frictionAngle);	
        contactPhysics->tangensOfFrictionAngle = std::tan(frictionAngle);
	contactPhysics->kn = Kna*Knb/(Kna+Knb);
	contactPhysics->ks = Ksa*Ksb/(Ksa+Ksb);
};

void Ip2_FrictMat_PolyhedraMat_FrictPhys::go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction){
	const shared_ptr<FrictMat>& mat1 = YADE_PTR_CAST<FrictMat>(pp1);
	const shared_ptr<PolyhedraMat>& mat2 = YADE_PTR_CAST<PolyhedraMat>(pp2);
	Ip2_FrictMat_FrictMat_FrictPhys().go(mat1,mat2,interaction);
}

//**************************************************************************************
Real Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::getPlasticDissipation() {return (Real) plasticDissipation;}
void Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::initPlasticDissipation(Real initVal) {plasticDissipation.reset(); plasticDissipation+=initVal;}
Real Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::elasticEnergy()
{
	Real energy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		FrictPhys* phys = dynamic_cast<FrictPhys*>(I->phys.get());
		if(phys) {
			energy += 0.5*(phys->normalForce.squaredNorm()/phys->kn + phys->shearForce.squaredNorm()/phys->ks);}
	}
	return energy;
}

//**************************************************************************************
// Apply forces on polyhedrons in collision based on geometric configuration
bool Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* I){
		if (!I->geom) {return true;} 
		const shared_ptr<PolyhedraGeom>& contactGeom(YADE_PTR_DYN_CAST<PolyhedraGeom>(I->geom));
		if(!contactGeom) {return true;} 
		const Body::id_t idA=I->getId1(), idB=I->getId2();
		const shared_ptr<Body>& A=Body::byId(idA), B=Body::byId(idB);

		PolyhedraPhys* phys = dynamic_cast<PolyhedraPhys*>(I->phys.get());	

		//erase the interaction when aAbB shows separation, otherwise keep it to be able to store previous separating plane for fast detection of separation 
		Vector3r shift2=scene->cell->hSize*I->cellDist.cast<Real>();
		if (A->bound->min[0] >= B->bound->max[0]+shift2[0] ||
				B->bound->min[0]+shift2[0] >= A->bound->max[0] ||
				A->bound->min[1] >= B->bound->max[1]+shift2[1] ||
				B->bound->min[1]+shift2[1] >= A->bound->max[1] ||
				A->bound->min[2] >= B->bound->max[2]+shift2[2] ||
				B->bound->min[2]+shift2[2] >= A->bound->max[2]) {
			return false;
		}
		
		//zero penetration depth means no interaction force
		if(!(contactGeom->equivalentPenetrationDepth > 1E-18) || !(contactGeom->penetrationVolume > 0)) {
			phys->normalForce = Vector3r(0.,0.,0.);
			phys->shearForce = Vector3r(0.,0.,0.);
			return true;
		}
    
		Real prop = std::pow(contactGeom->penetrationVolume,volumePower);
		Vector3r normalForce=contactGeom->normal*prop*phys->kn;

		//shear force: in case the polyhdras are separated and come to contact again, one
		//should not use the previous shear force
		if (contactGeom->isShearNew) shearForce = Vector3r::Zero();
		else shearForce = contactGeom->rotate(shearForce);

		const Vector3r& shearDisp = contactGeom->shearInc;
		shearForce -= phys->ks*shearDisp;
		const Real maxFs = normalForce.squaredNorm()*std::pow(phys->tangensOfFrictionAngle,2);

		if(shearForce.squaredNorm() > maxFs && maxFs){
			//PFC3d SlipModel, is using friction angle. CoulombCriterion
			const Real ratio = sqrt(maxFs) / shearForce.norm();

			//Store prev force for definition of plastic slip
			//Define the plastic work input and increment the total plastic energy dissipated
			const Vector3r trialForce=shearForce;
			shearForce *= ratio;
			
			if (scene->trackEnergy && traceEnergy) {
				const Real dissip=((1/phys->ks)*(trialForce-shearForce)).dot(shearForce);

				if (traceEnergy) plasticDissipation += dissip;
				else if(dissip>0) scene->energy->add(dissip,"plastDissip",plastDissipIx,false);

				// compute elastic energy as well
				scene->energy->add(0.5*(normalForce.squaredNorm()/phys->kn+shearForce.squaredNorm()/phys->ks),
					"elastPotential",elastPotentialIx,true);
			}
		} else {
			shearForce = Vector3r::Zero();
		}
		Vector3r F = -normalForce-shearForce;	
		if (contactGeom->equivalentPenetrationDepth != contactGeom->equivalentPenetrationDepth) exit(1);
		
		scene->forces.addForce (idA,F);
		scene->forces.addForce (idB, -F);
		scene->forces.addTorque(idA, -(A->state->pos-contactGeom->contactPoint).cross(F));
		scene->forces.addTorque(idB, (B->state->pos-contactGeom->contactPoint).cross(F));	

		/*
		FILE * fin = fopen("Forces.dat","a");
		fprintf(fin,"************** IDS %d %d **************\n",idA, idB);
		Vector3r T = (B->state->pos-contactGeom->contactPoint).cross(F);
		fprintf(fin,"volume\t%e\n",contactGeom->penetrationVolume);	
		fprintf(fin,"normal_force\t%e\t%e\t%e\n",normalForce[0],normalForce[1],normalForce[2]);	
		fprintf(fin,"shear_force\t%e\t%e\t%e\n",shearForce[0],shearForce[1],shearForce[2]);	
		fprintf(fin,"total_force\t%e\t%e\t%e\n",F[0],F[1],F[2]);		
		fprintf(fin,"torsion\t%e\t%e\t%e\n",T[0],T[1],T[2]);
		fprintf(fin,"A\t%e\t%e\t%e\n",A->state->pos[0],A->state->pos[1],A->state->pos[2]);
		fprintf(fin,"B\t%e\t%e\t%e\n",B->state->pos[0],B->state->pos[1],B->state->pos[2]);
		fprintf(fin,"centroid\t%e\t%e\t%e\n",contactGeom->contactPoint[0],contactGeom->contactPoint[1],contactGeom->contactPoint[2]);
		fclose(fin);
		*/
		//needed to be able to acces interaction forces in other parts of yade
		phys->normalForce = normalForce;
		phys->shearForce = shearForce;
		return true;
}

#endif // YADE_CGAL
