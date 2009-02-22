// 2007 © Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<string>
#include<map>
#include<iostream>
#include<typeinfo>
#include<boost/any.hpp>
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
class SimpleViscoelasticBodyParameters;

using namespace std;
using boost::shared_ptr;

/*! Miscillaneous utility functions which are believed to be generally useful.
 *
 * All data members are methods are static, no instance of Shop is created. It is not serializable either.
 */

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

		//! creates empty rootBody along with its parameters (boundingVolume etc.)
		static shared_ptr<MetaBody> rootBody();
		/*! creates engines and initilizers within given MetaBody: elastic contact law, with gravity, timestepper and damping.
		 *
		 * All parameters are take from Shop::defaults, which are user-settable.
		 */
		static void rootBodyActors(shared_ptr<MetaBody>);
		//! create default sphere, along with its boundingVolume etc. 
		static shared_ptr<Body> sphere(Vector3r center, Real radius);
		//! create default box with everything needed
		static shared_ptr<Body> box(Vector3r center, Vector3r extents);
		//! create default tetrahedron
		static shared_ptr<Body> tetra(Vector3r v[4]);

		//! Return vector of pairs (center,radius) loaded from a file with numbers inside
		static vector<pair<Vector3r,Real> > loadSpheresFromFile(string fname,Vector3r& minXYZ, Vector3r& maxXYZ);
		
		struct sphereGeomStruct{ double C0, C1, C2, r; };
		static sphereGeomStruct smallSdecXyzData[];
		//! Return vector of pairs (center,radius) with values from small.sdec.xyz (hardwired in the code, doesn't need that file actually)
		static vector<pair<Vector3r,Real> > loadSpheresSmallSdecXyz(Vector3r& minXYZ, Vector3r& maxXYZ);
		
		//! Save spheres in the current simulation into a text file
		static void saveSpheresToFile(string fileName);

		/*! Cache for class indices for physical actions (body external variables, Bex)
		 *
		 * It is necessary to populate the cache by calling initCache(); then supported
		 * actions can be used like Shop::Bex::force(bodyId)+=someForce and so on.
		 */
		class Bex{
			public:
			static int forceIdx,momentumIdx;
			static void initCache();
			#ifdef BEX_CONTAINER
				static const Vector3r& force(body_id_t, MetaBody* mb=NULL);
				static const Vector3r& momentum(body_id_t, MetaBody* mb=NULL);
			#else
				static Vector3r& force(body_id_t, MetaBody* mb=NULL);
				static Vector3r& momentum(body_id_t, MetaBody* mb=NULL);
			#endif
		};

		//! Estimate timestep based on P-wave propagation speed
		static Real PWaveTimeStep(shared_ptr<MetaBody> rb=shared_ptr<MetaBody>());

		//! Calculate inscribed circle center of trianlge
		static Vector3r inscribedCircleCenter(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2);

		/// Get viscoelastic parameters kn,cn,ks,cs from analytical solution of
		/// a problem of interaction of pair spheres with mass m, collision
		/// time tc and restitution coefficients en,es.
	    static void getViscoelasticFromSpheresInteraction(Real m, Real tc, Real en, Real es, shared_ptr<SimpleViscoelasticBodyParameters> b);

		//! Get unbalanced force of the whole simulation
		static Real unbalancedForce(bool useMaxForce=false, MetaBody* _rb=NULL);
		static Real kineticEnergy(MetaBody* _rb=NULL);

		//! create transientInteraction between 2 bodies, using existing MetaEngine in Omega
		static shared_ptr<Interaction> createExplicitInteraction(body_id_t id1, body_id_t id2);

		//! apply force on contact point on both bodies (reversed on body 2)
		static void applyForceAtContactPoint(const Vector3r& force, const Vector3r& contPt, body_id_t id1, const Vector3r& pos1, body_id_t id2, const Vector3r& pos2, MetaBody* rb);

		//! map scalar variable to 1d colorscale
		static Vector3r scalarOnColorScale(Real x, Real xmin=0., Real xmax=1.);

		//! wrap floating number periodically to the given range
		static Real periodicWrap(Real x, Real x0, Real x1, long* period=NULL);
};
