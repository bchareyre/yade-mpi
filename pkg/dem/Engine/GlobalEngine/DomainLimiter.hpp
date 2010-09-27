
#include<yade/pkg-common/PeriodicEngines.hpp>

class DomainLimiter: public PeriodicEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(DomainLimiter,PeriodicEngine,"Delete particles that are out of axis-aligned box given by *lo* and *hi*.",
		((Vector3r,lo,Vector3r(0,0,0),,"Lower corner of the domain."))
		((Vector3r,hi,Vector3r(0,0,0),,"Upper corner of the domain."))
		((long,nDeleted,0,Attr::readonly,"Cummulative number of particles deleted."))
	);
};
REGISTER_SERIALIZABLE(DomainLimiter);
