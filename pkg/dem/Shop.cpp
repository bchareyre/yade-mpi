// 2007 © Václav Šmilauer <eudoxos@arcig.cz>
#include"Shop.hpp"

#include<limits>

#include<boost/filesystem/convenience.hpp>
#include<boost/tokenizer.hpp>
#include<boost/tuple/tuple.hpp>

#include<yade/core/Scene.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Interaction.hpp>

#include<yade/pkg/common/Aabb.hpp>
#include<yade/pkg/dem/Clump.hpp>
#include<yade/pkg/common/InsertionSortCollider.hpp>

#include<yade/pkg/common/Box.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/dem/ViscoelasticPM.hpp>

#include<yade/pkg/common/Bo1_Sphere_Aabb.hpp>
#include<yade/pkg/common/Bo1_Box_Aabb.hpp>
#include<yade/pkg/dem/NewtonIntegrator.hpp>
#include<yade/pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#include<yade/pkg/dem/Ig2_Box_Sphere_ScGeom.hpp>
#include<yade/pkg/dem/Ip2_FrictMat_FrictMat_FrictPhys.hpp>

#include<yade/pkg/common/ForceResetter.hpp>

#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/InteractionLoop.hpp>
#include<yade/pkg/common/GravityEngines.hpp>

#include<yade/pkg/dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg/dem/ElasticContactLaw.hpp>

#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>


#include<yade/pkg/dem/Tetra.hpp>

#ifdef YADE_OPENGL
	#include<yade/pkg/common/Gl1_NormPhys.hpp>
#endif

#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif


CREATE_LOGGER(Shop);

/*! Flip periodic cell by given number of cells.

Still broken, some interactions are missed. Should be checked.
*/

Matrix3r Shop::flipCell(const Matrix3r& _flip){
	Scene* scene=Omega::instance().getScene().get(); const shared_ptr<Cell>& cell(scene->cell); const Matrix3r& trsf(cell->trsf);
	Vector3r size=cell->getSize();
	Matrix3r flip;
	if(_flip==Matrix3r::Zero()){
		bool hasNonzero=false;
		for(int i=0; i<3; i++) for(int j=0; j<3; j++) {
			if(i==j){ flip(i,j)=0; continue; }
			flip(i,j)=-floor(.5+trsf(i,j)/(size[j]/size[i]));
			if(flip(i,j)!=0) hasNonzero=true;
		}
		if(!hasNonzero) {LOG_TRACE("No flip necessary."); return Matrix3r::Zero();}
		LOG_DEBUG("Computed flip matrix: upper "<<flip(0,1)<<","<<flip(0,2)<<","<<flip(1,2)<<"; lower "<<flip(1,0)<<","<<flip(2,0)<<","<<flip(2,1));
	} else {
		flip=_flip;
	}

	// current cell coords of bodies
	vector<Vector3i > oldCells; oldCells.resize(scene->bodies->size());
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b) continue; cell->wrapShearedPt(b->state->pos,oldCells[b->getId()]);
	}

	// change cell trsf here
	Matrix3r trsfInc;
	for(int i=0; i<3; i++) for(int j=0; j<3; j++){
		if(i==j) { if(flip(i,j)!=0) LOG_WARN("Non-zero diagonal term at ["<<i<<","<<j<<"] is meaningless and will be ignored."); trsfInc(i,j)=0; continue; }
		// make sure non-diagonal entries are "integers"
		if(flip(i,j)!=double(int(flip(i,j)))) LOG_WARN("Flip matrix entry "<<flip(i,j)<<" at ["<<i<<","<<j<<"] not integer?! (will be rounded)");
		trsfInc(i,j)=int(flip(i,j))*size[j]/size[i];
	}
	TRWM3MAT(cell->trsf);
	TRWM3MAT(trsfInc);
	cell->trsf+=trsfInc;
	cell->postLoad(*cell);

	// new cell coords of bodies
	vector<Vector3i > newCells; newCells.resize(scene->bodies->size());
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b) continue;
		cell->wrapShearedPt(b->state->pos,newCells[b->getId()]);
	}

	// remove all potential interactions
	scene->interactions->eraseNonReal();
	// adjust Interaction::cellDist for real interactions;
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		Body::id_t id1=i->getId1(),id2=i->getId2();
		// this must be the same for both old and new interaction: cell2-cell1+cellDist
		// c₂-c₁+c₁₂=k; c'₂+c₁'+c₁₂'=k   (new cell coords have ')
		// c₁₂'=(c₂-c₁+c₁₂)-(c₂'-c₁')
		i->cellDist=(oldCells[id2]-oldCells[id1]+i->cellDist)-(newCells[id2]-newCells[id1]);
	}


	// force reinitialization of the collider
	bool colliderFound=false;
	FOREACH(const shared_ptr<Engine>& e, scene->engines){
		Collider* c=dynamic_cast<Collider*>(e.get());
		if(c){ colliderFound=true; c->invalidatePersistentData(); }
	}
	if(!colliderFound) LOG_WARN("No collider found while flipping cell; continuing simulation might give garbage results.");
	return flip;
}

