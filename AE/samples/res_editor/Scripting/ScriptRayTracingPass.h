// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBasePass.h"
#include "res_editor/Passes/RayTracingPass.h"

namespace AE::ResEditor
{
	struct RTInstanceIndex
	{
		uint		value	= 0;

		RTInstanceIndex ()					__NE___	{}
		explicit RTInstanceIndex (uint v)	__NE___	: value{v} {}
	};

	struct RTRayIndex
	{
		uint		value	= 0;

		RTRayIndex ()						__NE___	{}
		explicit RTRayIndex (uint v)		__NE___	: value{v} {}
	};

	struct RTCallableIndex
	{
		uint		value	= 0;

		RTCallableIndex ()					__NE___	{}
		explicit RTCallableIndex (uint v)	__NE___	: value{v} {}
	};

	struct RTShader
	{
		String		filename;
		String		defines;
		bool		isDefined	= false;

		RTShader () {}
		RTShader (const String &filename, const String &defines) :
			filename{filename}, defines{defines}, isDefined{true} {}
	};



	//
	// Ray Tracing Pass
	//

	class ScriptRayTracingPass final : public ScriptBasePass
	{
	// types
	public:
		using IterationDim_t	= RayTracingPass::IterationDim_t;

		struct Iteration
		{
			IterationDim_t		dim;

			ScriptBufferPtr		indirect;
			Bytes				indirectOffset;
			String				indirectCmdField;

			operator RayTracingPass::Iteration () C_Th___;
		};
		using Iterations_t	= Array< Iteration >;

	private:
		struct _Shader
		{
			Path		shaderPath;
			String		defines;
			bool		isDefined	= false;

			_Shader () {}
			_Shader (const RTShader &sh, bool isRequired) __Th___;
		};

		struct _NamedShader
		{
			String		name;
			_Shader		shader;

			ND_ bool  IsDefined () const	{ return shader.isDefined; }
		};

		struct _HitGroup
		{
			String		name;
			_Shader		closestHit;
			_Shader		anyHit;
			_Shader		intersection;

			ND_ bool  IsDefined () const	{ return closestHit.isDefined; }
		};


	// variables
	private:
		uint						_maxRayTypes		= 0;

		_NamedShader				_rayGen;
		Array<_NamedShader>			_missShaders;
		Array<_NamedShader>			_callableShaders;
		Array< Array<_HitGroup> >	_hitGroups;		// [instances] [ray types]

		ScriptDynamicUIntPtr		_maxRayRecursion;
		ScriptDynamicUIntPtr		_maxCallRecursion;

		Iterations_t				_iterations;


	// methods
	public:
		ScriptRayTracingPass () = delete;
		ScriptRayTracingPass (const String &defines, EFlags baseFlags)							__Th___;

		void  DispatchThreads1  (uint threadsX)													__Th___	{ return DispatchThreads3v({ threadsX, 1u, 1u }); }
		void  DispatchThreads2  (uint threadsX, uint threadsY)									__Th___	{ return DispatchThreads3v({ threadsX, threadsY, 1u }); }
		void  DispatchThreads3  (uint threadsX, uint threadsY, uint threadsZ)					__Th___	{ return DispatchThreads3v({ threadsX, threadsY, threadsZ }); }
		void  DispatchThreads2v (const packed_uint2 &threads)									__Th___	{ return DispatchThreads3v({ threads, 1u }); }
		void  DispatchThreads3v (const packed_uint3 &threads)									__Th___;
		void  DispatchThreadsDS (const ScriptDynamicDimPtr &ds)									__Th___;
		void  DispatchThreads1D (const ScriptDynamicUIntPtr &dyn)								__Th___;

		void  DispatchThreadsIndirect1 (const ScriptBufferPtr &ibuf)							__Th___;
		void  DispatchThreadsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset)				__Th___;
		void  DispatchThreadsIndirect3 (const ScriptBufferPtr &ibuf, const String &field)		__Th___;

		void  SetRayGen (const RTShader &sh)													__Th___;
		void  MaxRayTypes (uint stride)															__Th___;
		void  SetRayMiss (const RTRayIndex &missIndex, const RTShader &sh)						__Th___;
		void  SetCallable (const RTCallableIndex &callIndex, const RTShader &sh)				__Th___;

		void  SetTriangleHit1 (const RTRayIndex &rayIndex, const RTInstanceIndex &inst,
							   const RTShader &closestHit)										__Th___;
		void  SetTriangleHit2 (const RTRayIndex &rayIndex, const RTInstanceIndex &inst,
							   const RTShader &closestHit, const RTShader &anyHit)				__Th___;

		void  SetProceduralHit1 (const RTRayIndex &rayIndex, const RTInstanceIndex &inst,
								 const RTShader &intersection, const RTShader &closestHit)		__Th___;
		void  SetProceduralHit2 (const RTRayIndex &rayIndex, const RTInstanceIndex &inst,
								 const RTShader &intersection,
								 const RTShader &closestHit, const RTShader &anyHit)			__Th___;

		void  SetMaxRayRecursion1 (uint value)													__Th___;
		void  SetMaxRayRecursion2 (const ScriptDynamicUIntPtr &value)							__Th___;
		void  SetMaxCallableRecursion1 (uint value)												__Th___;
		void  SetMaxCallableRecursion2 (const ScriptDynamicUIntPtr &value)						__Th___;

		static void  Bind (const ScriptEnginePtr &se)											__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)								__Th___;

	// ScriptBasePass //
		RC<IPass>  ToPass ()																	C_Th_OV;


	private:
		ND_ auto  _CompilePipeline (OUT Bytes &ubSize)											C_Th___;
			void  _CompilePipeline2 (OUT Bytes &ubSize)											C_Th___;
			void  _CompilePipeline3 (const String &header, const String &pplnName,
									 uint shaderOpts, EPipelineOpt pplnOpt)						C_Th___;

		ND_ static auto  _CreateUBType ()														__Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)									C_Th_OV;
	};


} // AE::ResEditor
