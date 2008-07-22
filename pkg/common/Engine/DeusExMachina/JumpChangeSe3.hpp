// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<yade/core/PhysicalParameters.hpp>

/* Engine that applies sudden change of se3 on subscribed bodies if active.
 *
 * the amount deltaSe3 will be "added" to the current se3 (positions vector-added,
 * orientations composed by quaternion multiplication).
 */
class JumpChangeSe3: public DeusExMachina {
	public:
		JumpChangeSe3(){deltaSe3.position=Vector3r::ZERO; deltaSe3.orientation=Quaternionr::IDENTITY;};
		virtual ~JumpChangeSe3(){};
		virtual void applyCondition(MetaBody* mb){
			FOREACH(body_id_t id, subscribedBodies){
				const shared_ptr<Body>& b=Body::byId(id,mb);
				Se3r& se3=b->physicalParameters->se3;
				se3.position+=deltaSe3.position;
				se3.orientation=deltaSe3.orientation*se3.orientation;
			}
		}
		Se3r deltaSe3;
	protected:
		virtual void registerAttributes(){DeusExMachina::registerAttributes();REGISTER_ATTRIBUTE(deltaSe3);}
		REGISTER_CLASS_NAME(JumpChangeSe3);
		REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(JumpChangeSe3,false);
	