/* Apply force on contact point to 2 bodies; the force is oriented as it applies on the first body and is reversed on the second.
 */
void Shop::applyForceAtContactPoint(const Vector3r& force, const Vector3r& contPt, Body::id_t id1, const Vector3r& pos1, Body::id_t id2, const Vector3r& pos2, Scene* scene){
	scene->forces.addForce(id1,force);
	scene->forces.addForce(id2,-force);
	scene->forces.addTorque(id1,(contPt-pos1).cross(force));
	scene->forces.addTorque(id2,-(contPt-pos2).cross(force));
}


/*! Compute sum of forces in the whole simulation and averages stiffness.

Designed for being used with periodic cell, where diving the resulting components by
areas of the cell will give average stress in that direction.

Requires all .isReal() interaction to have phys deriving from NormShearPhys.
*/
Vector3r Shop::totalForceInVolume(Real& avgIsoStiffness, Scene* _rb){
	Scene* rb=_rb ? _rb : Omega::instance().getScene().get();
	Vector3r force(Vector3r::Zero()); Real stiff=0; long n=0;
	FOREACH(const shared_ptr<Interaction>&I, *rb->interactions){
		if(!I->isReal()) continue;
		NormShearPhys* nsi=YADE_CAST<NormShearPhys*>(I->phys.get());
		force+=Vector3r(abs(nsi->normalForce[0]+nsi->shearForce[0]),abs(nsi->normalForce[1]+nsi->shearForce[1]),abs(nsi->normalForce[2]+nsi->shearForce[2]));
		stiff+=(1/3.)*nsi->kn+(2/3.)*nsi->ks; // count kn in one direction and ks in the other two
		n++;
	}
	avgIsoStiffness= n>0 ? (1./n)*stiff : -1;
	return force;
}

Real Shop::unbalancedForce(bool useMaxForce, Scene* _rb){
	Scene* rb=_rb ? _rb : Omega::instance().getScene().get();
	rb->forces.sync();
	// get maximum force on a body and sum of all forces (for averaging)
	Real sumF=0,maxF=0,currF; int nb=0;
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || b->isClumpMember() || !b->isDynamic()) continue;
		if(!b->isClump()){ currF=rb->forces.getForce(b->id).norm(); }
		else { // for clump, sum forces from members on the clump itself
			Vector3r f(rb->forces.getForce(b->id)),m(Vector3r::Zero());
			b->shape->cast<Clump>().addForceTorqueFromMembers(b->state.get(),rb,f,m);
			currF=f.norm();
		}
		maxF=max(currF,maxF); sumF+=currF; nb++;
	}
	Real meanF=sumF/nb;
	// get max force on contacts
	sumF=0; nb=0;
	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal()) continue;
		shared_ptr<NormShearPhys> nsi=YADE_PTR_CAST<NormShearPhys>(I->phys); assert(nsi);
		sumF+=(nsi->normalForce+nsi->shearForce).norm(); nb++;
	}
	sumF/=nb;
	return (useMaxForce?maxF:meanF)/(sumF);
}

Real Shop::kineticEnergy(Scene* _scene, Body::id_t* maxId){
	Scene* scene=_scene ? _scene : Omega::instance().getScene().get();
	Real ret=0.;
	Real maxE=0; if(maxId) *maxId=Body::ID_NONE;
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || !b->isDynamic()) continue;
		const State* state(b->state.get());
		// ½(mv²+ωIω)
		Real E=0;
		if(scene->isPeriodic){
			/* Only take in account the fluctuation velocity, not the mean velocity of homothetic resize. */
			E=.5*state->mass*scene->cell->bodyFluctuationVel(state->pos,state->vel).squaredNorm();
		} else {
			E=.5*(state->mass*state->vel.squaredNorm());
		}
		if(b->isAspherical()){
			Matrix3r T(state->ori);
			// the tensorial expression http://en.wikipedia.org/wiki/Moment_of_inertia#Moment_of_inertia_tensor
			// inertia tensor rotation from http://www.kwon3d.com/theory/moi/triten.html
			Matrix3r mI; mI<<state->inertia[0],0,0, 0,state->inertia[1],0, 0,0,state->inertia[2];
			E+=.5*state->angVel.transpose().dot((T.transpose()*mI*T)*state->angVel);
		}
		else { E+=0.5*state->angVel.dot(state->inertia.cwise()*state->angVel);}
		if(maxId && E>maxE) { *maxId=b->getId(); maxE=E; }
		ret+=E;
	}
	return ret;
}

