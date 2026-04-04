// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/BitMath.h"

namespace AE::PipelineCompiler
{
	using AE::uint;
	using AE::usize;
	using AE::CharType;


	enum class EPathParamsFlags : uint
	{
		Unknown						= 0,		// auto-detect
		File						= 1 << 0,
		Folder						= 1 << 1,
		RecursiveFolder				= 1 << 2,
		_Last,
		All							= ((_Last - 1) << 1) - 1,
	};


	enum class EReflectionFlags : uint
	{
		Unknown						= 0,
		RenderTechniques			= 1 << 0,
		RTechPass_Pipelines			= 1 << 1,
		RTech_ShaderBindingTable	= 1 << 2,
		_Last,
		All							= ((_Last - 1) << 1) - 1,
	};


	enum class EPipelineCompilerFlags : uint
	{
		Unknown							= 0,
		AddNameMapping					= 1 << 0,	// for debugging
		IncludePipelinesFromCurrentDir	= 1 << 1,
		SearchShadersInCurrentDir		= 1 << 2,
		_BITOPS_
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
		EPipelineCompilerFlags	flags					= EPipelineCompilerFlags::Unknown;

		// input pipelines
		const PathParams *		inPipelines				= null;		// [inPipelineCount]	// files or folders
		usize					inPipelineCount			= 0;

		// input shaders
		const CharType * const*	shaderFolders			= null;		// [shaderFolderCount]
		usize					shaderFolderCount		= 0;

		// shader include directories
		const CharType * const*	shaderIncludeDirs		= null;		// [shaderIncludeDirCount]
		usize					shaderIncludeDirCount	= 0;

		// pipeline include directories
		const CharType * const*	pipelineIncludeDirs		= null;		// [pipelineIncludeDirCount]
		usize					pipelineIncludeDirCount	= 0;

		// output
		const CharType *		outputPackName			= null;
		EReflectionFlags		cppReflectionFlags		= EReflectionFlags::Unknown;
		const CharType *		outputCppStructsFile	= null;		// C++ reflection
		const CharType *		outputCppNamesFile		= null;		// C++ reflection
		const CharType *		outputScriptFile		= null;		// script reflection
		const CharType *		outputShaderHeader		= null;		// C++ / GLSL / MSL reflection to use in shader
	};


	// "CompilePipelines"
	using CompilePipelinesFn_t = bool (*) (const PipelinesInfo* info);

} // AE::PipelineCompiler
