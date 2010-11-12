/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg/common/GravityEngines.hpp>
#include<yade/pkg/common/PeriodicEngines.hpp>
#include<yade/core/Scene.hpp>
#include<boost/regex.hpp>
//#include<stdio.h>

YADE_PLUGIN((GravityEngine)(CentralGravityEngine)(AxialGravityEngine)(HdapsGravityEngine));

void GravityEngine::action(){
	#ifdef YADE_OPENMP
		const BodyContainer& bodies=*(scene->bodies.get());
		const long size=(long)bodies.size();
		#pragma omp parallel for schedule(static)
		for(long i=0; i<size; i++){
			const shared_ptr<Body>& b(bodies[i]);
	#else
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
	#endif
		// skip clumps, only apply forces on their constituents
		if(!b || b->isClump()) continue;
		scene->forces.addForce(b->getId(),gravity*b->state->mass);
	}
}

void CentralGravityEngine::action(){
	const Vector3r& centralPos=Body::byId(centralBody)->state->pos;
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || b->isClump() || b->getId()==centralBody) continue; // skip clumps and central body
		Real F=accel*b->state->mass;
		Vector3r toCenter=centralPos-b->state->pos; toCenter.normalize();
		scene->forces.addForce(b->getId(),F*toCenter);
		if(reciprocal) scene->forces.addForce(centralBody,-F*toCenter);
	}
}

void AxialGravityEngine::action(){
	FOREACH(const shared_ptr<Body>&b, *scene->bodies){
		if(!b || b->isClump()) continue;
		/* http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html */
		const Vector3r& x0=b->state->pos;
		const Vector3r& x1=axisPoint;
		const Vector3r x2=axisPoint+axisDirection;
		Vector3r closestAxisPoint=(x2-x1) * /* t */ (-(x1-x0).dot(x2-x1))/((x2-x1).squaredNorm());
		Vector3r toAxis=closestAxisPoint-x0; toAxis.normalize();
		scene->forces.addForce(b->getId(),acceleration*b->state->mass*toAxis);
	}
}


Vector2i HdapsGravityEngine::readSysfsFile(const string& name){
	char buf[256];
	ifstream f(name.c_str());
	if(!f.is_open()) throw std::runtime_error(("HdapsGravityEngine: unable to open file "+name).c_str());
	f.read(buf,256);f.close();
	const boost::regex re("\\(([0-9+-]+),([0-9+-]+)\\).*");
   boost::cmatch matches;
	if(!boost::regex_match(buf,matches,re)) throw std::runtime_error(("HdapsGravityEngine: error parsing data from "+name).c_str());
	//cerr<<matches[1]<<","<<matches[2]<<endl;
	return Vector2i(lexical_cast<int>(matches[1]),lexical_cast<int>(matches[2]));

}

void HdapsGravityEngine::action(){
	if(!calibrated) { calibrate=readSysfsFile(hdapsDir+"/calibrate"); calibrated=true; }
	Real now=PeriodicEngine::getClock();
	if(now-lastReading>1e-3*msecUpdate){
		Vector2i a=readSysfsFile(hdapsDir+"/position");
		lastReading=now;
		a-=calibrate;
		if(abs(a[0]-accel[0])>updateThreshold) accel[0]=a[0];
		if(abs(a[1]-accel[1])>updateThreshold) accel[1]=a[1];
		Quaternionr trsf(AngleAxisr(.5*accel[0]*M_PI/180.,-Vector3r::UnitY())*AngleAxisr(.5*accel[1]*M_PI/180.,-Vector3r::UnitX()));
		gravity=trsf*zeroGravity;
	}
	GravityEngine::action();
}
