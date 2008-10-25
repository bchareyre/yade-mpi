#include"WeightedAverage2d.hpp"

BOOST_PYTHON_MODULE(WeightedAverage2d)
{
	boost::python::class_<pyGaussAverage>("GaussAverage",python::init<python::tuple,python::tuple,python::tuple,Real>())
		.def("add",&pyGaussAverage::addPt)
		.def("avg",&pyGaussAverage::avg)
		.add_property("stDev",&pyGaussAverage::stDev_get,&pyGaussAverage::stDev_set)
		.add_property("relThreshold",&pyGaussAverage::relThreshold_get,&pyGaussAverage::relThreshold_set)
	;
};

