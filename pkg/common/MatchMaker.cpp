// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include <pkg/common/MatchMaker.hpp>

YADE_PLUGIN((MatchMaker));

Real MatchMaker::operator()(int id1, int id2, Real val1, Real val2) const {
	const int minId = std::min(id1, id2);
	const int maxId = std::max(id2, id2);
	
	const auto foundMatchItem = matchSet.find (std::make_pair(minId, maxId));
	
	if ( foundMatchItem != matchSet.end() ) {
		return foundMatchItem->second;
	} else {
		if(fbNeedsValues && (std::isnan(val1) || std::isnan(val2))) {
			throw std::invalid_argument("MatchMaker: no match for ("+boost::lexical_cast<string>(id1)+
			","+boost::lexical_cast<string>(id2)+"), and values required for algo computation '"+
			algo+"' not specified.");
		}
		return computeFallback(val1,val2);
	}
}

void MatchMaker::postLoad(MatchMaker&){
	if(algo=="val")      { fbPtr=&MatchMaker::fbVal; fbNeedsValues=false; }
	else if(algo=="zero"){ fbPtr=&MatchMaker::fbZero;fbNeedsValues=false; } 
	else if(algo=="avg") { fbPtr=&MatchMaker::fbAvg; fbNeedsValues=true;  }
	else if(algo=="min") { fbPtr=&MatchMaker::fbMin; fbNeedsValues=true;  }
	else if(algo=="max") { fbPtr=&MatchMaker::fbMax; fbNeedsValues=true;  }
	else if(algo=="harmAvg") { fbPtr=&MatchMaker::fbHarmAvg; fbNeedsValues=true; }
	else throw std::invalid_argument("MatchMaker:: algo '"+algo+"' not recognized (possible values: val, avg, min, max, harmAvg).");

	// Fill matchSet with values for a fast coefficient search
	for (const auto & m : matches) {
		const int minId = std::min((int)m[0], (int)m[1]);
		const int maxId = std::max((int)m[0], (int)m[1]);
		const auto idPair = std::make_pair(minId, maxId);
		matchSet.insert(std::make_pair(idPair, m[2]));
	}
}

Real MatchMaker::computeFallback(Real v1, Real v2) const { return (this->*MatchMaker::fbPtr)(v1,v2); }
