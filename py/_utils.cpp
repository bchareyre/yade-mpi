#include<yade/pkg-dem/Shop.hpp>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/lib-computational-geometry/Hull2d.hpp>
#include<cmath>

#include<numpy/ndarrayobject.h>

// #include"_utils.hpp"



using namespace boost::python;

#ifdef LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.utils");
#endif

python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);}
Vector3r tuple2vec(const python::tuple& t){return Vector3r(extract<double>(t[0])(),extract<double>(t[1])(),extract<double>(t[2])());}
bool isInBB(Vector3r p, Vector3r bbMin, Vector3r bbMax){return p[0]>bbMin[0] && p[0]<bbMax[0] && p[1]>bbMin[1] && p[1]<bbMax[1] && p[2]>bbMin[2] && p[2]<bbMax[2];}

bool ptInAABB(python::tuple p, python::tuple bbMin, python::tuple bbMax){return isInBB(tuple2vec(p),tuple2vec(bbMin),tuple2vec(bbMax));}

/* \todo implement groupMask */
python::tuple aabbExtrema(Real cutoff=0.0, bool centers=false){
	if(cutoff<0. || cutoff>1.) throw invalid_argument("Cutoff must be >=0 and <=1.");
	Real inf=std::numeric_limits<Real>::infinity();
	Vector3r minimum(inf,inf,inf),maximum(-inf,-inf,-inf);
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		shared_ptr<Sphere> s=dynamic_pointer_cast<Sphere>(b->geometricalModel); if(!s) continue;
		Vector3r rrr(s->radius,s->radius,s->radius);
		minimum=componentMinVector(minimum,b->physicalParameters->se3.position-(centers?Vector3r::ZERO:rrr));
		maximum=componentMaxVector(maximum,b->physicalParameters->se3.position+(centers?Vector3r::ZERO:rrr));
	}
	Vector3r dim=maximum-minimum;
	return python::make_tuple(minimum+.5*cutoff*dim,maximum-.5*cutoff*dim);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(aabbExtrema_overloads,aabbExtrema,0,2);

python::tuple negPosExtremeIds(int axis, Real distFactor=1.1){
	python::tuple extrema=aabbExtrema();
	Real minCoord=extract<double>(extrema[0][axis])(),maxCoord=extract<double>(extrema[1][axis])();
	python::list minIds,maxIds;
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		shared_ptr<Sphere> s=dynamic_pointer_cast<Sphere>(b->geometricalModel); if(!s) continue;
		if(b->physicalParameters->se3.position[axis]-s->radius*distFactor<=minCoord) minIds.append(b->getId());
		if(b->physicalParameters->se3.position[axis]+s->radius*distFactor>=maxCoord) maxIds.append(b->getId());
	}
	return python::make_tuple(minIds,maxIds);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(negPosExtremeIds_overloads,negPosExtremeIds,1,2);

python::tuple coordsAndDisplacements(int axis,python::tuple AABB=python::tuple()){
	Vector3r bbMin(Vector3r::ZERO), bbMax(Vector3r::ZERO); bool useBB=python::len(AABB)>0;
	if(useBB){bbMin=extract<Vector3r>(AABB[0])();bbMax=extract<Vector3r>(AABB[1])();}
	python::list retCoord,retDispl;
	FOREACH(const shared_ptr<Body>&b, *Omega::instance().getRootBody()->bodies){
		if(useBB && !isInBB(b->physicalParameters->se3.position,bbMin,bbMax)) continue;
		retCoord.append(b->physicalParameters->se3.position[axis]);
		retDispl.append(b->physicalParameters->se3.position[axis]-b->physicalParameters->refSe3.position[axis]);
	}
	return python::make_tuple(retCoord,retDispl);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(coordsAndDisplacements_overloads,coordsAndDisplacements,1,2);

void setRefSe3(){
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		b->physicalParameters->refSe3.position=b->physicalParameters->se3.position;
		b->physicalParameters->refSe3.orientation=b->physicalParameters->se3.orientation;
	}
}

Real PWaveTimeStep(){return Shop::PWaveTimeStep();};

