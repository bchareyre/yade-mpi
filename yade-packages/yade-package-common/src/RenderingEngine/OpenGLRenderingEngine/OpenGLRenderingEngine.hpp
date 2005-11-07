/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef OPENGLRENDERINGENGINE_HPP
#define OPENGLRENDERINGENGINE_HPP

#include <yade/yade-core/RenderingEngine.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/MetaDispatchingEngine1D.hpp>

#include "GLDrawBoundingVolumeFunctor.hpp"
#include "GLDrawInteractionGeometryFunctor.hpp"
#include "GLDrawGeometricalModelFunctor.hpp"
#include "GLDrawShadowVolumeFunctor.hpp"

class OpenGLRenderingEngine : public RenderingEngine
{	
	public : // FIXME - why public ?
		Vector3r	 lightPos;
		bool		 drawBoundingVolume
				,drawInteractionGeometry
				,drawGeometricalModel
				,castShadow
				,drawShadowVolumes
				,useFastShadowVolume
				,drawWireFrame
				,drawInside
		
				,needInit;
	
	private :
		DynLibDispatcher< BoundingVolume    , GLDrawBoundingVolumeFunctor, void , TYPELIST_1(const shared_ptr<BoundingVolume>&) > boundingVolumeDispatcher;
		DynLibDispatcher< InteractingGeometry , GLDrawInteractionGeometryFunctor, void , TYPELIST_2(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&) >interactionGeometryDispatcher;
		DynLibDispatcher< GeometricalModel  , GLDrawGeometricalModelFunctor, void , TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool) > geometricalModelDispatcher;
		DynLibDispatcher< GeometricalModel  , GLDrawShadowVolumeFunctor, void , TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, const Vector3r& ) > shadowVolumeDispatcher;

		vector<vector<string> >  boundingVolumeFunctorNames;
		vector<vector<string> >  interactionGeometryFunctorNames;
		vector<vector<string> >  geometricalModelFunctorNames;
		vector<vector<string> >  shadowVolumeFunctorNames;

	public :
		void addBoundingVolumeFunctor(const string& str1,const string& str2);
		void addInteractionGeometryFunctor(const string& str1,const string& str2);
		void addGeometricalModelFunctor(const string& str1,const string& str2);
		void addShadowVolumeFunctor(const string& str1,const string& str2);
			
		OpenGLRenderingEngine();
		virtual ~OpenGLRenderingEngine();
	
		void init();
		void render(const shared_ptr<MetaBody>& body);
	
	private :
		void renderGeometricalModel(const shared_ptr<MetaBody>& rootBody);
		void renderBoundingVolume(const shared_ptr<MetaBody>& rootBody);
		void renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody);
		void renderShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos);
		void renderSceneUsingShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos);
		void renderSceneUsingFastShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r lightPos);
	
	protected :
		void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(OpenGLRenderingEngine);
	REGISTER_BASE_CLASS_NAME(RenderingEngine);
};

REGISTER_SERIALIZABLE(OpenGLRenderingEngine,false);

#endif // OPENGLRENDERINGENGINE_HPP
