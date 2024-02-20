// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <dlfcn.h>
# include "base/Platforms/UnixLibrary.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{
/*
=================================================
    Open
=================================================
*/
    bool  UnixLibrary::Open (void* lib) __NE___
    {
        CHECK_ERR( _handle == null and lib != null );
        _handle = lib;
        return _handle != null;
    }

/*
=================================================
    Load
----
    RTLD_NOW    - all undefined symbols in the shared object are resolved before dlopen() returns.
    RTLD_GLOBAL - the symbols defined by this shared object will be made available
                  for symbol resolution of subsequently loaded shared objects.
    RTLD_LOCAL  - symbols defined in this shared object are not made available to resolve references in
                  subsequently loaded shared objects.
=================================================
*/
    bool  UnixLibrary::Load (NtStringView libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::dlopen( libName.c_str(), RTLD_NOW | RTLD_LOCAL );

        if_unlikely( _handle == null ){
            AE_LOG_DBG( "Error when loading shared library '"s << libName << "': " << ::dlerror() );
        }
        return _handle != null;
    }

    bool  UnixLibrary::Load (const Path &libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::dlopen( libName.c_str(), RTLD_NOW | RTLD_LOCAL );

        if_unlikely( _handle == null ){
            AE_LOG_DBG( "Error when loading shared library '"s << ToString( libName ) << "': " << ::dlerror() );
        }
        return _handle != null;
    }

/*
=================================================
    Unload
=================================================
*/
    void  UnixLibrary::Unload () __NE___
    {
        if ( _handle != null )
        {
            ::dlclose( _handle );
            _handle = null;
        }
    }

/*
=================================================
    _GetProcAddr
=================================================
*/
    void*  UnixLibrary::_GetProcAddr (const char* name) C_NE___
    {
        ASSERT( _handle != null );
        return ::dlsym( _handle, name );
    }

/*
=================================================
    GetPath
=================================================
*/
    Path  UnixLibrary::GetPath () C_NE___
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
