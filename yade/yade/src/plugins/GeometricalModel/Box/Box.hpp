#ifndef __BOX_H__
#define __BOX_H__

#include "CollisionGeometry.hpp"

class Box : public CollisionGeometry
{
	public : Vector3r extents;

	// construction
	public : Box (Vector3r e);
	public : Box ();
	public : ~Box ();

	public : bool collideWith(CollisionGeometry* collisionGeometry);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();

	public : void afterDeserialization();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Box);
	REGISTER_CLASS_INDEX(Box);

};

#include "ArchiveTypes.hpp"
using namespace ArchiveTypes;

REGISTER_SERIALIZABLE(Box,false);
#endif // __BOX_H__
