// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://man7.org/linux/man-pages/man7/inotify.7.html
*/

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
# include "base/Defines/StdInclude.h"

# include <errno.h>
# include <limits.h>
# include <signal.h>
# include <sys/inotify.h>
# include <sys/types.h>
# include <unistd.h>

# include "base/FileSystem/LinuxFileWatch.h"
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
	ND_ static uint  EnumCast (EFileSystemWatchBits events) __NE___
	{
		if ( events.All() )
		{
			return	IN_ALL_EVENTS;
		}

		uint	filter = IN_ONLYDIR;

		// TODO: IN_EXCL_UNLINK

		for (EFileSystemWatch bit : events)
		{
			switch_enum( bit )
			{
				case EFileSystemWatch::FileCreated :	filter |= IN_CREATE;							break;
				case EFileSystemWatch::FileOpened :		filter |= IN_OPEN;								break;
				case EFileSystemWatch::FileModified :	filter |= IN_MODIFY | IN_MOVE_SELF | IN_ATTRIB;	break;
				case EFileSystemWatch::FileClosed :		filter |= IN_CLOSE;								break;
				case EFileSystemWatch::FileDeleted :	filter |= IN_DELETE | IN_DELETE_SELF;			break;

				case EFileSystemWatch::DirCreated :		filter |= IN_CREATE;							break;
				case EFileSystemWatch::DirOpened :		filter |= IN_OPEN;								break;	// TODO: check
				case EFileSystemWatch::DirModified :	filter |= IN_MODIFY | IN_MOVE_SELF | IN_ATTRIB | IN_MOVE;	break;	// TODO: check
				case EFileSystemWatch::DirClosed :		filter |= IN_CLOSE;								break;	// TODO: check
				case EFileSystemWatch::DirDeleted :		filter |= IN_DELETE | IN_DELETE_SELF;			break;	// TODO: check

				case EFileSystemWatch::_Count :
				default :								DBG_WARNING( "unsupported event type" );
			}
			switch_end
		}
		return filter;
	}

/*
=================================================
	EnumCast (IN_*)
=================================================
*/
	ND_ static EFileSystemAction  EnumCast (const int bits) __NE___
	{
		if ( AllBits( bits, IN_CREATE ))
			return EFileSystemAction::Added;

		if ( AnyBits( bits, IN_DELETE | IN_DELETE_SELF ))
			return EFileSystemAction::Removed;

		if ( AnyBits( bits, IN_MODIFY | IN_MOVE_SELF | IN_ATTRIB | IN_CLOSE_WRITE ))
			return EFileSystemAction::Modified;

		if ( AnyBits( bits, IN_MOVED_FROM ))
			return EFileSystemAction::Renamed_OldName;

		if ( AnyBits( bits, IN_MOVED_TO ))
			return EFileSystemAction::Renamed_NewName;

		if ( AnyBits( bits, IN_OPEN ))
			return EFileSystemAction::Opened;

		// TODO: IN_IGNORED, IN_UNMOUNT

		return Default;
	}

	static constexpr uint	c_SubDirFilter = IN_CREATE | IN_DELETE_SELF;

} // namespace


/*
=================================================
	Start
=================================================
*/
	bool  LinuxFileWatch::Start (const Path &path, EFileSystemWatchBits events, Bool recursive, Bytes bufferSize) __NE___
	{
		return Start( path, events, recursive, Default, bufferSize );
	}

/*
=================================================
	Start
----
	linux specific optimization: use 'dirFilter' to disable watch on ignored directory
=================================================
*/
	bool  LinuxFileWatch::Start (const Path &path, EFileSystemWatchBits	events, Bool recursive, DirFilter_t dirFilter, Bytes bufferSize) __NE___
	{
		using BufferElem_t = decltype(_buffer)::value_type;
		StaticAssert( alignof(inotify_event) == alignof(BufferElem_t) );

		ASSERT( bufferSize >= sizeof(inotify_event) + NAME_MAX + 1 );

		CHECK_ERR( FileSystem::IsDirectory( path ));
		CHECK_ERR( _inotifyFd == -1 );

		_inotifyFd= ::inotify_init1( IN_NONBLOCK );
		CHECK_ERR( _inotifyFd >= 0 );

		_filter = EnumCast( events );
		CHECK_ERR( _filter != 0 );

		NOTHROW_ERR( _buffer.resize( usize{bufferSize} ));

		_baseDir 	= FileSystem::ToAbsolute( path );
		_recursive 	= recursive;

		// root
		{
			int wd = ::inotify_add_watch( _inotifyFd, _baseDir.native().c_str(), _filter );
			CHECK_ERR( wd >= 0 );

			WatchDir	dir;
			dir.parent	= Default;
			dir.path	= "";

			_watchMap.emplace( WatchDirId(wd), RVRef(dir) );  // throw
		}

		if ( recursive )
		{
			for (auto entry : FileSystem::EnumRecursive( _baseDir ))
			{
				if ( not entry.IsDirectory() )
					continue;

				if ( dirFilter and not dirFilter( entry ))
					continue;

				// always watch on directory creation and destruction to add/remove watch

				int wd = ::inotify_add_watch( _inotifyFd, entry.Get().native().c_str(), _filter | c_SubDirFilter );
				CHECK_ERR( wd >= 0 );

				WatchDir	dir;
				dir.parent	= Default;
				dir.path	= FileSystem::ToRelative( entry.Get(), _baseDir );

				//AE_LOGI( "Add watch "s << ToString( dir.path ) << "' wd:" << ToString(wd) );

				_watchMap.emplace( WatchDirId(wd), RVRef(dir) );  // throw
			}
		}

		return true;
	}

