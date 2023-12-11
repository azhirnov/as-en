// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/WindowsHeader.cpp.h"
#include "base/Utils/FileSystem.h"
#include "base/Algorithms/ArrayUtils.h"

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
        Path    dir;

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

        Path    curr = ToAbsolute( base );

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

        const Path  curr = ToAbsolute( base );

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

        Path    curr    = ToAbsolute( base );
        uint    depth   = backwardDepth;

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
    void  FileSystem::FindUnusedFilename (const Function< void (OUT Path &name, usize idx) >&   buildName,
                                          const Function< bool (const Path &) > &               consume) __Th___
    {
        CHECK_ERRV( buildName and consume );

        Path        fname;
        usize       min_index   = 0;
        usize       max_index   = 1;
        const usize step        = 100;

        for (;;)
        {
            for (; min_index < max_index;)
            {
                buildName( OUT fname, max_index );

                if ( not FileSystem::IsFile( fname ))
                    break;

                min_index = max_index;
                max_index += step;
            }

            for (usize index = min_index; index <= max_index; ++index)
            {
                buildName( OUT fname, index );

                if ( FileSystem::IsFile( fname ))
                    continue;

                if ( consume( fname ))
                    return;

                break;
            }
        }
    }

/*
=================================================
    CreateEmptyFile
=================================================
*/
    bool  FileSystem::CreateEmptyFile (const Path &p) __NE___
    {
    #ifdef AE_PLATFORM_WINDOWS

        FILE*   file = null;
        _wfopen_s( OUT &file, p.native().c_str(), L"w" );
        if ( file != null ) fclose( file );
        return file != null;

    #else

        FILE*   file = fopen( p.native().c_str(), "w" );
        if ( file != null ) fclose( file );
        return file != null;

    #endif
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
        uint    len = ::GetWindowsDirectoryW( buf, uint(CountOf( buf )) );  // win2000

        return Path{ WStringView{ buf, len }};
    }

// TODO:
//  SHGetKnownFolderPath
//  https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid

#endif // AE_PLATFORM_WINDOWS


} // AE::Base
