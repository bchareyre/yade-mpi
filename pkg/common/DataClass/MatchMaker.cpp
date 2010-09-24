// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/pkg-common/MatchMaker.hpp>
#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

YADE_PLUGIN((MatchMaker));
MatchMaker::~MatchMaker(){}

Real MatchMaker::operator()(int id1, int id2, Real val1, Real val2) const {
	FOREACH(const Vector3r& m, matches){
		if(((int)m[0]==id1 && (int)m[1]==id2) || ((int)m[0]==id2 && (int)m[1]==id1)) return m[2];
	}
	// no match
	if(isnan(val1) || isnan(val2)) throw std::invalid_argument("MatchMaker: no match for ("+lexical_cast<string>(id1)+","+lexical_cast<string>(id2)+") and values for fallback computation not specified.");
	return computeFallback(val1,val2);
}

void MatchMaker::postLoad(MatchMaker&){}

Real MatchMaker::computeFallback(Real v1, Real v2) const {
	if(fallback=="val") return val;
	if(fallback=="avg") return (v1+v2)/2.;
	if(fallback=="min") return min(v1,v2);
	if(fallback=="max") return max(v1,v2);
	if(fallback=="harmAvg") return 2*v1*v2/(v1+v2);
	throw std::invalid_argument("MatchMaker:: fallback '"+fallback+"' not recognized (possible values: val, avg, min, max, harmAvg).");
}
