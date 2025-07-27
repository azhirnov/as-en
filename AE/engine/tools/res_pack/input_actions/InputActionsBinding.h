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

	// "ConvertInputActions"
	using ConvertInputActionsFn_t = bool (*) (const InputActionsInfo* info);

} // AE::InputActions
