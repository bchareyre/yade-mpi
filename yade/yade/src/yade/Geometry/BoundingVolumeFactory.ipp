

inline void BoundingVolumeFactory::operator() (const shared_ptr<CollisionGeometry> cm, const Se3r& se3, shared_ptr<BoundingVolume> bv)
{
	buildBoundingVolume(cm, se3, bv);
}
