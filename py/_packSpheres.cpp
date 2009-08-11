// 2009 © Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/pkg-dem/SpherePack.hpp>

BOOST_PYTHON_MODULE(_packSpheres){
	python::class_<SpherePack>("SpherePack","Set of spheres as centers and radii",python::init<python::optional<python::list> >(python::args("list"),"Empty constructor, optionally taking list [ ((cx,cy,cz),r), … ] for initial data." ))
		.def("add",&SpherePack::add,"Add single sphere to packing, given center as 3-tuple and radius")
		.def("toList",&SpherePack::toList,"Return packing data as python list.")
		.def("toList_pointsAsTuples",&SpherePack::toList_pointsAsTuples,"Return packing data as python list, but using only pure-python data types (3-tuples instead of Vector3) (for pickling with cPickle)")
		.def("fromList",&SpherePack::fromList,"Make packing from given list, same format as for constructor. Discards current data.")
		.def("load",&SpherePack::fromFile,"Load packing from external text file (current data will be discarded).")
		.def("save",&SpherePack::toFile,"Save packing to external text file (will be overwritten).")
		.def("fromSimulation",&SpherePack::fromSimulation,"Make packing corresponding to the current simulation. Discards current data.")
		.def("makeCloud",&SpherePack::makeCloud,python::args("minCorner","maxCorner","rMean","rRelFuzz","num","periodic"),"Create random packing encosed in box given by minCorner and maxCorner, containing num spheres. Returns number of created spheres, which can be < num if the packing is too tight.")
		.def("aabb",&SpherePack::aabb_py,"Get axis-aligned bounding box coordinates, as 2 3-tuples.")
		.def("dim",&SpherePack::dim,"Return dimensions of the packing in terms of aabb(), as a 3-tuple.")
		.def("center",&SpherePack::midPt,"Return coordinates of the bounding box center.")
		.def_readonly("cellSize",&SpherePack::cellSize,"Size of periodic cell; is Vector3(0,0,0) if not periodic.")
		.def("cellFillVolume",&SpherePack::cellFillVolume,"Repeat the packing (if periodic) so that the results has dim() >= given size. The packing retains periodicity, but changes cellSize. Raises exception for non-periodic packing.")
		.def("cellRepeat",&SpherePack::cellRepeat,"Repeat the packing given number of times in each dimension. Periodicity is retained, cellSize changes. Raises exception for non-periodic packing.")
		.def("relDensity",&SpherePack::relDensity,"Relative packing density, measured as sum of spheres' volumes / aabb volume.\n(Sphere overlaps are ignored.)")
		.def("translate",&SpherePack::translate,"Translate all spheres by given vector.")
		.def("rotate",&SpherePack::rotate,"Rotate all spheres around packing center (in terms of aabb()), given axis and angle of the rotation.")
		.def("scale",&SpherePack::scale,"Scale the packing around its center (in terms of aabb()) by given factor (may be negative).")
		.def("__len__",&SpherePack::len,"Get number of spheres in the packing")
		.def("__getitem__",&SpherePack::getitem,"Get entry at given index, as tuple of center and radius.")
		.def("__iter__",&SpherePack::getIterator,"Return iterator over spheres.")
		;
	python::class_<SpherePack::iterator>("SpherePackIterator",python::init<SpherePack::iterator&>())
		.def("__iter__",&SpherePack::iterator::iter)
		.def("next",&SpherePack::iterator::next)
	;
}

