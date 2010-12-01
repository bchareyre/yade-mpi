// © 2010 Václav Šmilauer <eu@doxos.eu>
#ifdef YADE_SUBDOMAINS

#include<yade/pkg/common/SubdomainOptimizer.hpp>
#include<yade/pkg/dem/Clump.hpp>

YADE_PLUGIN((SubdomainOptimizer));
CREATE_LOGGER(SubdomainOptimizer);

#ifdef YADE_OPENMP

void SubdomainOptimizer::updateSceneBbox(){
	Real inf=std::numeric_limits<Real>::infinity();
	mn=Vector3r(inf,inf,inf); mx=Vector3r(-inf,-inf,-inf);
	nParticles=0;
	if(!scene->isPeriodic){ FOREACH(const shared_ptr<Body>& b, *scene->bodies){ if(!b || !b->state) continue; nParticles++; mn=mn.cwise().min(b->state->pos); mx=mx.cwise().max(b->state->pos); } } 
	else { mn=Vector3r::Zero(); mx=scene->cell->getSize(); }
}

void SubdomainOptimizer::initializeSplitPlanes(){
	// split planes for subdomains...
	int nSubdom(scene->bodies->maxSubdomains);
	assert(nSubdom>1); // the case ==1 should have been caught in ::action already
	switch(nSubdom){
		case 2: div="a"; break;
		case 3: div="aa"; break;
		case 4: div="ab"; break;
		case 5: div="aaaa"; break;
		case 6: div="aab"; break;
		case 7: div="aaaaaa"; break;
		case 8: div="abc"; break;
		default:
			throw std::runtime_error(("SubdomainOptimizer: unsupported number of subdomains ("+lexical_cast<string>(nSubdom)+"), currently only 1…8 is possible.").c_str());
	}
	Vector3r axesExtents(mx-mn);
	string axesBySizeStr;
	if(axesOrder.empty()){
		Vector3i axesBySize; // axes sorted by their extents
		// 2nd coeff always 1 for Vector3r
		axesExtents.maxCoeff(&axesBySize[0],&axesBySize[1]); axesExtents.minCoeff(&axesBySize[2],&axesBySize[1]);
		axesBySize[1]=(axesBySize[0]+axesBySize[2]==3 ? 0 : (axesBySize[0]+axesBySize[2]==1 ? 2 : 1)); // middle extents
		LOG_TRACE("axesBySize="<<axesBySize);
		for(size_t i=0; i<3; i++){ axesBySizeStr.push_back(axesBySize[i]==0 ? 'x' : (axesBySize[i]==1 ? 'y' : 'z')); }
	} else {
		if(axesOrder.size()!=3 || axesOrder.find('x')==string::npos || axesOrder.find('y')==string::npos || axesOrder.find('z')==string::npos){
			throw std::invalid_argument(("SubdomainOptimizer: axesOrder must be a combination of 'x', 'y', 'z' characters (not '"+axesOrder+"')").c_str());
		}
		axesBySizeStr=axesOrder;
	}
	string divXYZ; // div with a,b,c replaced by longest, middle and shortest axis (as x,y,z)
	for(size_t i=0; i<div.size(); i++) divXYZ.push_back(div[i]=='a' ? axesBySizeStr[0] : (div[i]=='b' ? axesBySizeStr[1] : (div[i]=='c' ? axesBySizeStr[2] : div[i]))); 
	vector<int> divNum; for(size_t i=0; i<divXYZ.size(); i++) divNum.push_back(divXYZ[i]=='x' ? 0 : (divXYZ[i]=='y' ? 1 : 2));
	Vector3i axCount(Vector3i::Zero()); // how many times we split along each axis (to know how to divide the extents)
	vector<int> axNth; int nthAxis=0;
	for(size_t i=0; i<divNum.size(); i++){
		if(axCount[divNum[i]]>0 && divNum[i-1]!=divNum[i]) throw std::runtime_error("Axes must not be interspersed in the div specification!");
		if(i>0 && divNum[i]!=divNum[i-1]) nthAxis++;
		axCount[divNum[i]]+=1;
		axNth.push_back(nthAxis);
	}
	for(size_t i=0; i<div.size(); i++) LOG_TRACE(i<<": div="<<div[i]<<" divXYZ="<<divXYZ[i]<<" divNum="<<divNum[i]<<" axNth="<<axNth[i]);
	LOG_TRACE("axesExtents="<<axesExtents<<", axesBySizeStr="<<axesBySizeStr<<", axCount="<<axCount);
	splits.clear();
	Vector3i axCount2(Vector3i::Zero()); // how many splits have we already made along this axis, to know which division to take
	for(size_t i=0; i<divNum.size(); i++){
		int ax=divNum[i];
		axCount2[ax]++;
		splits.push_back(SplitPlane(ax,mn[ax]+axCount2[ax]*axesExtents[ax]/(axCount[ax]+1),pow(2,axNth[i]),/*linConcentration*/nParticles/axesExtents[ax],/*axCount*/axCount[ax]+1,/*axNth*/axCount2[ax]));
		LOG_DEBUG("Added split plane #"<<i<<", split at "<<(ax==0?"x":(ax==1?"y":"z"))<<"="<<splits.rbegin()->lim<<", bump "<<splits.rbegin()->add<<", concentration "<<splits.rbegin()->concentration);
	}

	LOG_INFO("Simulation bounding corners are min="<<mn<<", max="<<mx<<", extents="<<axesExtents);
	LOG_INFO("Established "<<nSubdom<<" subdomains, with split specification "<<div<<"="<<divXYZ<<".");
}

