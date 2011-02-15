#include<yade/core/EnergyTracker.hpp>

Real EnergyTracker::total() const { Real ret=0; size_t sz=energies.size(); for(size_t id=0; id<sz; id++) ret+=energies.get(id); return ret; }
py::list EnergyTracker::keys_py() const { py::list ret; FOREACH(pairStringInt p, names) ret.append(p.first); return ret; }
py::list EnergyTracker::items_py() const { py::list ret; FOREACH(pairStringInt p, names) ret.append(py::make_tuple(p.first,energies.get(p.second))); return ret; }
py::dict EnergyTracker::perThreadData() const {
	py::dict ret;
	std::vector<std::vector<Real> > dta=energies.getPerThreadData();
	FOREACH(pairStringInt p,names) ret[p.first]=dta[p.second];
	return ret;
}

