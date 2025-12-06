// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
		https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
		https://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
*/

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "base/FileSystem/WindowsFileWatch.h"
# include "base/FileSystem/FileSystem.h"

namespace AE::Base
{
namespace
{
/*
=================================================
	EnumCast (EFileSystemWatchBits)
=================================================
*/
	ND_ static DWORD  EnumCast (EFileSystemWatchBits events) __NE___
	{
		DWORD	filter = 0;

		for (EFileSystemWatch bit : events)
		{
			switch_enum( bit )
			{
				case EFileSystemWatch::FileCreated :	filter |= FILE_NOTIFY_CHANGE_FILE_NAME;		break;
				case EFileSystemWatch::FileOpened :													break;	// not supported
				case EFileSystemWatch::FileModified :	filter |= FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;	break;
				case EFileSystemWatch::FileAttribs :	filter |= FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SECURITY;  break;
				case EFileSystemWatch::FileClosed :		filter |= FILE_NOTIFY_CHANGE_LAST_ACCESS;	break;
				case EFileSystemWatch::FileDeleted :	filter |= FILE_NOTIFY_CHANGE_FILE_NAME;		break;

				case EFileSystemWatch::DirCreated :		filter |= FILE_NOTIFY_CHANGE_DIR_NAME;		break;
				case EFileSystemWatch::DirOpened :													break;	// not supported
				case EFileSystemWatch::DirModified :	filter |= FILE_NOTIFY_CHANGE_ATTRIBUTES;	break;	// TODO: check
				case EFileSystemWatch::DirClosed :		filter |= FILE_NOTIFY_CHANGE_LAST_ACCESS;	break;
				case EFileSystemWatch::DirDeleted :		filter |= FILE_NOTIFY_CHANGE_DIR_NAME;		break;

				case EFileSystemWatch::_Count :
				default :								DBG_WARNING( "unsupported event type" );
			}
			switch_end
		}
		return filter;
	}

/*
=================================================
	EnumCast (FILE_ACTION_*)
=================================================
*/
	ND_ static EFileSystemAction  EnumCast (DWORD event) __NE___
	{
		switch ( event )
		{
			case FILE_ACTION_ADDED :				return EFileSystemAction::Added;
			case FILE_ACTION_REMOVED :				return EFileSystemAction::Removed;
			case FILE_ACTION_MODIFIED :				return EFileSystemAction::Modified;
			case FILE_ACTION_RENAMED_OLD_NAME :		return EFileSystemAction::Renamed_OldName;
			case FILE_ACTION_RENAMED_NEW_NAME :		return EFileSystemAction::Renamed_NewName;
		}
		return Default;
	}

} // namespace


/*
=================================================
	Start
=================================================
*/
	bool  WindowsFileWatch::Start (const Path &path, EFileSystemWatchBits events, Bool recursive, Bytes bufferSize) __NE___
	{
		StaticAssert( sizeof(_overlapped) == sizeof(OVERLAPPED) );

		CHECK_ERR( FileSystem::IsDirectory( path ));
		CHECK_ERR( _dir == null );

		_recursive = recursive;

		_filter = EnumCast( events );
		CHECK_ERR( _filter != 0 );

		NOTHROW_ERR( _buffer.resize( usize{bufferSize} ));

		_dir = ::CreateFileW( path.native().c_str(),
							  FILE_LIST_DIRECTORY,
							  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
							  null,
							  OPEN_EXISTING,
							  FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
							  null );

		if_unlikely( _dir == null )
		{
			WIN_CHECK_DEV( "CreateFile failed: " );
			return false;
		}

		_event = ::CreateEventA( null, TRUE, FALSE, null );
		CHECK_ERR( _event != null );

		return _Request();
	}

/*
=================================================
	Stop
=================================================
*/
	bool  WindowsFileWatch::_Request () __NE___
	{
		auto&	ov = _overlapped.Ref<OVERLAPPED>();
		ZeroMem( OUT ov );

		ov.hEvent = _event;

		::ResetEvent( _event );

		BOOL ok = ::ReadDirectoryChangesW(	_dir,				// winxp
											_buffer.data(),
											DWORD(ArraySizeOf( _buffer )),
											_recursive, // subtree
											_filter,
											null,
											&ov,
											null );
		if_unlikely( not ok )
		{
			WIN_CHECK_DEV( "ReadDirectoryChanges failed: " );
			return false;
		}

		return true;
	}

/*
=================================================
	Stop
=================================================
*/
	void  WindowsFileWatch::Stop () __NE___
	{
		if ( _dir == null )
			return;

		auto&	ov = _overlapped.Ref<OVERLAPPED>();
		::CancelIoEx( _dir, &ov );	// winvista

		::CloseHandle( _event );
		_event = null;

		::CloseHandle( _dir );
		_dir = null;

		Reconstruct( _buffer );
	}

/*
=================================================
	GetEvents
=================================================
*/
	bool  WindowsFileWatch::GetEvents (OUT EventArray_t &outEvents, milliseconds timeout) __NE___
	{
		outEvents.clear();
		if ( _dir == null or _event == null )
			return false;

		auto&	ov		= _overlapped.Ref<OVERLAPPED>();
		DWORD	written = 0;

		if ( timeout != Default )
		{
			DWORD res = ::WaitForSingleObject( _event, DWORD(timeout.count()) );
			if ( res != WAIT_OBJECT_0 )
				return false;
		}

		if_unlikely( ::GetOverlappedResult( _dir, &ov, OUT &written, FALSE ) == FALSE )	// winxp
		{
			auto	err = ::GetLastError();
			if ( err != ERROR_IO_INCOMPLETE )
			{
				WIN_CHECK_DEV2( err, "GetOverlappedResult failed: " );
			}
			return false;
		}

		ASSERT_MSG( written != 0, "buffer overflow!" );
		if ( written == 0 )
		{
			NOTHROW( outEvents.emplace_back( Path{}, EFileSystemAction::Error_BufferOverflow ));

			CHECK( _Request() );
			return true;
		}

		auto*	event	= Cast<FILE_NOTIFY_INFORMATION>( _buffer.data() );
		void*	mem_end	= _buffer.data() + Bytes{written};

		for (;;)
		{
			void*	event_end = event + Bytes{offsetof(FILE_NOTIFY_INFORMATION, FileName)} + Bytes{event->FileNameLength};
			CHECK_ERR( event_end <= mem_end, true );

			NOTHROW_ERR( outEvents.emplace_back( Path{ WString{ event->FileName, event->FileNameLength / sizeof(WCHAR) }},
												 EnumCast( event->Action )),
						 true );

			if_unlikely( event->NextEntryOffset == 0 )
				break;

			event = event + Bytes{event->NextEntryOffset};
		}

		CHECK( _Request() );
		return true;
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
