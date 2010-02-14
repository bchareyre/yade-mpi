#pragma once

#ifdef QUAD_PRECISION
	typedef long double quad;
	typedef quad Real;
#else
	typedef double Real;
#endif

namespace Wm3 {
// GeometricTools/WildMagic3/Include$ egrep '^typedef\s*([a-zA-Z0-9]+)<float> \1f;' *.h | perl -pe 's/^.*?:(typedef ([a-zA-Z0-9]+).*$)/\ttemplate<class T> class \2; typedef \2<Real> \2r;/'
//
	template<class T> class Math; typedef Math<Real> Mathr;
	template<class T> class Matrix3; typedef Matrix3<Real> Matrix3r;
	template<class T> class Quaternion; typedef Quaternion<Real> Quaternionr;
	template<class T> class Vector2; typedef Vector2<Real> Vector2r; typedef Vector2<int> Vector2i;
	template<class T> class Vector3; typedef Vector3<Real> Vector3r; typedef Vector3<int> Vector3i;
}
using namespace Wm3;
