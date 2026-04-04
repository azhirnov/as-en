// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/pipeline_compiler/ScriptObjects/BasePipeline.h"

namespace AE::PipelineCompiler
{
	using GraphicsPipelineSpecPtr	= ScriptRC< struct GraphicsPipelineSpecScriptBinding >;
	using GraphicsPipelinePtr		= ScriptRC< struct GraphicsPipelineScriptBinding >;



	//
	// Graphics Pipeline Specialization
	//
	struct GraphicsPipelineSpecScriptBinding final : BasePipelineSpec
	{
	// variables
	public:
		Graphics::GraphicsPipelineDesc	desc;
		Graphics::RenderState			renderState;
	private:
		VertexBufferInputPtr			_vertexBuffers;


	// methods
	public:
		GraphicsPipelineSpecScriptBinding (GraphicsPipelineScriptBinding* base, const String &name) __Th___;

		// called from RTGraphicsPass
		void  SetRenderPass (RenderPassName::Ref, SubpassName::Ref subpass) __Th___;

		void  SetSpecValueU (const String &name, uint  value)				__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, value ); }
		void  SetSpecValueI (const String &name, int   value)				__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }
		void  SetSpecValueF (const String &name, float value)				__Th___	{ return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }

		void  SetOptions (EPipelineOpt value)								__Th___	{ return BasePipelineSpec::_SetOptions( value ); }

		void  SetVertexInput (const String &name)							__Th___;
		void  SetVertexInput2 (const VertexBufferInputPtr &ptr)				__Th___;

		void  SetDynamicState (/*EPipelineDynamicState*/uint states)		__Th___;
		void  SetRenderState (const RenderState &state)						__Th___;
		void  SetRenderState2 (const String &name)							__Th___;
		void  SetViewportCount (uint value)									__Th___;

		void  AddToRenderTech (const String &rtech, const String &pass)		__Th___	{ return BasePipelineSpec::_AddToRenderTech( rtech, pass ); }

		ND_ uint	GetSubpassIndex ()										C_NE___;

		ND_ const GraphicsPipelineScriptBinding*	GetBase ()				const;
		ND_ ScriptVertexBufferInput*				GetVertexBuffers ()		const	{ return _vertexBuffers.Get(); }

		ND_ bool  Build (PipelineTemplUID uid)								__NE___;

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

	private:
		bool  _CheckTopology ();
	};



	//
	// Graphics Pipeline Template
	//
	struct GraphicsPipelineScriptBinding final : BasePipelineTmpl
	{
	// types
	public:
		using Specializations_t	= Array< GraphicsPipelineSpecPtr >;
		using ShaderIOMap_t		= FlatHashMap< EShader, Pair< ShaderStructTypePtr, ShaderStructTypePtr >>;


	// variables
	public:
		CompiledShaderPtr		vertex;
		CompiledShaderPtr		tessControl;
		CompiledShaderPtr		tessEval;
		CompiledShaderPtr		geometry;
		CompiledShaderPtr		fragment;

	private:
		Optional<FragOutput_t>	_fragOutput;
		Specializations_t		_pplnSpec;
		VertexBufferInputPtr	_vertexBuffers;
		ShaderIOMap_t			_shaderIO;


	// methods
	public:
		ND_ static GraphicsPipelinePtr  Create (const String &name)					__Th___;

		void  SetVertexShader		(const ScriptShaderPtr &shader)					__Th___;
		void  SetTessControlShader	(const ScriptShaderPtr &shader)					__Th___;
		void  SetTessEvalShader		(const ScriptShaderPtr &shader)					__Th___;
		void  SetGeometryShader		(const ScriptShaderPtr &shader)					__Th___;
		void  SetFragmentShader		(const ScriptShaderPtr &shader)					__Th___;

		void  SetFragmentOutputFromRenderTech (const String &renTechName, const String &passName)			__Th___;
		void  SetFragmentOutputFromRenderPass (const String &compatRPassName, const String &subpassName)	__Th___;

		void  SetVertexInput (const String &name)									__Th___;
		void  SetVertexInput2 (const VertexBufferInputPtr &ptr)						__Th___;

		void  SetShaderIO (EShader output, EShader input, const String &typeName)	__Th___;

		void  SetLayout (const String &name)										__Th___	{ BasePipelineTmpl::_SetLayout( name ); }
		void  SetLayout2 (const PipelineLayoutPtr &pl)								__Th___	{ BasePipelineTmpl::_SetLayout( pl ); }
		void  Define (const String &value)											__Th___	{ BasePipelineTmpl::_Define( value ); }
		void  AddFeatureSet (const String &name)									__Th___	{ BasePipelineTmpl::_AddFeatureSet( name ); }

		void  TestRenderPass (const String &compatRP, const String &subpass)		__Th___;
		void  GetSubpassShaderIO (OUT SubpassShaderIO &fragIO) const				__Th___;

		ND_ ScriptVertexBufferInput*		GetVertexBuffers ()						const	{ return _vertexBuffers.Get(); }
		ND_ Specializations_t const&		GetSpecializations ()					const	{ return _pplnSpec; }

		GraphicsPipelineSpecScriptBinding*	AddSpecialization (const String &name)	__Th___;
		GraphicsPipelineSpecPtr				AddSpecialization2 (const String &name)	__Th___;

		ND_ bool	Build ()														__NE___;
		ND_ usize	SpecCount ()													C_NE___	{ return _pplnSpec.size(); }

		static void  Bind (const ScriptEnginePtr &se)								__Th___;

	private:
		explicit GraphicsPipelineScriptBinding (const String &name)					__NE___	: BasePipelineTmpl{name} {}

		void  _Init ()																__Th___;
		void  _Prepare ()															__Th___;

		ND_ ShaderStructTypePtr  _GetShaderInput (EShader) const;
		ND_ ShaderStructTypePtr  _GetShaderOutput (EShader) const;
	};


} // AE::PipelineCompiler
