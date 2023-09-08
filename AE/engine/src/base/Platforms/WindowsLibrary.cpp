// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.h"
# include "base/Platforms/WindowsLibrary.h"

namespace AE::Base
{

/*
=================================================
    Open
=================================================
*/
    bool  WindowsLibrary::Open (void* lib) __NE___
    {
        CHECK_ERR( _handle == null and lib != null );
        _handle = lib;
        return true;
    }

/*
=================================================
    Load
=================================================
*/
    bool  WindowsLibrary::Load (NtStringView libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::LoadLibraryA( libName.c_str() );

        if_unlikely( _handle == null )
            WIN_CHECK_DEV( "LoadLibrary error: " );

        return _handle != null;
    }

    bool  WindowsLibrary::Load (const Path &libName) __NE___
    {
        CHECK_ERR( _handle == null );
        _handle = ::LoadLibraryW( libName.c_str() );

        if_unlikely( _handle == null )
            WIN_CHECK_DEV( "LoadLibrary error: " );

        return _handle != null;
    }

/*
=================================================
    Unload
=================================================
*/
    void  WindowsLibrary::Unload () __NE___
    {
        if ( _handle != null )
        {
            ::FreeLibrary( BitCast<HMODULE>(_handle) );
            _handle = null;
        }
    }

/*
=================================================
    GetPath
=================================================
*/
    Path  WindowsLibrary::GetPath () C_NE___
    {
        CHECK_ERR( _handle != null );

        wchar_t buf[MAX_PATH] = {};
        CHECK_ERR( ::GetModuleFileNameW( BitCast<HMODULE>(_handle), buf, DWORD(CountOf(buf)) ) != FALSE );

        CATCH_ERR( return Path{ buf };)
    }

/*
=================================================
    _GetProcAddress
=================================================
*/
    void*  WindowsLibrary::_GetProcAddress (NtStringView name) C_NE___
    {
        return BitCast<void*>(::GetProcAddress( BitCast<HMODULE>(_handle), name.c_str() ));
    }


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
