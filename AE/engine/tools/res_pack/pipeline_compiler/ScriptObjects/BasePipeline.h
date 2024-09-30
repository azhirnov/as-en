// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"
#include "ScriptObjects/ScriptShader.h"
#include "ScriptObjects/ScriptRenderPass.h"
#include "ScriptObjects/PipelineLayout.h"
#include "ScriptObjects/ScriptRenderState.h"
#include "ScriptObjects/ScriptVertexBufferInput.h"

namespace AE::PipelineCompiler
{

	//
	// Base Pipeline Template
	//
	struct BasePipelineTmpl : EnableScriptRC
	{
	// types
	public:
		using DSLayouts_t		= StaticArray< DescriptorSetLayoutDesc, PipelineLayoutDesc::DescrSetMap_t::capacity() >;
		using DSLayoutNames_t	= StaticArray< DescriptorSetName, PipelineLayoutDesc::DescrSetMap_t::capacity() >;
		using Includes_t		= Array< String >;

		enum class EStateBits : uint
		{
			Unknown		= 0,
			HasShaders	= 1 << 0,
			Define		= 1 << 1,
			Include		= 1 << 2,
			HasSpec		= 1 << 3,
			HasFeatures	= 1 << 4,
		};

		using FragOutput_t = Array< RPAttachment::ShaderIO >;


	// variables
	protected:
		const PipelineTmplName		_name;
		const String				_nameStr;

		Path						_filename;
		String						_defines;
		Includes_t					_includes;
		EStateBits					_states		= Default;
		Optional<PipelineTemplUID>	_pipelineUID;
		Array<ScriptFeatureSetPtr>	_features;
		EShaderStages				_stages		= Default;

		bool						_enabled	= true;

	private:
		PipelineLayoutPtr			_layoutPtr;
		Optional<PipelineLayoutUID>	_layoutUID;
		DSLayouts_t					_dsLayoutsFromReflection;
		DSLayoutNames_t				_dsLayoutNames;
		PushConstants				_pushConstants;


	// methods
	public:
		explicit BasePipelineTmpl (const String &name)		__Th___;

		ND_ PipelineLayoutPtr				GetLayout ()	C_Th___;
		ND_ bool							HasLayout ()	const	{ return bool(_layoutPtr); }

		ND_ EShaderStages					GetStages ()	const	{ return _stages; }
		ND_ ArrayView<ScriptFeatureSetPtr>  GetFeatures ()	const	{ return _features; }

		ND_ StringView						GetName ()		const	{ return _nameStr; }

		void  Disable ();
		void  Enable ();

	protected:
		void  _CompileShader (INOUT CompiledShaderPtr &outShader, const ScriptShaderPtr &inShader,
							  const ShaderStructTypePtr &shaderInput, const ShaderStructTypePtr &shaderOutput,
							  const Optional<FragOutput_t> &fragOut = NullOptional, const VertexBufferInputPtr &vbInput = Default)			__Th___;
		void  _OnAddSpecialization ()																										__Th___;
		bool  _AddLayout (CompiledShaderPtr shader);
		bool  _BuildLayout ();
		void  _SetLayout (PipelineLayoutPtr ptr)																							__Th___;
		void  _SetLayout (const String &name)																								__Th___;
		void  _Define (const String &value)																									__Th___;
		void  _Include (const String &value)																								__Th___;

		void  _AddFeatureSet (const String &name)																							__Th___;

		ND_ bool  _FragOutToSubpassShaderIO (const FragOutput_t &, OUT SubpassShaderIO &fragIO)												const;

		ND_ static bool  _FragmentOutputFromRenderTech (const String &renTechName, const String &passName, OUT FragOutput_t &)				__Th___;
		ND_ static bool  _FragmentOutputFromRenderPass (const String &rPassName, const String &subpassName, OUT FragOutput_t &)				__Th___;
		ND_ static bool  _FragmentOutputFromCompatRenderPass (const String &compatRPassName, const String &subpassName, OUT FragOutput_t &)	__Th___;

		ND_ static String		_FragOutputToGLSL (const FragOutput_t &fragOut)																__Th___;
		ND_ static String		_FragOutputToMSL (const FragOutput_t &fragOut)																__Th___;

		ND_ PipelineLayoutUID	_GetLayout ();

		ND_ EShaderVersion		_GetShaderVersion ()																						const;
	};
	AE_BIT_OPERATORS( BasePipelineTmpl::EStateBits );



	//
	// Base Pipeline Specialization
	//
	struct BasePipelineSpec : EnableScriptRC
	{
	// variables
	private:
		const PipelineName			_name;
		const String				_nameStr;

		BasePipelineTmpl*			_tmpl		= null;
		Optional<PipelineSpecUID>	_uid;
	protected:
		EPipelineOpt				_options	= Default;
		bool						_enabled	= true;

		Array<RenderTechniquePtr>	_linkedRTechs;


	// methods
	public:
		BasePipelineSpec () {}
		BasePipelineSpec (BasePipelineTmpl* tmpl, const String &name) __Th___;

		ND_ const BasePipelineTmpl*			GetBase ()		const	{ return _tmpl; }
		ND_ PipelineName const&				Name ()			const	{ return _name; }
		ND_ StringView						NameStr ()		const	{ return _nameStr; }
		ND_ PipelineSpecUID					UID ()			const	{ return _uid.value_or( Default ); }
		ND_ bool							IsBuilded ()	const	{ return _uid.has_value(); }
		ND_ ArrayView<RenderTechniquePtr>	GetRTechs ()	const	{ return _linkedRTechs; }
		ND_ ArrayView<ScriptFeatureSetPtr>  GetFeatures ()	const	{ return _tmpl->GetFeatures(); }

		void  Disable ();
		void  Enable ();


	protected:
		void  _AddToRenderTech (const String &rtech, const String &pass)												__Th___;

		void  _SetOptions (EPipelineOpt value)																			__Th___;

		void  _OnBuild (PipelineSpecUID uid)																			__NE___;

		static void  _ValidateRenderState (EPipelineDynamicState dynamicState, INOUT RenderState &state,
										   ArrayView<ScriptFeatureSetPtr> features)										__Th___;
		static void  _ValidateRenderPass (const RenderState &state, CompatRenderPassName::Optimized_t renderPass,
										  SubpassName::Optimized_t subpass, ArrayView<ScriptFeatureSetPtr> features)	__Th___;

		static void  _SetSpecValue (INOUT GraphicsPipelineDesc::SpecValues_t &values, const String &name, uint value)	__Th___;

		static void  _SetLocalGroupSize (String prefix, const uint3 &spec, const uint3 &maxSize, uint totalSize,
										 const uint3 &inSize, OUT packed_ushort3 &outSize)								__Th___;

		void  _CheckDepthStencil (const Graphics::RenderState &, const SubpassShaderIO &,
								  const CompatRenderPassName::Optimized_t &rpName,
								  const SubpassName::Optimized_t &subpass)												C_Th___;
	};


	using BasePipelineTmplPtr	= ScriptRC< BasePipelineTmpl >;
	using BasePipelineSpecPtr	= ScriptRC< BasePipelineSpec >;


} // AE::PipelineCompiler
