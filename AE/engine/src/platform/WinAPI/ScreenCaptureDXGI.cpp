// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "platform/WinAPI/ScreenCaptureDXGI.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4668)
#	pragma warning (disable: 4005)
#	pragma warning (disable: 5039)
# endif

# include <Windows.h>
# include <d3d11.h>
# include <dxgi1_6.h>
# ifdef AE_DEBUG
#	include <dxgidebug.h>
# endif
# ifdef AE_ENABLE_VULKAN
#	include "vulkan/vulkan_win32.h"
# endif
# include "base/Defines/Undef.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif

# include "res_loaders/DDS/DDSUtils.cpp.h"

namespace AE::App
{
	using namespace AE::Graphics;

/*
=================================================
	destructor
=================================================
*/
	ScreenCaptureDXGI::~ScreenCaptureDXGI () __NE___
	{
		CHECK( _dxDevice == null );
		CHECK( not _looping.load() );
		CHECK( _complete.load() );
	}

/*
=================================================
	_InitDX11
=================================================
*/
	bool  ScreenCaptureDXGI::_InitDX11 () __NE___
	{
		CHECK_ERR( _dxDevice == null );

		if ( not _dx11Lib.Load( "d3d11.dll" ))
			return false;

		decltype(&::D3D11CreateDevice)		create_dev;
		CHECK_ERR( _dx11Lib.GetProcAddr( "D3D11CreateDevice", OUT create_dev ));

		Unused( _dxgiLib.Load( "dxgi.dll" ));

		decltype(&::CreateDXGIFactory2)		create_factory;
		Unused( _dxgiLib.GetProcAddr( "CreateDXGIFactory2", OUT create_factory ));

		decltype(&::DXGIGetDebugInterface1)	get_debug_interface;
		Unused( _dxgiLib.GetProcAddr( "DXGIGetDebugInterface1", OUT get_debug_interface ));

		HRESULT					hr;
		ComPtr<IDXGIAdapter1>	selected_adapter;

		// find adapter
		{
			ComPtr<IDXGIFactory4>	factory;
			if ( create_factory )
			{
				UINT	flags = 0;
				DEBUG_ONLY( flags = DXGI_CREATE_FACTORY_DEBUG; )

				hr = create_factory( flags, __uuidof(IDXGIFactory4), OUT factory.VRef() );
				CHECK_ERR_MSG( SUCCEEDED(hr),
					"Failed to create DXGIFactory" );
			}
			CHECK_ERR( _FindAdapter( factory.GetOrNull(), OUT selected_adapter.VRef() ));
		}

		// setup debug layer
	  #ifdef AE_DEBUG
		if ( get_debug_interface != null )
		{
			get_debug_interface( 0, __uuidof(IDXGIDebug1),		OUT &_dxDebug );
			get_debug_interface( 0, __uuidof(IDXGIInfoQueue),	OUT &_dxDebugQueue );

			static constexpr GUID  AE_DXGI_DEBUG_ALL = { 0xe48ae283, 0xda80, 0x490b, { 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x08 } };

			if ( _dxDebug and _dxDebugQueue )
			{
				Cast<IDXGIDebug1>(_dxDebug)->EnableLeakTrackingForThread();

				DXGI_INFO_QUEUE_MESSAGE_SEVERITY	deny_severities [] = {
					DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO,
					DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE
				};

				DXGI_INFO_QUEUE_FILTER	dxgi_filter = {};
				dxgi_filter.DenyList.NumSeverities	= uint(CountOf( deny_severities ));
				dxgi_filter.DenyList.pSeverityList	= deny_severities;

				auto*	dbg_queue = Cast<IDXGIInfoQueue>(_dxDebugQueue);
				dbg_queue->PushRetrievalFilter( AE_DXGI_DEBUG_ALL, &dxgi_filter );
				dbg_queue->PushStorageFilter(   AE_DXGI_DEBUG_ALL, &dxgi_filter );
				dbg_queue->SetBreakOnSeverity(  AE_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,	true );
				dbg_queue->SetBreakOnSeverity(  AE_DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,		true );
			}
		}
	  #endif

		// create device
		const D3D_FEATURE_LEVEL feature_levels [] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		for (uint dbg_device = 0; dbg_device < 2 and _dxDevice == null; ++dbg_device)
		{
			#ifndef AE_DEBUG
				if ( dbg_device == 0 )
					continue;
			#endif

			const UINT flags = (dbg_device == 0 ? D3D11_CREATE_DEVICE_DEBUG : 0);

			for (uint fl = 0; fl < CountOf(feature_levels); ++fl)
			{
				D3D_FEATURE_LEVEL	feature_level;
				hr = create_dev( selected_adapter.GetOrNull(), D3D_DRIVER_TYPE_HARDWARE, null, flags, &feature_levels[fl], 1,
								 D3D11_SDK_VERSION, OUT Cast<ID3D11Device*>(&_dxDevice), OUT &feature_level,
								 OUT Cast<ID3D11DeviceContext*>(&_dxContext) );

				if ( SUCCEEDED(hr) )
					break;
			}
		}
		CHECK_ERR_MSG( _dxDevice != null,
			"Failed to create DX device" );

		return true;
	}

/*
=================================================
	_OpenDesktopInThread
=================================================
*/
	bool  ScreenCaptureDXGI::_OpenDesktopInThread () __NE___
	{
		HDESK	current_desktop = ::OpenInputDesktop( 0, FALSE, GENERIC_ALL );
		CHECK_ERR( current_desktop != null );

		// Attach desktop to this thread (only if it is new thread)
		bool desktop_attached = ::SetThreadDesktop( current_desktop ) != 0;
		::CloseDesktop( current_desktop );
		current_desktop = null;

		if ( not desktop_attached )
		{
			WIN_CHECK_DEV( "SetThreadDesktop: " );
			return false;
		}
		return true;
	}

/*
=================================================
	_InitDuplication
=================================================
*/
	bool  ScreenCaptureDXGI::_InitDuplication () __NE___
	{
		CHECK_ERR( _dxDevice != null );
		CHECK_ERR( _desktopDuplication == null );

		ComPtr<IDXGIDevice>		dxgi_device;
		ComPtr<IDXGIAdapter>	dxgi_adapter;
		ComPtr<IDXGIOutput>		dxgi_output;
		ComPtr<IDXGIOutput1>	dxgi_output1;
		ComPtr<IDXGIOutput5>	dxgi_output5;

		HRESULT		hr = Cast<ID3D11Device>(_dxDevice)->QueryInterface(__uuidof(IDXGIDevice), OUT dxgi_device.VRef() );
		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to get DXGI device" );

		hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), OUT dxgi_adapter.VRef() );
		dxgi_device = null;

		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to get DXGI adapter" );

		uint	output_id = 0;
		for (; output_id < 8; ++output_id)
		{
			hr = dxgi_adapter->EnumOutputs( output_id, OUT &dxgi_output );
			if ( FAILED(hr) )
				break;

			DXGI_OUTPUT_DESC	desc = {};
			dxgi_output->GetDesc( OUT &desc );

			if ( desc.Monitor == _config.monitorHandle or _config.monitorHandle == null )
			{
				_config.monitorHandle = desc.Monitor;
				break;
			}

			dxgi_output = null;
		}

		dxgi_adapter = null;
		CHECK_ERR_MSG( dxgi_output,
			"Failed to create DXGI output" );

		AE_LOGI( "Start desktop duplication on output ("s << ToString(output_id) << ")" );

		hr = dxgi_output->QueryInterface(__uuidof(IDXGIOutput1), OUT dxgi_output1.VRef() );
		dxgi_output = null;
		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to get DXGI Output1" );

		dxgi_output1->QueryInterface(__uuidof(IDXGIOutput5), OUT dxgi_output5.VRef() );

		if ( dxgi_output5 )
		{
			// docs:
			// "This method allows directly receiving the original back buffer format used by a running fullscreen application.
			//  For comparison, using the original DuplicateOutput function always converts the fullscreen surface to a 32-bit BGRA format."

			FixedArray< DXGI_FORMAT, 8 >	dxgi_formats;

			if ( _config.surfaceFormats.empty() )
			{
				dxgi_formats.push_back( DXGI_FORMAT_R16G16B16A16_FLOAT );
				dxgi_formats.push_back( DXGI_FORMAT_R10G10B10A2_UNORM );
				dxgi_formats.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
				dxgi_formats.push_back( DXGI_FORMAT_B8G8R8A8_UNORM );
			}
			else
			{
				for (auto ae_fmt : _config.surfaceFormats) {
					dxgi_formats.push_back( DXGI_FORMAT(ResLoader::PixelFormatToDDSFormat( ae_fmt )) );
				}
			}

			hr = dxgi_output5->DuplicateOutput1(
						Cast<ID3D11Device>(_dxDevice),
						0,
						UINT(dxgi_formats.size()),
						dxgi_formats.data(),
						OUT Cast<IDXGIOutputDuplication*>(&_desktopDuplication) );
			dxgi_output5 = null;
		}

		if ( _desktopDuplication == null )
		{
			hr = dxgi_output1->DuplicateOutput( Cast<ID3D11Device>(_dxDevice), OUT Cast<IDXGIOutputDuplication*>(&_desktopDuplication) );
		}
		dxgi_output1 = null;

		if ( FAILED(hr) )
		{
			if ( hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE )
			{
				RETURN_ERR( "There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again." );
			}
			if ( hr == E_ACCESSDENIED )
				return false;

			RETURN_ERR( "Failed to get duplicate output in DUPLICATIONMANAGER" );
		}

		DXGI_OUTDUPL_DESC	output_desc;
		Cast<IDXGIOutputDuplication>(_desktopDuplication)->GetDesc( OUT &output_desc );

		_inHostMemory	= output_desc.DesktopImageInSystemMemory;
		_refreshRate	= float(output_desc.ModeDesc.RefreshRate.Numerator) / float(output_desc.ModeDesc.RefreshRate.Denominator);
		_displayDim.x	= output_desc.ModeDesc.Width;
		_displayDim.y	= output_desc.ModeDesc.Height;

		_surfaceFormat	= ResLoader::DDSFormatToPixelFormat( ResLoader::DXGI_FORMAT(output_desc.ModeDesc.Format) );

		switch_enum( output_desc.Rotation )
		{
			case DXGI_MODE_ROTATION_UNSPECIFIED :	_rotation = Default;						break;
			case DXGI_MODE_ROTATION_IDENTITY :		_rotation = Monitor::EOrientation::Deg_0;	break;
			case DXGI_MODE_ROTATION_ROTATE90 :		_rotation = Monitor::EOrientation::Deg_90;	break;
			case DXGI_MODE_ROTATION_ROTATE180 :		_rotation = Monitor::EOrientation::Deg_180;	break;
			case DXGI_MODE_ROTATION_ROTATE270 :		_rotation = Monitor::EOrientation::Deg_270;	break;
		}
		switch_end

		return true;
	}

