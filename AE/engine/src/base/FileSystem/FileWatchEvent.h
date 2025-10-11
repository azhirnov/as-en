// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		Unknown		= 0xFF
	};


	//
	// File Watch Event
	//
	struct FileWatchEvent
	{
		Path				path;
		EFileSystemAction	action;
	};
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_LOGS
/*
=================================================
	ToString (EFileSystemAction)
=================================================
*/
	Nd__In StringView  ToString (EFileSystemAction value)
	{
		switch_enum( value )
		{
			case EFileSystemAction::Added :					return "Added";
			case EFileSystemAction::Removed :				return "Removed";
			case EFileSystemAction::Renamed_OldName :		return "Renamed_OldName";
			case EFileSystemAction::Renamed_NewName :		return "Renamed_NewName";
			case EFileSystemAction::Modified :				return "Modified";
			case EFileSystemAction::Opened :				return "Opened";
			case EFileSystemAction::Error_BufferOverflow :	return "Error_BufferOverflow";
			case EFileSystemAction::Unknown :
			default :										break;
		}
		switch_end
		return Default;
	}

#endif // AE_ENABLE_LOGS

} // AE::Base
