#ifndef __BVCFFINALCOLLIDER_H__
#define __BVCFFINALCOLLIDER_H__

#include "CollisionModel.hpp"
#include "Contact.hpp"
#include "FinalCollider.hpp"

class BVCFFinalCollider : public FinalCollider
{
	typedef struct BoxBoxCollisionInfo
	{
		bool isNormalPrincipalAxis;
		bool invertNormal;
		float penetrationDepth;
		int code;
		Vector3 normal;
	} BoxBoxCollisionInfo;

	// construction	
	public : BVCFFinalCollider ();
	public : ~BVCFFinalCollider ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	protected : static bool collideSphereSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideSphereAABox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideAABoxSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideSphereBox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideBoxSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideBoxBox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideTerrainSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideSphereTerrain(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideTerrainBox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : static bool collideBoxTerrain(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	
	
	private : static void lineClosestApproach (const Vector3 pa, const Vector3 ua, const Vector3 pb, const Vector3 ub, float &alpha, float &beta);
	private : static bool testSeparatingAxis(float expr1, float expr2, Vector3 n,int c,BoxBoxCollisionInfo* bbInfo);
	private : static int clipPolygon(Vector3 quad,const std::vector<Vector3>& polygon, std::vector<Vector3>& clipped);
	private : static void clipLeft(float sizeX, std::vector<Vector3> &polygon, Vector3 v1, Vector3 v2);
	private : static void clipRight(float sizeX, std::vector<Vector3>& polygon, Vector3 v1, Vector3 v2);
	private : static void clipTop(float sizeY, std::vector<Vector3>& polygon, Vector3 v1, Vector3 v2);
	private : static void clipBottom(float sizeY, std::vector<Vector3> &polygon, Vector3 v1, Vector3 v2);

	
	private : static float sqrDistTriPoint(const Vector3& p, const std::vector<Vector3>& tri, Vector3& pt);

	REGISTER_CLASS_NAME(BVCFFinalCollider);

};

REGISTER_CLASS(BVCFFinalCollider,false);
//EXPORT_DYNLIB(BVCFFinalCollider);

#endif // __BVCFFINALCOLLIDER_H__
