#include<yade/lib/base/Math.hpp>
template<> const Real Math<Real>::EPSILON = DBL_EPSILON;
template<> const Real Math<Real>::ZERO_TOLERANCE = 1e-20;
template<> const Real Math<Real>::MAX_REAL = DBL_MAX;
template<> const Real Math<Real>::PI = 4.0*atan(1.0);
template<> const Real Math<Real>::TWO_PI = 2.0*Math<Real>::PI;
template<> const Real Math<Real>::HALF_PI = 0.5*Math<Real>::PI;
template<> const Real Math<Real>::DEG_TO_RAD = Math<Real>::PI/180.0;
template<> const Real Math<Real>::RAD_TO_DEG = 180.0/Math<Real>::PI;

template<> int ZeroInitializer<int>(){ return (int)0; }
template<> Real ZeroInitializer<Real>(){ return (Real)0; }

#ifdef YADE_MASK_ARBITRARY
bool operator==(const mask_t& g, int i) { return g == mask_t(i); }
bool operator==(int i, const mask_t& g) { return g == i; }
bool operator!=(const mask_t& g, int i) { return !(g == i); }
bool operator!=(int i, const mask_t& g) { return g != i; }
mask_t operator&(const mask_t& g, int i) { return g & mask_t(i); }
mask_t operator&(int i, const mask_t& g) { return g & i; }
mask_t operator|(const mask_t& g, int i) { return g | mask_t(i); }
mask_t operator|(int i, const mask_t& g) { return g | i; }
bool operator||(const mask_t& g, bool b) { return (g == 0) || b; }
bool operator||(bool b, const mask_t& g) { return g || b; }
bool operator&&(const mask_t& g, bool b) { return (g == 0) && b; }
bool operator&&(bool b, const mask_t& g) { return g && b; }
#endif
