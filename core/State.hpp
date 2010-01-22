// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/lib-serialization/Serializable.hpp>
/*! State (internal & spatial variables) of a body.

For now, I put position, orientation, velocity and angular velocity here,
since (AFAIK) we have no bodies that lack them. If in the future
someone needs bodies without orientation, then orientation and angular
velocity can be pushed to a derived class (and the rest of code adapted
to that).

All state variables are initialized to zeros.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the Material class.
*/
class State: public Serializable{
	public:
		Se3r se3;

		/// linear motion
		Vector3r& pos;
		Vector3r vel;
		Vector3r accel;
		Real mass;

		/// rotational motion
		Quaternionr& ori;
		Vector3r angVel;
		Vector3r angAccel;
		Vector3r angMom;
		Vector3r inertia;
		//
		/// reference values
		Vector3r refPos;
		Quaternionr refOri;

		//! mutex for updating the parameters from within the interaction loop (only used rarely)
		boost::mutex updateMutex;

		
		//! Bitmask for degrees of freedom where velocity will be always zero, regardless of applied forces
		unsigned blockedDOFs; 
		// bits
		enum {DOF_NONE=0,DOF_X=1,DOF_Y=2,DOF_Z=4,DOF_RX=8,DOF_RY=16,DOF_RZ=32};
		//! shorthand for all DOFs blocked
		static const unsigned DOF_ALL=DOF_X|DOF_Y|DOF_Z|DOF_RX|DOF_RY|DOF_RZ;
		//! shorthand for all displacements blocked
		static const unsigned DOF_XYZ=DOF_X|DOF_Y|DOF_Z;
		//! shorthand for all rotations blocked
		static const unsigned DOF_RXRYRZ=DOF_RX|DOF_RY|DOF_RZ; 

		//! Return DOF_* constant for given axis∈{0,1,2} and rotationalDOF∈{false(default),true}; e.g. axisDOF(0,true)==DOF_RX
		static unsigned axisDOF(int axis, bool rotationalDOF=false){return 1<<(axis+(rotationalDOF?3:0));}		
		//! set DOFs according to two Vector3r arguments (blocked is when disp[i]==1.0 or rot[i]==1.0)
		void setDOFfromVector3r(Vector3r disp,Vector3r rot=Vector3r::ZERO);
		//! Getter of blockedDOFs for list of strings (e.g. DOF_X | DOR_RX | DOF_RZ → ['x','rx','rz'])
		std::vector<std::string> blockedDOFs_vec_get() const;
		//! Setter of blockedDOFs from list of strings (['x','rx','rz'] → DOF_X | DOR_RX | DOF_RZ)
		void blockedDOFs_vec_set(const std::vector<std::string>& dofs);

		//! Return displacement (current-reference position)
		Vector3r displ() const {return pos-refPos;}
		//! Return rotation (current-reference orientation, as Vector3r)
		Vector3r rot() const { Quaternionr relRot=refOri.Conjugate()*ori; Vector3r axis; Real angle; relRot.ToAxisAngle(axis,angle); return axis*angle; }

		// python access functions: pos and ori are references to inside Se3r and cannot be pointed to directly
		Vector3r pos_get() const {return pos;}
		void pos_set(const Vector3r& p) {pos=p;}
		Quaternionr ori_get() const {return ori; }
		void ori_set(const Quaternionr& o){ori=o;}



	State(): se3(Vector3r::ZERO,Quaternionr::IDENTITY),pos(se3.position),vel(Vector3r::ZERO),accel(Vector3r::ZERO),mass(0.),ori(se3.orientation),angVel(Vector3r::ZERO),angAccel(Vector3r::ZERO),angMom(Vector3r::ZERO),inertia(Vector3r::ZERO),refPos(Vector3r::ZERO),refOri(Quaternionr::IDENTITY),blockedDOFs(DOF_NONE){}

	YADE_CLASS_BASE_DOC_ATTRS_PY(State,Serializable,"State of a body (spatial configuration, internal variables).",
		((se3,"Position and orientation as one object."))
		((vel,"Current linear velocity."))
		((accel,"Current acceleration."))
		((mass,"Mass of this body"))
		((angVel,"Current angular velocity"))
		((angAccel,"Current angular acceleration"))
		((angMom,"Current angular momentum"))
		((inertia,"Inertia of associated body, in local coordinate system."))
		((refPos,"Reference position"))
		((refOri,"Reference orientation"))
		((blockedDOFs,"[Will be overridden]")),
		.add_property("blockedDOFs",&State::blockedDOFs_vec_get,&State::blockedDOFs_vec_set,"Degress of freedom where linear/angular velocity will be always zero, regardless of applied force/torque. List of any combination of 'x','y','z','rx','ry','rz'.")
		// references must be set using wrapper funcs
		.add_property("pos",&State::pos_get,&State::pos_set,"Current position.")
		.add_property("ori",&State::ori_get,&State::ori_set,"Current orientation.") 
	);
};
REGISTER_SERIALIZABLE(State);