shared_ptr<FrictMat> Shop::defaultGranularMat(){
	shared_ptr<FrictMat> mat(new FrictMat);
	mat->density=2e3;
	mat->young=30e9;
	mat->poisson=.3;
	mat->frictionAngle=.5236; //30˚
	return mat;
}

/*! Create body - sphere. */
shared_ptr<Body> Shop::sphere(Vector3r center, Real radius, shared_ptr<Material> mat){
	shared_ptr<Body> body(new Body);
	body->material=mat ? mat : static_pointer_cast<Material>(defaultGranularMat());
	body->state->pos=center;
	body->state->mass=4.0/3.0*Mathr::PI*radius*radius*radius*body->material->density;
	body->state->inertia=Vector3r(2.0/5.0*body->state->mass*radius*radius,2.0/5.0*body->state->mass*radius*radius,2.0/5.0*body->state->mass*radius*radius);
	body->bound=shared_ptr<Aabb>(new Aabb);
	body->shape=shared_ptr<Sphere>(new Sphere(radius));
	return body;
}

/*! Create body - box. */
shared_ptr<Body> Shop::box(Vector3r center, Vector3r extents, shared_ptr<Material> mat){
	shared_ptr<Body> body(new Body);
	body->material=mat ? mat : static_pointer_cast<Material>(defaultGranularMat());
	body->state->pos=center;
	Real mass=8.0*extents[0]*extents[1]*extents[2]*body->material->density;
	body->state->mass=mass;
	body->state->inertia=Vector3r(mass*(4*extents[1]*extents[1]+4*extents[2]*extents[2])/12.,mass*(4*extents[0]*extents[0]+4*extents[2]*extents[2])/12.,mass*(4*extents[0]*extents[0]+4*extents[1]*extents[1])/12.);
	body->bound=shared_ptr<Aabb>(new Aabb);
	body->shape=shared_ptr<Box>(new Box(extents));
	return body;
}

/*! Create body - tetrahedron. */
shared_ptr<Body> Shop::tetra(Vector3r v_global[4], shared_ptr<Material> mat){
	shared_ptr<Body> body(new Body);
	body->material=mat ? mat : static_pointer_cast<Material>(defaultGranularMat());
	Vector3r centroid=(v_global[0]+v_global[1]+v_global[2]+v_global[3])*.25;
	Vector3r v[4]; for(int i=0; i<4; i++) v[i]=v_global[i]-centroid;
	body->state->pos=centroid;
	body->state->mass=body->material->density*TetrahedronVolume(v);
	// inertia will be calculated below, by TetrahedronWithLocalAxesPrincipal
	body->bound=shared_ptr<Aabb>(new Aabb);
	body->shape=shared_ptr<Tetra>(new Tetra(v[0],v[1],v[2],v[3]));
	// make local axes coincident with principal axes
	TetrahedronWithLocalAxesPrincipal(body);
	return body;
}


void Shop::saveSpheresToFile(string fname){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	ofstream f(fname.c_str());
	if(!f.good()) throw runtime_error("Unable to open file `"+fname+"'");

	FOREACH(shared_ptr<Body> b, *scene->bodies){
		if (!b->isDynamic()) continue;
		shared_ptr<Sphere>	intSph=dynamic_pointer_cast<Sphere>(b->shape);
		if(!intSph) continue;
		const Vector3r& pos=b->state->pos;
		f<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<intSph->radius<<endl; // <<" "<<1<<" "<<1<<endl;
	}
	f.close();
}

Real Shop::getSpheresVolume(const shared_ptr<Scene>& _scene){
	const shared_ptr<Scene> scene=(_scene?_scene:Omega::instance().getScene());
	Real vol=0;
	FOREACH(shared_ptr<Body> b, *scene->bodies){
		if (!b || !b->isDynamic()) continue;
		Sphere* s=dynamic_cast<Sphere*>(b->shape.get());
		if(!s) continue;
		vol += (4/3.)*Mathr::PI*pow(s->radius,3);
	}
	return vol;
}

Real Shop::getPorosity(const shared_ptr<Scene>& _scene, Real _volume){
	const shared_ptr<Scene> scene=(_scene?_scene:Omega::instance().getScene());
	Real V;
	if(!scene->isPeriodic){
		if(_volume<=0) throw std::invalid_argument("utils.porosity must be given (positive) *volume* for aperiodic simulations.");
		V=_volume;
	} else {
		V=scene->cell->getVolume();
	}
	Real Vs=Shop::getSpheresVolume();
	return (V-Vs)/V;
}


