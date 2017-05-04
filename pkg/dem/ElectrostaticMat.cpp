// 2016 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#include"ElectrostaticMat.hpp"


YADE_PLUGIN((ElectrostaticMat)(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys)(ElectrostaticPhys)(Law2_ScGeom_ElectrostaticPhys))


ElectrostaticPhys::ElectrostaticPhys(const FrictPhys & obj) : FrictPhys(obj), DebyeLength(1e-6), InterConst(1e-10), A(1e-19)
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

        if(DebyeLength == 0)
        {
            Real sc(0);
            Temp += 273.15; // to Kelvin

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
                const Real kB(1.38064852e-23); /* J/K */
                const Real nA(6.02214086e26); /* 1/kg */
                const Real e(1.60217662e-19); /* A.s */
                const Real VacPerm(8.854187817e-12); /* F/m */


                Real tmp = RelPerm*VacPerm*kB*Temp/(e*e*nA*sc);
                phys->DebyeLength = pow(tmp,-0.5);
            }

            //std::cout << "Debye length^-1: " << phys->DebyeLength << std::endl;

            phys->InterConst = 1e-15*Temp*Temp*pow(tanh(SurfCharge/(0.3456*Temp)),2);
            phys->A = A;

            //std::cout << "K: " << phys->InterConst << std::endl;
        }
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
        Real K(phys->DebyeLength);
        Vector3r& normalForce(phys->normalForce);


        /* constitutive law */
        Real DLEF(phys->DebyeLength*phys->InterConst); // Double Layer Electrostatic Force
        Real VdW(0.);

        if(geom->penetrationDepth <= 0.)
        {
            DLEF*=exp(-K*(r-a1-a2));
            VdW = -phys->A/(6*pow(r-a1-a2,2));
        }

        normalForce = (DLEF+VdW)*a1*a2/(a1+a2)*geom->normal;

	if (!scene->isPeriodic) {
                applyForceAtContactPoint(normalForce, geom->contactPoint , id1, s1->se3.position, id2, s2->se3.position);
	} else {
                scene->forces.addForce(id1,normalForce);
                scene->forces.addForce(id2,-normalForce);
	}
	return true;
}
