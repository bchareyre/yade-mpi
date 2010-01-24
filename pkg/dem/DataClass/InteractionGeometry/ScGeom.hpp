// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/State.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
/*! Class representing geometry of two spheres in contact.
 *
 * The code under SCG_SHEAR is experimental and is used only if ElasticContactLaw::useShear is explicitly true
 */

#define SCG_SHEAR

class ScGeom: public GenericSpheresContact {
	public:
		// inherited from GenericSpheresContact: Vector3r& normal; 
		Vector3r contactPoint;
		Real penetrationDepth;
		Real &radius1, &radius2;

		#ifdef SCG_SHEAR
			//! Total value of the current shear. Update the value using updateShear.
			Vector3r shear;
			//! Normal of the contact in the previous step
			Vector3r prevNormal;
			//! update shear on this contact given dynamic parameters of bodies. Should be called from constitutive law, exactly once per iteration. Returns shear increment in this update, which is already added to shear.
			Vector3r updateShear(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);
		#endif
		

		ScGeom():contactPoint(Vector3r::ZERO),radius1(GenericSpheresContact::refR1),radius2(GenericSpheresContact::refR2)
		#ifdef SCG_SHEAR
			,shear(Vector3r::ZERO), prevNormal(Vector3r::ZERO) /*initialized to proper value by geom functor*/
		#endif
			{ createIndex(); }
		virtual ~ScGeom();

		void updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);

	YADE_CLASS_BASE_DOC_ATTRS(ScGeom,GenericSpheresContact,"Class representing geometry of two spheres in contact.",
		((contactPoint,"Reference point of the contact. |ycomp|"))
		#ifdef SCG_SHEAR
			((shear,"Total value of the current shear. Update the value using ScGeom::updateShear. |ycomp|"))
			((prevNormal,"Normal of the contact in the previous step. |ycomp|"))
		#endif
	);
	REGISTER_CLASS_INDEX(ScGeom,InteractionGeometry);
};
REGISTER_SERIALIZABLE(ScGeom);

