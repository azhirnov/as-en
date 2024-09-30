// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ScriptConfig.h"
#include "ScriptObjects/ScriptFeatureSet.h"
#include "ScriptObjects/DescriptorSetLayout.h"
#include "ScriptObjects/PipelineLayout.h"
#include "ScriptObjects/RenderTechnique.h"
#include "ScriptObjects/GraphicsPipeline.h"
#include "ScriptObjects/ComputePipeline.h"
#include "ScriptObjects/MeshPipeline.h"
#include "ScriptObjects/TilePipeline.h"
#include "ScriptObjects/RayTracingPipeline.h"
#include "ScriptObjects/ScriptRenderPass.h"
#include "ScriptObjects/ScriptSampler.h"
#include "ScriptObjects/ScriptShader.h"
#include "ScriptObjects/RayTracingShaderBinding.h"

#include "Packer/HashToName.h"

#include "Compiler/SpirvCompiler.h"
#include "Compiler/MetalCompiler.h"

namespace AE::PipelineCompiler
{

	//
	// Compiled Shader
	//
	struct CompiledShader final
	{
		using Data_t = Union< NullUnion, SpirvBytecode_t, MetalBytecode_t, SpirvWithTrace, /*source*/String >;

		mutable ShaderUID				uid			= Default;
		EShaderVersion					version		= Default;
		EShader							type		= Default;
		Data_t							data;
		SpirvCompiler::ShaderReflection	reflection;
		Unique<ShaderTrace>				trace;

		CompiledShader ()									__NE___;
		CompiledShader (CompiledShader &&)					__NE___;
		~CompiledShader ()									__NE___;

		ND_ bool	operator == (const CompiledShader &rhs)	C_NE___;
		ND_ HashVal	CalcHash ()								C_NE___;
	};


	//
	// Shader Debugger Features
	//
	struct ShaderDebuggerFeatures
	{
		bool	shaderDeviceClock				= false;
		bool	shaderSubgroupClock				= false;
		bool	fragmentStoresAndAtomics		= false;
		bool	vertexPipelineStoresAndAtomics	= false;
	};


	//
	// Objects Storage
	//
	struct ObjectStorage final
	{
	// types
	private:
		struct ShaderSrcKey
		{
			String			source;
			String			resources;
			Array<String>	include;
			EShader			type		= Default;
			EShaderVersion	version		= Default;
			EShaderOpt		options		= Default;
			ShaderDefines_t	defines;

			ShaderSrcKey () {}

