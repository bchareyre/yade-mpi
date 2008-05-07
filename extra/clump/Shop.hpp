// 2007 © Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<string>
#include<map>
#include<iostream>
#include<typeinfo>
#include<boost/any.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/lambda/lambda.hpp>

#include<Wm3Vector3.h>
#include<Wm3Quaternion.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/core/Body.hpp>

#include<boost/function.hpp>

/*
#include<yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Omega.hpp>
*/
class MetaBody;
class Body;

using namespace std;
using boost::shared_ptr;

class Shop{
	private:
		DECLARE_LOGGER;
	public:
		/*! map of <attribute name,value>. */
		static map<string,boost::any> defaults;
		/*! fills Shop::defaults with values. Called automatically. */ 
		static void init();
		/*! Calls Shop::init if Shop::defaults is empty (when setting or getting a default). */
		static void ensureInit(){if(defaults.size()==0) init();}
	public:
		static bool hasDefault(const string& key){return defaults.find(key)!=defaults.end();}
		/*! Retrieve default value from the map. User is responsible for casting it to the right type. */
		template <typename valType> static valType getDefault(const string& key);
		//template <typename valType> static valType getDefault(string key){ensureInit(); return boost::any_cast<valType>(defaults[key]);}
		/*! Set the default value. Overrides existing value or creates new entry. Integer types are always cast to long (be careful when retrieving them). */
		template <typename valType> static void setDefault(string key, const valType value){
			ensureInit(); //cerr<<"Shop: Setting `"<<key<<"'="<<value<<" (type `"<<typeid(valType).name()<<"')."<<endl;
			defaults[key]=boost::any(value);}

		static shared_ptr<MetaBody> rootBody();
		static void rootBodyActors(shared_ptr<MetaBody>);
		static shared_ptr<Body> sphere(Vector3r center, Real radius);
		static shared_ptr<Body> box(Vector3r center, Vector3r extents);
		static shared_ptr<Body> tetra(Vector3r v[4]);

		static vector<pair<Vector3r,Real> > loadSpheresFromFile(string fname,Vector3r& minXYZ, Vector3r& maxXYZ);
		
		struct sphereGeomStruct{ double C0, C1, C2, r; };
		static sphereGeomStruct smallSdecXyzData[];
		static vector<pair<Vector3r,Real> > loadSpheresSmallSdecXyz(Vector3r& minXYZ, Vector3r& maxXYZ);
		
		static void saveSpheresToFile(string fileName);

		// (true || boost::lambda::_1) means that true is the default
		static int createCohesion(Real limitNormalForce, Real limitShearForce, int groupMask=0,
			boost::function<bool(body_id_t,body_id_t)> linkOK = (true || boost::lambda::_1) );

		static Real ElasticWaveTimestepEstimate(shared_ptr<MetaBody>);

		static void GLDrawLine(Vector3r from, Vector3r to, Vector3r color=Vector3r(1,1,1));
		static void GLDrawArrow(Vector3r from, Vector3r to, Vector3r color=Vector3r(1,1,1));
		static void GLDrawText(std::string text, Vector3r pos, Vector3r color=Vector3r(1,1,1));
		static void GLDrawNum(Real n, Vector3r pos, Vector3r color=Vector3r(1,1,1), unsigned precision=3);

		class Bex{
			public:
			static int forceIdx,momentumIdx,globalStiffnessIdx;
			static void init();
			static Vector3r& force(body_id_t);
			static Vector3r& momentum(body_id_t);
			static Vector3r& globalStiffness(body_id_t);
			static Vector3r& globalRStiffness(body_id_t);
		};

};
