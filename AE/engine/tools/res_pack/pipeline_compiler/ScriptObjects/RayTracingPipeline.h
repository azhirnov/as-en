// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/BasePipeline.h"

namespace AE::PipelineCompiler
{

	//
	// Ray Tracing Pipeline Specialization
	//
	struct RayTracingPipelineSpecScriptBinding final : BasePipelineSpec
	{
	// variables
	public:
		Graphics::RayTracingPipelineDesc	desc;


	// methods
	public:
		RayTracingPipelineSpecScriptBinding () {}
		RayTracingPipelineSpecScriptBinding (const RayTracingPipelineScriptBinding* base, const String &name) __Th___;

		void  SetSpecValueU (const String &name, uint  value)			__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, value ); }
		void  SetSpecValueI (const String &name, int   value)			__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }
		void  SetSpecValueF (const String &name, float value)			__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }

		void  SetOptions (EPipelineOpt value)							__Th___	{ return BasePipelineSpec::_SetOptions( value ); }

		void  MaxRecursionDepth (uint value)							__Th___;
		void  MaxPipelineRayPayloadSize (Bytes value)					__Th___;
		void  MaxPipelineRayHitAttributeSize (Bytes value)				__Th___;

		void  SetDynamicState (/*EPipelineDynamicState*/uint states)	__Th___;

		void  AddToRenderTech (const String &rtech, const String &pass) __Th___	{ return BasePipelineSpec::_AddToRenderTech( rtech, pass ); }

		ND_ const RayTracingPipelineScriptBinding*	GetBase ()			const	{ return Cast<RayTracingPipelineScriptBinding>( BasePipelineSpec::GetBase() ); }

		ND_ bool  Build (PipelineTemplUID uid);

		static void  Bind (const ScriptEnginePtr &se)					__Th___;
	};
	using RayTracingPipelineSpecPtr = ScriptRC< RayTracingPipelineSpecScriptBinding >;



	//
	// Ray Tracing Pipeline Template
	//
	struct RayTracingPipelineScriptBinding final : BasePipelineTmpl
	{
	// types
	private:
		using Specializations_t	= Array< RayTracingPipelineSpecPtr >;

		struct GeneralShader
		{
			String				name;
			CompiledShaderPtr	shader;
		};

		struct TriangleHitGroup
		{
			String				name;
			CompiledShaderPtr	closestHit;
			CompiledShaderPtr	anyHit;
		};

		struct ProceduralHitGroup
		{
			String				name;
			CompiledShaderPtr	intersection;
			CompiledShaderPtr	closestHit;
			CompiledShaderPtr	anyHit;
		};


	// variables
	private:
		Array<GeneralShader>		_generalShaders;
		Array<TriangleHitGroup>		_triangleGroups;
		Array<ProceduralHitGroup>	_proceduralGroups;
		FlatHashSet<String>			_uniqueNames;

		Specializations_t			_pplnSpec;
		bool						_isPrepared		= false;
		ubyte						_isSupported	= 0;


	// methods
	public:
		RayTracingPipelineScriptBinding ();
		explicit RayTracingPipelineScriptBinding (const String &name)						__Th___;

		void  AddGeneralShader      (const String &name, const ScriptShaderPtr &shader)		__Th___;
		void  AddTriangleHitGroup   (const String &name, const ScriptShaderPtr &closestHit,   const ScriptShaderPtr &anyHit) __Th___;
		void  AddProceduralHitGroup (const String &name, const ScriptShaderPtr &intersection, const ScriptShaderPtr &closestHit, const ScriptShaderPtr &anyHit) __Th___;

		void  SetLayout (const String &name)												__Th___	{ BasePipelineTmpl::_SetLayout( name ); }
		void  SetLayout2 (const PipelineLayoutPtr &pl)										__Th___	{ BasePipelineTmpl::_SetLayout( pl ); }
		void  Define (const String &value)													__Th___	{ BasePipelineTmpl::_Define( value ); }
		void  AddFeatureSet (const String &name)											__Th___	{ BasePipelineTmpl::_AddFeatureSet( name ); }

		ND_ RayTracingPipelineSpecPtr				AddSpecialization2 (const String &name)	__Th___;
		ND_ RayTracingPipelineSpecScriptBinding*	AddSpecialization (const String &name)	__Th___;

		ND_ bool	Build ();
		ND_ usize	SpecCount ()															const	{ return _pplnSpec.size(); }

		ND_ Specializations_t const&	GetSpecializations ()								const	{ return _pplnSpec; }

		ND_ bool  HasGeneralShader (const String &name, EShader type)						const;
		ND_ bool  HasTriangleHitGroup (const String &name, EShader type)					const;
		ND_ bool  HasProceduralHitGroup (const String &name, EShader type)					const;

		ND_ uint  GetRayGenShader	(const String &name)									const;
		ND_ uint  GetMissShader		(const String &name)									const;
		ND_ uint  GetHitGroup		(const String &name)									const;
		ND_ uint  GetCallableShader	(const String &name)									const;

		ND_ StringView  GetRayGenShaderName (uint idx)										const;
		ND_ StringView  GetMissShaderName (uint idx)										const;
		ND_ StringView  GetHitGroupName (uint idx)											const;
		ND_ StringView  GetCallableShaderName (uint idx)									const;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;

	private:
		void  _Prepare ()																	__Th___;
		void  _CheckSupport ()																__Th___;
	};
	using RayTracingPipelinePtr = ScriptRC< RayTracingPipelineScriptBinding >;


} // AE::PipelineCompiler
