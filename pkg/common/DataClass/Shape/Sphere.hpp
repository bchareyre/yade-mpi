#pragma once
#include<yade/core/Shape.hpp>

// HACK to work around https://bugs.launchpad.net/yade/+bug/528509
// see comments there for explanation
namespace yade{

class Sphere: public Shape{
	public:
		Sphere(Real _radius): radius(_radius){ createIndex(); }
		virtual ~Sphere ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Sphere,Shape,"Geometry of spherical particle.",
		((Real,radius,NaN,"Radius [m]")),
		createIndex(); /*ctor*/
	);
	REGISTER_CLASS_INDEX(Sphere,Shape);
};

}
// necessary
using namespace yade; 

// must be outside yade namespace
REGISTER_SERIALIZABLE(Sphere);
