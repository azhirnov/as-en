// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://renderdoc.org/docs/in_application_api.html
*/

#include "graphics_rhi/Vulkan/Utils/RenderDocApi.h"

#ifdef AE_ENABLE_RENDERDOC
# ifndef AE_ENABLE_VULKAN
#	error RenderDocAPI only compatible with Vulkan backend
# endif

# include "renderdoc_app.h"

namespace AE::Graphics
{
namespace
{
	using RDocApi_t	= RENDERDOC_API_1_6_0;
}

/*
=================================================
	EnableVkLayer
=================================================
*/
	bool  RenderDocApi::EnableVkLayer () __NE___
	{
		// see 'enable_environment' field in 'renderdoc.json'
		return PlatformUtils::SetEnvironmentVariable( "ENABLE_VULKAN_RENDERDOC_CAPTURE", "1" );
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool  RenderDocApi::Initialize (VkInstance instance) __NE___
	{
		constexpr Version3			min_ver		{ 1, 4, 0 };
		constexpr RENDERDOC_Version	min_ver2	= RENDERDOC_Version( (min_ver.major * 10000) + (min_ver.minor * 100) + (min_ver.patch) );

		if ( _api != null )
			return true;

		#ifdef AE_PLATFORM_WINDOWS
			CHECK_ERR( _lib.Open( "renderdoc.dll" ));
		#else
			CHECK_ERR( _lib.Open( "librenderdoc.so" ));
		#endif

		pRENDERDOC_GetAPI	fn;
		CHECK_ERR( _lib.GetProcAddr( "RENDERDOC_GetAPI", OUT fn ));

		RDocApi_t*	rdoc_api = null;
		CHECK_ERR( fn( min_ver2, OUT reinterpret_cast<void**>(&rdoc_api) ) == 1 );

		int	major, minor, patch;
		rdoc_api->GetAPIVersion( OUT &major, OUT &minor, OUT &patch );
		CHECK_ERR( Version3(major, minor, patch) >= min_ver );

		// setup
		{
			rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_RefAllResources,		1 );	// default: 0
			rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_CaptureAllCmdLists,	1 );
			rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_VerifyBufferAccess,	0 );
			rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_CaptureCallstacks,		0 );
			rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_APIValidation,			0 );	// already enabled

			// disable key bindings
			rdoc_api->SetCaptureKeys( null, 0 );
			rdoc_api->SetFocusToggleKeys( null, 0 );

			// hide UI
			rdoc_api->MaskOverlayBits( eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None );

			//rdoc_api->UnloadCrashHandler();
		}

		_api	= rdoc_api;
		_device	= RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE( instance );

		CHECK_ERR( _device != null );
		return true;
	}
	
/*
=================================================
	Deinitialize
=================================================
*/
	void  RenderDocApi::Deinitialize () __NE___
	{
		_api = null;
		_device	= null;
		_wndHandle.store( null );
	}

/*
=================================================
	SetWindow
=================================================
*/
	void  RenderDocApi::SetWindow (const NativeWindow &wndHandle) C_NE___
	{
		if ( _api == null )
			return;

		#if defined(AE_PLATFORM_WINDOWS)
			_wndHandle.store( wndHandle.hWnd );

		#elif defined(AE_PLATFORM_ANDROID)
			_wndHandle.store( wndHandle.nativeWindow );

		#elif defined(AE_PLATFORM_LINUX)
			_wndHandle.store( wndHandle.x11Window );
		#else
		#	error Unsupported platform!
		#endif
			
		auto*	rdoc_api = Cast<RDocApi_t>(_api);
		rdoc_api->SetActiveWindow( _device, _wndHandle.load() );
	}
	
/*
=================================================
	CaptureFolder
=================================================
*/
	void  RenderDocApi::CaptureFolder (NtStringView path) C_NE___
	{
		CHECK_ERRV( not path.empty() );
		
		auto*	rdoc_api = Cast<RDocApi_t>(_api);
		rdoc_api->SetCaptureFilePathTemplate( path.c_str() );
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

		auto*		rdoc_api	= Cast<RDocApi_t>(_api);
		const uint	count		= rdoc_api->GetNumCaptures();

		char		fname [512];

		for (uint i = 0; i < count; ++i)
		{
			uint	path_len	= 0;
			ulong	timestamp	= 0;

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
	bool  RenderDocApi::BeginFrame (NtStringView name) C_NE___
	{
		if ( _api == null )
			return false;
		
		auto*	rdoc_api = Cast<RDocApi_t>(_api);
		rdoc_api->StartFrameCapture( _device, _wndHandle.load() );

		if ( not name.empty() )
			rdoc_api->SetCaptureTitle( name.c_str() );
		
		_captureIdx.Inc();
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

		Cast<RDocApi_t>(_api)->DiscardFrameCapture( _device, _wndHandle.load() );
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

		Cast<RDocApi_t>(_api)->EndFrameCapture( _device, _wndHandle.load() );

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
		
		auto*	rdoc_api = Cast<RDocApi_t>(_api);
		rdoc_api->TriggerCapture();

		_captureIdx.Inc();
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

		if ( count == 0 )
			return false;

		Cast<RDocApi_t>(_api)->TriggerMultiFrameCapture( count );
		
		_captureIdx.fetch_add( count );
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
	bool  RenderDocApi::EnableVkLayer ()								__NE___	{ return false; }
	bool  RenderDocApi::Initialize (VkInstance)							__NE___	{ return false; }
	void  RenderDocApi::Deinitialize ()									__NE___	{}
	void  RenderDocApi::SetWindow (const NativeWindow &)				C_NE___	{}
	void  RenderDocApi::PrintCaptures ()								C_NE___	{}
	void  RenderDocApi::CaptureFolder (NtStringView)					C_NE___ {}
	bool  RenderDocApi::BeginFrame (NtStringView)						C_NE___	{ return false; }
	bool  RenderDocApi::CancelFrame ()									C_NE___	{ return false; }
	bool  RenderDocApi::EndFrame ()										C_NE___	{ return false; }
	bool  RenderDocApi::IsFrameCapturing ()								C_NE___	{ return false; }
	bool  RenderDocApi::TriggerFrameCapture ()							C_NE___	{ return false; }
	bool  RenderDocApi::TriggerMultiFrameCapture (uint)					C_NE___	{ return false; }

} // AE::Graphics

#endif // not AE_ENABLE_RENDERDOC