/*
=================================================
	_ReleaseDuplication
=================================================
*/
	void  ScreenCaptureDXGI::_ReleaseDuplication () __NE___
	{
		if ( _desktopDuplication )
		{
			Cast<IDXGIOutputDuplication>(_desktopDuplication)->Release();
			_desktopDuplication = null;
		}
	}

/*
=================================================
	_GetFrame
=================================================
*/
	bool  ScreenCaptureDXGI::_GetFrame (OUT bool		&outTimeout,
										OUT bool		&outRecreate,
										OUT FrameInfo	&frameInfo,
										milliseconds	timeout) __NE___
	{
		StaticAssert( sizeof(MoveRect) == sizeof(DXGI_OUTDUPL_MOVE_RECT) );
		StaticAssert( sizeof(RectI) == sizeof(RECT) );
		StaticAssert( alignof(decltype(_tempBuffer)::value_type) == alignof(DXGI_OUTDUPL_MOVE_RECT) );
		StaticAssert( alignof(decltype(_tempBuffer)::value_type) == alignof(RECT) );

		outTimeout	= false;
		outRecreate	= false;

		CHECK_ERR( _desktopDuplication != null );

		ComPtr<IDXGIResource>		desktop_resource;
		DXGI_OUTDUPL_FRAME_INFO		dx_frame_info;
		IDXGIOutputDuplication*		desk_dupl		= Cast<IDXGIOutputDuplication>(_desktopDuplication);

		_ReleaseFrame();

		// Get new frame
		HRESULT hr = desk_dupl->AcquireNextFrame( uint(timeout.count()), OUT &dx_frame_info, OUT &desktop_resource );
		if ( hr == DXGI_ERROR_WAIT_TIMEOUT )
		{
			outTimeout = true;
			return true;
		}
		if ( FAILED(hr) )
		{
			AE_LOGW( "AcquireNextFrame: failed to get desktop duplication resource, code 0x"s << ToString<16>(hr) );
			outRecreate = true;
			return true;
		}

		hr = desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D), OUT &_acquiredDesktopImage );
		desktop_resource = null;

		if_unlikely( FAILED(hr) )
		{
			desk_dupl->ReleaseFrame();
			RETURN_ERR( "failed to get _acquiredDesktopImage" );
		}

		frameInfo.moveRects.clear();
		frameInfo.dirtyRects.clear();

		if ( dx_frame_info.LastPresentTime.QuadPart != 0 )
			_lastPresentTime = WindowsUtils::QueryPerformanceCounterToTimePoint( dx_frame_info.LastPresentTime.QuadPart );

		if ( dx_frame_info.LastMouseUpdateTime.QuadPart != 0 )
			_lastMouseUpdateTime = WindowsUtils::QueryPerformanceCounterToTimePoint( dx_frame_info.LastMouseUpdateTime.QuadPart );

		frameInfo.lastPresentTime		= _lastPresentTime;
		frameInfo.lastMouseUpdateTime	= _lastMouseUpdateTime;
		frameInfo.accumulatedFrames		= dx_frame_info.AccumulatedFrames;
		frameInfo.pointerPos			= int2{ dx_frame_info.PointerPosition.Position.x, dx_frame_info.PointerPosition.Position.y };

		// Get metadata
		if ( dx_frame_info.TotalMetadataBufferSize != 0 )
		{
			_tempBuffer.resize( dx_frame_info.TotalMetadataBufferSize / sizeof(_tempBuffer[0]) );  // throw

			uint	buf_size		= dx_frame_info.TotalMetadataBufferSize;
			uint	move_rect_size	= 0;
			uint	dirty_rect_size	= 0;
			auto*	ptr				= _tempBuffer.data();

			hr = desk_dupl->GetFrameMoveRects( buf_size, OUT Cast<DXGI_OUTDUPL_MOVE_RECT>(ptr), OUT &move_rect_size );
			if ( SUCCEEDED(hr) ){
				frameInfo.moveRects.assign( Cast<MoveRect>(ptr), Cast<MoveRect>(ptr) + move_rect_size / sizeof(DXGI_OUTDUPL_MOVE_RECT) );
			}else{
				DBG_WARNING( "Failed to get frame move rects" );
			}

			buf_size	= dx_frame_info.TotalMetadataBufferSize - move_rect_size;
			ptr			= ptr + Bytes{move_rect_size};

			hr = desk_dupl->GetFrameDirtyRects( buf_size, OUT Cast<RECT>(ptr), OUT &dirty_rect_size );
			if ( SUCCEEDED(hr) ){
				frameInfo.dirtyRects.assign( Cast<RectI>(ptr), Cast<RectI>(ptr) + dirty_rect_size / sizeof(RECT) );
			}else{
				DBG_WARNING( "Failed to get frame dirty rects" );
			}

			ASSERT( move_rect_size % sizeof(DXGI_OUTDUPL_MOVE_RECT) == 0 );
			ASSERT( dirty_rect_size % sizeof(RECT) == 0 );
		}

		return true;
	}

