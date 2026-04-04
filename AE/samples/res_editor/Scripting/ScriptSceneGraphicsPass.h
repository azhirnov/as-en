// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptBaseRenderPass.h"
#include "Scripting/ScriptGeomSource.h"
#include "Scripting/ScriptRayTracingPass.h"
#include "Passes/Scene.h"

namespace AE::ResEditor
{

	//
	// Scene Graphics Subpass
	//
	class ScriptSceneGraphicsSubpass final : public ScriptBaseRenderPass
	{
		friend class ScriptScene;
		friend class ScriptSceneGraphicsPass;

	// types
	private:
		using PipelinePaths_t			= Array< Path >;
		using UniquePipelines_t			= FlatHashSet< Path, PathHasher >;
		using EDebugMode				= IPass::EDebugMode;
		using PipelineNames_t			= ScriptGeomSource::PipelineNames_t;
		using PipelinesPerInstance_t	= Array< PipelineNames_t >;
		using ShadingRate				= SceneGraphicsSubpass::ShadingRate;


	// variables
	private:
		ScriptScenePtr			_scene;
		String					_passName;

		PipelinePaths_t			_pipelines;
		UniquePipelines_t		_uniquePplns;

		ERenderLayer			_renderLayer	= ERenderLayer::Opaque;
		ShadingRate				_shadingRate;

		PipelinesPerInstance_t	_pplnPerInst;	// \__ temporary data
		String					_dslName;		// /


	// methods
	private:
		ScriptSceneGraphicsSubpass ()														__Th___ {}

		RC<IPass>  ToPass ()																__Th_OV	{ return null; }	// unused

		ND_ RC<SceneGraphicsSubpass>  _ToPass2 (SceneGraphicsPass &)						__Th___;
	};



	//
	// Scene Graphics Pass
	//
	class ScriptSceneGraphicsPass final : public ScriptBaseRenderPass
	{
		friend class ScriptScene;

	// types
	private:
		using Subpasses_t				= Array< ScriptSceneGraphicsSubpassPtr >;
		using PipelinePaths_t			= ScriptSceneGraphicsSubpass::PipelinePaths_t;
		using UniquePipelines_t			= ScriptSceneGraphicsSubpass::UniquePipelines_t;
		using EDebugMode				= IPass::EDebugMode;
		using PipelineNames_t			= ScriptGeomSource::PipelineNames_t;
		using ShadingRate				= SceneGraphicsSubpass::ShadingRate;


	// variables
	private:
		ScriptScenePtr			_scene;
		String					_subpassName;
		const String			_passName;

		Subpasses_t				_subpasses;

		// will be copied to subpass
		PipelinePaths_t			_pipelines;
		UniquePipelines_t		_uniquePplns;

		ERenderLayer			_renderLayer	= ERenderLayer::Opaque;
		ShadingRate				_shadingRate;


	// methods
	private:
		ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName)				__Th___;

	public:
		ScriptSceneGraphicsPass () = delete;

		void  AddPipeline (const String &pplnFile)											__Th___;
		void  AddPipelines (const String &pplnsFolder)										__Th___;

		void  SetLayer (ERenderLayer layer)													__Th___;

		void  NextSubpass1 ()																__Th___;
		void  NextSubpass2 (const String &passName)											__Th___;

		void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp, EShadingRateCombinerOp) __Th___;	// TODO: remove

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																__Th_OV;


	private:
		ND_ RTechInfo	_CompilePipelines (OUT RC<SceneData>&)								C_Th___;
			static void	_CompilePipelines2 (ScriptEnginePtr, ArrayView<Output>,
											ArrayView<ScriptSceneGraphicsSubpassPtr>,
											const ScriptScene &scene,
											const ScriptDynamicDim* dynamicDim)				__Th___;

		ND_ static auto  _CreateUBType ()													__Th___;

		void  _MoveTo (OUT ScriptSceneGraphicsSubpass &dst)									__NE___;

		void  _WithPipelineCompiler ()														C_Th___;
		void  _SetDynamicDimension2 (const ScriptDynamicDimPtr &value)						__Th___	{ ScriptBasePass::_SetDynamicDimension( value ); }

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								__Th_OV;
	};


} // AE::ResEditor