Real elasticEnergyInAABB(python::tuple AABB){
	Vector3r bbMin=extract<Vector3r>(AABB[0])(), bbMax=extract<Vector3r>(AABB[1])();
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
	Real E=0;
	FOREACH(const shared_ptr<Interaction>&i, *rb->transientInteractions){
		if(!i->interactionPhysics) continue;
		shared_ptr<NormalShearInteraction> bc=dynamic_pointer_cast<NormalShearInteraction>(i->interactionPhysics); if(!bc) continue;
		shared_ptr<Dem3DofGeom> geom=dynamic_pointer_cast<Dem3DofGeom>(i->interactionGeometry); if(!bc){LOG_ERROR("NormalShearInteraction contact doesn't have SpheresContactGeomety associated?!"); continue;}
		const shared_ptr<Body>& b1=Body::byId(i->getId1(),rb), b2=Body::byId(i->getId2(),rb);
		bool isIn1=isInBB(b1->physicalParameters->se3.position,bbMin,bbMax), isIn2=isInBB(b2->physicalParameters->se3.position,bbMin,bbMax);
		if(!isIn1 && !isIn2) continue;
		LOG_DEBUG("Interaction #"<<i->getId1()<<"--#"<<i->getId2());
		Real weight=1.;
		if((!isIn1 && isIn2) || (isIn1 && !isIn2)){
			//shared_ptr<Body> bIn=isIn1?b1:b2, bOut=isIn2?b2:b1;
			Vector3r vIn=(isIn1?b1:b2)->physicalParameters->se3.position, vOut=(isIn2?b1:b2)->physicalParameters->se3.position;
			#define _WEIGHT_COMPONENT(axis) if(vOut[axis]<bbMin[axis]) weight=min(weight,abs((vOut[axis]-bbMin[axis])/(vOut[axis]-vIn[axis]))); else if(vOut[axis]>bbMax[axis]) weight=min(weight,abs((vOut[axis]-bbMax[axis])/(vOut[axis]-vIn[axis])));
			_WEIGHT_COMPONENT(0); _WEIGHT_COMPONENT(1); _WEIGHT_COMPONENT(2);
			assert(weight>=0 && weight<=1);
			//cerr<<"Interaction crosses AABB boundary, weight is "<<weight<<endl;
			//LOG_DEBUG("Interaction crosses AABB boundary, weight is "<<weight);
		} else {assert(isIn1 && isIn2); /* cerr<<"Interaction inside, weight is "<<weight<<endl;*/ /*LOG_DEBUG("Interaction inside, weight is "<<weight);*/}
		E+=geom->refLength*weight*(.5*bc->kn*pow(geom->strainN(),2)+.5*bc->ks*pow(geom->strainT().Length(),2));
	}
	return E;
}

/* return histogram ([bin1Min,bin2Min,…],[value1,value2,…]) from projections of interactions
 * to the plane perpendicular to axis∈[0,1,2]; the number of bins can be specified and they cover
 * the range (0,π), since interactions are bidirecional, hence periodically the same on (π,2π).
 *
 * Only contacts using SpheresContactGeometry are considered.
 * Both bodies must be in the mask (except if mask is 0, when all bodies are considered)
 * If the projection is shorter than minProjLen, it is skipped.
 *
 * If both bodies are _outside_ the aabb (if specified), the interaction is skipped.
 *
 */
