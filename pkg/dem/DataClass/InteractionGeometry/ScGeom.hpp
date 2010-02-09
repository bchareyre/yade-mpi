// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/State.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
/*! Class representing geometry of two spheres in contact.
 *
 * The code under SCG_SHEAR is experimental and is used only if ElasticContactLaw::useShear is explicitly true
 */

#define SCG_SHEAR

class ScGeom: public GenericSpheresContact {
	public:
		// inherited from GenericSpheresContact: Vector3r& normal; 
		Real penetrationDepth;
		Real &radius1, &radius2;

		#ifdef SCG_SHEAR
			//! update shear on this contact given dynamic parameters of bodies. Should be called from constitutive law, exactly once per iteration. Returns shear increment in this update, which is already added to shear.
			Vector3r updateShear(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);
		#endif
		
		virtual ~ScGeom();

		void updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ScGeom,GenericSpheresContact,"Class representing geometry of two spheres in contact.",
		((Vector3r,contactPoint,Vector3r::ZERO,"Reference point of the contact. |ycomp|"))
		#ifdef SCG_SHEAR
			((Vector3r,shear,Vector3r::ZERO,"Total value of the current shear. Update the value using ScGeom::updateShear. |ycomp|"))
			((Vector3r,prevNormal,Vector3r::ZERO,"Normal of the contact in the previous step. |ycomp|"))
		#endif
		,
		/* extra initializers */ ((radius1,GenericSpheresContact::refR1)) ((radius2,GenericSpheresContact::refR2)),
		/* ctor */ createIndex();,
		/* py */
	);
	REGISTER_CLASS_INDEX(ScGeom,InteractionGeometry);
};
REGISTER_SERIALIZABLE(ScGeom);

