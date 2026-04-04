// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptBaseRenderPass.h"
#include "Scripting/ScriptGeomSource.h"
#include "Scripting/ScriptRayTracingPass.h"
#include "Passes/Scene.h"

namespace AE::ResEditor
{

	//
	// Scene Ray Query Pass
	//
	class ScriptSceneRayQueryPass final : public ScriptBasePass
	{
		friend class ScriptScene;

	// types
	private:
		using EDebugMode	= IPass::EDebugMode;
		using Iterations_t	= ScriptComputePass::Iterations_t;


	// variables
	private:
		ScriptScenePtr		_scene;
		const String		_passName;
		Path				_pplnPath;

		uint3				_localSize		{0};
		uint				_subgroupSize	= 0;
		Iterations_t		_iterations;


	// methods
	private:
		ScriptSceneRayQueryPass (ScriptScenePtr scene, const String &passName)				__Th___;

	public:
		ScriptSceneRayQueryPass () = delete;

		void  SetPipeline (const String &pplnFile)											__Th___;
		void  SetPipelineWithDefine (const String &def)										__Th___;

		void  LocalSize1  (uint x)															__Th___	{ return LocalSize3v({ x, 1u, 1u }); }
		void  LocalSize2  (uint x, uint y)													__Th___	{ return LocalSize3v({ x, y, 1u }); }
		void  LocalSize3  (uint x, uint y, uint z)											__Th___	{ return LocalSize3v({ x, y, z }); }
		void  LocalSize2v (const packed_uint2 &v)											__Th___	{ return LocalSize3v({ v, 1u }); }
		void  LocalSize3v (const packed_uint3 &v)											__Th___;

		void  SubgroupSize (uint value)														__Th___	{ _subgroupSize = value; }
		void  MinSubgroupSize (uint value)													__Th___;

		void  DispatchGroups1  (uint groupCountX)											__Th___	{ return DispatchGroups3v({ groupCountX, 1u, 1u }); }
		void  DispatchGroups2  (uint groupCountX, uint groupCountY)							__Th___	{ return DispatchGroups3v({ groupCountX, groupCountY, 1u }); }
		void  DispatchGroups3  (uint groupCountX, uint groupCountY, uint groupCountZ)		__Th___	{ return DispatchGroups3v({ groupCountX, groupCountY, groupCountZ }); }
		void  DispatchGroups2v (const packed_uint2 &groupCount)								__Th___	{ return DispatchGroups3v({ groupCount, 1u }); }
		void  DispatchGroups3v (const packed_uint3 &groupCount)								__Th___;
		void  DispatchGroupsDS (const ScriptDynamicDimPtr &ds)								__Th___;
		void  DispatchGroups1D (const ScriptDynamicUIntPtr &dyn)							__Th___;
		void  DispatchGroups2D (const ScriptDynamicUInt2Ptr &dyn)							__Th___;
		void  DispatchGroups3D (const ScriptDynamicUInt3Ptr &dyn)							__Th___;

		void  DispatchThreads1  (uint threadsX)												__Th___	{ return DispatchThreads3v({ threadsX, 1u, 1u }); }
		void  DispatchThreads2  (uint threadsX, uint threadsY)								__Th___	{ return DispatchThreads3v({ threadsX, threadsY, 1u }); }
		void  DispatchThreads3  (uint threadsX, uint threadsY, uint threadsZ)				__Th___	{ return DispatchThreads3v({ threadsX, threadsY, threadsZ }); }
		void  DispatchThreads2v (const packed_uint2 &threads)								__Th___	{ return DispatchThreads3v({ threads, 1u }); }
		void  DispatchThreads3v (const packed_uint3 &threads)								__Th___;
		void  DispatchThreadsDS (const ScriptDynamicDimPtr &ds)								__Th___;
		void  DispatchThreads1D (const ScriptDynamicUIntPtr &dyn)							__Th___;
		void  DispatchThreads2D (const ScriptDynamicUInt2Ptr &dyn)							__Th___;
		void  DispatchThreads3D (const ScriptDynamicUInt3Ptr &dyn)							__Th___;

		void  DispatchGroupsIndirect1 (const ScriptBufferPtr &ibuf)							__Th___;
		void  DispatchGroupsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset)			__Th___;
		void  DispatchGroupsIndirect3 (const ScriptBufferPtr &ibuf, const String &field)	__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																__Th_OV;


	private:
		ND_ auto  _CompilePipeline (OUT RC<SceneData> &, OUT ScriptRTScenePtr &)			C_Th___;
			void  _CompilePipeline2 (ScriptEnginePtr)										C_Th___;
			void  _CompilePipeline3 (const String &cs, uint line, const String &pplnName,
									 uint shaderOpts, EPipelineOpt pplnOpt)					C_Th___;
			void  _CreateRTScene (OUT ScriptRTScenePtr &)									C_Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								__Th_OV;
	};


} // AE::ResEditor
