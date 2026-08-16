// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/SamplerDesc.h"
#include "res_pack/pipeline_compiler/Packer/PackCommon.h"

namespace AE::PipelineCompiler
{
	using namespace AE::Graphics;

	struct ScriptSampler;


	//
	// Sampler Serializer
	//

	class SamplerSerializer final : public Serializing::ISerializable
	{
	// types
	public:
		using FSNameArr_t = ArrayView< FeatureSetName::Optimized_t >;

		static constexpr uint	MaxCount = 1 << 16;


	// variables
	private:
		FSNameArr_t								_features;
		SamplerDesc								_desc;
		Optional<SamplerYcbcrConversionDesc>	_ycbcrDesc;


	// methods
	public:
		SamplerSerializer () {}

		ND_ FSNameArr_t							GetFeatures ()		const	{ return _features; }
		ND_ SamplerDesc const&					GetDesc ()			const	{ return _desc; }

		ND_ SamplerYcbcrConversionDesc const&	YcbcrDesc ()		const	{ return *_ycbcrDesc; }
		ND_ bool								HasYcbcr ()			const	{ return _ycbcrDesc.has_value(); }
		ND_ auto const&							YcbcrDescOpt ()		const	{ return _ycbcrDesc; }

		ND_ bool  Create (const ScriptSampler &samp);
		ND_ bool  operator == (const ScriptSampler &rhs) const;

		ND_ String  ToString (const HashToName &) const;

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};

} // AE::PipelineCompiler

