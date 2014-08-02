// =======================================================
// Some plugins from removed CPP-fiels

#include <yade/pkg/dem/DemXDofGeom.hpp>
#include <yade/pkg/common/TorqueEngine.hpp>
#include <yade/pkg/common/ForceResetter.hpp>
#include <yade/pkg/common/FieldApplier.hpp>
#include <yade/pkg/common/Callbacks.hpp>
#include <yade/pkg/common/BoundaryController.hpp>
#include <yade/pkg/common/NormShearPhys.hpp>
#include <yade/pkg/common/Recorder.hpp>
#include <yade/pkg/common/CylScGeom6D.hpp>
#include <yade/pkg/common/Box.hpp>
#include <yade/pkg/common/StepDisplacer.hpp>
#include <yade/pkg/common/PeriodicEngines.hpp>
#include <yade/pkg/common/ElastMat.hpp>
#include <yade/pkg/common/PyRunner.hpp>
#include <yade/pkg/common/Sphere.hpp>
#include <yade/pkg/common/Aabb.hpp>

YADE_PLUGIN((IntrCallback)
	#ifdef YADE_BODY_CALLBACK
		(BodyCallback)
	#endif
);

YADE_PLUGIN((ForceResetter)(TorqueEngine)(FieldApplier)(BoundaryController)
            (NormPhys)(NormShearPhys)(Recorder)(CylScGeom6D)(CylScGeom)(Box)
            (StepDisplacer)(GenericSpheresContact)
            (PeriodicEngine)(Sphere)(Aabb)(ElastMat)(FrictMat)(PyRunner)
            );
