//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can just uncomment the following line
// #define CAPILLARYPHYS1
#ifdef CAPILLARYPHYS1

#include <pkg/dem/CapillaryPhys1.hpp>
#include<pkg/dem/ScGeom.hpp>

#include<core/Omega.hpp>
#include<core/Scene.hpp>

CapillaryPhys1::~CapillaryPhys1()
{
}

YADE_PLUGIN((CapillaryPhys1));



YADE_PLUGIN((Ip2_FrictMat_FrictMat_CapillaryPhys1));

void Ip2_FrictMat_FrictMat_CapillaryPhys1::go( const shared_ptr<Material>& b1 //FrictMat
					, const shared_ptr<Material>& b2 // FrictMat
					, const shared_ptr<Interaction>& interaction)
{
	ScGeom* geom = YADE_CAST<ScGeom*>(interaction->geom.get());
	if(geom)
	{
	  
		if(!interaction->phys)
		{      
 			const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
 			const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);

 			if (!interaction->phys) interaction->phys = shared_ptr<CapillaryPhys1>(new CapillaryPhys1());
			const shared_ptr<CapillaryPhys1>& contactPhysics = YADE_PTR_CAST<CapillaryPhys1>(interaction->phys);
			
			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			Real Da 	= geom->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
			Real Db 	= geom->radius2; // FIXME - as above
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;
			Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
			Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere

			contactPhysics->tangensOfFrictionAngle = std::tan(std::min(fa,fb));
			contactPhysics->kn = Kn;
			contactPhysics->ks = Ks;
			contactPhysics->computeBridge =computeDefault;
		}
	}
};

#endif //CAPILLARYPHYS1
 
