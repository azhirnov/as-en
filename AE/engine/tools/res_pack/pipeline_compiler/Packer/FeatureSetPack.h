// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/FeatureSet.h"
#include "res_pack/pipeline_compiler/Packer/PackCommon.h"

namespace AE::PipelineCompiler
{
	using namespace AE::Graphics;

	struct ScriptFeatureSet;


	//
	// Feature Set Serializer
	//

	class FeatureSetSerializer final : public Serializing::ISerializable
	{
	// types
	public:
		static constexpr uint	MaxCount = 1 << 8;


	// variables
	private:
		FeatureSet	_fs;


	// methods
	public:
		FeatureSetSerializer () {}

		ND_ FeatureSet const&  Get ()	const	{ return _fs; }

		bool  Create (const ScriptFeatureSet &fs);
		ND_ String  ToString () const;

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};


} // AE::PipelineCompiler
