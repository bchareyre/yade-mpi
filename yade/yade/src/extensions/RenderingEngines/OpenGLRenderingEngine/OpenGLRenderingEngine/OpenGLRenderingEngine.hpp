/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OPRNGLRENDERINGENGINE_H__
#define __OPRNGLRENDERINGENGINE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "RenderingEngine.hpp"
#include "DynLibDispatcher.hpp"

#include "GLDrawBoundingVolumeFunctor.hpp"
#include "GLDrawInteractionGeometryFunctor.hpp"
#include "GLDrawGeometricalModelFunctor.hpp"
#include "GLDrawShadowVolumeFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class OpenGLRenderingEngine : public RenderingEngine
{	
	public : Vector3r lightPos;
	public : bool drawBoundingVolume;
	public : bool drawInteractionGeometry;
	public : bool drawGeometricalModel;
	public : bool castShadow;
	public : bool drawShadowVolumes;
	public : bool useFastShadowVolume;
	
	private : DynLibDispatcher< BodyBoundingVolume    , GLDrawBoundingVolumeFunctor, void , TYPELIST_1(const shared_ptr<BodyBoundingVolume>&) > boundingVolumeDispatcher;
	private : DynLibDispatcher< BodyInteractionGeometry , GLDrawInteractionGeometryFunctor, void , TYPELIST_2(const shared_ptr<BodyInteractionGeometry>&, const shared_ptr<BodyPhysicalParameters>&) >interactionGeometryDispatcher;
	private : DynLibDispatcher< BodyGeometricalModel  , GLDrawGeometricalModelFunctor, void , TYPELIST_2(const shared_ptr<BodyGeometricalModel>&, const shared_ptr<BodyPhysicalParameters>&) > geometricalModelDispatcher;
	private : DynLibDispatcher< BodyGeometricalModel  , GLDrawShadowVolumeFunctor, void , TYPELIST_3(const shared_ptr<BodyGeometricalModel>&, const shared_ptr<BodyPhysicalParameters>&, const Vector3r& ) > shadowVolumeDispatcher;

	private : vector<vector<string> >  boundingVolumeFunctorNames;
	private : vector<vector<string> >  interactionGeometryFunctorNames;
	private : vector<vector<string> >  geometricalModelFunctorNames;
	private : vector<vector<string> >  shadowVolumeFunctorNames;
	public  : void addBoundingVolumeFunctor(const string& str1,const string& str2);
	public  : void addInteractionGeometryFunctor(const string& str1,const string& str2);
	public  : void addGeometricalModelFunctor(const string& str1,const string& str2);
	public  : void addShadowVolumeFunctor(const string& str1,const string& str2);
	
	private : bool needInit;

			
	public : OpenGLRenderingEngine();
	public : ~OpenGLRenderingEngine();
	
	public : void init();
	public : void render(shared_ptr<ComplexBody> body);
	
	private : void renderGeometricalModel(shared_ptr<ComplexBody> rootBody);
	private : void renderBoundingVolume(shared_ptr<ComplexBody> rootBody);
	private : void renderShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos);
	private : void renderSceneUsingShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos);
	private : void renderSceneUsingFastShadowVolumes(shared_ptr<ComplexBody> rootBody,Vector3r lightPos);
	
	REGISTER_CLASS_NAME(OpenGLRenderingEngine);
	public : void postProcessAttributes(bool deserializing);
	
	public : void registerAttributes();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(OpenGLRenderingEngine,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __OPRNGLRENDERINGENGINE_H__
