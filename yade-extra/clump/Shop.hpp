// 2007 © Václav Šmilauer <eudoxos@arcig.cz>

#include<string>
#include<map>
#include<iostream>
#include<boost/any.hpp>
#include<boost/shared_ptr.hpp>

#include<Wm3Vector3.h>
#include<Wm3Quaternion.h>
#include<yade/yade-lib-base/yadeWm3.hpp>

/*
#include<yade/yade-core/MetaBody.hpp>
#include<yade/yade-core/Body.hpp>
#include<yade/yade-core/Omega.hpp>
*/
class MetaBody;
class Body;

using namespace std;
using boost::shared_ptr;

class Shop{
	private:
	public:
		/*! map of <attribute name,value>. */
		static map<string,boost::any> defaults;
		/*! fills Shop::defaults with values. Called automatically. */ 
		static void init();
		/*! Calls Shop::init if Shop::defaults is empty (when setting or getting a default). */
		static void ensureInit(){if(defaults.size()==0) init();}
	public:
		/*! Retrieve default value from the map. User is responsible for casting it to the right type. */
		template <typename valType> static valType getDefault(string key){ensureInit(); return boost::any_cast<valType>(defaults[key]);}
		//template <typename valType> static valType getDefault(string key){ensureInit(); return boost::any_cast<valType>(defaults[key]);}
		/*! Set the default value. Overrides existing value or creates new entry. Integer types are always cast to long (be careful when retrieving them). */
		template <typename valType> static void setDefault(string key, const valType value){ensureInit(); defaults[key]=boost::any(value);}

		static shared_ptr<MetaBody> rootBody();
		static void rootBodyActors(shared_ptr<MetaBody>);
		static shared_ptr<Body> sphere(Vector3r center, Real radius);
		static shared_ptr<Body> box(Vector3r center, Vector3r extents);
};

