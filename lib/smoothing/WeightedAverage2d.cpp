#include"WeightedAverage2d.hpp"

/* Tell whether point is inside polygon
 *
 * See _utils.cpp: pointInsidePolygon for docs and license.
 */
bool pyGaussAverage::pointInsidePolygon(const Vector2r& pt, const vector<Vector2r>& vertices){
	int i /*current node*/, j/*previous node*/; bool inside=false; int rows=(int)vertices.size();
	const Real& testx=pt[0],testy=pt[1];
	for(i=0,j=rows-1; i<rows; j=i++){
		const Real& vx_i=vertices[i][0], vy_i=vertices[i][1], vx_j=vertices[j][0], vy_j=vertices[j][1];
		if (((vy_i>testy)!=(vy_j>testy)) && (testx < (vx_j-vx_i) * (testy-vy_i) / (vy_j-vy_i) + vx_i) ) inside=!inside;
	}
	return inside;
}

BOOST_PYTHON_MODULE(WeightedAverage2d)
{
	boost::python::class_<pyGaussAverage>("GaussAverage",python::init<python::tuple,python::tuple,python::tuple,Real>())
		.def("add",&pyGaussAverage::addPt)
		.def("avg",&pyGaussAverage::avg)
		.add_property("stDev",&pyGaussAverage::stDev_get,&pyGaussAverage::stDev_set)
		.add_property("relThreshold",&pyGaussAverage::relThreshold_get,&pyGaussAverage::relThreshold_set)
		.add_property("clips",&pyGaussAverage::clips_get,&pyGaussAverage::clips_set)
	;
};