vector<pair<Vector3r,Real> > Shop::loadSpheresFromFile(const string& fname, Vector3r& minXYZ, Vector3r& maxXYZ, Vector3r* cellSize){
	if(!boost::filesystem::exists(fname)) {
		throw std::invalid_argument(string("File with spheres `")+fname+"' doesn't exist.");
	}
	vector<pair<Vector3r,Real> > spheres;
	ifstream sphereFile(fname.c_str());
	if(!sphereFile.good()) throw std::runtime_error("File with spheres `"+fname+"' couldn't be opened.");
	Vector3r C;
	Real r=0;
	string line;
	size_t lineNo=0;
	while(std::getline(sphereFile, line, '\n')){
		lineNo++;
		boost::tokenizer<boost::char_separator<char> > toks(line,boost::char_separator<char>(" \t"));
		vector<string> tokens; FOREACH(const string& s, toks) tokens.push_back(s);
		if(tokens.empty()) continue;
		if(tokens[0]=="##PERIODIC::"){
			if(tokens.size()!=4) throw std::invalid_argument(("Spheres file "+fname+":"+lexical_cast<string>(lineNo)+" contains ##PERIODIC::, but the line is malformed.").c_str());
			if(cellSize){ *cellSize=Vector3r(lexical_cast<Real>(tokens[1]),lexical_cast<Real>(tokens[2]),lexical_cast<Real>(tokens[3])); }
			continue;
		}
		if(tokens.size()!=4) throw std::invalid_argument(("Line "+lexical_cast<string>(lineNo)+" in the spheres file "+fname+" has "+lexical_cast<string>(tokens.size())+" columns (must be 4).").c_str());
		C=Vector3r(lexical_cast<Real>(tokens[0]),lexical_cast<Real>(tokens[1]),lexical_cast<Real>(tokens[2]));
		r=lexical_cast<Real>(tokens[3]);
		for(int j=0; j<3; j++) { minXYZ[j]=(spheres.size()>0?min(C[j]-r,minXYZ[j]):C[j]-r); maxXYZ[j]=(spheres.size()>0?max(C[j]+r,maxXYZ[j]):C[j]+r);}
		spheres.push_back(pair<Vector3r,Real>(C,r));
	}
	return spheres;
}

Real Shop::PWaveTimeStep(const shared_ptr<Scene> _rb){
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());
	Real dt=std::numeric_limits<Real>::infinity();
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->material || !b->shape) continue;
		shared_ptr<ElastMat> ebp=dynamic_pointer_cast<ElastMat>(b->material);
		shared_ptr<Sphere> s=dynamic_pointer_cast<Sphere>(b->shape);
		if(!ebp || !s) continue;
		Real density=b->state->mass/((4/3.)*Mathr::PI*pow(s->radius,3));
		dt=min(dt,s->radius/sqrt(ebp->young/density));
	}
	return dt;
}
/* Project 3d point into 2d using spiral projection along given axis;
 * the returned tuple is
 *
 *  (height relative to the spiral, distance from axis, theta )
 *
 * dH_dTheta is the inclination of the spiral (height increase per radian),
 * theta0 is the angle for zero height (by given axis).
 */
boost::tuple<Real,Real,Real> Shop::spiralProject(const Vector3r& pt, Real dH_dTheta, int axis, Real periodStart, Real theta0){
	int ax1=(axis+1)%3,ax2=(axis+2)%3;
	Real r=sqrt(pow(pt[ax1],2)+pow(pt[ax2],2));
	Real theta;
	if(r>Mathr::ZERO_TOLERANCE){
		theta=acos(pt[ax1]/r);
		if(pt[ax2]<0) theta=Mathr::TWO_PI-theta;
	}
	else theta=0;
	Real hRef=dH_dTheta*(theta-theta0);
	long period;
	if(isnan(periodStart)){
		Real h=Shop::periodicWrap(pt[axis]-hRef,hRef-Mathr::PI*dH_dTheta,hRef+Mathr::PI*dH_dTheta,&period);
		return boost::make_tuple(r,h,theta);
	}
	else{
		// Real hPeriodStart=(periodStart-theta0)*dH_dTheta;
		//TRVAR4(hPeriodStart,periodStart,theta0,theta);
		//Real h=Shop::periodicWrap(pt[axis]-hRef,hPeriodStart,hPeriodStart+2*Mathr::PI*dH_dTheta,&period);
		theta=Shop::periodicWrap(theta,periodStart,periodStart+2*Mathr::PI,&period);
		Real h=pt[axis]-hRef+period*2*Mathr::PI*dH_dTheta;
		//TRVAR3(pt[axis],pt[axis]-hRef,period);
		//TRVAR2(h,theta);
		return boost::make_tuple(r,h,theta);
	}
}

