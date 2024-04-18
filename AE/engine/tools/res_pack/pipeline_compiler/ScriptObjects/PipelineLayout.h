// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/DescriptorSetLayout.h"

namespace AE::PipelineCompiler
{

	//
	// Pipeline Layout
	//
	struct PipelineLayout final : EnableScriptRC
	{
	// types
	public:
		using UniqueTypes_t	= DescriptorSetLayout::UniqueTypes_t;
		using DSLayoutArr_t	= Array< Tuple< DescriptorSetLayoutPtr, DescriptorSetName >>;
		using PCArray_t		= Array< Tuple< String, ShaderStructTypePtr, EShader >>;
		using MSLBindings	= DescriptorSetLayout::MSLBindings;

		struct DebugInfo
		{
			uint			dsIndex	= UMax;
			EShaderOpt		mode	= Default;
			EShaderStages	stages	= Default;

			ND_ bool  IsDefined () const	{ return dsIndex != UMax; }
		};

	private:
		static constexpr char				_DbgShaderTraceStr []	= "ShaderTrace";
		static constexpr DescriptorSetName	_DbgShaderTrace			{_DbgShaderTraceStr};


	// variables
	private:
		DSLayoutArr_t					_dsLayouts;
		PCArray_t						_pushConstants;
		Array< ScriptFeatureSetPtr >	_features;
		const String					_name;
		PipelineLayoutDesc				_desc;
		Optional<PipelineLayoutUID>		_uid;
		DebugInfo						_dbgInfo;
		String							_defines;


	// methods
	public:
		PipelineLayout () {}
		explicit PipelineLayout (const String &name)											__Th___;

		void  AddFeatureSet (const String &name)												__Th___;
		void  Define (const String &value)														__Th___;

		void  AddDSLayout  (uint index, const String &typeName)									__Th___;
		void  AddDSLayout2 (const String &name, uint index, const String &typeName)				__Th___;
		void  AddDSLayout3 (uint index, const DescriptorSetLayoutPtr &ds)						__Th___;
		void  AddDSLayout4 (const String &name, uint index, const DescriptorSetLayoutPtr &ds)	__Th___;

		void  AddPushConst1 (const String &name, const String &structName, EShader stage)		__Th___;
		void  AddPushConst2 (const String &name, const ShaderStructTypePtr &ptr, EShader stage)	__Th___;

		void  AddDebugDSLayout1 (EShaderOpt dbgMode, /*EShaderStages*/uint stages)				__Th___;
		void  AddDebugDSLayout2 (uint index, EShaderOpt dbgMode, /*EShaderStages*/uint stages)	__Th___;

		ND_ String  ToGLSL (EShaderStages stages, INOUT UniqueTypes_t &uniqueTypes)				C_Th___;
			void	ToMSL  (EShaderStages stages, INOUT UniqueTypes_t &uniqueTypes,
							INOUT String &typesStr, INOUT String &declStr)						C_Th___;

		ND_ bool	MslDescSetAsArgumentBuffer ()												const;

		ND_ StringView							Name ()											const	{ return _name; }
		ND_ DebugInfo const&					GetDebugDS ()									const	{ return _dbgInfo; }
		ND_ bool								HasDebugDS ()									const	{ return _dbgInfo.IsDefined(); }

		ND_ Optional<PipelineLayoutUID>			GetLayout ()									const	{ return _uid; }
		ND_ PipelineLayoutDesc const&			GetDesc ()										const	{ return _desc; }
		ND_ DSLayoutArr_t const&				Layouts ()										const	{ return _dsLayouts; }
		ND_ PCArray_t const&					PushConstants ()								const	{ return _pushConstants; }
		ND_ ArrayView< ScriptFeatureSetPtr >	GetFeatures ()									const	{ return _features; }
		ND_ String const&						GetDefines ()									const	{ return _defines; }

		static void  Bind (const ScriptEnginePtr &se)											__Th___;

		ND_ bool  Build ();
	};
	using PipelineLayoutPtr = ScriptRC< PipelineLayout >;


} // AE::PipelineCompiler
