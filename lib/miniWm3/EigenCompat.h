#pragma once

#ifdef YADE_EIGEN_COMPAT
	#define EIG_FUN
	#define WM3_FUN __attribute__((deprecated))
#else
	#define EIG_FUN 
	//__attribute__((warning("This function should only be used with eigen-compat enabled.")))
	#define WM3_FUN
#endif
