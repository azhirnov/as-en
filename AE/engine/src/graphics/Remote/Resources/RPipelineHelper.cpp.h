// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/REnumCast.h"

# ifdef AE_ENABLE_GLSL_TRACE
#	include "ShaderTrace.h"
# else
#	include "Packer/ShaderTraceDummy.h"
# endif

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

namespace
{

/*
=================================================
	ConvertLogFormat
=================================================
*/
	ND_ auto  ConvertLogFormat (ShaderDebugger::ELogFormat fmt) __NE___
	{
		using ELogFormat = PipelineCompiler::ShaderTrace::ELogFormat;

		StaticAssert( uint(ELogFormat::_Count)		== 5	);
		StaticAssert( uint(ELogFormat::_Count)		== uint(ShaderDebugger::ELogFormat::_Count)	);
		StaticAssert( uint(ELogFormat::Unknown)		== uint(ShaderDebugger::ELogFormat::Unknown) );
		StaticAssert( uint(ELogFormat::Text)		== uint(ShaderDebugger::ELogFormat::Text) );
		StaticAssert( uint(ELogFormat::VS_Console)	== uint(ShaderDebugger::ELogFormat::VS_Console)	);
		StaticAssert( uint(ELogFormat::VS)			== uint(ShaderDebugger::ELogFormat::VS)	);
		StaticAssert( uint(ELogFormat::VSCode)		== uint(ShaderDebugger::ELogFormat::VSCode)	);

		return ELogFormat(fmt);
	}


} // namespace
} // AE::Graphics