shared_ptr<Interaction> Shop::createExplicitInteraction(Body::id_t id1, Body::id_t id2, bool force){
	IGeomDispatcher* geomMeta=NULL;
	IPhysDispatcher* physMeta=NULL;
	shared_ptr<Scene> rb=Omega::instance().getScene();
	if(rb->interactions->find(Body::id_t(id1),Body::id_t(id2))!=0) throw runtime_error(string("Interaction #")+lexical_cast<string>(id1)+"+#"+lexical_cast<string>(id2)+" already exists.");
	FOREACH(const shared_ptr<Engine>& e, rb->engines){
		if(!geomMeta) { geomMeta=dynamic_cast<IGeomDispatcher*>(e.get()); if(geomMeta) continue; }
		if(!physMeta) { physMeta=dynamic_cast<IPhysDispatcher*>(e.get()); if(physMeta) continue; }
		InteractionLoop* id(dynamic_cast<InteractionLoop*>(e.get()));
		if(id){ geomMeta=id->geomDispatcher.get(); physMeta=id->physDispatcher.get(); }
		if(geomMeta&&physMeta){break;}
	}
	if(!geomMeta) throw runtime_error("No IGeomDispatcher in engines or inside InteractionLoop.");
	if(!physMeta) throw runtime_error("No IPhysDispatcher in engines or inside InteractionLoop.");
	shared_ptr<Body> b1=Body::byId(id1,rb), b2=Body::byId(id2,rb);
	if(!b1) throw runtime_error(("No body #"+lexical_cast<string>(id1)).c_str());
	if(!b2) throw runtime_error(("No body #"+lexical_cast<string>(id2)).c_str());
	shared_ptr<Interaction> i=geomMeta->explicitAction(b1,b2,/*force*/force);
	assert(force && i);
	if(!i) return i;
	physMeta->explicitAction(b1->material,b2->material,i);
	i->iterMadeReal=rb->iter;
	rb->interactions->insert(i);
	return i;
}

Vector3r Shop::inscribedCircleCenter(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2)
{
	return v0+((v2-v0)*(v1-v0).norm()+(v1-v0)*(v2-v0).norm())/((v1-v0).norm()+(v2-v1).norm()+(v0-v2).norm());
}

void Shop::getViscoelasticFromSpheresInteraction( Real tc, Real en, Real es, shared_ptr<ViscElMat> b)
{
    b->kn = 1/tc/tc * ( Mathr::PI*Mathr::PI + pow(log(en),2) );
    b->cn = -2.0 /tc * log(en);
    b->ks = 2.0/7.0 /tc/tc * ( Mathr::PI*Mathr::PI + pow(log(es),2) );
    b->cs = -2.0/7.0 /tc * log(es);

    if (abs(b->cn) <= Mathr::ZERO_TOLERANCE ) b->cn=0;
    if (abs(b->cs) <= Mathr::ZERO_TOLERANCE ) b->cs=0;
}

/* This function is copied almost verbatim from scientific python, module Visualization, class ColorScale
 *
 */
Vector3r Shop::scalarOnColorScale(Real x, Real xmin, Real xmax){
	Real xnorm=min((Real)1.,max((x-xmin)/(xmax-xmin),(Real)0.));
	if(xnorm<.25) return Vector3r(0,4.*xnorm,1);
	if(xnorm<.5)  return Vector3r(0,1,1.-4.*(xnorm-.25));
	if(xnorm<.75) return Vector3r(4.*(xnorm-.5),1.,0);
	return Vector3r(1,1-4*(xnorm-.75),0);
}

/* Wrap floating point number into interval (x0,x1〉such that it is shifted
 * by integral number of the interval range. If given, *period will hold
 * this number. The wrapped value is returned.
 */
Real Shop::periodicWrap(Real x, Real x0, Real x1, long* period){
	Real xNorm=(x-x0)/(x1-x0);
	Real xxNorm=xNorm-floor(xNorm);
	if(period) *period=(long)floor(xNorm);
	return x0+xxNorm*(x1-x0);
}

