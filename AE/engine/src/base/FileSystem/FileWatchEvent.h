// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Platforms/PlatformEnums.h"

namespace AE::Base
{

	enum class EFileSystemWatch : ubyte
	{
		FileCreated,
		FileOpened,		// only in Linux/Android
		FileModified,
		FileAttribs,
		FileClosed,
		FileDeleted,

		DirCreated,
		DirOpened,		// remove?
		DirModified,
		DirClosed,		// remove?
		DirDeleted,

		_Count,
	};
	using EFileSystemWatchBits = EnumSet<EFileSystemWatch>;


	enum class EFileSystemAction : ubyte
	{
		Added,
		Removed,
		Renamed_OldName,
		Renamed_NewName,
		Modified,			// write, attribute changed, ...
		Opened,				// only in Linux/Android

		Error_BufferOverflow,
		_Count,
		Unknown		= _Count
	};


	//
	// File Watch Event
	//
	struct FileWatchEvent
	{
		Path				path;
		EFileSystemAction	action;
	};

} // AE::Base
