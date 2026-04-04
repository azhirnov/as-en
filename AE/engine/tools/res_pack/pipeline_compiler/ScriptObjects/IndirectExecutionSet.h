// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/pipeline_compiler/ScriptObjects/RenderTechnique.h"

namespace AE::PipelineCompiler
{

	//
	// Indirect Execution Set
	//
	struct IndirectExecutionSet : EnableScriptRC
	{
	// types
	private:
		using RTechPasses_t = FlatHashSet< RTBasePassPtr >;
		using Pipelines_t	= Array< BasePipelineSpecPtr >;

		enum class EState
		{
			Initial,
			AddPipelines,
			Packing,
		};


	// variables
	private:
		const IndirectExecutionSetName		_name;
		const String						_nameStr;
		RTechPasses_t						_rtechPasses;
		Pipelines_t							_pipelines;

		Optional<IndirectExecutionSetUID>	_uid;
		EState								_state			= EState::Initial;
		EShaderStages						_usedStages		= Default;

		// from feature sets
		uint								_maxPipeCount	= UMax;
		EShaderStages						_shaderStages	= EShaderStages::All;
		EShaderStages						_pipeBinding	= EShaderStages::All;


	// methods
	public:
		~IndirectExecutionSet ();

		ND_ static IndirectExecutionSetPtr  Create (const String &name)		__Th___;

		void  AddPipeline (const String &name)								__Th___;

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

		ND_ bool  Build ()													__NE___;

		ND_ String  ToGlslCppHeader ()										C_Th___;

		ND_ IndirectExecutionSetName	Name ()								C_NE___	{ return _name; }
		ND_ StringView					NameStr ()							C_NE___	{ return _nameStr; }
		ND_ IndirectExecutionSetUID		UID ()								C_NE___	{ return _uid.value_or( Default ); }

	private:
		explicit IndirectExecutionSet (const String &name)					__NE___;

		void  _Validate ()													__Th___;

		void  _AddToRenderTech (const String &rtech, const String &pass)	__Th___;
		void  _AddPipeline2 (const BasePipelineSpecPtr &ptr)				__Th___;

		template <typename T>
		void  _ValidatePipe (BasePipelineSpec const* initial, BasePipelineSpec const* pending)											__Th___;

		void  _ValidatePipe2 (MeshPipelineSpecScriptBinding const& initial, MeshPipelineSpecScriptBinding const& pending)				__Th___;
		void  _ValidatePipe2 (ComputePipelineSpecScriptBinding const& initial, ComputePipelineSpecScriptBinding const& pending)			__Th___;
		void  _ValidatePipe2 (GraphicsPipelineSpecScriptBinding const& initial, GraphicsPipelineSpecScriptBinding const& pending)		__Th___;
		void  _ValidatePipe2 (RayTracingPipelineSpecScriptBinding const& initial, RayTracingPipelineSpecScriptBinding const& pending)	__Th___;
	};


} // AE::PipelineCompiler
