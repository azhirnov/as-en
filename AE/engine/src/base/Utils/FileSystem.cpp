// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/FileSystem.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Platforms/WindowsHeader.h"

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
        CHECK_ERR( Exists( base ));

        Path    curr = ToAbsolute( base );

        for (; not curr.empty(); --depth)
        {
            result = (Path{ curr } /= ref);

            if ( Exists( result ))
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

            if ( FileSystem::Exists( result ))
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
        CHECK_ERR( Exists( base ));

        const Path  curr = ToAbsolute( base );

        result = (Path{ curr } /= ref);

        if ( FileSystem::Exists( result ))
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
        CHECK_ERR( Exists( base ));

        Path    curr    = ToAbsolute( base );
        uint    depth   = backwardDepth;

        for (; not curr.empty(); --depth)
        {
            result = (Path{ curr } /= ref);

            if ( Exists( result ))
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
    uint    len = ::GetWindowsDirectoryW( buf, uint(CountOf( buf )) );

    return Path{ WStringView{ buf, len }};
}
#endif // AE_PLATFORM_WINDOWS


} // AE::Base
