/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Body.hpp"

#include <limits>
#include "Scene.hpp"
#include "Omega.hpp"

//! This could be -1 if id_t is re-typedef'ed as `int'
const Body::id_t Body::ID_NONE=Body::id_t(-1);

const shared_ptr<Body>& Body::byId(Body::id_t _id, Scene* rb){return (*((rb?rb:Omega::instance().getScene().get())->bodies))[_id];}
const shared_ptr<Body>& Body::byId(Body::id_t _id, shared_ptr<Scene> rb){return (*(rb->bodies))[_id];}

Body::Body(Body::id_t newId, int newGroup): id(newId), groupMask(newGroup), state(shared_ptr<State>(new State)), clumpId(ID_NONE){}