python::tuple interactionAnglesHistogram(int axis, int mask=0, size_t bins=20, python::tuple aabb=python::tuple(), Real minProjLen=1e-6){
	if(axis<0||axis>2) throw invalid_argument("Axis must be from {0,1,2}=x,y,z.");
	Vector3r bbMin(Vector3r::ZERO), bbMax(Vector3r::ZERO); bool useBB=python::len(aabb)>0; if(useBB){bbMin=extract<Vector3r>(aabb[0])();bbMax=extract<Vector3r>(aabb[1])();}
	Real binStep=Mathr::PI/bins; int axis2=(axis+1)%3, axis3=(axis+2)%3;
	vector<Real> cummProj(bins,0.);
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
	FOREACH(const shared_ptr<Interaction>& i, *rb->transientInteractions){
		if(!i->isReal()) continue;
		const shared_ptr<Body>& b1=Body::byId(i->getId1(),rb), b2=Body::byId(i->getId2(),rb);
		if(!b1->maskOk(mask) || !b2->maskOk(mask)) continue;
		if(useBB && !isInBB(b1->physicalParameters->se3.position,bbMin,bbMax) && !isInBB(b2->physicalParameters->se3.position,bbMin,bbMax)) continue;
		shared_ptr<SpheresContactGeometry> scg=dynamic_pointer_cast<SpheresContactGeometry>(i->interactionGeometry); if(!scg) continue;
		Vector3r n(scg->normal); n[axis]=0.; Real nLen=n.Length();
		if(nLen<minProjLen) continue; // this interaction is (almost) exactly parallel to our axis; skip that one
		Real theta=acos(n[axis2]/nLen)*(n[axis3]>0?1:-1); if(theta<0) theta+=Mathr::PI;
		int binNo=theta/binStep;
		cummProj[binNo]+=nLen;
	}
	python::list val,binMid;
	for(size_t i=0; i<(size_t)bins; i++){ val.append(cummProj[i]); binMid.append(i*binStep);}
	return python::make_tuple(binMid,val);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(interactionAnglesHistogram_overloads,interactionAnglesHistogram,1,4);

python::tuple bodyNumInteractionsHistogram(python::tuple aabb=python::tuple()){
	Vector3r bbMin(Vector3r::ZERO), bbMax(Vector3r::ZERO); bool useBB=python::len(aabb)>0; if(useBB){bbMin=extract<Vector3r>(aabb[0])();bbMax=extract<Vector3r>(aabb[1])();}
	const shared_ptr<MetaBody>& rb=Omega::instance().getRootBody();
	vector<int> bodyNumInta; bodyNumInta.resize(rb->bodies->size(),-1 /* uninitialized */);
	int maxInta=0;
	FOREACH(const shared_ptr<Interaction>& i, *rb->transientInteractions){
		if(!i->isReal()) continue;
		const body_id_t id1=i->getId1(), id2=i->getId2(); const shared_ptr<Body>& b1=Body::byId(id1,rb), b2=Body::byId(id2,rb);
		if(useBB && isInBB(b1->physicalParameters->se3.position,bbMin,bbMax)) bodyNumInta[id1]=bodyNumInta[id1]>0?bodyNumInta[id1]+1:1;
		if(useBB && isInBB(b2->physicalParameters->se3.position,bbMin,bbMax)) bodyNumInta[id2]=bodyNumInta[id2]>0?bodyNumInta[id2]+1:1;
		maxInta=max(max(maxInta,bodyNumInta[b1->getId()]),bodyNumInta[b2->getId()]);
	}
	vector<int> bins; bins.resize(maxInta+1);
	for(size_t id=0; id<bodyNumInta.size(); id++){ if(bodyNumInta[id]>=0) bins[bodyNumInta[id]]+=1; }
	python::list count,num;
	for(size_t n=1; n<bins.size(); n++){
		if(bins[n]==0) continue;
		num.append(n); count.append(bins[n]);
	}
	return python::make_tuple(num,count);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(bodyNumInteractionsHistogram_overloads,bodyNumInteractionsHistogram,0,1);

Vector3r inscribedCircleCenter(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2)
{
	return Shop::inscribedCircleCenter(v0,v1,v2);
}

python::dict getViscoelasticFromSpheresInteraction(Real m, Real tc, Real en, Real es)
{
    shared_ptr<SimpleViscoelasticBodyParameters> b = shared_ptr<SimpleViscoelasticBodyParameters>(new SimpleViscoelasticBodyParameters());
    Shop::getViscoelasticFromSpheresInteraction(m,tc,en,es,b);
	python::dict d;
	d["kn"]=b->kn;
	d["cn"]=b->cn;
	d["ks"]=b->ks;
	d["cs"]=b->cs;
    return d;
}
/* reset highlight of all bodies */
void highlightNone(){
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		if(!b->geometricalModel) continue;
		b->geometricalModel->highlight=false;
	}
}

/*!Sum moments acting on given bodies
 *
 * @param mask is Body::groupMask that must match for a body to be taken in account.
 * @param axis is the direction of axis with respect to which the moment is calculated.
 * @param axisPt is a point on the axis.
 *
 * The computation is trivial: moment from force is is by definition r×F, where r
 * is position relative to axisPt; moment from moment is m; such moment per body is
 * projected onto axis.
 */
Real sumBexTorques(python::tuple ids, const Vector3r& axis, const Vector3r& axisPt){
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
	rb->bex.sync();
	Real ret=0;
	size_t len=python::len(ids);
	for(size_t i=0; i<len; i++){
		const Body* b=(*rb->bodies)[python::extract<int>(ids[i])].get();
		const Vector3r& m=rb->bex.getTorque(b->getId());
		const Vector3r& f=rb->bex.getForce(b->getId());
		Vector3r r=b->physicalParameters->se3.position-axisPt;
		ret+=axis.Dot(m+r.Cross(f));
	}
	return ret;
}
/* Sum forces actiong on bodies within mask.
 *
 * @param mask groupMask of matching bodies
 * @param direction direction in which forces are summed
 *
 */
Real sumBexForces(python::tuple ids, const Vector3r& direction){
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
	rb->bex.sync();
	Real ret=0;
	size_t len=python::len(ids);
	for(size_t i=0; i<len; i++){
		body_id_t id=python::extract<int>(ids[i]);
		const Vector3r& f=rb->bex.getForce(id);
		ret+=direction.Dot(f);
	}
	return ret;
}

/* Set wire display of all/some/none bodies depending on the filter. */
void wireSome(string filter){
	enum{none,all,noSpheres,unknown};
	int mode=(filter=="none"?none:(filter=="all"?all:(filter=="noSpheres"?noSpheres:unknown)));
	if(mode==unknown) { LOG_WARN("Unknown wire filter `"<<filter<<"', using noSpheres instead."); mode=noSpheres; }
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		if(!b->geometricalModel) return;
		bool wire;
		switch(mode){
			case none: wire=false; break;
			case all: wire=true; break;
			case noSpheres: wire=!(bool)(dynamic_pointer_cast<Sphere>(b->geometricalModel)); break;
			default: throw logic_error("No such case possible");
		}
		b->geometricalModel->wire=wire;
	}
}
void wireAll(){wireSome("all");}
void wireNone(){wireSome("none");}
void wireNoSpheres(){wireSome("noSpheres");}


