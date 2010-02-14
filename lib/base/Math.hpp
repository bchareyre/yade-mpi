#if defined(YADE_EIGEN) and defined(YADE_NOWM3)
	/* eigen math wrapper that replaces wm3, use with caution

		you MUST make symlinks from the wm3-eigen branch into this directory like this:
	
		wm3-eigen/eigen/yadeMath.cpp → lib/base/yadeEigenWrapper.cpp
		wm3-eigen/eigen/yadeMath.hpp → lib/base/yadeMath.hpp
		wm3-eigen/eigen/yadeMath.inl → lib/base/yadeMath.inl
	
		(names will be made more meaningful once this settles down)

		good luck
	*/
	#warning @@@@@@@@@@ Using wrapper around eigen to emulate wm3; this is EXPERIMENTAL for developers only; numerical results might very well be garbage. @@@@@@@@@@
	#include<yade/lib-base/yadeMath.hpp>
#else
	#include<yade/lib-base/yadeWm3Extra_dont_include_directly.hpp>
#endif
