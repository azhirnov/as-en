// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <dlfcn.h>

# include "base/Utils/Helpers.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Containers/NtStringView.h"
# include "base/Utils/FileSystem.h"

namespace AE::Base
{

    //
    // Unix Dynamic Library
    //

    class UnixLibrary final : public Noncopyable
    {
    // variables
    private:
        void *      _handle = null;


    // methods
    public:
        UnixLibrary ()                                              __NE___ {}
        ~UnixLibrary ()                                             __NE___ { Unload(); }

        ND_ bool  Open (void* lib)                                  __NE___;

        ND_ bool  Load (NtStringView libName)                       __NE___;
        ND_ bool  Load (StringView libName)                         __NE___ { return Load( NtStringView{libName} ); }
        ND_ bool  Load (const char* libName)                        __NE___ { return Load( NtStringView{libName} ); }
        ND_ bool  Load (const Path &libName)                        __NE___;
            void  Unload ()                                         __NE___;

        template <typename T>
        ND_ bool  GetProcAddr (NtStringView name, OUT T &result)    C_NE___;

        ND_ Path  GetPath ()                                        C_NE___;

        ND_ explicit operator bool ()                               C_NE___ { return _handle != null; }
    };



/*
=================================================
    Open
=================================================
*/
    inline bool  UnixLibrary::Open (void* lib) __NE___
    {
        CHECK_ERR( _handle == null and lib != null );
        _handle = lib;
        return _handle != null;
    }

/*
=================================================
    Load
=================================================
*/
    inline bool  UnixLibrary::Load (NtStringView libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
        return _handle != null;
    }

    inline bool  UnixLibrary::Load (const Path &libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
        return _handle != null;
    }

/*
=================================================
    Unload
=================================================
*/
    inline void  UnixLibrary::Unload () __NE___
    {
        if ( _handle != null )
        {
            ::dlclose( _handle );
            _handle = null;
        }
    }

/*
=================================================
    GetProcAddr
=================================================
*/
    template <typename T>
    inline bool  UnixLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
    {
        ASSERT( _handle != null );
        ASSERT( not name.empty() );

        result = BitCast<T>( ::dlsym( _handle, name.c_str() ));
        return result != null;
    }

/*
=================================================
    GetPath
=================================================
*/
    inline Path  UnixLibrary::GetPath () C_NE___
    {
    #if defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_APPLE)
        // not supported
        return Default;
    #else

        CHECK_ERR( _handle != null );

        char    buf [PATH_MAX] = {};
        CHECK_ERR( ::dlinfo( _handle, RTLD_DI_ORIGIN, buf ) == 0 );

        NOTHROW_ERR( return Path{ buf };)
    #endif
    }


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
