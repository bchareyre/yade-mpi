// 2016 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#include"ElectrostaticMat.hpp"


YADE_PLUGIN((ElectrostaticMat)(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys)(ElectrostaticPhys)(Law2_ScGeom_ElectrostaticPhys))


ElectrostaticPhys::ElectrostaticPhys(const FrictPhys & obj) : FrictPhys(obj), DebyeLength(1.e-6), InterConst(1.e-10), A(1.e-19)
{
}


CREATE_LOGGER(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys);
void Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys::go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction){
	if (interaction->phys) return;

        // Inheritance
        Ip2_FrictMat_FrictMat_FrictPhys::go(material1,material2,interaction);

        FrictPhys* ph = YADE_CAST<FrictPhys*>(interaction->phys.get());

        // Cast to Electrostatic
        shared_ptr<ElectrostaticPhys> phys(new ElectrostaticPhys(*ph));
        interaction->phys = phys;

        // Electrostatic behaviour
        const Real kB(1.38064852e-23); /* J/K Boltzmann*/
        const Real nA(6.02214086e26); /* 1/kg Avogadro*/
        const Real e(1.60217662e-19); /* A.s Electron charge*/
        const Real VacPerm(8.854187817e-12); /* F/m Permittivity of vacuum*/
        const Real pi(acos(-1));
        
        Real sc(0); // Sum of charges
        Temp += 273.15; // to Kelvin

        if(DebyeLength == 0)
        {
            for(vector<Vector2r>::const_iterator it=Ions.begin();it < Ions.end();it++)
            {
                sc += it->x()*it->x()*it->y();
            }

            if(sc == 0)
            {
                LOG_ERROR("Null sum of charge. Assuming Debye Length as 1micron (Debye Length in totaly pure water, pH=7)");
                phys->DebyeLength=1e-6;
            }
            else
            {
                Real tmp = RelPerm*VacPerm*kB*Temp/(e*e*nA*sc);
                phys->DebyeLength = pow(tmp,0.5);
            }
        }
        else
        {
            phys->DebyeLength = DebyeLength;
        }
        
        phys->A = A;
        
        if(A == 0)
            LOG_ERROR("Hamaker constant (A) is null. Van Der Waals force will not be calculated");
        
        //std::cout << "Debye length^-1: " << phys->DebyeLength << std::endl;
        if(Z == 0)
            phys->InterConst = 64.*pi*RelPerm*VacPerm*pow(kB*Temp/e*tanh(z*e*SurfCharge/(4.*kB*Temp)),2);
        else
            phys->InterConst = Z;
        
        if(phys->InterConst == 0)
            LOG_ERROR("Interaction constant is null. Set SurfCharge and Temp or Z. Double Layer Electrostatic Interaction will not be calculated.");
}


CREATE_LOGGER(ElectrostaticPhys);

ElectrostaticPhys::~ElectrostaticPhys(){};



/********************** Law2_ScGeom_ElectrostaticPhys ****************************/
CREATE_LOGGER(Law2_ScGeom_ElectrostaticPhys);
bool Law2_ScGeom_ElectrostaticPhys::go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction){


    // Inheritance
    Law2_ScGeom_FrictPhys_CundallStrack::go(iGeom,iPhys,interaction);

        // Geometric
	ScGeom* geom=static_cast<ScGeom*>(iGeom.get());
    ElectrostaticPhys* phys=static_cast<ElectrostaticPhys*>(iPhys.get());

    // Get bodies properties
	Body::id_t id1 = interaction->getId1();
 	Body::id_t id2 = interaction->getId2();
	const shared_ptr<Body> b1 = Body::byId(id1,scene);
	const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();

    // Terms in the force
    Real& a1(geom->radius1);
    Real& a2(geom->radius2);
    Real r((s1->se3.position-s2->se3.position).norm());
    Real D(r-a1-a2);
    Real K(1./phys->DebyeLength);
    Vector3r& normalForce(phys->normalForce);


    /* constitutive law */
    Real DLEF(0.); // Double Layer Electrostatic Force
    Real VdW(0.);  // Van Der Waals Force


    if(geom->penetrationDepth <= 0.)
    {
        DLEF = -phys->InterConst*K*exp(-K*D);
        VdW = phys->A/(6.*pow(D,2));

        Real f_VdW = VdW*a1*a2/(a1+a2);
        Real f_DLE = DLEF*a1*a2/(a1+a2);

        normalForce = (f_VdW + f_DLE)*geom->normal;
/*
        LOG_ERROR("DLEF: "+std::to_string(DLEF*1e10));
        LOG_ERROR("K: "+std::to_string(K));
        LOG_ERROR("Z: "+std::to_string(phys->InterConst*1e10));
        LOG_ERROR("D: "+std::to_string(D*1e10));//*/

    }
    else
    {
        normalForce = 0.*geom->normal;
    }


	if (!scene->isPeriodic) {
                applyForceAtContactPoint(normalForce, geom->contactPoint , id1, s1->se3.position, id2, s2->se3.position);
	} else {
                scene->forces.addForce(id1,normalForce);
                scene->forces.addForce(id2,-normalForce);
	}
	return true;
}
