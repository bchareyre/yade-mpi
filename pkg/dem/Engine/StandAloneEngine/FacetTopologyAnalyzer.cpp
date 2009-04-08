#include"FacetTopologyAnalyzer.hpp"
#include<yade/pkg-common/InteractingFacet.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>

CREATE_LOGGER(FacetTopologyAnalyzer);
YADE_PLUGIN("FacetTopologyAnalyzer");

#ifndef FACET_TOPO
void FacetTopologyAnalyzer::action(MetaBody* rb){
	throw runtime_error("FACET_TOPO was not enabled in InteractingFacet.hpp at compile-time. Do not use FacetTopologyAnalyzer or recompile.");
}
#else
void FacetTopologyAnalyzer::action(MetaBody* rb){
	commonEdgesFound=0;
	LOG_DEBUG("Projection axis for analysis is "<<projectionAxis);
	vector<shared_ptr<VertexData> > vv;
	// minimum facet edge length (tolerance scale)
	Real minSqLen=numeric_limits<Real>::infinity();
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
		while(++j<nVertices && (vv[j]->coord-vv[i]->coord)<=tolerance){
			shared_ptr<VertexData> &vi=vv[i], &vj=vv[j];
			if(abs(vi->pos[0]-vj->pos[0])<=tolerance &&
				abs(vi->pos[1]-vj->pos[1])<=tolerance &&
				abs(vi->pos[2]-vj->pos[2])<=tolerance &&
				(vi->pos-vj->pos).SquaredLength()<=tolerance*tolerance){
				// OK, these two vertices touch
				LOG_TRACE("Vertices "<<vi->id<<"/"<<vi->vertexNo<<" and "<<vj->id<<"/"<<vj->vertexNo<<" close enough.");
				// add vertex to the nextIndetical of the one that has lower index; the one that is added will have isLowestIndex=false
				if(vi->index<vj->index){ vi->nextIdentical.push_back(vj); vj->isLowestIndex=false; }
				else{                    vj->nextIdentical.push_back(vi); vi->isLowestIndex=false; }
			}
		}
	}
	// identity chains start always at lower indices, this way we get all of them
	sort(vv.begin(),vv.end(),VertexIndexComparator());
	int maxVertexId=0;
	FOREACH(shared_ptr<VertexData>& v, vv){
		if(v->vertexId<0){
			assert(v->isLowestIndex);
			v->vertexId=maxVertexId++;
		}
		FOREACH(shared_ptr<VertexData>& vNext, v->nextIdentical){
			vNext->vertexId=v->vertexId;
		}
		if(v->vertexId>=0) continue; // already assigned
	}
	LOG_DEBUG("Found "<<maxVertexId<<" unique vertices.");
	// add FacetTopology for all facets; index within the topo array is the body id
	vector<shared_ptr<FacetTopology> > topo(rb->bodies->size()); // initialized with the default ctor
	FOREACH(shared_ptr<VertexData>& v, vv){
		if(!topo[v->id]) topo[v->id]=shared_ptr<FacetTopology>(new FacetTopology(v->id));
		topo[v->id]->vertices[v->vertexNo]=v->vertexId;
	}
	// make sure all facets have their vertex id's assigned
	// add non-empty ones to topo1 that will be used for adjacency search afterwards
	vector<shared_ptr<FacetTopology> > topo1;
	for(size_t i=0; i<topo.size(); i++){
		shared_ptr<FacetTopology> t=topo[i];
		if(!t) continue;
		if(t->vertices[0]<0 || t->vertices[1]<0 || t->vertices[2]<0){
			LOG_FATAL("Facet #"<<i<<": some vertex has no integrized vertexId assigned!!");
			LOG_FATAL("Vertices are: "<<t->vertices[0]<<","<<t->vertices[1]<<","<<t->vertices[2]);
			throw logic_error("Facet vertex has no integrized vertex assigned?!");
		}
		topo1.push_back(t);
	}
	sort(topo1.begin(),topo1.end(),TopologyIndexComparator());
	size_t nTopo=topo1.size();
	for(size_t i=0; i<nTopo; i++){
		size_t j=i;
		while(++j<nTopo){
			const shared_ptr<FacetTopology>& ti(topo1[i]), &tj(topo1[j]);
			LOG_TRACE("Analyzing possibly-adjacent facets #"<<ti->id<<" #"<<tj->id<<" (vertices "<<ti->vertices[0]<<","<<ti->vertices[1]<<","<<ti->vertices[2]<<"; "<<tj->vertices[0]<<","<<ti->vertices[1]<<","<<ti->vertices[2]<<")");
			vector<size_t> vvv; // array of common vertices
			for(size_t k=0; k<3; k++){
				if     (ti->vertices[k]==tj->vertices[0]) vvv.push_back(ti->vertices[k]);
				else if(ti->vertices[k]==tj->vertices[1]) vvv.push_back(ti->vertices[k]);
				else if(ti->vertices[k]==tj->vertices[2]) vvv.push_back(ti->vertices[k]);
			}
			if(vvv.size()==0) break; // reached end of those that have the lowest-id vertex in common
			if(vvv.size()==1) continue; // only one edge in common
			assert(vvv.size()!=3); // same coords? nonsense
			assert(vvv.size()==2);
			vector<int> edge(2,0);
			// identify what edge are we at, for both facets
			for(int k=0; k<2; k++){
				for(edge[k]=0; edge[k]<3; edge[k]++){
					const shared_ptr<FacetTopology>& tt( k==0 ? ti : tj);
					size_t v1=tt->vertices[edge[k]],v2=tt->vertices[(edge[k]+1)%3];
					if((vvv[0]==v1 && vvv[1]==v2) || (vvv[0]==v2 && vvv[1]==v1)) break;
					if(edge[k]==2){
						LOG_FATAL("No edge identified for 2 vertices "<<vvv[0]<<","<<vvv[1]<<" (facet #"<<tt->id<<" is: "<<tt->vertices[0]<<","<<tt->vertices[1]<<","<<tt->vertices[2]<<")");
						throw logic_error("No edge identified for given vertices.");
					}
				}
			}
			// add adjacency information to the facet itself
			YADE_PTR_CAST<InteractingFacet>((*rb->bodies)[ti->id]->interactingGeometry)->edgeAdjIds[edge[0]]=tj->id;
			YADE_PTR_CAST<InteractingFacet>((*rb->bodies)[tj->id]->interactingGeometry)->edgeAdjIds[edge[1]]=tj->id;
			commonEdgesFound++;
			LOG_TRACE("Added adjacency information for #"<<ti->id<<"+#"<<tj->id<<" (common edges "<<edge[0]<<"+"<<edge[1]<<")");
		}
	}
}
#endif
