// 2009 © Václav Šmilauer <eudoxos@arcig.cz

#include"numpy_boost.hpp"

// helper macro do assign Vector3r values to a subarray
#define VECTOR3R_TO_NUMPY(vec,arr) arr[0]=vec[0]; arr[1]=vec[1]; arr[2]=vec[2]