void Shop::getStressForEachBody(vector<Shop::bodyState>& bodyStates){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	bodyStates.resize(scene->bodies->size());
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		Vector3r normalStress,shearStress;
		if(!I->isReal()) continue;
		
		//NormShearPhys + Dem3DofGeom
		const NormShearPhys* physNSP = YADE_CAST<NormShearPhys*>(I->phys.get());
		//Dem3DofGeom* geom=YADE_CAST<Dem3DofGeom*>(I->geom.get());	//For the moment only for Dem3DofGeom!!!
		// FIXME: slower, but does not crash
		Dem3DofGeom* geomDDG=dynamic_cast<Dem3DofGeom*>(I->geom.get());	//For the moment only for Dem3DofGeom!!!
		
		//FrictPhys + ScGeom
		const FrictPhys* physFP = YADE_CAST<FrictPhys*>(I->phys.get());
		ScGeom* geomScG=dynamic_cast<ScGeom*>(I->geom.get());
		
		const Body::id_t id1=I->getId1(), id2=I->getId2();
		
		if(((physNSP) and (geomDDG)) or ((physFP) and (geomScG))){
			if ((physNSP) and (geomDDG)) {
				Real minRad=(geomDDG->refR1<=0?geomDDG->refR2:(geomDDG->refR2<=0?geomDDG->refR1:min(geomDDG->refR1,geomDDG->refR2)));
				Real crossSection=Mathr::PI*pow(minRad,2);
		
				normalStress=((1./crossSection)*geomDDG->normal.dot(physNSP->normalForce))*geomDDG->normal;
				for(int i=0; i<3; i++){
					int ix1=(i+1)%3,ix2=(i+2)%3;
					shearStress[i]=geomDDG->normal[ix1]*physNSP->shearForce[ix1]+geomDDG->normal[ix2]*physNSP->shearForce[ix2];
					shearStress[i]/=crossSection;
				}
			}	else if ((physFP) and (geomScG)) {
				Real minRad=(geomScG->radius1<=0?geomScG->radius2:(geomScG->radius2<=0?geomScG->radius1:min(geomScG->radius1,geomScG->radius2)));
				Real crossSection=Mathr::PI*pow(minRad,2);
				
				normalStress=((1./crossSection)*geomScG->normal.dot(physFP->normalForce))*geomScG->normal;
				for(int i=0; i<3; i++){
					int ix1=(i+1)%3,ix2=(i+2)%3;
					shearStress[i]=geomScG->normal[ix1]*physFP->shearForce[ix1]+geomScG->normal[ix2]*physFP->shearForce[ix2];
					shearStress[i]/=crossSection;
				}
			}
			
			bodyStates[id1].normStress+=normalStress;
			bodyStates[id2].normStress+=normalStress;

			bodyStates[id1].shearStress+=shearStress;
			bodyStates[id2].shearStress+=shearStress;
		}
	}
}

/* Return the stress tensor decomposed in 2 contributions, from normal and shear forces.
The formulation follows the [Thornton2000]_ article
"Numerical simulations of deviatoric shear deformation of granular media", eq (3) and (4)
 */
py::tuple Shop::normalShearStressTensors(bool compressionPositive, bool splitNormalTensor, Real thresholdForce){
  
	//*** Stress tensor split into shear and normal contribution ***/
	Scene* scene=Omega::instance().getScene().get();
	if (!scene->isPeriodic){ throw runtime_error("Can't compute stress of periodic cell in aperiodic simulation."); }
	Matrix3r sigN(Matrix3r::Zero()),sigT(Matrix3r::Zero());
	//const Matrix3r& cellHsize(scene->cell->Hsize);   //Disabled because of warning.
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		// if compression has positive sign, we need to change sign for both normal and shear force
		// make copy to Fs since shearForce is used at multiple places (less efficient, but avoids confusion)
		Vector3r Fs=(compressionPositive?-1:1)*phys->shearForce;
		Real N=(compressionPositive?-1:1)*phys->normalForce.dot(n), T=Fs.norm();
		bool hasShear=(T>0);
		Vector3r t; if(hasShear) t=Fs/T;
		// Real R=(Body::byId(I->getId2(),scene)->state->pos+cellHsize*I->cellDist.cast<Real>()-Body::byId(I->getId1(),scene)->state->pos).norm();
		Real R=.5*(geom->refR1+geom->refR2);
		for(int i=0; i<3; i++) for(int j=i; j<3; j++){
			sigN(i,j)+=R*N*n[i]*n[j];
			if(hasShear) sigT(i,j)+=R*T*n[i]*t[j];
		}
	}
	Real vol=scene->cell->getVolume();
	sigN*=2/vol; sigT*=2/vol;
	// fill terms under the diagonal
	sigN(1,0)=sigN(0,1); sigN(2,0)=sigN(0,2); sigN(2,1)=sigN(1,2);
	sigT(1,0)=sigT(0,1); sigT(2,0)=sigT(0,2); sigT(2,1)=sigT(1,2);
	
	// *** Normal stress tensor split into two parts according to subnetworks of strong and weak forces (or other distinction if a threshold value for the force is assigned) ***/
	Real Fmean(0); Matrix3r f, fs, fw;
	/// FIXME: this does not compute Fmean as it should?
	fabricTensor(Fmean,f,fs,fw,false,compressionPositive,NaN);
	Matrix3r sigNStrong(Matrix3r::Zero()), sigNWeak(Matrix3r::Zero());
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		Real N=(compressionPositive?-1:1)*phys->normalForce.dot(n);
		// Real R=(Body::byId(I->getId2(),scene)->state->pos+cellHsize*I->cellDist.cast<Real>()-Body::byId(I->getId1(),scene)->state->pos).norm();
		Real R=.5*(geom->refR1+geom->refR2);
		Real Fsplit=(!isnan(thresholdForce))?thresholdForce:Fmean;
		if (compressionPositive?(N<Fsplit):(N>Fsplit)){
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				sigNStrong(i,j)+=R*N*n[i]*n[j];}
		}
		else{
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				sigNWeak(i,j)+=R*N*n[i]*n[j];}
		}
	}
	sigNStrong*=2/vol; sigNWeak*=2/vol;
	// fill terms under the diagonal
	sigNStrong(1,0)=sigNStrong(0,1); sigNStrong(2,0)=sigNStrong(0,2); sigNStrong(2,1)=sigNStrong(1,2);
	sigNWeak(1,0)=sigNWeak(0,1); sigNWeak(2,0)=sigNWeak(0,2); sigNWeak(2,1)=sigNWeak(1,2);
	
	/// tensile forces are taken as positive!
	if (splitNormalTensor){return py::make_tuple(sigNStrong,sigNWeak);} // return strong-weak or tensile-compressive parts of the stress tensor (only normal part)
	return py::make_tuple(sigN,sigT); // return normal and shear components
}