/*
=================================================
	_ReleaseFrame
----
	docs:
	"The application must release the frame before it acquires the next frame.
	 After the frame is released, the surface that contains the desktop bitmap becomes invalid.
	 For performance reasons, we recommend that you release the frame just before you call the AcquireNextFrame method to acquire the next frame."
=================================================
*/
	void  ScreenCaptureDXGI::_ReleaseFrame () __NE___
	{
		if ( _acquiredDesktopImage == null )
			return;

		HRESULT hr = Cast<IDXGIOutputDuplication>(_desktopDuplication)->ReleaseFrame();

		if ( _acquiredDesktopImage )
		{
			Cast<ID3D11Texture2D>(_acquiredDesktopImage)->Release();
			_acquiredDesktopImage = null;
		}

		if ( hr != S_OK						and
			 hr != DXGI_ERROR_INVALID_CALL	and
			 hr != DXGI_ERROR_ACCESS_LOST	)
		{
			CHECK_MSG( SUCCEEDED(hr), "Failed ReleaseFrame()" );
		}
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  ScreenCaptureDXGI::_Destroy () __NE___
	{
		_ReleaseFrame();
		_ReleaseDuplication();

		if ( _dxContext )
		{
			Cast<ID3D11DeviceContext>(_dxContext)->Release();
			_dxContext = null;
		}

		if ( _dxDevice )
		{
			Cast<ID3D11Device>(_dxDevice)->Release();
			_dxDevice = null;
		}

	  #ifdef AE_DEBUG
		if ( _dxDebug )
		{
			Cast<IDXGIDebug1>(_dxDebug)->Release();
			_dxDebug = null;
		}
		if ( _dxDebugQueue )
		{
			Cast<IDXGIInfoQueue>(_dxDebugQueue)->Release();
			_dxDebugQueue = null;
		}
	  #endif

		_dxgiLib.Unload();
		_dx11Lib.Unload();
	}

/*
=================================================
	GetDescription
=================================================
*/
	IScreenCapture::Description  ScreenCaptureDXGI::GetDescription () C_NE___
	{
		if ( not _looping.load() )
			return Default;

		Description		desc;
		desc.dimension		= _displayDim;
		desc.refreshRate	= _refreshRate;
		desc.orientation	= _rotation;
		desc.outputFormat	= _surfaceFormat;
		desc.monitorHandle	= _config.monitorHandle;
		return desc;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Start
=================================================
*/
	bool  ScreenCaptureDXGI_HostAccess::Start (const Config &cfg) __NE___
	{
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		CHECK_ERR( cfg.hostImageFormat != Default );

		_config = cfg;
		_syncAccess->unusedImages.SetRange< 0, QueueSize >();

		// run in separate thread
		_looping.store( true );
		_complete.store( false );

		Threading::SyncEvent	init;
		bool					ok = false;

		_dxThread = StdThread{ [this, &init, &ok] ()
		{
			auto	self = GetRC();		// keep alive
			ThreadUtils::SetName( "ScreenCaptureDXGI_HostAccess" );

			ok = _InitDX11();
			Unused( _OpenDesktopInThread() );	// may fail if already attached
			ok = ok and _InitDuplication();

			if ( not ok )
			{
				_DestroyStagingImages();
				_Destroy();
			}

			init.Signal();

			if ( not ok )
				return;

			_ThreadFn();

			_complete.store( true );
			AE_LOGI( "ScreenCaptureDXGI_HostAccess thread finished" );
		}};

		init.Wait();

		if ( not ok )
			Finish();

		return ok;
	}

/*
=================================================
	_FindAdapter
=================================================
*/
	bool  ScreenCaptureDXGI_HostAccess::_FindAdapter (void*, OUT void**) __NE___
	{
		// keep default
		return true;
	}

/*
=================================================
	Finish
=================================================
*/
	void  ScreenCaptureDXGI_HostAccess::Finish () __NE___
	{
		_looping.store( false );

		_dxThread.join();
	}

	AsyncTask  ScreenCaptureDXGI_HostAccess::FinishAsync () __NE___
	{
		return _FinishTask( GetRC<ScreenCaptureDXGI_HostAccess>() );
	}

	auto  ScreenCaptureDXGI_HostAccess::_FinishTask (RC<ScreenCaptureDXGI_HostAccess> self) __NE___ -> InlineCoro<ETaskQueue::Background>
	{
		self->_looping.store( false );

		for (; not self->_complete.load(); )
		{
			Coro_Continue();
		}

		// should not block
		self->_dxThread.join();
		self = null;
	}

/*
=================================================
	_RecreateDuplication
=================================================
*/
	inline bool  ScreenCaptureDXGI_HostAccess::_RecreateDuplication () __NE___
	{
		for (uint i = 0; _looping.load(); ++i)
		{
			if ( i > 2 )
				ThreadUtils::MicroSleep( milliseconds{ Min( 1000, i * 10 )});

			if ( _InitDuplication() )
			{
				AE_LOGI( "duplication restarted" );
				return true;
			}
		}
		return false;
	}

/*
=================================================
	_ThreadFn
=================================================
*/
	inline void  ScreenCaptureDXGI_HostAccess::_ThreadFn () __NE___
	{
		bool		is_timeout;
		bool		recreate;
		FrameInfo	frame_info;

		{
			auto&	fmt_info = EPixelFormat_GetInfo( _surfaceFormat );
			_rowPitch = ImageUtils::RowSize( _displayDim.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
		}

		for (; _looping.load();)
		{
			if_unlikely( not _GetFrame( OUT is_timeout, OUT recreate, OUT frame_info, milliseconds{500} ))
			{
				_syncAccess->error = ErrorCode::Failed_Acquire;
				break;
			}

			if_unlikely( is_timeout )
			{
				_syncAccess->error = ErrorCode::Timeout;
				ThreadUtils::Sleep_15ms();
				continue;
			}

			if_unlikely( recreate )
			{
				_syncAccess->error = ErrorCode::Error_NeedRecreate;
				_ReleaseDuplication();
				if ( not _RecreateDuplication() )
					break;

				_syncAccess->error = ErrorCode::OK;
				continue;
			}

			if_unlikely( not _CopyToStaging( frame_info ))
			{
				// error code already set
				break;
			}

			_MapNextImage();
		}

		_DestroyStagingImages();
		_Destroy();
	}

/*
=================================================
	_DestroyStagingImages
----
	used in separate thread
=================================================
*/
	void  ScreenCaptureDXGI_HostAccess::_DestroyStagingImages () __NE___
	{
		uint	idx = _syncAccess->index;

		// unmap
		if ( _dxContext != null			and
			 idx < QueueSize			and
			 _dxStagingImages[idx] != null )
		{
			auto*	ctx		= Cast<ID3D11DeviceContext>( _dxContext );
			auto*	image	= Cast<ID3D11Texture2D>( _dxStagingImages[idx] );
			ctx->Unmap( image, 0 );
		}

		for (void* dx_tex : _dxStagingImages)
		{
			if ( dx_tex )
			{
				Cast<ID3D11Texture2D>(dx_tex)->Release();
				dx_tex = null;
			}
		}
		_dxStagingImages.fill( null );
	}

/*
=================================================
	_CopyToStaging
----
	used in separate thread
=================================================
*/
	bool  ScreenCaptureDXGI_HostAccess::_CopyToStaging (const FrameInfo &srcFrameInfo) __NE___
	{
		auto*	src_tex		= Cast<ID3D11Texture2D>( _acquiredDesktopImage );
		auto*	dev			= Cast<ID3D11Device>( _dxDevice );
		auto*	ctx			= Cast<ID3D11DeviceContext>( _dxContext );

		int		idx			= _syncAccess->unusedImages.ExtractBitIndex();	// 1 -> 0
		ASSERT( idx >= 0 and idx < int(QueueSize) );
		ASSERT( not _syncAccess->unusedImages.Has( idx ));

		// not a critical error, try again later
		if_unlikely( idx < 0 or idx >= int(QueueSize) )
			return true;

		auto&	dst_frame	= _frameInfos[ idx ];
		dst_frame.lastPresentTime		= srcFrameInfo.lastPresentTime;
		dst_frame.lastMouseUpdateTime	= srcFrameInfo.lastMouseUpdateTime;
		dst_frame.accumulatedFrames		= srcFrameInfo.accumulatedFrames;
		dst_frame.moveRects				= Array<MoveRect>{srcFrameInfo.moveRects};
		dst_frame.dirtyRects			= Array<RectI>{srcFrameInfo.dirtyRects};

		D3D11_TEXTURE2D_DESC	tex_desc;
		src_tex->GetDesc( OUT &tex_desc );

		CHECK( tex_desc.Width  == _displayDim.x );
		CHECK( tex_desc.Height == _displayDim.y );
		CHECK( tex_desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM );

		// create staging image
		if_unlikely( _dxStagingImages[ idx ] == null )
		{
			tex_desc.Usage			= D3D11_USAGE_STAGING;
			tex_desc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
			tex_desc.MiscFlags		= 0;
			tex_desc.BindFlags		= 0;

			HRESULT  hr = dev->CreateTexture2D( &tex_desc, null, OUT Cast<ID3D11Texture2D *>(&_dxStagingImages[ idx ]) );
			if_unlikely( FAILED(hr) )
			{
				auto	sync = _syncAccess.WriteLock();
				sync->unusedImages.Set( idx );				// 0 -> 1
				sync->error = ErrorCode::Failed_StagingAlloc;
				return false;
			}
		}

		// TODO: skip copy if no dirty rects

		auto*	dst_tex = Cast<ID3D11Texture2D>( _dxStagingImages[ idx ]);

		ctx->CopyResource( dst_tex, src_tex );

		// add to queue
		{
			auto	sync = _syncAccess.WriteLock();

			ASSERT( sync->error < ErrorCode::_Errors );
			if ( sync->error < ErrorCode::_Errors )
				sync->error = ErrorCode::OK;

			sync->captureQueue.push_back( ubyte(idx) );
		}
		return true;
	}

/*
=================================================
	_MapNextImage
----
	used in separate thread
=================================================
*/
	void  ScreenCaptureDXGI_HostAccess::_MapNextImage () __NE___
	{
		auto	sync	= _syncAccess.WriteLock();
		usize	qsize	= sync->captureQueue.size();

		// skip some frames to avoid stall
		if ( qsize < QueueSize/2 )
			return;

		auto*	ctx = Cast<ID3D11DeviceContext>( _dxContext );

		// unmap previous
		if ( sync->index < QueueSize )
		{
			const ubyte	i		= sync->index;
			auto*		image	= Cast<ID3D11Texture2D>( _dxStagingImages[i] );

			ctx->Unmap( image, 0 );
			sync->unusedImages.Set( i );	// 0 -> 1
			sync->index = UMax;
		}

		const ubyte	i	= sync->captureQueue.front();
		sync->captureQueue.erase( 0 );

		ASSERT( i < QueueSize );
		ASSERT( not sync->unusedImages.Has( i ));

		auto*	image = Cast<ID3D11Texture2D>( _dxStagingImages[i] );

		D3D11_MAPPED_SUBRESOURCE	sub_res;
		HRESULT	hr = ctx->Map( image, 0, D3D11_MAP_READ, 0, OUT &sub_res );

		if_unlikely( FAILED(hr) )
		{
			sync->unusedImages.Set( i );	// 0 -> 1
			return;
		}

		sync->mappedPtr	= sub_res.pData;
		sync->index		= i;

		ASSERT( _rowPitch == sub_res.RowPitch );

		if ( _syncRead )
		{
			auto&			frame_info	= _frameInfos[i];
			ImageMemView	mem_view	{ sync->mappedPtr, _rowPitch * _displayDim.y, uint3{}, uint3{_displayDim, 1u},
										  _rowPitch, Default, _surfaceFormat, EImageAspect::Color };
			bool			ok			= true;

			TRY{
				ok = _syncRead( mem_view, frame_info, sync->error );
			}
			CATCH_ALL(
				ok = false;
			)

			if_unlikely( not ok )
			{
				sync->error = ErrorCode::Failed_UserException;
				_looping.store( false );
			}
		}
	}

/*
=================================================
	ReadHostImage
=================================================
*/
	IScreenCapture::ErrorCode
		ScreenCaptureDXGI_HostAccess::ReadHostImage (const ReadImageFn_t &fn) __NE___
	{
		auto	sync = _syncAccess.ReadLock();

		if ( sync->mappedPtr == null )
			return sync->error;

		const uint		i			= sync->index;
		auto&			frame_info	= _frameInfos[i];
		ImageMemView	mem_view	{ sync->mappedPtr, _rowPitch * _displayDim.y, uint3{}, uint3{_displayDim, 1u},
									  _rowPitch, Default, _surfaceFormat, EImageAspect::Color };

		fn( mem_view, frame_info );
		return ErrorCode::OK;
	}

/*
=================================================
	SetReadImageCallback
=================================================
*/
	bool  ScreenCaptureDXGI_HostAccess::SetReadImageCallback (SyncReadImageFn_t fn) __NE___
	{
		ASSERT_MSG( not _syncRead, "override previous callback" );

		// TODO: thread safe

		_syncRead = RVRef(fn);
		return true;
	}

/*
=================================================
	GetState
=================================================
*/
	IScreenCapture::EState
		ScreenCaptureDXGI_HostAccess::GetState () C_NE___
	{
		EState	result = Default;

		if ( _complete.load() )
		{
			result = EState::Finished;
		}else
		if ( _looping.load() )
		{
			ErrorCode	err = ErrorCode::OK;
			{
				auto	sync = _syncAccess.ReadNoLock();
				if ( sync.try_lock_shared() )
				{
					err = sync->error;
					sync.unlock_shared();
				}
			}

			if ( err == ErrorCode::Error_NeedRecreate )
				result = EState::Paused;
			else
			if ( err == ErrorCode::OK )
				result = EState::Active;
			else
				result = EState::ActiveWithError;
		}
		else
		{
			result = EState::WillFinish;
		}
		return result;
	}

/*
=================================================
	destructor
=================================================
*/
	ScreenCaptureDXGI_HostAccess::~ScreenCaptureDXGI_HostAccess () __NE___
	{
		for (void* dx_tex : _dxStagingImages) {
			CHECK( dx_tex == null );
		}
	}
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
	Start
=================================================
*/
	bool  ScreenCaptureDXGI_Vulkan::Start (const Config &cfg) __NE___
	{
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		CHECK_ERR( cfg.hostImageFormat == Default );

		_config = cfg;
		_looping.store( true );

		CHECK_ERR( _InitDX11() );
		CHECK_ERR( _InitDuplication() );
		CHECK_ERR( _CheckVulkanCompatibility() );

		return true;
	}

/*
=================================================
	_CheckVulkanCompatibility
=================================================
*/
	bool  ScreenCaptureDXGI_Vulkan::_CheckVulkanCompatibility () __NE___
	{
		VDevice const&	dev = GraphicsScheduler().GetDevice();
		auto&			ext = dev.GetVExtensions();

		CHECK_ERR(	ext.externalMemoryWin32		and
					ext.keyedMutexWin32			and
					ext.dedicatedAllocation		and
					ext.memRequirements2		);

		_fnGetMemoryWin32HandleProperties = vkGetDeviceProcAddr( dev.GetVkDevice(), "vkGetMemoryWin32HandlePropertiesKHR" );
		CHECK_ERR( _fnGetMemoryWin32HandleProperties != null );


		VkPhysicalDeviceExternalImageFormatInfo	ext_fmt = {};
		ext_fmt.sType		= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO;
		ext_fmt.handleType	= VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;

		VkPhysicalDeviceImageFormatInfo2	img_fmt = {};
		img_fmt.sType	= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		img_fmt.pNext	= &ext_fmt;
		img_fmt.format	= VK_FORMAT_B8G8R8A8_UNORM;
		img_fmt.type	= VK_IMAGE_TYPE_2D;
		img_fmt.tiling	= VK_IMAGE_TILING_OPTIMAL;
		img_fmt.usage	= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		VkExternalImageFormatProperties	ext_props = {};
		VkImageFormatProperties2		fmt_props = {};

		ext_props.sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES;
		fmt_props.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		fmt_props.pNext = &ext_props;

		VK_CHECK( vkGetPhysicalDeviceImageFormatProperties2( dev.GetVkPhysicalDevice(), &img_fmt, OUT &fmt_props ));

		CHECK_ERR( AllBits( ext_props.externalMemoryProperties.externalMemoryFeatures, VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT ));
		CHECK_ERR( AllBits( ext_props.externalMemoryProperties.externalMemoryFeatures, VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT ));
		CHECK_ERR( AllBits( ext_props.externalMemoryProperties.compatibleHandleTypes, VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT ));

		return true;
	}

/*
=================================================
	Finish
=================================================
*/
	void  ScreenCaptureDXGI_Vulkan::Finish () __NE___
	{
		_Destroy();
	}

	AsyncTask  ScreenCaptureDXGI_Vulkan::FinishAsync () __NE___
	{
		return _FinishTask( GetRC<ScreenCaptureDXGI_Vulkan>() );
	}

	auto  ScreenCaptureDXGI_Vulkan::_FinishTask (RC<ScreenCaptureDXGI_Vulkan> self) __NE___ -> InlineCoro<ETaskQueue::Background>
	{
		self->_Destroy();
		co_return;
	}

/*
=================================================
	_FindAdapter
=================================================
*/
	bool  ScreenCaptureDXGI_Vulkan::_FindAdapter (void* factory, OUT void** adapter) __NE___
	{
		CHECK_ERR( factory != null );

		VDevice const&	dev = GraphicsScheduler().GetDevice();

		auto&	id_props = dev.GetVProperties().deviceIdProperties;
		LUID	luid;

		CHECK_ERR( id_props.deviceLUIDValid == VK_TRUE );
		StaticAssert( sizeof(luid) == sizeof(id_props.deviceLUID) );

		memcpy( OUT &luid, id_props.deviceLUID, sizeof(luid) );

		HRESULT hr = Cast<IDXGIFactory4>(factory)->EnumAdapterByLuid( luid, __uuidof(IDXGIAdapter1), OUT adapter );
		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to find DXGI adapter with same LUID as in Vulkan device" );

		return true;
	}

/*
=================================================
	AcquireImage
=================================================
*/
	IScreenCapture::ErrorCode
		ScreenCaptureDXGI_Vulkan::AcquireImage (OUT ImageID				&imageId,
												OUT ImageViewID			&viewId,
												OUT FrameInfo			&frameInfo,
												Graphics::CommandBatch&	cmdBatch,
												milliseconds			timeout) __NE___
	{
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		bool	is_timeout;
		bool	recreate;

		if_unlikely( not _GetFrame( OUT is_timeout, OUT recreate, OUT frameInfo, timeout ))
			return ErrorCode::Failed_Acquire;

		if_unlikely( is_timeout )
			return ErrorCode::Timeout;

		if_unlikely( recreate )
			return ErrorCode::Error_NeedRecreate;

		SharedImage		shared_image;
		if ( not _CreateVulkanImage( _acquiredDesktopImage, OUT shared_image ))
		{
			_DestroyVulkanImage( shared_image );
			return ErrorCode::Failed_VkInterop;
		}

		cmdBatch.SetKeyedMutexAcquireRelease( &shared_image.keyedMutexAcqRel );

		// TODO

		imageId = shared_image.image;
		viewId	= shared_image.view;
		return ErrorCode::OK;
	}

/*
=================================================
	_CreateVulkanImage
=================================================
*/
	bool  ScreenCaptureDXGI_Vulkan::_CreateVulkanImage (void* acquiredDesktopImage, OUT SharedImage &sharedImage) __NE___
	{
		CHECK_ERR( acquiredDesktopImage != null );

		auto*	dx_image = Cast<ID3D11Texture2D>(acquiredDesktopImage);
		HRESULT	hr;

		D3D11_TEXTURE2D_DESC	dxtex_desc;
		dx_image->GetDesc( OUT &dxtex_desc );

		CHECK_ERR( AllBits( dxtex_desc.MiscFlags, D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX ));

		// TODO: use keyed mutex as semaphore

		ComPtr<IDXGIKeyedMutex>	dx_mtx;
		hr = dx_image->QueryInterface(__uuidof(IDXGIKeyedMutex), OUT dx_mtx.VRef() );
		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to get IDXGIKeyedMutex" );

		ComPtr<IDXGIResource1>	dx_res;
		hr = dx_image->QueryInterface(__uuidof(IDXGIResource1), OUT dx_res.VRef() );
		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed to get IDXGIResource1" );

		// TODO
		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgikeyedmutex-acquiresync
		//dx_mtr->AcquireSync();
		//dx_mtr->ReleaseSync();


		// import handle
		VkImportMemoryWin32HandleInfoKHR	handle_info = {};
		handle_info.sType		= VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
		handle_info.handleType	= VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;

		hr = dx_res->CreateSharedHandle( null, DXGI_SHARED_RESOURCE_READ, null, OUT &handle_info.handle );
		dx_res = null;

		CHECK_ERR_MSG( SUCCEEDED(hr),
			"Failed: CreateSharedHandle" );

		sharedImage.dxHandle = handle_info.handle;

		ResourceManager&	res_mngr	= GraphicsScheduler().GetResourceManager();
		VDevice const&		dev			= res_mngr.GetDevice();

		VkMemoryWin32HandlePropertiesKHR	mem_props = {};
		mem_props.sType = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR;

		auto*	vkGetMemoryWin32HandlePropertiesKHR = FnUnsafeCast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(_fnGetMemoryWin32HandleProperties);
		CHECK_ERR( vkGetMemoryWin32HandlePropertiesKHR != null );
		VK_CHECK( vkGetMemoryWin32HandlePropertiesKHR( dev.GetVkDevice(), handle_info.handleType, handle_info.handle, OUT &mem_props ));


		// create image
		VkExternalMemoryImageCreateInfo	ext_mem_info = {};
		ext_mem_info.sType			= VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
		ext_mem_info.handleTypes	= handle_info.handleType;

		VkImageCreateInfo	image_ci = {};
		image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_ci.pNext			= &ext_mem_info;
		image_ci.flags			= 0;
		image_ci.imageType		= VK_IMAGE_TYPE_2D;
		image_ci.format			= VK_FORMAT_B8G8R8A8_UNORM;		// TODO
		image_ci.extent.width	= dxtex_desc.Width;
		image_ci.extent.height	= dxtex_desc.Height;
		image_ci.extent.depth	= 1;
		image_ci.mipLevels		= 1;
		image_ci.arrayLayers	= 1;
		image_ci.samples		= VK_SAMPLE_COUNT_1_BIT;
		image_ci.tiling			= VK_IMAGE_TILING_OPTIMAL;
		image_ci.usage			= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
		image_ci.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;

		VkImage		vk_image;
		VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &image_ci, null, OUT &vk_image ));


		// allocate memory
		VkImageMemoryRequirementsInfo2	mem_req_info = {};
		mem_req_info.sType	= VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
		mem_req_info.image	= vk_image;

		VkMemoryDedicatedRequirements	dedication_req = {};
		dedication_req.sType	= VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

		VkMemoryRequirements2	mem_req = {};
		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_req.pNext	= &dedication_req;

		dev.vkGetImageMemoryRequirements2( dev.GetVkDevice(), &mem_req_info, OUT &mem_req );

		mem_props.memoryTypeBits &= mem_req.memoryRequirements.memoryTypeBits;
		CHECK_ERR( mem_props.memoryTypeBits != 0 );

		// specs:
		// "Importing memory object payloads from Windows handles does not transfer ownership of the handle to the Vulkan implementation.
		//  For handle types defined as NT handles, the application must release handle ownership using the CloseHandle system call when the handle is no longer needed.
		//  For handle types defined as NT handles, the imported memory object holds a reference to its payload."

		VkMemoryDedicatedAllocateInfo	dedication_info = {};
		dedication_info.sType		= VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
		dedication_info.pNext		= &handle_info;
		dedication_info.image		= vk_image;

		VkMemoryAllocateInfo	mem_alloc = {};
		mem_alloc.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext				= &dedication_info;
		mem_alloc.allocationSize	= mem_req.memoryRequirements.size;

		CHECK_ERR( dev.GetMemoryTypeIndex( mem_props.memoryTypeBits,
										   VkMemoryPropertyFlagBits(0), VkMemoryPropertyFlagBits(0),
										   VkMemoryPropertyFlagBits(0), VkMemoryPropertyFlagBits(0),
										   OUT mem_alloc.memoryTypeIndex ));

		VK_CHECK_ERR( dev.vkAllocateMemory( dev.GetVkDevice(), &mem_alloc, null, OUT &sharedImage.vkMemory ));

		VkBindImageMemoryInfo	bind = {};
		bind.sType	= VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
		bind.memory	= sharedImage.vkMemory;
		bind.image	= vk_image;
		VK_CHECK_ERR( dev.vkBindImageMemory2KHR( dev.GetVkDevice(), 1, &bind ));


		VulkanImageDesc		vk_image_desc;
		vk_image_desc.image			= vk_image;
		vk_image_desc.imageType		= image_ci.imageType;
		vk_image_desc.flags			= VkImageCreateFlagBits(image_ci.flags);
		vk_image_desc.options		= Default;
		vk_image_desc.usage			= VkImageUsageFlagBits(image_ci.usage);
		vk_image_desc.format		= image_ci.format;
		vk_image_desc.samples		= image_ci.samples;
		vk_image_desc.tiling		= image_ci.tiling;
		vk_image_desc.dimension		= uint3{ image_ci.extent.width, image_ci.extent.height, image_ci.extent.depth };
		vk_image_desc.arrayLayers	= image_ci.arrayLayers;
		vk_image_desc.mipLevels		= image_ci.mipLevels;
		vk_image_desc.memFlags		= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		vk_image_desc.canBeDestroyed= true;
		vk_image_desc.allocMemory	= false;

		sharedImage.image = res_mngr.CreateImage( vk_image_desc );
		CHECK_ERR( sharedImage.image );

		sharedImage.view = res_mngr.CreateImageView( ImageViewDesc{}, sharedImage.image );
		CHECK_ERR( sharedImage.view );

		return true;
	}

/*
=================================================
	_DestroyVulkanImage
=================================================
*/
	void  ScreenCaptureDXGI_Vulkan::_DestroyVulkanImage (SharedImage &sharedImage) __NE___
	{
		ResourceManager&	res_mngr	= GraphicsScheduler().GetResourceManager();
		VDevice const&		dev			= res_mngr.GetDevice();

	//	VK_CHECK( dev.vkDeviceWaitIdle( dev.GetVkDevice() ));

		res_mngr.ImmediatelyRelease( sharedImage.view );
		res_mngr.ImmediatelyRelease( sharedImage.image );

		dev.vkFreeMemory( dev.GetVkDevice(), sharedImage.vkMemory, null );

		::CloseHandle( sharedImage.dxHandle );
	}

/*
=================================================
	GetState
=================================================
*/
	IScreenCapture::EState
		ScreenCaptureDXGI_Vulkan::GetState () C_NE___
	{
		EState	result = Default;

		if ( _complete.load() )
		{
			result = EState::Finished;
		}else
		if ( _looping.load() )
		{
			// TODO
			result = EState::Active;
		}
		else
		{
			result = EState::WillFinish;
		}
		return result;
	}

/*
=================================================
	destructor
=================================================
*/
	ScreenCaptureDXGI_Vulkan::~ScreenCaptureDXGI_Vulkan () __NE___
	{
	}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------


} // AE::App

#endif // AE_PLATFORM_WINDOWS
