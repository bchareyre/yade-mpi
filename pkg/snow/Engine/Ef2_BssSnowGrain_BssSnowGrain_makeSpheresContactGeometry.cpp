// ÂŠ 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// ÂŠ 2004 Janek Kozicki <cosurgi@berlios.de>
// ÂŠ 2007 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// ÂŠ 2008 VĂĄclav Ĺ milauer <eudoxos@arcig.cz>

#include"Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Omega.hpp>


Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry::Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry()
{
}

void Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry::registerAttributes()
{	
}

bool Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	bool result = g.go(cm1,cm2,se32,se32,c);
	std::cerr << "-------------------1a\n";
	return result;
}


bool Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	std::cerr << "-------------------2a\n";
	return g.goReverse(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();

