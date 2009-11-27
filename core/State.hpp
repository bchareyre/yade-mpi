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
		Vector3r inertia;

		Vector3r prevAngMom;

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

	State(): se3(Vector3r::ZERO,Quaternionr::IDENTITY),pos(se3.position),vel(Vector3r::ZERO),accel(Vector3r::ZERO),mass(0.),ori(se3.orientation),angVel(Vector3r::ZERO),angAccel(Vector3r::ZERO),inertia(Vector3r::ZERO),prevAngMom(Vector3r::ZERO),refPos(Vector3r::ZERO),refOri(Quaternionr::IDENTITY),blockedDOFs(DOF_NONE){}

	REGISTER_CLASS_AND_BASE(State,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(pos)(vel)(accel)(mass)(ori)(angVel)(angAccel)(inertia)(refPos)(refOri)(blockedDOFs)(prevAngMom));
};
REGISTER_SERIALIZABLE(State);
