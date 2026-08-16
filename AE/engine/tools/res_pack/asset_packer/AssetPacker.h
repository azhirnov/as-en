// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"
#include "base/CompileTime/Math.h"

namespace AE::AssetPacker
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
		All							= Base::CT_AllBitMask2<EPathParamsFlags>,
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


	struct AssetInfo
	{
		const PathParams *		inFiles					= null;		// [inFileCount]	// files or folders
		usize					inFileCount				= 0;

		// to use #include in scripts from 'inFiles'
		const CharType * const*	inIncludeFolders		= null;		// [inIncludeFolderCount]
		usize					inIncludeFolderCount	= 0;

		// used to search images, fonts, etc.
		const CharType * const*	inResourceFolders		= null;		// [inResourceFolderCount]
		usize					inResourceFolderCount	= 0;

		const CharType *		tempFile				= null;
		const CharType *		outputArchive			= null;
		const CharType *		outputScriptFile		= null;
	};

	// "PackAssets"
	using PackAssetsFn_t = bool (*) (const AssetInfo* info);

} // AE::AssetPacker
