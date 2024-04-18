// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	file format:

		uint	magic				(InputActions_Name)
		uint	count

		uint	magic	[count]		(InputActionsAndroid_Name, InputActionsGLFW_Name, InputActionsWinAPI_Name, ...)
		uint	offset	[count]		offsets to data for deserialization, use 'SerializableInputActions'.
*/

#pragma once

#include "base/Common.h"

#ifdef AE_BUILD_INPUT_ACTIONS_BINDING
#	define AE_IA_API	AE_DLL_EXPORT
#else
#	define AE_IA_API	AE_DLL_IMPORT
#endif

namespace AE::InputActions
{
	using AE::usize;
	using AE::CharType;


	struct InputActionsInfo
	{
		// input
		const CharType* const*	inFiles				= null;		// [inFileCount]
		usize					inFileCount			= 0;

		// to use #include in scripts from 'inFiles'
		const CharType * const*	inIncludeFolders		= null;		// [inIncludeFolderCount]
		usize					inIncludeFolderCount	= 0;

		// output
		const CharType *		outputPackName		= null;
		const CharType *		outputScriptFile	= null;
		const CharType *		outputCppFile		= null;		// reflection
	};

	extern "C" bool AE_IA_API ConvertInputActions (const InputActionsInfo* info);

} // AE::InputActions