			ND_ bool	operator == (const ShaderSrcKey &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		using UniqueShaderSrc_t		= HashMap< ShaderSrcKey, CompiledShaderPtr, DefaultHasher_CalcHash<ShaderSrcKey> >;
		using CompiledShaderMap_t	= HashSet< CompiledShader, DefaultHasher_CalcHash<CompiledShader> >;
		//-------------------------------------------------------------------

		using RenderPassInfoMap_t	= HashMap< CompatRenderPassName::Optimized_t, SerializableRenderPassInfo >;
		using RenderPassMap_t		= HashMap< CompatRenderPassName, CompatibleRenderPassDescPtr, CompatRenderPassName::Hasher_t, CompatRenderPassName::EqualTo_t >;
		using RenderPassToCompat_t	= HashMap< RenderPassName, CompatRenderPassName >;
		//-------------------------------------------------------------------

		using FeatureSets_t			= HashMap< FeatureSetName,		ScriptFeatureSetPtr >;
		using DSLayouts_t			= HashMap< String,				DescriptorSetLayoutPtr >;
		using PplnLayouts_t			= HashMap< PipelineLayoutName,	PipelineLayoutPtr,		PipelineLayoutName::Hasher_t,	PipelineLayoutName::EqualTo_t	>;
		using GPipelines_t			= HashMap< PipelineTmplName,	GraphicsPipelinePtr,	PipelineTmplName::Hasher_t,		PipelineTmplName::EqualTo_t		>;
		using MPipelines_t			= HashMap< PipelineTmplName,	MeshPipelinePtr,		PipelineTmplName::Hasher_t,		PipelineTmplName::EqualTo_t		>;
		using CPipelines_t			= HashMap< PipelineTmplName,	ComputePipelinePtr,		PipelineTmplName::Hasher_t,		PipelineTmplName::EqualTo_t		>;
		using TPipelines_t			= HashMap< PipelineTmplName,	TilePipelinePtr,		PipelineTmplName::Hasher_t,		PipelineTmplName::EqualTo_t		>;
		using RTPipelines_t			= HashMap< PipelineTmplName,	RayTracingPipelinePtr,	PipelineTmplName::Hasher_t,		PipelineTmplName::EqualTo_t		>;
		using UniqueNames_t			= HashSet< String >;
		using RTechMap_t			= HashMap< String, RenderTechniquePtr >;
		using CompatRTConsts		= CompatibleRenderPassDesc::ScriptConstants;
		using StructTypeConsts		= ShaderStructType::Constants;
		using SamplerRefs_t			= Array< ScriptSamplerPtr >;
		using SamplerMap_t			= HashMap< String, usize >;			// index in 'samplerRefs'
		using HashToNameMap_t		= HashMap< HashToName::NameHash, String, HashToName::NameHashHash >;
		using RenderStateMap_t		= HashMap< String, RenderStatePtr >;
		using VBInputMap_t			= HashMap< String, VertexBufferInputPtr >;
		using StructTypeMap_t		= HashMap< String, ShaderStructTypePtr >;
		using RTShaderBindingMap_t	= HashMap< String, RayTracingShaderBindingPtr >;


	// variables
	private:
		UniqueShaderSrc_t		_shaderSrcMap;
		CompiledShaderMap_t		_compiledShaders;

	public:
		LinearAllocator<>		allocator;

		FeatureSets_t			featureSets;
		DSLayouts_t				dsLayouts;
		PplnLayouts_t			pplnLayouts;

		GPipelines_t			gpipelines;
		MPipelines_t			mpipelines;
		CPipelines_t			cpipelines;
		TPipelines_t			tpipelines;
		RTPipelines_t			rtpipelines;
		RTechMap_t				rtechMap;
		RTShaderBindingMap_t	rtShaderBindings;

		SamplerRefs_t			samplerRefs;
		SamplerMap_t			samplerMap;

		RenderPassMap_t			compatibleRPs;
		RenderPassInfoMap_t		serializedRPs;
		RenderPassToCompat_t	renderPassToCompat;

		// unique names
		UniqueNames_t			pplnTmplNames;
		UniqueNames_t			pplnSpecNames;
		HashToNameMap_t			hashToName;

		// states
		Path					pipelineFilename;
		RenderStateMap_t		renderStatesMap;
		VBInputMap_t			vbInputMap;
		StructTypeMap_t			structTypes;

		// other
		Array< Path >				shaderFolders;
		Ptr<PipelineStorage>		pplnStorage;
		Unique< SpirvCompiler >		spirvCompiler;
		Unique< MetalCompiler >		metalCompiler;
		Unique< CompatRTConsts >	_compatRPConstPtr;
		Unique< StructTypeConsts >	_structTypeConstPtr;

		NamedID_HashCollisionCheck	_hashCollisionCheck;

		// config
		ECompilationTarget		target				= Default;
		EShaderVersion			shaderVersion		= Default;
		EShaderVersion			sprvToMslVersion	= Default;
		EShaderOpt				shaderOptions		= Default;
		EStructLayout			defaultLayout		= EStructLayout::Compatible_Std140;
		EPipelineOpt			defaultPipelineOpt	= Default;
		String					defaultFeatureSet;
		String					defaultShaderDefines;


	// methods
	public:
		ObjectStorage ();
		~ObjectStorage ();

		ND_ bool  Build ()																											__NE___;
			void  Clear ();

		void  TestRenderPass (const String &compatRP, const String &subpass, const SubpassShaderIO &fragIO,
							  bool requireDepth, bool requireStencil, StringView pplnName)											__Th___;
		void  TestRenderPass (const CompatRenderPassName::Optimized_t &compatRP, const SubpassName::Optimized_t &subpass,
							  const SubpassShaderIO &fragIO, bool requireDepth, bool requireStencil, StringView pplnName)			__Th___;

		ND_ CompatibleRenderPassDescPtr  RenderPassExists (const String &rpName, const String &subpass)								__Th___;
		ND_ CompatibleRenderPassDescPtr  RenderPassExists (RenderPassName::Ref rpName, SubpassName::Ref subpass)					C_Th___;
		ND_ RenderPassSpecPtr			 GetRenderPass (RenderPassName::Ref rpName)													C_Th___;

		void  CompileShaderGLSL (INOUT CompiledShaderPtr &shader, const ScriptShaderPtr &inShader, EShaderVersion version,
								 const String &defines, const String &resources, ArrayView<String> include,
								 ArrayView<ScriptFeatureSetPtr> features, uint debugDSIndex, bool useMetalArgBuffer)				__Th___;

		void  CompileShaderMSL (INOUT CompiledShaderPtr &shader, const ScriptShaderPtr &inShader, EShaderVersion version,
								const String &defines, const String &resources, ArrayView<String> include,
								ArrayView<ScriptFeatureSetPtr> features)															__Th___;

		ND_ String  GetShaderExtensionsGLSL (INOUT Version2 &spirvVer, EShaderStages stage, ArrayView<ScriptFeatureSetPtr> features)__Th___;
		ND_ String  GetShaderExtensionsMSL  (INOUT Version2 &metalVer, EShaderStages stage, ArrayView<ScriptFeatureSetPtr> features)__Th___;


		template <typename NameType>
		void  AddName (const String &name)																							__Th___;

		template <usize Size, uint UID, bool Optimize, uint Seed>
		ND_ String  GetName (const NamedID<Size, UID, Optimize, Seed> &id)															C_NE___;

		template <usize Size, uint UID, bool Optimize, uint Seed>
		ND_ bool  HasHashName (const NamedID<Size, UID, Optimize, Seed> &id)														C_NE___;

		ND_ bool  HasHashCollisions ()																								C_NE___	{ return _hashCollisionCheck.HasCollisions(); }

		ND_ Array<ScriptFeatureSetPtr>	GetDefaultFeatureSets ()																	__Th___;
		ND_ FSNameArr_t					CopyFeatures (ArrayView<ScriptFeatureSetPtr> feats);

		static void  Bind (const ScriptEnginePtr &se)																				__Th___;
		static void  Bind_EShaderVersion (const ScriptEnginePtr &se)																__Th___;
		static void  Bind_EShaderOpt (const ScriptEnginePtr &se)																	__Th___;
		static void  Bind_EAccessType (const ScriptEnginePtr &se)																	__Th___;
		static void  Bind_EImageType (const ScriptEnginePtr &se)																	__Th___;
		static void  Bind_ECompilationTarget (const ScriptEnginePtr &se)															__Th___;
		static void  Bind_EStructLayout (const ScriptEnginePtr &se)																	__Th___;
		static void  Bind_EValueType (const ScriptEnginePtr &se)																	__Th___;
		static void  Bind_EShaderPreprocessor (const ScriptEnginePtr &se)															__Th___;

		ND_ static ShaderDebuggerFeatures  GetShaderDebuggerFeatures (ArrayView<ScriptFeatureSetPtr> features);

		ND_ bool  CompilePipeline (const ScriptEnginePtr &se, const Path &pplnPath, ArrayView<Path> includeDirs);
		ND_ bool  CompilePipelineFromSource (const ScriptEnginePtr &se, const Path &pplnPath,
											 StringView source, ArrayView<Path> includeDirs);
		ND_ bool  BuildRenderTechniques ();
		ND_ bool  SavePack (const Path &filename, bool addNameMapping)																const;
		ND_ bool  SavePack (WStream &stream, bool addNameMapping, OUT PipelinePackOffsets &offsets)									const;
		ND_ bool  SaveCppStructs (const Path &filename)																				const;
		ND_ bool  SaveCppNames (const Path &filename, EReflectionFlags flags)														const;


		ND_ static Ptr<ObjectStorage>  Instance ();
			static void  SetInstance (ObjectStorage* inst);

	private:
		void  _CompileShaderGLSL (const ShaderSrcKey &info, ArrayView<ScriptFeatureSetPtr> features, uint debugDSIndex,
								  const PathAndLine &shaderPath, const String &entry, OUT CompiledShader &compiled)					__Th___;
		void  _CompileShaderMSL (const ShaderSrcKey &info, ArrayView<ScriptFeatureSetPtr> features, const PathAndLine &shaderPath,
								 OUT CompiledShader &compiled)																		__Th___;

		static void  _SetAndSortDefines (OUT ShaderDefines_t &defines, const String &def);
	};



/*
=================================================
	AddName
=================================================
*/
	template <typename NameType>
	void  ObjectStorage::AddName (const String &name) __Th___
	{
		CHECK_THROW_MSG( not name.empty() );
		CHECK_THROW_MSG( name.length() <= NameType::MaxStringLength() );

		typename NameType::WithString_t		name_hash {name};

		_hashCollisionCheck.Add( name_hash, name );

		hashToName.emplace( HashToName::NameHash{ uint(name_hash.GetHash32()), NameType::GetUID() }, name );
	}

/*
=================================================
	GetName
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	String  ObjectStorage::GetName (const NamedID<Size, UID, Optimize, Seed> &id) C_NE___
	{
		auto	it = hashToName.find( HashToName::NameHash{ uint(id.GetHash32()), id.GetUID() });
		if ( it != hashToName.end() )
			return it->second;
		else
			return Default;
	}

/*
=================================================
	HasHashName
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	bool  ObjectStorage::HasHashName (const NamedID<Size, UID, Optimize, Seed> &id) C_NE___
	{
		auto	it = hashToName.find( HashToName::NameHash{ uint(id.GetHash32()), id.GetUID() });
		return it != hashToName.end();
	}


} // AE::PipelineCompiler


AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EShaderVersion,						"EShaderVersion"		);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EShaderOpt,							"EShaderOpt"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EImageType,							"EImageType"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EAccessType,							"EAccessType"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::ECompilationTarget,					"ECompilationTarget"	);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EStructLayout,						"EStructLayout"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EMutableRenderState,					"EMutableRenderState"	);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EValueType,							"EValueType"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::EShaderPreprocessor,					"EShaderPreprocessor"	);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::ShaderStructType::EUsage,				"ShaderStructTypeUsage"	);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::ETessPatch,							"ETessPatch"			);
AE_DECL_SCRIPT_TYPE(	AE::PipelineCompiler::ETessSpacing,							"ETessSpacing"			);

AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ScriptFeatureSet,						"FeatureSet"			);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::DescriptorSetLayout,					"DescriptorSetLayout"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::PipelineLayout,						"PipelineLayout"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RenderTechnique,						"RenderTechnique"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::GraphicsPipelineScriptBinding,		"GraphicsPipeline"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::GraphicsPipelineSpecScriptBinding,	"GraphicsPipelineSpec"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::MeshPipelineScriptBinding,			"MeshPipeline"			);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::MeshPipelineSpecScriptBinding,		"MeshPipelineSpec"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ComputePipelineScriptBinding,			"ComputePipeline"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ComputePipelineSpecScriptBinding,		"ComputePipelineSpec"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::TilePipelineScriptBinding,			"TilePipeline"			);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::TilePipelineSpecScriptBinding,		"TilePipelineSpec"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RayTracingPipelineScriptBinding,		"RayTracingPipeline"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RayTracingPipelineSpecScriptBinding,	"RayTracingPipelineSpec");
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RTGraphicsPass,						"GraphicsPass"			);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RTComputePass,						"ComputePass"			);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ScriptRenderState,					"NamedRenderState"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ScriptVertexBufferInput,				"VertexBufferInput"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ShaderStructType,						"ShaderStructType"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::ScriptShader,							"Shader"				);
AE_DECL_SCRIPT_OBJ_RC(	AE::PipelineCompiler::RayTracingShaderBinding,				"RayTracingShaderBinding");

AE_DECL_SCRIPT_OBJ(		AE::PipelineCompiler::ArraySize,							"ArraySize"				);
AE_DECL_SCRIPT_OBJ(		AE::PipelineCompiler::Align,								"Align"					);
AE_DECL_SCRIPT_OBJ(		AE::PipelineCompiler::ScriptConfig,							"GlobalConfig"			);
AE_DECL_SCRIPT_OBJ(		AE::PipelineCompiler::VertexDivisor,						"VertexDivisor"			);
