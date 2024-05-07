// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/WindowsHeader.cpp.h"
#include "base/FileSystem/FileSystem.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
	FindAndSetCurrent
=================================================
*/
	bool  FileSystem::FindAndSetCurrent (const Path &ref, uint depth)
	{
		return FindAndSetCurrent( CurrentPath(), ref, depth );
	}

	bool  FileSystem::FindAndSetCurrent (const Path &base, const Path &ref, uint depth)
	{
		Path	dir;

		if ( Search( base, ref, depth, depth, OUT dir ))
			return SetCurrentPath( dir );

		return false;
	}

/*
=================================================
	SearchBackward
=================================================
*/
	bool  FileSystem::SearchBackward (const Path &ref, uint depth, OUT Path &result)
	{
		return SearchBackward( CurrentPath(), ref, depth, OUT result );
	}

	bool  FileSystem::SearchBackward (const Path &base, const Path &ref, uint depth, OUT Path &result)
	{
		CHECK_ERR( IsDirectory( base ));

		Path	curr = ToAbsolute( base );

		for (; not curr.empty(); --depth)
		{
			result = (Path{ curr } /= ref);

			if ( IsFileOrDirectory( result ))
				return true;

			if ( depth == 0 )
				break;

			curr = curr.parent_path();
		}

		result.clear();
		return false;
	}

/*
=================================================
	RecursiveSearchForward
=================================================
*/
namespace {
	static bool  RecursiveSearchForward (const Path &curr, const Path &ref, uint depth, OUT Path &result)
	{
		for (auto& dir : FileSystem::Enum( curr ))
		{
			if ( not dir.IsDirectory() )
				continue;

			result = (Path{ dir.Get() } /= ref);

			if ( FileSystem::IsFileOrDirectory( result ))
				return true;

			if ( depth > 0 )
			{
				if ( RecursiveSearchForward( dir.Get(), ref, depth-1, OUT result ))
					return true;
			}
		}

		result.clear();
		return false;
	}
} // namespace

/*
=================================================
	SearchForward
=================================================
*/
	bool  FileSystem::SearchForward (const Path &ref, uint depth, OUT Path &result)
	{
		return SearchForward( CurrentPath(), ref, depth, OUT result );
	}

	bool  FileSystem::SearchForward (const Path &base, const Path &ref, uint depth, OUT Path &result)
	{
		CHECK_ERR( IsDirectory( base ));

		const Path	curr = ToAbsolute( base );

		result = (Path{ curr } /= ref);

		if ( FileSystem::IsFileOrDirectory( result ))
			return true;

		return RecursiveSearchForward( curr, ref, depth, OUT result );
	}

/*
=================================================
	Search
=================================================
*/
	bool  FileSystem::Search (const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result)
	{
		return Search( CurrentPath(), ref, backwardDepth, forwardDepth, OUT result );
	}

	bool  FileSystem::Search (const Path &base, const Path &ref, const uint backwardDepth, const uint forwardDepth, OUT Path &result)
	{
		CHECK_ERR( IsDirectory( base ));

		Path	curr	= ToAbsolute( base );
		uint	depth	= backwardDepth;

		for (; not curr.empty(); --depth)
		{
			result = (Path{ curr } /= ref);

			if ( IsFileOrDirectory( result ))
				return true;

			if ( depth == 0 )
				break;

			if ( SearchForward( curr, ref, forwardDepth, OUT result ))
				return true;

			curr = curr.parent_path();
		}

		result.clear();
		return false;
	}

/*
=================================================
	FindUnusedFilename
=================================================
*/
	bool  FileSystem::FindUnusedFilename (const Function< void (OUT Path &name, usize idx) >&	buildName,
										  const Function< bool (const Path &) > &				consume,
										  const uint											maxStep) __Th___
	{
		CHECK_ERR( buildName and consume );

		Path	fname;
		usize	min_index	= 0;
		usize	max_index	= 1;

		const usize	max_iter	= 10'000;
		const usize	max_files	= 10'000;

		// find range
		for (; min_index < max_index;)
		{
			buildName( OUT fname, max_index );

			if ( not FileSystem::IsFile( fname ))
				break;

			min_index = max_index;
			max_index += maxStep;
		}

		// binary search in range
		usize2	range { min_index, max_index };

		for_likely(; range.x+2 < range.y; )
		{
			usize	mid = range.x + ((range.y - range.x) >> 1);

			buildName( OUT fname, mid );

			range = (FileSystem::IsFile( fname ) ?
						usize2{mid + 1, range.y} :
						usize2{range.x, mid});
		}

		// search from lower bound
		for (usize i = 0, fcount = 0;
			 i <= max_iter and fcount < max_files;
			 ++i)
		{
			buildName( OUT fname, range.x + i );

			if ( FileSystem::IsFile( fname ))
				continue;

			++fcount;

			if ( consume( fname ))
				return true;  // exit

			// add new file
		}

		RETURN_ERR( "failed or incomplete" );
	}