void SubdomainOptimizer::adjustSplitPlanes(){
	assert(nParticles>0);
	FOREACH(SplitPlane& sp, splits){
		assert(sp.aboveSplit>=0);
		int dist=sp.aboveSplit-(sp.axNth*1./sp.axCount)*nParticles; // by how many particles to shift to get in the median position
		// concentration correction, sp.dist is the previous distance value
		if(sp.dist>50){
			// 1.0 could be replaced by some inertia parameter that would prevent adjust to fluctuations too fast
			Real factor=1.0*(sp.dist-dist)/sp.dist;
			// prevent concentration from droping too fast (to zero, for instance); the .3 is tunable, again
			sp.concentration*=max(.3,factor);
		}
		sp.lim+=dist/sp.concentration;
		LOG_DEBUG("SplitPlane shifted by "<<dist/sp.concentration<<" to "<<(sp.ax==0?"x":(sp.ax==1?"y":"z"))<<"="<<sp.lim<<", being "<<dist<<" off ideal (nParticles="<<nParticles<<", aboveSplit="<<sp.aboveSplit<<"), concentration "<<sp.concentration);
		sp.dist=dist;
		sp.aboveSplit=0;
	}
}

void SubdomainOptimizer::action(){
	// check before we run that we are not useless
	const int nSubdom(scene->bodies->maxSubdomains);
	if(nSubdom==1){
		LOG_INFO("No subdomain created, since only one could be created; killing myself (dead=true).");
		scene->bodies->clearSubdomains();
		// FOREACH(const shared_ptr<Body>& b, *scene->bodies){ if(!b) continue; b->subDomId=Body::ID_NONE; } // not necessary
		dead=true;
		return;
	}

	// initialize, if we run for the first time
	if(div.empty()){
		// 1. determine extents of the whole simulation, based on centroid positions
		updateSceneBbox();
		// 2a. determine bounding boxes (might have infinite bounds) for each subdomain
		initializeSplitPlanes();
	}
	// 2b. adjust subdomains to be in equilibrium
	else adjustSplitPlanes();

	// 3. re-initialize empty subdomains, so that we can put the bodies there anew
	scene->bodies->setupSubdomains();

	// 4. loop over particles, assign to subdomains, colorize if desired
	const bool isPeriodic(scene->isPeriodic);
	const Cell& cell=*(scene->cell);
	const Vector3r colors[8]={Vector3r(1,0,0), Vector3r(0,1,0), Vector3r(0,0,1), Vector3r(.7,.7,0), Vector3r(.7,0,.7), Vector3r(0,.7,.7), Vector3r(.3,.5,.8), Vector3r(.57,.57,.57) };
	nParticles=0;
	vector<int> nInSubdom(nSubdom,0);
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || !b->state) continue;
		nParticles++;
		if(b->isClumpMember()) continue;
		int subDom=0;
		Vector3r pos=(isPeriodic ? cell.wrapPt(cell.unshearPt(b->state->pos)) : b->state->pos);
		FOREACH(SplitPlane& sp, splits) if(b->state->pos[sp.ax]>sp.lim) {
			subDom+=sp.add; sp.aboveSplit++;
			if(b->isClump()) sp.aboveSplit+=static_pointer_cast<Clump>(b->shape)->members.size(); // count clumps above the limit as well
		}
		//assert(subDom<nSubdom);
		if(subDom>=nSubdom) subDom=nSubdom-1;
		scene->bodies->setBodySubdomain(b,subDom);
		nInSubdom[subDom]++;
		if(b->shape && colorize) b->shape->color=colors[subDom];
		if(b->isClump()){
			FOREACH(Clump::MemberMap::value_type& c, static_pointer_cast<Clump>(b->shape)->members){
				const shared_ptr<Body>& member=(*scene->bodies)[c.first];
				scene->bodies->setBodySubdomain(member,subDom);
				nInSubdom[subDom]++;
				if(colorize && member->shape) member->shape->color=colors[subDom];
			}
		}
	}

	// spit out some information
	string splitInfo, subdomInfo;
	FOREACH(const SplitPlane& sp, splits) splitInfo+=string(" [")+(sp.ax==0?"x":(sp.ax==1?"y":"z"))+"="+lexical_cast<string>(sp.lim)+", "+lexical_cast<string>(nParticles-sp.aboveSplit)+"/"+lexical_cast<string>(sp.aboveSplit)+"]";
	for(int i=0; i<nSubdom; i++) subdomInfo+=" #"+lexical_cast<string>(i)+" ("+lexical_cast<string>(nInSubdom[i])+")";
	LOG_DEBUG("Split planes: "<<splitInfo);
	LOG_INFO("Subdomains: "<<subdomInfo);
}

#else
void SubdomainOptimizer::action(){
	LOG_INFO("Compiled without OpenMP support, no subdomains will be created; killing myself.");
	dead=true;
}
#endif

#endif /* YADE_SUBDOMAINS */
