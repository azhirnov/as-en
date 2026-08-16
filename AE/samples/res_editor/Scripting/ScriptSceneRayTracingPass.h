// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptBaseRenderPass.h"
#include "Scripting/ScriptGeomSource.h"
#include "Scripting/ScriptRayTracingPass.h"
#include "Passes/Scene.h"

namespace AE::ResEditor
{

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
		RC<IPass>  ToPass ()																__Th_OV;


	private:
		ND_ RTechInfo	_CompilePipelines (OUT PipelineName &, OUT RTShaderBindingName &,
										   OUT RC<SceneData> &, OUT ScriptRTScenePtr &)		C_Th___;
			void		_CompilePipelines2 (ScriptEnginePtr se,
											OUT PipelineName &, OUT RTShaderBindingName &)	C_Th___;
			void		_CreateRTScene (RTShaderBindingName::Ref , OUT ScriptRTScenePtr &)	C_Th___;

			void		_WithPipelineCompiler ()											C_Th___;

	// ScriptBasePass //
			void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)							__Th_OV;
	};


} // AE::ResEditor
