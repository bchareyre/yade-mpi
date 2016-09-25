
/*CWBoon 2015 */
#ifdef YADE_POTENTIAL_BLOCKS
#include "PotentialBlock2AABB.hpp"
#include<pkg/dem/PotentialBlock.hpp>
#include<pkg/common/Aabb.hpp>

void PotentialBlock2AABB::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
	PotentialBlock* pp = static_cast<PotentialBlock*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }	
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	Matrix3r r = se3.orientation.toRotationMatrix();

	if(pp->AabbMinMax == false){
		Real distFromCentre = 1.05*pp->R; // std::max(maxD, pp->R);
		halfSize = Vector3r(distFromCentre,distFromCentre,distFromCentre); 
		aabb->min = se3.position-halfSize;
		aabb->max = se3.position+halfSize;
		return;
	}else{
		Matrix3r r=se3.orientation.toRotationMatrix();
		Vector3r halfSizeMin(Vector3r::Zero());Vector3r halfSizeMax(Vector3r::Zero());
		if(pp->vertices.size() ==0){
			//pp->vertices.clear();
			pp->vertices.push_back(Vector3r(pp->maxAabbRotated[0],pp->maxAabbRotated[1],pp->maxAabbRotated[2]));
			pp->vertices.push_back(Vector3r(pp->maxAabbRotated[0],pp->maxAabbRotated[1],-pp->minAabbRotated[2]));
			pp->vertices.push_back(Vector3r(-pp->minAabbRotated[0],-pp->minAabbRotated[1],pp->maxAabbRotated[2]));
			pp->vertices.push_back(Vector3r(-pp->minAabbRotated[0],-pp->minAabbRotated[1],-pp->minAabbRotated[2]));
			pp->vertices.push_back(Vector3r(-pp->minAabbRotated[0],pp->maxAabbRotated[1],pp->maxAabbRotated[2]));
			pp->vertices.push_back(Vector3r(-pp->minAabbRotated[0],pp->maxAabbRotated[1],-pp->minAabbRotated[2]));
			pp->vertices.push_back(Vector3r(pp->maxAabbRotated[0],-pp->minAabbRotated[1],pp->maxAabbRotated[2]));
			pp->vertices.push_back(Vector3r(pp->maxAabbRotated[0],-pp->minAabbRotated[1],-pp->minAabbRotated[2]));
		}
		Vector3r aabbMin(0,0,0);
		Vector3r aabbMax(0,0,0);
		for (int i=0; i<8; i++){
			Vector3r vertex = r*(pp->oriAabb.conjugate()*pp->vertices[i]);
			if(vertex.x() < aabbMin.x()){ aabbMin.x() = vertex.x(); }
			if(vertex.y() < aabbMin.y()){ aabbMin.y() = vertex.y(); }
			if(vertex.z() < aabbMin.z()){ aabbMin.z() = vertex.z(); }
			if(vertex.x() > aabbMax.x()){ aabbMax.x() = vertex.x(); }
			if(vertex.y() > aabbMax.y()){ aabbMax.y() = vertex.y(); }
			if(vertex.z() > aabbMax.z()){ aabbMax.z() = vertex.z(); }
		}
		aabb->min=  se3.position + 1.05*aabbMin;
		aabb->max = se3.position + 1.05*aabbMax;
		halfSizeMin = -1.0*aabbMin;
		halfSizeMax = 1.0*aabbMax;
	
		pp->halfSize = 1.1*Vector3r(std::max(halfSizeMin.x(),halfSizeMax.x()),std::max(halfSizeMin.y(),halfSizeMax.y()),std::max(halfSizeMin.z(),halfSizeMax.z()));
		return;
	}
	
}
	
YADE_PLUGIN((PotentialBlock2AABB));
#endif // YADE_POTENTIAL_BLOCKS
