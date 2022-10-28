// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/IAllocator.h"
#include "base/CompileTime/StringToID.h"

#if defined(AE_TEST_PIPELINE_COMPILER) or defined(AE_DBG_OR_DEV)
# include "HashToName.h"
#endif

namespace AE::PipelineCompiler
{
	using namespace AE::Base;
	

	static constexpr uint	PackOffsets_Name			= uint("PackOff"_StringToID);
	static constexpr uint	NameMapping_Name			= uint("NameMap"_StringToID);
	
	static constexpr uint	FeatureSetPack_Version		= 1;
	static constexpr uint	FeatureSetPack_Name			= uint("FSpack"_StringToID);

	static constexpr uint	RenderPassPack_Version		= 1;
	static constexpr uint	RenderPassPack_Name			= uint("RPpack"_StringToID);
	static constexpr uint	RenderPassPack_VkRpBlock	= uint("VKRP"_StringToID);
	static constexpr uint	RenderPassPack_MtlRpBlock	= uint("MtlRP"_StringToID);
	STATIC_ASSERT( RenderPassPack_VkRpBlock != RenderPassPack_MtlRpBlock );

	static constexpr uint	SamplerPack_Version			= 1;
	static constexpr uint	SamplerPack_Name			= uint("SampPack"_StringToID);

	static constexpr uint	PipelinePack_Version		= 1;
	static constexpr uint	PipelinePack_Name			= uint("PplnPack"_StringToID);
	
	static constexpr uint	ShaderPack_Version			= 1;
	static constexpr uint	ShaderPack_Name				= uint("ShPack"_StringToID);

	
	STATIC_ASSERT( PackOffsets_Name != NameMapping_Name );
	STATIC_ASSERT( PackOffsets_Name != FeatureSetPack_Name );
	STATIC_ASSERT( PackOffsets_Name != RenderPassPack_Name );
	STATIC_ASSERT( PackOffsets_Name != SamplerPack_Name );
	STATIC_ASSERT( PackOffsets_Name != PipelinePack_Name );
	STATIC_ASSERT( PackOffsets_Name != ShaderPack_Name );
	
	STATIC_ASSERT( NameMapping_Name != FeatureSetPack_Name );
	STATIC_ASSERT( NameMapping_Name != RenderPassPack_Name );
	STATIC_ASSERT( NameMapping_Name != SamplerPack_Name );
	STATIC_ASSERT( NameMapping_Name != PipelinePack_Name );
	STATIC_ASSERT( NameMapping_Name != ShaderPack_Name );

	STATIC_ASSERT( FeatureSetPack_Name != RenderPassPack_Name );
	STATIC_ASSERT( FeatureSetPack_Name != SamplerPack_Name );
	STATIC_ASSERT( FeatureSetPack_Name != PipelinePack_Name );
	STATIC_ASSERT( FeatureSetPack_Name != ShaderPack_Name );
	
	STATIC_ASSERT( RenderPassPack_Name != SamplerPack_Name );
	STATIC_ASSERT( RenderPassPack_Name != PipelinePack_Name );
	STATIC_ASSERT( RenderPassPack_Name != ShaderPack_Name );
	
	STATIC_ASSERT( SamplerPack_Name != PipelinePack_Name );
	STATIC_ASSERT( SamplerPack_Name != ShaderPack_Name );
	
	STATIC_ASSERT( PipelinePack_Name != ShaderPack_Name );


	//
	// Pipeline Pack Offsets
	//
	struct PipelinePackOffsets
	{
		using Offset_t	= ulong;

		Offset_t	allocSize			= 0;

		Offset_t	featureSetOffset	= UMax;		// FeatureSetPack_Name
		Offset_t	featureSetDataSize	= 0;

		Offset_t	samplerOffset		= UMax;		// SamplerPack_Name
		Offset_t	samplerDataSize		= 0;

		Offset_t	renderPassOffset	= UMax;		// RenderPassPack_Name
		Offset_t	renderPassDataSize	= 0;

		Offset_t	pipelineOffset		= UMax;		// PipelinePack_Name
		Offset_t	pipelineDataSize	= 0;

		Offset_t	shaderOffset		= UMax;		// ShaderPack_Name
		Offset_t	shaderDataSize		= 0;

		Offset_t	nameMappingOffset	= UMax;		// NameMapping_Name
		Offset_t	nameMappingDataSize	= 0;
	};


} // AE::PipelineCompiler


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::PipelineCompiler::PipelinePackOffsets >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::PipelineCompiler::PipelinePackOffsets >	{ static constexpr bool  value = true; };

} // AE::Base
