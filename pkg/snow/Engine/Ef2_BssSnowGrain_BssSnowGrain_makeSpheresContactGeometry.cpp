// ÂŠ 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// ÂŠ 2004 Janek Kozicki <cosurgi@berlios.de>
// ÂŠ 2007 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// ÂŠ 2008 VĂĄclav Ĺ milauer <eudoxos@arcig.cz>

#include"Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-snow/BssSnowGrain.hpp>
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
//	bool result = g.go(cm1,cm2,se31,se32,c);
////	std::cerr << "-------------------1a\n";
//	return result;

	BssSnowGrain* s1=static_cast<BssSnowGrain*>(cm1.get()), *s2=static_cast<BssSnowGrain*>(cm2.get());
	Vector3r normal=se32.position-se31.position;
	Real penetrationDepthSq=pow((s1->radius+s2->radius),2) - normal.SquaredLength();
	if (penetrationDepthSq>0 || c->isReal)
	{
		shared_ptr<SpheresContactGeometry> scm;
		if(c->interactionGeometry) scm=YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
		else { scm=shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry()); c->interactionGeometry=scm; }

		Real penetrationDepth=s1->radius+s2->radius-normal.Normalize(); /* Normalize() works in-place and returns length before normalization; from here, normal is unit vector */
		scm->contactPoint=se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal=normal;
		scm->penetrationDepth=penetrationDepth;

		int id1 = c->getId1();
		int id2 = c->getId2();

		if(s1->depth.find(id2) == s1->depth.end())
			s1->depth[id2] = penetrationDepth;
		if(s2->depth.find(id1) == s2->depth.end())
			s2->depth[id1] = penetrationDepth;

		Real d1 = s1->depth[id2];
		Real d2 = s2->depth[id1];
		if(d1 != d2)
			std::cerr << "bad initial penetration?\n";

		penetrationDepth -= d1;
		scm->penetrationDepth=penetrationDepth;

		scm->radius1=s1->radius;
		scm->radius2=s2->radius;
		return true;
	}
	return false;
}


bool Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();

