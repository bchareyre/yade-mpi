#include<yade/pkg/common/Callbacks.hpp>
BodyCallback::~BodyCallback(){};
IntrCallback::~IntrCallback(){};
YADE_PLUGIN((IntrCallback)(BodyCallback));
