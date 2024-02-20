// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    docs:
    https://renderdoc.org/docs/in_application_api.html
*/

#include "graphics/Vulkan/Utils/RenderDocApi.h"

#ifdef AE_ENABLE_RENDERDOC
# ifndef AE_ENABLE_VULKAN
#   error RenderDocAPI only compatible with Vulkan backend
# endif

# include "renderdoc_app.h"

namespace AE::Graphics
{
namespace
{
    using RDocApi_t     = RENDERDOC_API_1_6_0;

    static constexpr Version3           min_ver     { 1, 4, 0 };
    static constexpr RENDERDOC_Version  min_ver2    = RENDERDOC_Version( (min_ver.major * 10000) + (min_ver.minor * 100) + (min_ver.patch) );
}

/*
=================================================
    Initialize
=================================================
*/
    bool  RenderDocApi::Initialize (VkInstance instance, const NativeWindow &wndHandle) __NE___
    {
        if ( _api != null )
            return true;

        CHECK_ERR( _lib.Load( AE_RENDERDOC_LIB ));

        pRENDERDOC_GetAPI   fn;
        CHECK_ERR( _lib.GetProcAddr( "RENDERDOC_GetAPI", OUT fn ));

        RDocApi_t*  rdoc_api = null;
        CHECK_ERR( fn( min_ver2, OUT reinterpret_cast<void**>(&rdoc_api) ) == 1 );

        int major, minor, patch;
        rdoc_api->GetAPIVersion( OUT &major, OUT &minor, OUT &patch );
        CHECK_ERR( Version3(major, minor, patch) >= min_ver );

        // setup
        {
            rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_RefAllResources,       1 );    // default: 0
            rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_CaptureAllCmdLists,    1 );
            rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_VerifyBufferAccess,    0 );
            rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_CaptureCallstacks,     0 );
            rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_APIValidation,         0 );

            RENDERDOC_InputButton   key = eRENDERDOC_Key_PrtScrn;
            rdoc_api->SetCaptureKeys( &key, 1 );
        }

        _api    = rdoc_api;
        _device = RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE( instance );

        #if defined(AE_PLATFORM_WINDOWS)
            _wndHandle  = wndHandle.hWnd;

        #elif defined(AE_PLATFORM_ANDROID)
            _wndHandle  = wndHandle.nativeWindow;

        #elif defined(AE_PLATFORM_LINUX)
            _wndHandle  = wndHandle.x11Window;
        #else
        #   error Unsupported platform!
        #endif

        CHECK_ERR( _device != null );

        rdoc_api->SetActiveWindow( _device, _wndHandle );
        return true;
    }

/*
=================================================
    PrintCaptures
=================================================
*/
    void  RenderDocApi::PrintCaptures () C_NE___
    {
        if ( _api == null )
            return;

        auto*       rdoc_api    = Cast<RDocApi_t>(_api);
        const uint  count       = rdoc_api->GetNumCaptures();

        char        fname [512];

        for (uint i = 0; i < count; ++i)
        {
            uint    path_len    = 0;
            ulong   timestamp   = 0;

            if ( rdoc_api->GetCapture( i, OUT fname, OUT &path_len, OUT &timestamp ) != 1 )
                break;

            AE_LOGI( "RenderDoc capture ["s << ToString(i) << "]: '" << StringView(fname, path_len) << "'" );
        }
    }

/*
=================================================
    BeginFrame
=================================================
*/
    bool  RenderDocApi::BeginFrame () C_NE___
    {
        if ( _api == null )
            return false;

        Cast<RDocApi_t>(_api)->StartFrameCapture( _device, _wndHandle );
        return true;
    }

/*
=================================================
    CancelFrame
=================================================
*/
    bool  RenderDocApi::CancelFrame () C_NE___
    {
        if ( _api == null )
            return false;

        Cast<RDocApi_t>(_api)->DiscardFrameCapture( _device, _wndHandle );
        return true;
    }

/*
=================================================
    EndFrame
=================================================
*/
    bool  RenderDocApi::EndFrame () C_NE___
    {
        if ( _api == null )
            return false;

        Cast<RDocApi_t>(_api)->EndFrameCapture( _device, _wndHandle );

        // TODO: use GetCapture() to print capture name
        return true;
    }

/*
=================================================
    TriggerFrameCapture
=================================================
*/
    bool  RenderDocApi::TriggerFrameCapture () C_NE___
    {
        if ( _api == null )
            return false;

        Cast<RDocApi_t>(_api)->TriggerCapture();
        return true;
    }

/*
=================================================
    TriggerMultiFrameCapture
=================================================
*/
    bool  RenderDocApi::TriggerMultiFrameCapture (uint count) C_NE___
    {
        if ( _api == null )
            return false;

        Cast<RDocApi_t>(_api)->TriggerMultiFrameCapture( count );
        return true;
    }

/*
=================================================
    IsFrameCapturing
=================================================
*/
    bool  RenderDocApi::IsFrameCapturing () C_NE___
    {
        if ( _api == null )
            return false;

        return Cast<RDocApi_t>(_api)->IsFrameCapturing();
    }

} // AE::Graphics
//-----------------------------------------------------------------------------

#else // AE_ENABLE_RENDERDOC

namespace AE::Graphics
{
    bool  RenderDocApi::Initialize (VkInstance, const NativeWindow &)   __NE___ { return false; }
    void  RenderDocApi::PrintCaptures ()                                C_NE___ {}
    bool  RenderDocApi::BeginFrame ()                                   C_NE___ { return false; }
    bool  RenderDocApi::CancelFrame ()                                  C_NE___ { return false; }
    bool  RenderDocApi::EndFrame ()                                     C_NE___ { return false; }
    bool  RenderDocApi::IsFrameCapturing ()                             C_NE___ { return false; }
    bool  RenderDocApi::TriggerFrameCapture ()                          C_NE___ { return false; }
    bool  RenderDocApi::TriggerMultiFrameCapture (uint)                 C_NE___ { return false; }

} // AE::Graphics

#endif // not AE_ENABLE_RENDERDOC
