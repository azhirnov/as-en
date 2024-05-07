// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"
#include "ScriptObjects/DescriptorSetLayout.h"
#include "ScriptObjects/BasePipeline.h"
#include "ScriptObjects/RayTracingShaderBinding.h"

namespace AE::PipelineCompiler
{
	struct RenderTechnique;


	enum class EMutableRenderState : uint
	{
		Unknown		= 0,

		// TODO
	};


	//
	// Render Technique Base Pass
	//
	struct RTBasePass : EnableScriptRC
	{
		friend struct RenderTechnique;

	// types
	private:
		using PipelineRefs_t = FlatHashSet< BasePipelineSpecPtr >;


	// variables
	protected:
		const String			_name;
		const usize				_passIndex	= UMax;

		RenderTechnique*		_rtech		= null;
		DescriptorSetLayoutPtr	_dsLayout;
		PipelineRefs_t			_pipelineRefs;


	// methods
	public:
		RTBasePass () {}
		RTBasePass (RenderTechnique* rtech, const String &name, usize passIdx)	__Th___;

		ND_ StringView				Name ()										const	{ return _name; }
		ND_ usize					PassIndex ()								const	{ return _passIndex; }

		ND_ PipelineRefs_t const&	GetPipelines ()								const	{ return _pipelineRefs; }
		ND_ DescriptorSetLayoutPtr	GetDSLayout ()								const	{ return _dsLayout; }

		virtual void  AddPipeline (const BasePipelineSpecPtr &ptr)				__Th___;

	protected:
		void  _SetDSLayout (const String &typeName)								__Th___;
	};
	using RTBasePassPtr = ScriptRC< RTBasePass >;



	//
	// Render Technique Graphics/Mesh Pass
	//
	struct RTGraphicsPass final : RTBasePass
	{
		friend struct RenderTechnique;

	// variables
	private:
		RenderState				_renderState;
		EMutableRenderState		_mutableStates	= Default;
		RenderPassName			_renderPass;
		SubpassName				_subpass;


	// methods
	public:
		RTGraphicsPass () {}
		RTGraphicsPass (RenderTechnique* rtech, const String &name, usize passIdx) __Th___ : RTBasePass{ rtech, name, passIdx } {}

		void  SetRenderPass (const String &rp, const String &subpass)	__Th___;

		void  SetRenderState1 (const RenderState &value)				__Th___;
		void  SetRenderState2 (const String &name)						__Th___;
		void  SetMutableStates (EMutableRenderState values)				__Th___;
		void  SetDSLayout (const String &typeName)						__Th___	{ return _SetDSLayout( typeName ); }

		void  AddPipeline (const BasePipelineSpecPtr &ptr)				__Th_OV;

		ND_ RenderPassName const&		GetRenderPassName ()			C_NE___	{ return _renderPass; }
		ND_ SubpassName const&			GetSubpassName ()				C_NE___	{ return _subpass; }

		ND_ bool						IsFirstSubpass ()				C_NE___;
		ND_ RenderPassSpecPtr			GetRenderPass ()				C_NE___;
		ND_ CompatibleRenderPassDescPtr	GetCompatRenderPass ()			C_NE___;

		static void  Bind (const ScriptEnginePtr &se)					__Th___;
	};
	using RTGraphicsPassPtr = ScriptRC< RTGraphicsPass >;



	//
	// Render Technique Compute/RayTracing Pass
	//
	struct RTComputePass final : RTBasePass
	{
		friend struct RenderTechnique;

	// variables
	private:


	// methods
	public:
		RTComputePass () {}
		RTComputePass (RenderTechnique* rtech, const String &name, usize passIdx) __Th___ : RTBasePass{ rtech, name, passIdx } {}

		void  SetDSLayout (const String &typeName)				__Th___	{ return _SetDSLayout( typeName ); }

		void  AddPipeline (const BasePipelineSpecPtr &ptr)		__Th_OV;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;
	};
	using RTComputePassPtr = ScriptRC< RTComputePass >;



	//
	// Render Technique
	//
	struct RenderTechnique final : EnableScriptRC
	{
		friend struct RTBasePass;
		friend struct RTGraphicsPass;

	// types
	private:
		using PassMap_t		= HashMap< RenderTechPassName, usize >;
		using PassArray_t	= Array< RTBasePassPtr >;
		using SBTArray_t	= Array< RayTracingShaderBindingPtr >;


	// variables
	private:
		const String					_name;
		Array< ScriptFeatureSetPtr >	_features;
		PassArray_t						_passes;
		PassMap_t						_passMap;
		SBTArray_t						_rtSBTs;
		Optional<RenderTechUID>			_uid;


	// methods
	public:
		RenderTechnique () {}
		explicit RenderTechnique (const String &name)					__Th___;

			void				AddFeatureSet (const String &name)		__Th___;

		ND_	RTGraphicsPass*		AddGraphicsPass (const String &name)	__Th___;
		ND_	RTGraphicsPassPtr	AddGraphicsPass2 (const String &name)	__Th___;
		ND_	RTComputePass*		AddComputePass (const String &name)		__Th___;
		ND_	RTComputePassPtr	AddComputePass2 (const String &name)	__Th___;

		ND_	RTGraphicsPass*		CopyGraphicsPass (const String &newName, const String &rtechName, const String &passName)	__Th___;
		ND_	RTGraphicsPassPtr	CopyGraphicsPass2 (const String &newName, const String &rtechName, const String &passName)	__Th___;

		ND_	RTComputePass*		CopyComputePass (const String &newName, const String &rtechName, const String &passName)	__Th___;
		ND_	RTComputePassPtr	CopyComputePass2 (const String &newName, const String &rtechName, const String &passName)	__Th___;

			void				AddSBT (RayTracingShaderBindingPtr sbt)	__Th___;

		ND_ RTBasePassPtr		GetPass (const String &name);

		ND_ PassArray_t const&	GetPasses ()							const	{ return _passes; }
		ND_ SBTArray_t const&	GetSBTs ()								const	{ return _rtSBTs; }

		ND_ bool				Build ()								__NE___;
		ND_ bool				HasUID ()								const	{ return _uid.has_value(); }
		ND_ StringView			Name ()									const	{ return _name; }

		static void  Bind (const ScriptEnginePtr &se)					__Th___;
	};


} // AE::PipelineCompiler
