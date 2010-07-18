// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
// © 2006 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>

#pragma once
#include<yade/core/Interaction.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/State.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
/*! Class representing geometry of two bodies in contact.
 *
 * The code under SCG_SHEAR is experimental and is used only if ElasticContactLaw::useShear is explicitly true
 */

#define SCG_SHEAR
#define IGCACHE

class ScGeom: public GenericSpheresContact {
	private:
		//cached values
		Vector3r twist_axis;//rotation vector arounf normal
		Vector3r orthonormal_axis;//rotation vector in contact plane
		Vector3r shearInc;//shear disp. increment
	public:
		// inherited from GenericSpheresContact: Vector3r& normal; 
		Real &radius1, &radius2;
		Real penetrationDepth;
		virtual ~ScGeom();
		
		//!precompute values of shear increment and interaction rotation data. Update contact normal to the vurrentNormal value. Precondition : the value of normal is not updated outside (and before) this function.
		void precompute(const State& rbp1, const State& rbp2, const Scene* scene, const shared_ptr<Interaction>& c, const Vector3r& currentNormal, bool isNew, bool avoidGranularRatcheting=true);

		//! Rotates a "shear" vector to keep track of contact orientation. Returns reference of the updated vector.
		Vector3r& rotate(Vector3r& tangentVector) const;
		const Vector3r& shearIncrement() const {return shearInc;}

//!________________REMOVED IN NEXT STEP___________________________________
		//! update shear on this contact given dynamic parameters of bodies. Should be called from constitutive law, exactly once per iteration. Returns shear increment in this update, which is already added to shear.
		void updateShear(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);
//!________________END REMOVED IN NEXT STEP___________________________________
		
		// Add method which returns the impact velocity (then, inside the contact law, this can be split into shear and normal component). Handle periodicity.
		Vector3r getIncidentVel(const State* rbp1, const State* rbp2, Real dt, const Vector3r& shiftVel, bool avoidGranularRatcheting=true);
		// Implement another version of getIncidentVel which does not handle periodicity.
		Vector3r getIncidentVel(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ScGeom,GenericSpheresContact,"Class representing :yref:`geometry<InteractionGeometry>` of two :yref:`bodies<Body>` in contact. The contact has 3 DOFs (normal and 2×shear) and uses incremental algorithm for updating shear. (For shear formulated in total displacements and rotations, see :yref:`Dem3DofGeom` and related classes).\n\nWe use symbols $\\vec{x}$, $\\vec{v}$, $\\vec{\\omega}$ respectively for position, linear and angular velocities (all in global coordinates) and $r$ for particles radii; subscripted with 1 or 2 to distinguish 2 spheres in contact. Then we compute unit contact normal\n\n.. math::\n\n\t\\vec{n}=\\frac{\\vec{x}_2-\\vec{x}_1}{||\\vec{x}_2-\\vec{x}_1||}\n\nRelative velocity of spheres is then\n\n.. math::\n\n\t\\vec{v}_{12}=(\\vec{v}_2+\\vec{\\omega}_2\\times(-r_2\\vec{n}))-(\\vec{v}_1+\\vec{\\omega}_1\\times(r_1\\vec{n}))\n\nand its shear component\n\n.. math::\n\n\t\\Delta\\vec{v}_{12}^s=\\vec{v}_{12}-(\\vec{n}\\cdot\\vec{v}_{12})\\vec{n}.\n\nTangential displacement increment over last step then reads\n\n.. math::\n\n\t\\vec{x}_{12}^s=\\Delta t \\vec{v}_{12}^s.",
		((Vector3r,contactPoint,Vector3r::Zero(),"Reference point of the contact. |ycomp|")),
		/* extra initializers */ ((radius1,GenericSpheresContact::refR1)) ((radius2,GenericSpheresContact::refR2)),
		/* ctor */ createIndex(); shearInc=Vector3r::Zero(); twist_axis=orthonormal_axis=Vector3r::Zero();,
		/* py */
		.def_readwrite("penetrationDepth",&ScGeom::penetrationDepth,"documentation")
		.def_readonly("shearIncrement",&ScGeom::shearInc,"documentation")
	);
	REGISTER_CLASS_INDEX(ScGeom,GenericSpheresContact);
};
REGISTER_SERIALIZABLE(ScGeom);