/* Tell us whether a point lies in polygon given by array of points.
 *  @param xy is the point that is being tested
 *  @param vertices is Numeric.array (or list or tuple) of vertices of the polygon.
 *         Every row of the array is x and y coordinate, numer of rows is >= 3 (triangle).
 *
 * Copying the algorithm from http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 * is gratefully acknowledged:
 *
 * License to Use:
 * Copyright (c) 1970-2003, Wm. Randolph Franklin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
 *   2. Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
 *   3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission. 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://numpy.scipy.org/numpydoc/numpy-13.html told me how to use Numeric.array from c
 */
bool pointInsidePolygon(python::tuple xy, python::object vertices){
	Real testx=python::extract<double>(xy[0])(),testy=python::extract<double>(xy[1])();
	char** vertData; int rows, cols; PyArrayObject* vert=(PyArrayObject*)vertices.ptr();
	int result=PyArray_As2D((PyObject**)&vert /* is replaced */ ,&vertData,&rows,&cols,PyArray_DOUBLE);
	if(result!=0) throw invalid_argument("Unable to cast vertices to 2d array");
	if(cols!=2 || rows<3) throw invalid_argument("Vertices must have 2 columns (x and y) and at least 3 rows.");
	int i /*current node*/, j/*previous node*/; bool inside=false;
	for(i=0,j=rows-1; i<rows; j=i++){
		double vx_i=*(double*)(vert->data+i*vert->strides[0]), vy_i=*(double*)(vert->data+i*vert->strides[0]+vert->strides[1]), vx_j=*(double*)(vert->data+j*vert->strides[0]), vy_j=*(double*)(vert->data+j*vert->strides[0]+vert->strides[1]);
		if (((vy_i>testy)!=(vy_j>testy)) && (testx < (vx_j-vx_i) * (testy-vy_i) / (vy_j-vy_i) + vx_i) ) inside=!inside;
	}
	Py_DECREF(vert);
	return inside;
}

