// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBaseRenderPass.h"
#include "res_editor/Scripting/ScriptGeomSource.h"
#include "res_editor/Scripting/ScriptRayTracingPass.h"
#include "res_editor/Passes/Scene.h"

namespace AE::ResEditor
{

	//
	// Scene
	//
	class ScriptScene final : public EnableScriptRC
	{
	// types
	private:
		struct GeometryInstance
		{
			ScriptGeomSourcePtr		geom;
			float4x4				transform;
		};
		using GeomInstances_t = Array< GeometryInstance >;


	// variables
	public:
		GeomInstances_t				_geomInstances;
	private:
		ScriptBaseControllerPtr		_controller;
		uint						_passCount			= 0;
		bool						_hasRayTracingPass	= false;

		RC<SceneData>				_scene;


	// methods
	public:
		ScriptScene () {}

		void  SetDebugName (const String &name)												__Th___;

		// default controller, can be overridden by pass
		void  InputController (const ScriptBaseControllerPtr &)								__Th___;

		void  InputGeometry1 (const ScriptGeomSourcePtr	&,
							  const packed_float3		&pos,
							  const packed_float3		&rotation,
							  float						scale)								__Th___;
		void  InputGeometry2 (const ScriptGeomSourcePtr	&,
							  const packed_float3		&pos)								__Th___;
		void  InputGeometry3 (const ScriptGeomSourcePtr	&)									__Th___;
		void  InputGeometry4 (const ScriptGeomSourcePtr	&,
							  const packed_float4x4		&mat)								__Th___;

		ScriptSceneGraphicsPass*	AddGraphicsPass (const String &name)					__Th___;
		ScriptSceneRayTracingPass*	AddRayTracingPass (const String &name)					__Th___;

		ND_ RC<SceneData>			 ToScene ()												__Th___;
		ND_ ScriptBaseControllerPtr  GetController ()										__Th___	{ return _controller; }

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
	};



	//
	// Scene Graphics Pass
	//
	class ScriptSceneGraphicsPass final : public ScriptBaseRenderPass
	{
		friend class ScriptScene;

	// types
	private:
		using PipelinePaths_t			= Array< Path >;
		using UniquePipelines_t			= FlatHashSet< Path, PathHasher >;
		using EDebugMode				= IPass::EDebugMode;
		using PipelineNames_t			= ScriptGeomSource::PipelineNames_t;
		using PipelinesPerInstance_t	= Array< PipelineNames_t >;


	// variables
	private:
		ScriptScenePtr			_scene;
		const String			_passName;

		PipelinePaths_t			_pipelines;
		UniquePipelines_t		_uniquePplns;

		ERenderLayer			_renderLayer	= ERenderLayer::Opaque;


	// methods
	private:
		ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName)				__Th___;

	public:
		ScriptSceneGraphicsPass () = delete;

		void  AddPipeline (const String &pplnFile)											__Th___;
		void  AddPipelines (const String &pplnsFolder)										__Th___;

		void  SetLayer (ERenderLayer layer)													__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																C_Th_OV;


	private:
		ND_ RTechInfo	_CompilePipelines (OUT PipelinesPerInstance_t &, OUT RC<SceneData> &)C_Th___;
			void		_CompilePipelines2 (ScriptEnginePtr se, OUT PipelinesPerInstance_t &)C_Th___;

		ND_ static auto  _CreateUBType ()													__Th___;

		void  _WithPipelineCompiler ()														C_Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								C_Th_OV;
	};



	//
	// Scene Ray Tracing Pass
	//
	class ScriptSceneRayTracingPass final : public ScriptBasePass
	{
		friend class ScriptScene;

	// types
	private:
		using EDebugMode	= IPass::EDebugMode;
		using Iterations_t	= ScriptRayTracingPass::Iterations_t;


	// variables
	private:
		ScriptScenePtr		_scene;
		const String		_passName;
		Path				_pipeline;
		Iterations_t		_iterations;


	// methods
	private:
		ScriptSceneRayTracingPass (ScriptScenePtr scene, const String &passName)			__Th___;

	public:
		ScriptSceneRayTracingPass () = delete;

		void  SetPipeline (const String &pplnFile)											__Th___;

		void  DispatchThreads1  (uint threadsX)												__Th___	{ return DispatchThreads3v({ threadsX, 1u, 1u }); }
		void  DispatchThreads2  (uint threadsX, uint threadsY)								__Th___	{ return DispatchThreads3v({ threadsX, threadsY, 1u }); }
		void  DispatchThreads3  (uint threadsX, uint threadsY, uint threadsZ)				__Th___	{ return DispatchThreads3v({ threadsX, threadsY, threadsZ }); }
		void  DispatchThreads2v (const packed_uint2 &threads)								__Th___	{ return DispatchThreads3v({ threads, 1u }); }
		void  DispatchThreads3v (const packed_uint3 &threads)								__Th___;
		void  DispatchThreadsDS (const ScriptDynamicDimPtr &ds)								__Th___;
		void  DispatchThreads1D (const ScriptDynamicUIntPtr &dyn)							__Th___;

		void  DispatchThreadsIndirect1 (const ScriptBufferPtr &ibuf)						__Th___;
		void  DispatchThreadsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset)			__Th___;
		void  DispatchThreadsIndirect3 (const ScriptBufferPtr &ibuf, const String &field)	__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																C_Th_OV;


	private:
		ND_ RTechInfo	_CompilePipelines (OUT PipelineName &, OUT RTShaderBindingName &,
										   OUT RC<SceneData> &, OUT ScriptRTScenePtr &)		C_Th___;
			void		_CompilePipelines2 (ScriptEnginePtr se,
											OUT PipelineName &, OUT RTShaderBindingName &)	C_Th___;
			void		_CreateRTScene (RTShaderBindingName::Ref , OUT ScriptRTScenePtr &)	C_Th___;

		ND_ static auto  _CreateUBType ()													__Th___;

		void  _WithPipelineCompiler ()														C_Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								C_Th_OV;
	};


} // AE::ResEditor