/* Return the fabric tensor as according to [Satake1982]. */
/* as side-effect, set Gl1_NormShear::strongWeakThresholdForce */
void Shop::fabricTensor(Real& Fmean, Matrix3r& fabric, Matrix3r& fabricStrong, Matrix3r& fabricWeak, bool splitTensor, bool revertSign, Real thresholdForce){
	Scene* scene=Omega::instance().getScene().get();
	if (!scene->isPeriodic){ throw runtime_error("Can't compute fabric tensor of periodic cell in aperiodic simulation."); }
	
	// *** Fabric tensor ***/
	fabric=Matrix3r::Zero(); 
	int count=0; // number of interactions
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		const Vector3r& n=geom->normal;
		for(int i=0; i<3; i++) for(int j=i; j<3; j++){
			fabric(i,j)+=n[i]*n[j];
		}
		count++;
	}
	// fill terms under the diagonal
	fabric(1,0)=fabric(0,1); fabric(2,0)=fabric(0,2); fabric(2,1)=fabric(1,2);
	fabric/=count;
	
	// *** Average contact force ***/
	// calculate average contact force
	Fmean=0; // initialize
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		Real f=(revertSign?-1:1)*phys->normalForce.dot(n);  
		//Real f=phys->normalForce.norm();
		Fmean+=f;
	}
	Fmean/=count; 

	#ifdef YADE_OPENGL
		Gl1_NormPhys::maxWeakFn=Fmean;
	#endif
	
	// *** Weak and strong fabric tensors ***/
	// evaluate two different parts of the fabric tensor 
	// make distinction between strong and weak network of contact forces
	fabricStrong=Matrix3r::Zero(); 
	fabricWeak=Matrix3r::Zero(); 
	int nStrong(0), nWeak(0); // number of strong and weak contacts respectively
	if (!splitTensor & !isnan(thresholdForce)) {LOG_WARN("The bool splitTensor should be set to True if you specified a threshold value for the contact force, otherwise the function will return only the fabric tensor and not the two separate contributions.");}
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		const Vector3r& n=geom->normal;
		Real  f=(revertSign?-1:1)*phys->normalForce.dot(n); 
		// slipt the tensor according to the mean contact force or a threshold value if this is given
		Real Fsplit=(!isnan(thresholdForce))?thresholdForce:Fmean;
		if (revertSign?(f<Fsplit):(f>Fsplit)){ // reminder: forces are compared with their sign
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				fabricStrong(i,j)+=n[i]*n[j];
			}
			nStrong++;
		}
		else{
			for(int i=0; i<3; i++) for(int j=i; j<3; j++){
				fabricWeak(i,j)+=n[i]*n[j];
			}
			nWeak++;
		}
	}
	// fill terms under the diagonal
	fabricStrong(1,0)=fabricStrong(0,1); fabricStrong(2,0)=fabricStrong(0,2); fabricStrong(2,1)=fabricStrong(1,2);
	fabricWeak(1,0)=fabricWeak(0,1); fabricWeak(2,0)=fabricWeak(0,2); fabricWeak(2,1)=fabricWeak(1,2);
	fabricStrong/=nStrong;
	fabricWeak/=nWeak;
	
	// *** Compute total fabric tensor from the two tensors above ***/
	Matrix3r fabricTot(Matrix3r::Zero()); 
	int q(0);
	if(!count==0){ // compute only if there are some interactions
		q=nStrong*1./count; 
		fabricTot=(1-q)*fabricWeak+q*fabricStrong;
	}
}

