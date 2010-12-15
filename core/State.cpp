// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/core/State.hpp>
#include<boost/foreach.hpp>
#include<stdexcept>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

CREATE_LOGGER(State);

void State::setDOFfromVector3r(Vector3r disp,Vector3r rot){
	blockedDOFs=((disp[0]==1.0)?DOF_X :0)|((disp[1]==1.0)?DOF_Y :0)|((disp[2]==1.0)?DOF_Z :0)|
		((rot [0]==1.0)?DOF_RX:0)|((rot [1]==1.0)?DOF_RY:0)|((rot [2]==1.0)?DOF_RZ:0);
}

std::string State::blockedDOFs_vec_get() const {
	std::string ret;
	#define _SET_DOF(DOF_ANY,ch) if((blockedDOFs & State::DOF_ANY)!=0) ret.push_back(ch);
	_SET_DOF(DOF_X,'x'); _SET_DOF(DOF_Y,'y'); _SET_DOF(DOF_Z,'z'); _SET_DOF(DOF_RX,'X'); _SET_DOF(DOF_RY,'Y'); _SET_DOF(DOF_RZ,'Z');
	#undef _SET_DOF
	return ret;
}

#ifdef YADE_DEPREC_DOF_LIST
// support deprecated syntax, later take std::string arg directly (more efficient)
void State::blockedDOFs_vec_set(const python::object& dofs0){
	python::extract<std::string> dofStr(dofs0);
	python::extract<std::vector<std::string> > dofLst(dofs0);
	blockedDOFs=0;
	if(dofStr.check()){
		string dofs(dofStr());
#else
	void State::blockedDOFs_vec_set(const std::string& dofs){
		blockedDOFs=0;
#endif
		FOREACH(char c, dofs){
			#define _GET_DOF(DOF_ANY,ch) if(c==ch) { blockedDOFs|=State::DOF_ANY; continue; }
			_GET_DOF(DOF_X,'x'); _GET_DOF(DOF_Y,'y'); _GET_DOF(DOF_Z,'z'); _GET_DOF(DOF_RX,'X'); _GET_DOF(DOF_RY,'Y'); _GET_DOF(DOF_RZ,'Z');
			#undef _GET_DOF
			throw std::invalid_argument("Invalid  DOF specification `"+lexical_cast<string>(c)+"' in '"+dofs+"', characters must be ∈{x,y,z,X,Y,Z}.");
		}
#ifdef YADE_DEPREC_DOF_LIST
	} else 
	if(dofLst.check()){
		LOG_WARN("Specifying State.blockedDOFs as list ['x','y','rx','rz'] is deprecated, use string 'xyXZ' instead.");
		FOREACH(const std::string s, dofLst()){
			#define _GET_DOF(DOF_ANY,str) if(s==str) { blockedDOFs|=State::DOF_ANY; continue; }
			_GET_DOF(DOF_X,"x"); _GET_DOF(DOF_Y,"y"); _GET_DOF(DOF_Z,"z"); _GET_DOF(DOF_RX,"rx"); _GET_DOF(DOF_RY,"ry"); _GET_DOF(DOF_RZ,"rz");
			#undef _GET_DOF
			throw std::invalid_argument("Invalid  DOF specification `"+s+"', must be ∈{x,y,z,rx,ry,rz}.");
		}
	} else {
		throw std::invalid_argument("Invalid type: State.blockedDOFs must be either list of strings or string.");
	}
#endif
}
