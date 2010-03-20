#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <yade/core/Scene.hpp>

class ForceRecorder: public Recorder {
	public:
		Vector3r totalForce;
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ForceRecorder,Recorder,"Engine saves the resulting force affecting to Subscribed bodies. For instance, can be useful for defining the forces, which affect to _buldozer_ during its work.",
		((std::vector<int>,subscribedBodies,,"Lists of bodies whose state will be measured")),
		initRun=true;
	);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(ForceRecorder);
