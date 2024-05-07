// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::PipelineCompiler
{
	struct ShaderTrace
	{
		enum class ELogFormat : uint
		{
			Unknown,
			Text,
			VS_Console,
			VS,
			VSCode,
			_Count
		};

		ND_ bool  operator == (const ShaderTrace &)			const	{ RETURN_ERR( "not supported" ); }

		ND_ Unique<ShaderTrace>  Clone ()					const	{ RETURN_ERR( "not supported" ); }

		ND_ bool  Serialize (Serializing::Serializer &)		C_NE___	{ RETURN_ERR( "not supported" ); }
		ND_ bool  Deserialize (Serializing::Deserializer &) __NE___	{ return false; }

		ND_ bool  ParseShaderTrace (const void*, Bytes, ELogFormat, OUT Array<String> &) const	{ RETURN_ERR( "not supported" ); }
	};

} // AE::PipelineCompiler