/*
=================================================
	Stop
=================================================
*/
	void  LinuxFileWatch::Stop () __NE___
	{
		for (auto& [wd, entry] : _watchMap)
		{
			::inotify_rm_watch( _inotifyFd, int(wd) );

			ASSERT_MSG( FileSystem::IsDirectory( _baseDir / entry.path ),
				"has file watch for non existing directory '"s << ToString(entry.path) << "'" );
		}
		Reconstruct( _watchMap );

		::close( _inotifyFd );
		_inotifyFd = -1;

		Reconstruct( _buffer );
	}

/*
=================================================
	GetEvents
=================================================
*/
	bool  LinuxFileWatch::GetEvents (OUT EventArray_t &outEvents, milliseconds timeoutMS) __NE___
	{
		outEvents.clear();

		fd_set	watch_set;
		FD_ZERO( &watch_set );
		FD_SET( _inotifyFd, &watch_set );

		timeval	timeout;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 0;

		if ( timeoutMS.count() >= 1000 )
		{
			timeout.tv_sec = timeoutMS.count() / 1000;
			timeout.tv_usec = (timeoutMS.count() - timeout.tv_sec * 1000) * 1'000'000;
		}
		else
		{
			timeout.tv_usec = timeoutMS.count() * 1'000'000;
		}

		int	count = ::select( _inotifyFd + 1, &watch_set, null, null, &timeout );
		if ( count == 0 )
			return false;  // timeout

		if_unlikely( count < 0 )
		{
			UNIX_CHECK_DEV( "select() failed: " );
			return false;
		}

		int readn = ::read( _inotifyFd, OUT _buffer.data(), int{ArraySizeOf(_buffer)} );
		if ( readn == 0 )
			return true;  // empty

		if_unlikely( readn < 0 )
		{
			UNIX_CHECK_DEV( "read() failed: " );
			return false;
		}

		void*				ptr = _buffer.data();
		void const* const	end	= ptr + Bytes{uint(readn)};

		for (; ptr < end;)
		{
			auto&	event = *static_cast< inotify_event const *>( ptr );

			ptr += Bytes{ sizeof(inotify_event) + event.len };
			ASSERT( ptr <= end );

			if_unlikely( event.mask == IN_Q_OVERFLOW )
			{
				NOTHROW( outEvents.emplace_back( Path{}, EFileSystemAction::Error_BufferOverflow ));
				break;
			}


			// remove watch
			if ( AllBits( event.mask, IN_DELETE_SELF ))
			{
				//AE_LOGI( "Remove watch wd:"s << ToString(event.wd) );

				::inotify_rm_watch( _inotifyFd, event.wd );
				_watchMap.erase( WatchDirId(event.wd) );
			}

			if ( event.len == 0 or event.name[0] == 0 )
				continue;

			// relative to '_baseDir'
			Path path;
			{
				auto it = _watchMap.find( WatchDirId(event.wd) );
				if_unlikely( it == _watchMap.end() )
				{
					DBG_WARNING( "'wd' is not exists in map" );
					continue;
				}

				path = Path{it->second.path} / event.name;
			}

			// add watch
			if ( AllBits( event.mask, IN_CREATE | IN_ISDIR ) and _recursive )
			{
				Path	abs_path = _baseDir / path;
				ASSERT( abs_path.is_absolute() );
				
				int wd = ::inotify_add_watch( _inotifyFd, abs_path.native().c_str(), _filter | c_SubDirFilter );
				CHECK_ERR( wd >= 0 );

				WatchDir	dir;
				dir.parent	= Default;
				dir.path	= path;

				//AE_LOGI( "Add watch "s << ToString( dir.path ) << "' wd:" << ToString(wd) );

				_watchMap.emplace( WatchDirId(wd), RVRef(dir) );  // throw
			}

			if ( NoBits( event.mask, _filter ))
				continue;

			auto action = EnumCast( event.mask );
			if_unlikely( action == Default )
				continue;  // unknown event

			NOTHROW_ERR( outEvents.emplace_back( RVRef(path), action ), true );
		}
		return not outEvents.empty();
	}


} // AE::Base

#endif // AE_PLATFORM_LINUX or AE_PLATFORM_ANDROID
