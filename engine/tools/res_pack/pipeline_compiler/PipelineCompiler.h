// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
#	define AE_PC_API	AE_DLL_EXPORT
#else
#	define AE_PC_API	AE_DLL_IMPORT
#endif


namespace AE::PipelineCompiler
{
	using AE::uint;
	using AE::usize;
	using AE::CharType;


	enum class EPathParamsFlags
	{
		Unknown		= 0,
		Recursive	= 1 << 0,
		_Last,
		All			= ((_Last - 1) << 1) - 1,
	};


	struct PathParams
	{
		const CharType *	path		= null;
		usize				priority	: 16;
		usize				flags		: 8;	// EPathParamsFlags
		
		PathParams () : priority{0}, flags{0} {}

		PathParams (const CharType* inPath, usize inPriority = 0, EPathParamsFlags inFlags = EPathParamsFlags::Unknown) :
			path{ inPath },
			priority{ inPriority },
			flags{ usize(inFlags) }
		{}
	};


	struct PipelinesInfo
	{
		// input pipelines
		const PathParams *		pipelineFolders		= null;		// [pipelineFolderCount]
		usize					pipelineFolderCount	= 0;
		const PathParams *		inPipelines			= null;		// [inPipelineCount]
		usize					inPipelineCount		= 0;

		// input shaders
		const CharType * const*	shaderFolders		= null;		// [shaderFolderCount]
		usize					shaderFolderCount	= 0;

		// include directories
		const CharType * const*	includeDirs			= null;		// [includeDirCount]
		usize					includeDirCount		= 0;

		// output
		const CharType *		outputPackName		= null;
		const CharType *		outputCppFile		= null;
		const CharType *		outputScriptFile	= null;
		bool					addNameMapping		= false;
	};


	extern "C" bool AE_PC_API CompilePipelines (const PipelinesInfo *info);


} // AE::PipelineCompiler
