#include"FacetTopologyAnalyzer.hpp"
#include<yade/pkg-common/InteractingFacet.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>
void FacetTopologyAnalyzer::action(MetaBody* rb){
	LOG_DEBUG("Projection axis for analysis is "<<projectionAxis);
	vector<shared_ptr<VertexData> > vv;
	// minimum facet edge length (tolerance scale)
	Real minSqLen=0;
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		shared_ptr<InteractingFacet> f=dynamic_pointer_cast<InteractingFacet>(b->interactingGeometry);
		if(!f) continue;
		const Vector3r& pos=b->physicalParameters->se3.position;
		for(size_t i=0; i<3; i++){
			vv.push_back(shared_ptr<VertexData>(new VertexData(b->getId(),i,f->vertices[i]+pos,(f->vertices[i]+pos).Dot(projectionAxis))));
			minSqLen=min(minSqLen,(f->vertices[i]-f->vertices[(i+1)%3]).SquaredLength());
		}
	}
	LOG_DEBUG("Added data for "<<vv.size()<<" vertices ("<<vv.size()/3.<<" facets).");
	Real tolerance=sqrt(minSqLen)*relTolerance;
	LOG_DEBUG("Absolute tolerance is "<<tolerance);
	sort(vv.begin(),vv.end(),VertexComparator());
	size_t nVertices=vv.size(), j;
	for(size_t i=0; i<nVertices; i++){
		j=i;
		while(++j<nVertices && (vv[j]->coord-vv[i]->coord)<tolerance){
			shared_ptr<VertexData> &vi=vv[i], &vj=vv[j];
			if(abs(vi->pos[0]-vj->pos[0])<tolerance &&
				abs(vi->pos[1]-vj->pos[1])<tolerance &&
				abs(vi->pos[2]-vj->pos[2])<tolerance &&
				(vi->pos-vj->pos).SquaredLength()<tolerance*tolerance){
				// OK, these two vertices touch
				LOG_DEBUG("Vertices "<<vi->id<<"/"<<vi->vertexNo<<" and "<<vj->id<<"/"<<vj->vertexNo<<" close enough.");
				// add vertex to the nextIndetical of the one that has lower index; the one that is added will have isLowestIndex=false
				if(vi->index<vj->index){ vi->nextIdentical.push_back(vj); vj->isLowestIndex=false; }
				else{                    vj->nextIdentical.push_back(vi); vi->isLowestIndex=false; }
			}
		}
	}
	/* TODO: create vertex clusters of touching vertices; find what facets they belong to; find whether facets that touch have common edges; compute mutual angle between faces on edge and save that to InteractingFacet */
}

