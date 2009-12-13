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
		Vector3r& normal; // unit vector in the direction from sphere1 center to sphere2 center
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
		
		/* keep this for reference, until it is implemented elsewhere, like Dem6DofGeom, if it ever exists */
		#if 0
			// initial relative orientation, used for bending and torsion computation
			Quaternionr initRelOri12;
			void bendingTorsionAbs(Vector3r& bend, Real& tors);
			void bendingTorsionRel(Vector3r& bend, Real& tors){ bendingTorsionAbs(bend,tors); bend/=d0; tors/=d0;}
			Vector3r relRotVector() const;
		#endif


		ScGeom():normal(GenericSpheresContact::normal),contactPoint(Vector3r::ZERO),radius1(GenericSpheresContact::refR1),radius2(GenericSpheresContact::refR2)
		#ifdef SCG_SHEAR
			,shear(Vector3r::ZERO), prevNormal(Vector3r::ZERO) /*initialized to proper value by geom functor*/
		#endif
			{ createIndex(); }
		virtual ~ScGeom();

		void updateShearForce(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);

	REGISTER_ATTRIBUTES(Serializable,
			(normal)
			(contactPoint)
			(radius1)
			(radius2)
			#ifdef SCG_SHEAR
				(shear)
				(prevNormal)
			#endif
			);
	REGISTER_CLASS_AND_BASE(ScGeom,InteractionGeometry);
	REGISTER_CLASS_INDEX(ScGeom,InteractionGeometry);
};

__attribute__((deprecated)) typedef ScGeom SpheresContactGeometry;

REGISTER_SERIALIZABLE(ScGeom);