/* Compute area of convex hull when when taking (swept) spheres crossing the plane at coord, perpendicular to axis.

	All spheres that touch the plane are projected as hexagons on their circumference to the plane.
	Convex hull from this cloud is computed.
	The area of the hull is returned.

*/
Real approxSectionArea(Real coord, int axis){
	std::list<Vector2r> cloud;
	if(axis<0 || axis>2) throw invalid_argument("Axis must be ∈ {0,1,2}");
	const int ax1=(axis+1)%3, ax2=(axis+2)%3;
	const Real sqrt3=sqrt(3);
	Vector2r mm(-10.,0),mx(0,0);
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		Sphere* s=dynamic_cast<Sphere*>(b->geometricalModel.get());
		if(!s) continue;
		const Vector3r& pos(b->physicalParameters->se3.position); const Real r(s->radius);
		if((pos[axis]>coord && (pos[axis]-r)>coord) || (pos[axis]<coord && (pos[axis]+r)<coord)) continue;
		Vector2r c(pos[ax1],pos[ax2]);
		cloud.push_back(c+Vector2r(r,0)); cloud.push_back(c+Vector2r(-r,0));
		cloud.push_back(c+Vector2r( r/2., sqrt3*r)); cloud.push_back(c+Vector2r( r/2.,-sqrt3*r));
		cloud.push_back(c+Vector2r(-r/2., sqrt3*r)); cloud.push_back(c+Vector2r(-r/2.,-sqrt3*r));
		if(mm[0]==-10.){ mm=c, mx=c;}
		mm=Vector2r(min(c[0]-r,mm[0]),min(c[1]-r,mm[1]));
		mx=Vector2r(max(c[0]+r,mx[0]),max(c[1]+r,mx[1]));
	}
	if(cloud.size()==0) return 0;
	ConvexHull2d ch2d(cloud);
	vector<Vector2r> hull=ch2d();
	return simplePolygonArea2d(hull);
}
/* Find all interactions deriving from NormalShearInteraction that cross plane given by a point and normal
	(the normal may not be normalized in this case, though) and sum forces (both normal and shear) on them.
	
	Return a 3-tuple with the components along global x,y,z axes.

	(This could be easily extended to return sum of only normal forces or only of shear forces.)
*/
Vector3r forcesOnPlane(const Vector3r& planePt, const Vector3r&  normal){
	Vector3r ret(Vector3r::ZERO);
	MetaBody* rootBody=Omega::instance().getRootBody().get();
	FOREACH(const shared_ptr<Interaction>&I, *rootBody->interactions){
		if(!I->isReal()) continue;
		NormalShearInteraction* nsi=dynamic_cast<NormalShearInteraction*>(I->interactionPhysics.get());
		if(!nsi) continue;
		Vector3r pos1,pos2;
		Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(I->interactionGeometry.get()); // Dem3DofGeom has copy of se3 in itself, otherwise we have to look up the bodies
		if(d3dg){ pos1=d3dg->se31.position; pos2=d3dg->se32.position; }
		else{ pos1=Body::byId(I->getId1(),rootBody)->physicalParameters->se3.position; pos2=Body::byId(I->getId2(),rootBody)->physicalParameters->se3.position; }
		Real dot1=(pos1-planePt).Dot(normal), dot2=(pos2-planePt).Dot(normal);
		if(dot1*dot2>0) continue; // both interaction points on the same side of the plane
		// if pt1 is on the negative plane side, d3dg->normal.Dot(normal)>0, the force is well oriented;
		// otherwise, reverse its contribution
		ret+=(dot1<0.?1.:-1.)*(nsi->normalForce+nsi->shearForce);
	}
	return ret;
}

/* Less general than forcesOnPlane, computes force on plane perpendicular to axis, passing through coordinate coord. */
Vector3r forcesOnCoordPlane(Real coord, int axis){
	Vector3r planePt(Vector3r::ZERO); planePt[axis]=coord;
	Vector3r normal(Vector3r::ZERO); normal[axis]=1;
	return forcesOnPlane(planePt,normal);
}


/* Project 3d point into 2d using spiral projection along given axis;
 * the returned tuple is
 * 	
 *  ( (height relative to the spiral, distance from axis), theta )
 *
 * dH_dTheta is the inclination of the spiral (height increase per radian),
 * theta0 is the angle for zero height (by given axis).
 */
python::tuple spiralProject(const Vector3r& pt, Real dH_dTheta, int axis=2, Real periodStart=std::numeric_limits<Real>::quiet_NaN(), Real theta0=0){
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
		return python::make_tuple(python::make_tuple(r,h),theta);
	}
	else{
		// Real hPeriodStart=(periodStart-theta0)*dH_dTheta;
		//TRVAR4(hPeriodStart,periodStart,theta0,theta);
		//Real h=Shop::periodicWrap(pt[axis]-hRef,hPeriodStart,hPeriodStart+2*Mathr::PI*dH_dTheta,&period);
		theta=Shop::periodicWrap(theta,periodStart,periodStart+2*Mathr::PI,&period);
		Real h=pt[axis]-hRef+period*2*Mathr::PI*dH_dTheta;
		//TRVAR3(pt[axis],pt[axis]-hRef,period);
		//TRVAR2(h,theta);
		return python::make_tuple(python::make_tuple(r,h),theta);
	}
}
BOOST_PYTHON_FUNCTION_OVERLOADS(spiralProject_overloads,spiralProject,2,5);

