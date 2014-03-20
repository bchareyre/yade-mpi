/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                         *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v3 or later. See file LICENSE for details. *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#pragma once
#include<yade/lib/serialization/Serializable.hpp>
#include<yade/lib/multimethods/Indexable.hpp>

class LBMnode: public Serializable{
    public:
        int
                i,                      /*! node index in x direction */
                j,                      /*! node index in y direction */
                k,                      /*! node index in z direction */
                body_id;                /*! the node belongs to the body indexed with body_id*/

        short int IsolNb;               /*! number of boundary links of a fluid boundary nodes*/

        bool
                isObstacle,             /*! the node belongs to an obstacle  */
                isObstacleBoundary,     /*! the node is an obstacle boundary  */
                isFluidBoundary,        /*! the node is a fluid boundary  */
                wasObstacle,            /*! the node was an obstacle  */
                isNewObstacle,          /*! the node is an new obstacle node   */
                isNewFluid,             /*! the node is an new fluid node   */
                applyBC,                /*! the node is subject to the one boundary condition */
                applyXmBC,              /*! the node is subject to the left boundary condition */
                applyXpBC,              /*! the node is subject to the right boundary condition */
                applyYpBC,              /*! the node is subject to the top boundary condition */
                applyYmBC,              /*! the node is subject to the bottom boundary condition */
                applyZpBC,              /*! the node is subject to the front boundary condition NOT USED NOW*/
                applyZmBC,              /*! the node is subject to the back boundary condition NOT USED NOW*/
                applyYmXmBC,            /*! the node is subject to the bottom-left boundary condition */
                applyYpXmBC,            /*! the node is subject to the top-left boundary condition */
                applyYmXpBC,            /*! the node is subject to the bottom-right boundary condition */
                applyYpXpBC;            /*! the node is subject to the top-right boundary condition */

        Vector3r    posb,               /*! the node position  */
                    velb;               /*! the node velocity  */

        Real rhob;                      /*! the node density  */                    /*! the node density  */
        vector<int> neighbour_id;       /*! list of adjacent nodes  */
        vector<int> links_id;            /*! list of links  */
        vector<Real> f;
        vector<Real> fprecol;
        vector<Real> fpostcol;

        void DispatchBoundaryConditions(int SizeNx,int SizeNy,int SizeNz);
        bool checkIsNewFluid();
        bool checkIsNewObstacle();
        void MixteBC(string lbmodel,Real density, Vector3r U, string where);
        void SetCellIndexesAndPosition(int indI, int indJ, int indK);
        void setAsObstacle(){isObstacle=true;}
        void setAsFluid(){isObstacle=false;}

    virtual ~LBMnode();
	YADE_CLASS_BASE_DOC(LBMnode,Serializable,"Node class for Lattice Boltzmann Method ");
};
REGISTER_SERIALIZABLE(LBMnode);

#endif //LBM_ENGINE