py::tuple Shop::fabricTensor(bool splitTensor, bool revertSign, Real thresholdForce){
	Real Fmean; Matrix3r fabric, fabricStrong, fabricWeak;
	fabricTensor(Fmean,fabric,fabricStrong,fabricWeak,splitTensor,revertSign,thresholdForce);
	// returns fabric tensor or alternatively the two distinct contributions according to strong and weak subnetworks (or, if thresholdForce is specified, the distinction is made according to that value and not the mean one)
	if (!splitTensor){return py::make_tuple(fabric);} 
	else{return py::make_tuple(fabricStrong,fabricWeak);}
}

Matrix3r Shop::stressTensorOfPeriodicCell(bool smallStrains){
	Scene* scene=Omega::instance().getScene().get();
	if (!scene->isPeriodic){ throw runtime_error("Can't compute stress of periodic cell in aperiodic simulation."); }
// 	if (smallStrains){volume = scene->getVolume()cell->refSize[0]*scene->cell->refSize[1]*scene->cell->refSize[2];}
// 	else volume = scene->cell->trsf.determinant()*scene->cell->refSize[0]*scene->cell->refSize[1]*scene->cell->refSize[2];
	// Using the function provided by Cell (BC)
	Real volume = scene->cell->getVolume();
	Matrix3r stress = Matrix3r::Zero();
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		Dem3DofGeom* geom=YADE_CAST<Dem3DofGeom*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		Real l;
		if (smallStrains){l = geom->refLength;}
		else l=(geom->se31.position-geom->se32.position).norm();
		Vector3r& n=geom->normal;
		Vector3r& fT=phys->shearForce;
		Real fN=phys->normalForce.dot(n);

		stress += l*(fN*n*n.transpose() + .5*(fT*n.transpose() + n*fT.transpose()));

		/*Real fT0=fT[0]; Real fT1=fT[1]; Real fT2=fT[2];
		Real n0=n[0]; Real n1=n[1]; Real n2=n[2];

		Real s00 = l*(n0*n0*fN + fT0*n0);
		Real s01 = l*(n0*n1*fN + .5*(fT0*n1 + fT1*n0));
		Real s02 = l*(n0*n2*fN + .5*(fT0*n2 + fT2*n0));
		Real s11 = l*(n1*n1*fN + fT1*n1);
		Real s12 = l*(n1*n2*fN + .5*(fT1*n2 + fT2*n1));
		Real s22 = l*(n2*n2*fN + fT2*n2);

		stress(0,0) += s00;
		stress(0,1) += s01;
		stress(1,0) += s01;
		stress(0,2) += s02;
		stress(2,0) += s02;
		stress(1,1) += s11;
		stress(1,2) += s12;
		stress(2,1) += s12;
		stress(2,2) += s22;
		*/
	}
	stress/=volume;
	return stress;
}

void Shop::getStressLWForEachBody(vector<Matrix3r>& bStresses, bool revertSign){
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	bStresses.resize(scene->bodies->size());
	for (size_t k=0;k<scene->bodies->size();k++) bStresses[k]=Matrix3r::Zero();
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		GenericSpheresContact* geom=YADE_CAST<GenericSpheresContact*>(I->geom.get());
		NormShearPhys* phys=YADE_CAST<NormShearPhys*>(I->phys.get());
		Vector3r f=phys->normalForce+phys->shearForce;
		if (revertSign) f=-f;//revert sign for some laws, so that every interaction type can give the same result
		//Sum f_i*l_j/V for each contact of each particle
		bStresses[I->getId1()]-=(3.0/(4.0*Mathr::PI*pow(geom->refR1,3)))*f*((geom->contactPoint-Body::byId(I->getId1(),scene)->state->pos).transpose());
		if (!scene->isPeriodic)
			bStresses[I->getId2()]+=(3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*f*((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos)).transpose());
		else
			bStresses[I->getId2()]+=(3.0/(4.0*Mathr::PI*pow(geom->refR2,3)))*f* ((geom->contactPoint- (Body::byId(I->getId2(),scene)->state->pos + (scene->cell->hSize*I->cellDist.cast<Real>()))).transpose());
	}
}

py::tuple Shop::getStressLWForEachBody(bool revertSign){
	vector<Matrix3r> bStresses;
	getStressLWForEachBody(bStresses,revertSign);
	return py::make_tuple(bStresses);
}
