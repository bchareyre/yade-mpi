// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<string.h>
#include<vector>
#include<Wm3Vector3.h>
// for body_id_t
#include<yade/core/Interaction.hpp>

/* Container for Body External Variables (bex), typically forces and torques from interactions.
 * Values should be reset at every iteration by BexResetter.
 * If you want to add your own bex, there are 4 steps:
 *
 * 	1. Create storage vector
 * 	2. Create accessor function
 * 	3. Update the resize function
 * 	4. Update the reset function
 *
 */
class BexContainer {
	private:
		std::vector<Vector3r> _force;
		std::vector<Vector3r> _torque;
		size_t size;
	public:
		BexContainer(): size(0){}
		inline void ensureSize(body_id_t id){ if(size<=(size_t)id) resize(min((size_t)1.5*(id+100),(size_t)(id+2000)));}
		//! Get writable reference to force acting on body # id
		inline Vector3r& force(body_id_t id){ ensureSize(id); return _force[id]; }
		//! Get writable reference to torque acting on body # id
		inline Vector3r& torque(body_id_t id){ ensureSize(id); return _torque[id]; }
		//! Set all bex's to zero
		void reset(){
			memset(_force[0], 0,sizeof(Vector3r)*size);
			memset(_torque[0],0,sizeof(Vector3r)*size);
		}
		/*! Resize the container; this happens automatically,
		 * but you may want to set the size beforehand to avoid resizes as the simulation grows.
		 */
		void resize(size_t newSize){
			_force.resize(newSize);
			_torque.resize(newSize);
			size=newSize;
			std::cerr<<"[DEBUG] BexContainer: Resized to "<<size<<std::endl;
		}
};