// for now, don't return anything, since we would have to include the whole yadeControl.cpp because of pyInteraction
void Shop__createExplicitInteraction(body_id_t id1, body_id_t id2){ (void) Shop::createExplicitInteraction(id1,id2);}

//Vector3r Shop__scalarOnColorScale(Real scalar){ return Shop::scalarOnColorScale(scalar);}

BOOST_PYTHON_FUNCTION_OVERLOADS(unbalancedForce_overloads,Shop::unbalancedForce,0,1);
Real Shop__kineticEnergy(){return Shop::kineticEnergy();}
python::tuple Shop__totalForceInVolume(){Real stiff; Vector3r ret=Shop::totalForceInVolume(stiff); return python::make_tuple(ret,stiff); }

BOOST_PYTHON_MODULE(_utils){
	// http://numpy.scipy.org/numpydoc/numpy-13.html mentions this must be done in module init, otherwise we will crash
	import_array();

	def("PWaveTimeStep",PWaveTimeStep,"Get timestep accoring to the velocity of P-Wave propagation; computed from sphere radii, rigidities and masses.");
	def("aabbExtrema",aabbExtrema,aabbExtrema_overloads(args("cutoff","centers"),"Return coordinates of box enclosing all bodies\n centers: do not take sphere radii in account, only their centroids (default=False)\n cutoff: 0-1 number by which the box will be scaled around its center (default=0)"));
	def("ptInAABB",ptInAABB,"Return True/False whether the point (3-tuple) p is within box given by its min (3-tuple) and max (3-tuple) corners");
	def("negPosExtremeIds",negPosExtremeIds,negPosExtremeIds_overloads(args("axis","distFactor"),"Return list of ids for spheres (only) that are on extremal ends of the specimen along given axis; distFactor multiplies their radius so that sphere that do not touch the boundary coordinate can also be returned."));
	def("approxSectionArea",approxSectionArea,"Compute area of convex hull when when taking (swept) spheres crossing the plane at coord, perpendicular to axis.");
	def("coordsAndDisplacements",coordsAndDisplacements,coordsAndDisplacements_overloads(args("AABB"),"Return tuple of 2 same-length lists for coordinates and displacements (coordinate minus reference coordinate) along given axis (1st arg); if the AABB=((x_min,y_min,z_min),(x_max,y_max,z_max)) box is given, only bodies within this box will be considered."));
	def("setRefSe3",setRefSe3,"Set reference positions and orientation of all bodies equal to their current ones.");
	def("interactionAnglesHistogram",interactionAnglesHistogram,interactionAnglesHistogram_overloads(args("axis","mask","bins","aabb")));
	def("bodyNumInteractionsHistogram",bodyNumInteractionsHistogram,bodyNumInteractionsHistogram_overloads(args("aabb")));
	def("elasticEnergy",elasticEnergyInAABB);
	def("inscribedCircleCenter",inscribedCircleCenter);
	def("getViscoelasticFromSpheresInteraction",getViscoelasticFromSpheresInteraction);
	def("unbalancedForce",&Shop::unbalancedForce,unbalancedForce_overloads(args("useMaxForce")));
	def("kineticEnergy",Shop__kineticEnergy);
	def("sumBexForces",sumBexForces);
	def("sumBexTorques",sumBexTorques);
	def("forcesOnPlane",forcesOnPlane);
	def("forcesOnCoordPlane",forcesOnCoordPlane);
	def("totalForceInVolume",Shop__totalForceInVolume,"Return summed forces on all interactions and average isotropic stiffness, as tuple (Vector3,float)");
	def("createInteraction",Shop__createExplicitInteraction);
	def("spiralProject",spiralProject,spiralProject_overloads(args("axis","periodStart","theta0")));
	def("pointInsidePolygon",pointInsidePolygon);
	def("scalarOnColorScale",Shop::scalarOnColorScale);
	def("highlightNone",highlightNone);
	def("wireAll",wireAll);
	def("wireNone",wireNone);
	def("wireNoSpheres",wireNoSpheres);
}


