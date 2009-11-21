// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/core/State.hpp>
#include<boost/foreach.hpp>
#include<stdexcept>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

void State::setDOFfromVector3r(Vector3r disp,Vector3r rot){
	blockedDOFs=((disp[0]==1.0)?DOF_X :0)|((disp[1]==1.0)?DOF_Y :0)|((disp[2]==1.0)?DOF_Z :0)|
		((rot [0]==1.0)?DOF_RX:0)|((rot [1]==1.0)?DOF_RY:0)|((rot [2]==1.0)?DOF_RZ:0);
}

std::vector<std::string> State::blockedDOFs_vec_get() const {
	std::vector<std::string> ret;
	#define _SET_DOF(DOF_ANY,str) if((blockedDOFs & State::DOF_ANY)!=0) ret.push_back(str);
	_SET_DOF(DOF_X,"x"); _SET_DOF(DOF_Y,"y"); _SET_DOF(DOF_Z,"z"); _SET_DOF(DOF_RX,"rx"); _SET_DOF(DOF_RY,"ry"); _SET_DOF(DOF_RZ,"rz");
	#undef _SET_DOF
	return ret;
}
void State::blockedDOFs_vec_set(const std::vector<std::string>& dofs){
	FOREACH(const std::string s, dofs){
		#define _GET_DOF(DOF_ANY,str) if(s==str) { blockedDOFs|=State::DOF_ANY; continue; }
		_GET_DOF(DOF_X,"x"); _GET_DOF(DOF_Y,"y"); _GET_DOF(DOF_Z,"z"); _GET_DOF(DOF_RX,"rx"); _GET_DOF(DOF_RY,"ry"); _GET_DOF(DOF_RZ,"rz");
		#undef _GET_DOF
		throw std::invalid_argument("Invalid  DOF specification `"+s+"', must be ∈{x,y,z,rx,ry,rz}.");
	}
}

