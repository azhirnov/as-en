// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/BasePipeline.h"

namespace AE::PipelineCompiler
{

	//
	// Compute Pipeline Specialization
	//
	struct ComputePipelineSpecScriptBinding final : BasePipelineSpec
	{
	// variables
	public:
		Graphics::ComputePipelineDesc	desc;


	// methods
	public:
		ComputePipelineSpecScriptBinding () {}
		ComputePipelineSpecScriptBinding (ComputePipelineScriptBinding* base, const String &name) __Th___;

		void  SetSpecValueU (const String &name, uint  value)	__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, value ); }
		void  SetSpecValueI (const String &name, int   value)	__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }
		void  SetSpecValueF (const String &name, float value)	__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }

		void  SetOptions (EPipelineOpt value)					__Th___	{ return BasePipelineSpec::_SetOptions( value ); }

		void  SetLocalGroupSize1 (uint x)						__Th___	{ SetLocalGroupSize3( x, UMax, UMax ); }
		void  SetLocalGroupSize2 (uint x, uint y)				__Th___	{ SetLocalGroupSize3( x, y, UMax ); }
		void  SetLocalGroupSize3 (uint x, uint y, uint z)		__Th___;

		void  SetDynamicState (/*EPipelineDynamicState*/uint states)	__Th___;

		void  AddToRenderTech (const String &rtech, const String &pass) __Th___	{ return BasePipelineSpec::_AddToRenderTech( rtech, pass ); }

		ND_ bool  Build (PipelineTemplUID uid)					__NE___;

		ND_ const ComputePipelineScriptBinding*	GetBase ()		const	{ return Cast<ComputePipelineScriptBinding>( BasePipelineSpec::GetBase() ); }

		static void  Bind (const ScriptEnginePtr &se)			__Th___;
	};
	using ComputePipelineSpecPtr = ScriptRC< ComputePipelineSpecScriptBinding >;



	//
	// Compute Pipeline Template
	//
	struct ComputePipelineScriptBinding final : BasePipelineTmpl
	{
	// types
		using Specializations_t	= Array< ComputePipelineSpecPtr >;


	// variables
	public:
		CompiledShaderPtr	shader;
	private:
		Specializations_t	_pplnSpec;
		bool				_isPrepared	= false;


	// methods
	public:
		ComputePipelineScriptBinding ();
		explicit ComputePipelineScriptBinding (const String &name) __Th___;

		void  SetShader (const ScriptShaderPtr &shader)		__Th___;

		void  SetLayout (const String &name)				__Th___	{ BasePipelineTmpl::_SetLayout( name ); }
		void  SetLayout2 (const PipelineLayoutPtr &pl)		__Th___	{ BasePipelineTmpl::_SetLayout( pl ); }
		void  Define (const String &value)					__Th___	{ BasePipelineTmpl::_Define( value ); }
		void  AddFeatureSet (const String &name)			__Th___	{ BasePipelineTmpl::_AddFeatureSet( name ); }

		ComputePipelineSpecScriptBinding*	AddSpecialization (const String &name)	__Th___;
		ComputePipelineSpecPtr				AddSpecialization2 (const String &name)	__Th___;

		ND_ bool	Build ()								__NE___;
		ND_ usize	SpecCount ()							C_NE___	{ return _pplnSpec.size(); }

		static void  Bind (const ScriptEnginePtr &se)		__Th___;

	private:
		void  _Prepare ()									__Th___;
	};
	using ComputePipelinePtr = ScriptRC< ComputePipelineScriptBinding >;


} // AE::PipelineCompiler
