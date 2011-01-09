#include<yade/pkg/common/Callbacks.hpp>

#ifdef YADE_BODY_CALLBACK
	BodyCallback::~BodyCallback(){};
#endif

IntrCallback::~IntrCallback(){};

YADE_PLUGIN((IntrCallback)
	#ifdef YADE_BODY_CALLBACK
		(BodyCallback)
	#endif
);
