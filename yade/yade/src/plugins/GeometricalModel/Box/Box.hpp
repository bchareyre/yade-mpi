#ifndef __BOX_H__
#define __BOX_H__

#include "CollisionModel.hpp"

class Box : public CollisionModel
{
	public : Vector3 extents;

	// construction
	public : Box (Vector3 e);
	public : Box ();
	public : ~Box ();

	public : bool collideWith(CollisionModel* collisionModel);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Box);
	REGISTER_CLASS_INDEX(Box);

};

#include "ArchiveTypes.hpp"
using namespace ArchiveTypes;

REGISTER_SERIALIZABLE(Box,false);
#endif // __BOX_H__