/*
=================================================
	MakeUniqueName
=================================================
*/
	bool  FileSystem::MakeUniqueName (INOUT Path &inoutPath) __Th___
	{
		const Path	folder	= inoutPath.parent_path();
		const Path	name	= inoutPath.stem();
		const Path	ext		= inoutPath.extension();

		const auto	BuildName = [&] (OUT Path &fname, usize idx)
		{{
			fname = folder / (((Path{name} += '-') += ToString(idx)) += ext);
		}};

		const auto	Consume = [&] (const Path &path) -> bool
		{{
			inoutPath = FileSystem::Normalize( path );
			return true; // exit
		}};

		return FindUnusedFilename( BuildName, Consume );
	}

/*
=================================================
	CreateEmptyFile
=================================================
*/
	bool  FileSystem::CreateEmptyFile (const Path &p) __NE___
	{
	#ifdef AE_PLATFORM_WINDOWS

		FILE*	file = null;
		_wfopen_s( OUT &file, p.native().c_str(), L"w" );
		if ( file != null ) fclose( file );
		return file != null;

	#else

		FILE*	file = fopen( p.native().c_str(), "w" );
		if ( file != null ) fclose( file );
		return file != null;

	#endif
	}

/*
=================================================
	ToShortPath
=================================================
*/
	StringView  FileSystem::ToShortPath (StringView file) __NE___
	{
		const uint	max_parts = 3;

		usize	i = Max( file.length(), 1u ) - 1;
		uint	j = 0;

		for (; i < file.length() and j < max_parts; --i)
		{
			const char	c = file[i];

			if_unlikely( (c == '\\') or (c == '/') )
				++j;
		}

		if ( i < file.length() )
			return file.substr( i + (j == max_parts ? 2 : 0) );
		else
			return file;
	}

/*
=================================================
	MergeDirectory
=================================================
*/
	bool  FileSystem::MergeDirectory (const Path &from, const Path &to, const ECopyOpt options) __NE___
	{
		CHECK_ERR( IsDirectory( from ));

		if ( options == ECopyOpt::FileReplace )
			return CopyDirectory( from, to );

		if ( options == ECopyOpt::ReplaceAll )
		{
			return	DeleteDirectory( to )	and
					CopyDirectory( from, to );
		}

		if ( options == ECopyOpt::FileKeep )
		{
			std::error_code	ec;
			const auto		opt = _ae_fs_::copy_options::recursive | _ae_fs_::copy_options::skip_existing;
			_ae_fs_::copy( from, to, opt, OUT ec );
			ASSERT_MSG( not ec, "MergeDirectory('" + from.string() + "', '" + to.string() + "'): " + ec.message() );
			return not ec;
		}

		bool	ok = true;

		for (auto& entry : EnumRecursive(from))
		{
			const Path	src = entry.Get();
			const Path	dst = to / ToRelative( src, from );

			if ( entry.IsDirectory() )
			{
				CreateDirectories( dst );
			}
			else
			if ( entry.IsFile() )
			{
				const bool	exists = IsFile( dst );
				switch_enum( options )
				{
					case ECopyOpt::FileNewest :
					{
						if ( exists ){
							if ( LastWriteTime( src ) > LastWriteTime( dst ))
								ok &= CopyFile( src, dst );
						}else
							ok &= CopyFile( src, dst );
						break;
					}
					case ECopyOpt::FileLargest :
					{
						if ( exists ){
							if ( FileSize( src ) > FileSize( dst ))
								ok &= CopyFile( src, dst );
						}else
							ok &= CopyFile( src, dst );
						break;
					}
					case ECopyOpt::FileKeepBoth :
					{
						if ( exists ){
							Path	new_dst = dst;
							if ( MakeUniqueName( INOUT new_dst ))
								ok &= CopyFile( src, new_dst );
						}else
							ok &= CopyFile( src, dst );
						break;
					}

					case ECopyOpt::ReplaceAll :
					case ECopyOpt::FileReplace :
					case ECopyOpt::FileKeep :
					default :
						DBG_WARNING( "unsupported option" );
				}
				switch_end
			}
		}
		return ok;
	}
//-----------------------------------------------------------------------------


#ifdef AE_PLATFORM_WINDOWS
/*
=================================================
	GetWindowsPath
=================================================
*/
	Path  FileSystem::GetWindowsPath ()
	{
		wchar_t buf[MAX_PATH];
		uint	len = ::GetWindowsDirectoryW( buf, uint(CountOf( buf )) );  // win2000

		return Path{ WStringView{ buf, len }};
	}

// TODO:
// 	SHGetKnownFolderPath
//  https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid

#endif // AE_PLATFORM_WINDOWS


} // AE::Base
