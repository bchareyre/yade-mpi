// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
// © 2006 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>

#pragma once
#include<yade/core/Interaction.hpp>
#include<yade/core/IGeom.hpp>
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
		Vector3r twist_axis;//rotation vector around normal
		Vector3r orthonormal_axis;//rotation vector in contact plane
	public:
		// inherited from GenericSpheresContact: Vector3r& normal; 
		Real &radius1, &radius2;
		virtual ~ScGeom();
		
		//!precompute values of shear increment and interaction rotation data. Update contact normal to the vurrentNormal value. Precondition : the value of normal is not updated outside (and before) this function.
		void precompute(const State& rbp1, const State& rbp2, const Scene* scene, const shared_ptr<Interaction>& c, const Vector3r& currentNormal, bool isNew, const Vector3r& shift2, bool avoidGranularRatcheting=true);

		//! Rotates a "shear" vector to keep track of contact orientation. Returns reference of the updated vector.
		Vector3r& rotate(Vector3r& tangentVector) const;
		const Vector3r& shearIncrement() const {return shearInc;}

		// Add method which returns the impact velocity (then, inside the contact law, this can be split into shear and normal component). Handle periodicity.
		Vector3r getIncidentVel(const State* rbp1, const State* rbp2, Real dt, const Vector3r& shiftVel, const Vector3r& shift2, bool avoidGranularRatcheting=true);
		// Implement another version of getIncidentVel which does not handle periodicity.
		Vector3r getIncidentVel(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting=true);
		
		// convenience version to be called from python
		Vector3r getIncidentVel_py(shared_ptr<Interaction> i, bool avoidGranularRatcheting); 
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ScGeom,GenericSpheresContact,"Class representing :yref:`geometry<IGeom>` of two :yref:`bodies<Body>` in contact. The contact has 3 DOFs (normal and 2×shear) and uses incremental algorithm for updating shear. (For shear formulated in total displacements and rotations, see :yref:`Dem3DofGeom` and related classes).\n\nWe use symbols $\\vec{x}$, $\\vec{v}$, $\\vec{\\omega}$ respectively for position, linear and angular velocities (all in global coordinates) and $r$ for particles radii; subscripted with 1 or 2 to distinguish 2 spheres in contact. Then we compute unit contact normal\n\n.. math::\n\n\t\\vec{n}=\\frac{\\vec{x}_2-\\vec{x}_1}{||\\vec{x}_2-\\vec{x}_1||}\n\nRelative velocity of spheres is then\n\n.. math::\n\n\t\\vec{v}_{12}=(\\vec{v}_2+\\vec{\\omega}_2\\times(-r_2\\vec{n}))-(\\vec{v}_1+\\vec{\\omega}_1\\times(r_1\\vec{n}))\n\nand its shear component\n\n.. math::\n\n\t\\Delta\\vec{v}_{12}^s=\\vec{v}_{12}-(\\vec{n}\\cdot\\vec{v}_{12})\\vec{n}.\n\nTangential displacement increment over last step then reads\n\n.. math::\n\n\t\\vec{x}_{12}^s=\\Delta t \\vec{v}_{12}^s.",
		((Real,penetrationDepth,NaN,(Attr::noSave|Attr::readonly),"Penetration distance of spheres (positive if overlapping)"))
		((Vector3r,shearInc,Vector3r::Zero(),(Attr::noSave|Attr::readonly),"Shear displacement increment in the last step"))
		,
		/* extra initializers */ ((radius1,GenericSpheresContact::refR1)) ((radius2,GenericSpheresContact::refR2)),
		/* ctor */ createIndex(); twist_axis=orthonormal_axis=Vector3r::Zero();,
		/* py */ .def("incidentVel",&ScGeom::getIncidentVel_py,(python::arg("i"),python::arg("avoidGranularRatcheting")=true),"Return incident velocity of the interaction.")
	);
	REGISTER_CLASS_INDEX(ScGeom,GenericSpheresContact);
};
REGISTER_SERIALIZABLE(ScGeom);

class ScGeom6D: public ScGeom {
	public:
		virtual ~ScGeom6D();
		const Real& getTwist() const {return twist;}
		const Vector3r& getBending() const {return bending;}

		void precomputeRotations(const State& rbp1, const State& rbp2, bool isNew, bool creep=false);
		void initRotations(const State& rbp1, const State& rbp2);
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ScGeom6D,ScGeom,"Class representing :yref:`geometry<IGeom>` of two :yref:`bodies<Body>` in contact. The contact has 6 DOFs (normal, 2×shear, twist, 2xbending) and uses :yref:`ScGeom` incremental algorithm for updating shear.",
		((Quaternionr,orientationToContact1,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,orientationToContact2,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,initialOrientation1,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,initialOrientation2,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,twistCreep,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,currentContactOrientation,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Quaternionr,initialContactOrientation,Quaternionr(1.0,0.0,0.0,0.0),,""))
		((Real,twist,0,(Attr::noSave | Attr::readonly),"Elastic twist angle of the contact."))
		((Vector3r,bending,Vector3r::Zero(),(Attr::noSave | Attr::readonly),"Bending at contact as a vector defining axis of rotation and angle (angle=norm)."))
		,
		/* extra initializers */,
		/* ctor */ createIndex(); twist=0;bending=Vector3r::Zero();,
 		/* py */
	);
	REGISTER_CLASS_INDEX(ScGeom6D,ScGeom);
};
REGISTER_SERIALIZABLE(ScGeom6D);

