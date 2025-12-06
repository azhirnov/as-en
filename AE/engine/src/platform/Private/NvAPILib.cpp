// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// TODO
//NvAPI_GetDisplayPortInfo
//NvAPI_GetHDMISupportInfo
//NvAPI_Disp_GetOutputMode,	NvAPI_Disp_SetOutputMode
//NvAPI_Disp_GetHdrToneMapping, NvAPI_Disp_SetHdrToneMapping
//NvAPI_DISP_GetMonitorColorCapabilities
//NvAPI_DISP_GetDisplayIdByDisplayName
//NvAPI_DISP_SetAdaptiveSyncData, NvAPI_DISP_GetAdaptiveSyncData

#ifdef AE_PLATFORM_WINDOWS
# ifdef AE_ENABLE_NVAPI

#	define NVAPI_USE_STDINT	1
#	define NVOS_IS_UNIX		0

#	include <stdint.h>
#	include <nvapi.h>
#	include "platform/Private/NvAPILib.h"

#	define NVAPI_CHECK( /* expr */... )														\
	{																						\
		const NvAPI_Status	_nv_err_ = (__VA_ARGS__);										\
		Unused( _CheckNvAPIError(	_nv_err_, AE_TOSTRING( __VA_ARGS__ ),					\
									AE_FUNCTION_NAME, AE::Base::SourceLoc::current() ));	\
	}

#	define PRIVATE_NVAPI_CHECK_R( _expr_, _msg_, _ret_ )									\
	{																						\
		const NvAPI_Status	_nv_err_ = (_expr_);											\
		if_unlikely( not _CheckNvAPIError( _nv_err_, _msg_, AE_FUNCTION_NAME,				\
											AE::Base::SourceLoc::current() ))				\
			return _ret_;																	\
	}

# define PRIVATE_NVAPI_CHECK2_R( _func_, _ret_ )											\
		PRIVATE_NVAPI_CHECK_R( (_func_), AE_TOSTRING( _func_ ), (_ret_) )

#	define NVAPI_CHECK_ERR( /* expr, return_on_error */... )								\
		PRIVATE_NVAPI_CHECK2_R( AE_PRIVATE_GETARG_0( __VA_ARGS__, ),						\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default, ))

# define NVAPI_CHECK_ERRV( _expr_ )															\
		PRIVATE_NVAPI_CHECK2_R( (_expr_), void() )


namespace AE::App
{
/*
=================================================
	destructor
=================================================
*/
	NvAPILib::~NvAPILib () __NE___
	{
		if ( _initialized.load() )
		{
			NvAPI_Unload();
			_initialized.store( false );
		}
	}

/*
=================================================
	Load
=================================================
*/
	bool  NvAPILib::Load () __NE___
	{
		if ( _initialized.load() )
			return true;

		if ( NvAPI_Initialize() != NVAPI_OK )
			return false;

		{
			NvPhysicalGpuHandle	gpu_handles [NVAPI_MAX_PHYSICAL_GPUS];
			NvU32				gpu_count	= 0;

			auto err = NvAPI_EnumPhysicalGPUs( OUT gpu_handles, OUT &gpu_count );
			if ( err != NVAPI_OK or gpu_count == 0 )
			{
				NvAPI_Unload();
				return false;  // 'NVAPI_NO_IMPLEMENTATION' when running with RenderDoc
			}
		}

		_initialized.store( true );

		//_Print();
		return true;
	}

/*
=================================================
	SetHDRMode
=================================================
*/
	bool  NvAPILib::SetHDRMode (const RectI &windowRegion, const EColorSpace colorSpace) __NE___
	{
		if ( not _initialized.load() )
			return false;

		NvPhysicalGpuHandle	gpu_handles [NVAPI_MAX_PHYSICAL_GPUS];
		NvU32				gpu_count	= 0;

		NV_GPU_DISPLAYIDS	display_ids [8];
		NvU32				disp_count	= 0;

		NVAPI_CHECK_ERR( NvAPI_EnumPhysicalGPUs( OUT gpu_handles, OUT &gpu_count ));

		if ( gpu_count == 0 )
			return false;

		CHECK_MSG( gpu_count == 1, "only one GPU is supported" );

		display_ids[0].version = NV_GPU_DISPLAYIDS_VER;
		disp_count = NvU32(CountOf( display_ids ));

		NVAPI_CHECK_ERR( NvAPI_GPU_GetConnectedDisplayIds( gpu_handles[0], OUT display_ids, OUT &disp_count, 0 ));

		if ( disp_count == 0 )
			return false;

		uint	changed_disp = 0;

		for (NvU32 i = 0; i < disp_count; ++i)
		{
			const auto	disp_id		= display_ids[i].displayId;
			bool		wnd_on_disp	= true;

			NV_SCANOUT_INFORMATION	scan;
			scan.version = NV_SCANOUT_INFORMATION_VER;
			if ( NvAPI_GPU_GetScanoutConfigurationEx( disp_id, OUT &scan ) == NVAPI_OK )
			{
				RectI	disp_rect{	scan.sourceDesktopRect.sX,
									scan.sourceDesktopRect.sY,
									scan.sourceDesktopRect.sX + scan.sourceDesktopRect.sWidth,
									scan.sourceDesktopRect.sY + scan.sourceDesktopRect.sHeight };
				wnd_on_disp = disp_rect.Intersects( windowRegion );
			}

			if ( not wnd_on_disp )
				continue;

			NV_HDR_CAPABILITIES	hdr_cap;
			hdr_cap.version = NV_HDR_CAPABILITIES_VER;
			NVAPI_CHECK_ERR( NvAPI_Disp_GetHdrCapabilities( disp_id, OUT &hdr_cap ));

			NV_DISPLAY_OUTPUT_MODE	disp_mode;
			NV_HDR_COLOR_DATA		hdr_data	= {};
			//NV_COLOR_DATA			col_data	= {};

			//col_data.version	= NV_COLOR_DATA_VER;
			//col_data.size		= sizeof(col_data);
			//col_data.cmd		= NV_HDR_CMD_SET;

			hdr_data.version	= NV_HDR_COLOR_DATA_VER;
			hdr_data.cmd		= NV_HDR_CMD_SET;
			hdr_data.static_metadata_descriptor_id = NV_STATIC_METADATA_TYPE_1;

			const bool	is_scRGB	= AnyEqual( colorSpace, EColorSpace::Extended_sRGB_linear, EColorSpace::Extended_sRGB_nonlinear );
			const bool	is_HDR10	= AnyEqual( colorSpace, EColorSpace::HDR10_ST2084, EColorSpace::HDR10_HLG, EColorSpace::DolbyVision );

			if ( is_scRGB or is_HDR10 )
			{
				disp_mode = NV_DISPLAY_OUTPUT_MODE_HDR10;

				hdr_data.hdrMode			= NV_HDR_MODE_UHDA;
				hdr_data.hdrColorFormat		= NV_COLOR_FORMAT_RGB;
				hdr_data.hdrDynamicRange	= NV_DYNAMIC_RANGE_VESA;
				hdr_data.hdrBpc				= is_scRGB ? NV_BPC_16 : NV_BPC_10;

				hdr_data.mastering_display_data.displayPrimary_x0	= hdr_cap.display_data.displayPrimary_x0;
				hdr_data.mastering_display_data.displayPrimary_y0	= hdr_cap.display_data.displayPrimary_y0;

				hdr_data.mastering_display_data.displayPrimary_x1	= hdr_cap.display_data.displayPrimary_x1;
				hdr_data.mastering_display_data.displayPrimary_y1	= hdr_cap.display_data.displayPrimary_y1;

				hdr_data.mastering_display_data.displayPrimary_x2	= hdr_cap.display_data.displayPrimary_x2;
				hdr_data.mastering_display_data.displayPrimary_y2	= hdr_cap.display_data.displayPrimary_y2;

				hdr_data.mastering_display_data.displayWhitePoint_x	= hdr_cap.display_data.displayWhitePoint_x;
				hdr_data.mastering_display_data.displayWhitePoint_y	= hdr_cap.display_data.displayWhitePoint_y;

				hdr_data.mastering_display_data.max_display_mastering_luminance	= hdr_cap.display_data.desired_content_max_luminance;
				hdr_data.mastering_display_data.min_display_mastering_luminance	= hdr_cap.display_data.desired_content_min_luminance;
				hdr_data.mastering_display_data.max_frame_average_light_level	= hdr_cap.display_data.desired_content_max_frame_average_luminance;
				hdr_data.mastering_display_data.max_content_light_level			= hdr_cap.display_data.desired_content_max_luminance;
			}
			else
			{
				disp_mode = NV_DISPLAY_OUTPUT_MODE_SDR;

				hdr_data.hdrMode			= NV_HDR_MODE_OFF;
				hdr_data.hdrColorFormat		= NV_COLOR_FORMAT_RGB;
				hdr_data.hdrDynamicRange	= NV_DYNAMIC_RANGE_VESA;
				hdr_data.hdrBpc				= NV_BPC_8;
			}

			NVAPI_CHECK_ERR( NvAPI_Disp_HdrColorControl( disp_id, &hdr_data ));
			NVAPI_CHECK_ERR( NvAPI_Disp_SetOutputMode( disp_id, &disp_mode ));
			//NVAPI_CHECK_ERR( NvAPI_Disp_ColorControl( disp_id, &col_data ));

			++changed_disp;
		}

		//CHECK( changed_disp > 0 );
		return true;
	}

/*
=================================================
	IsHDRMode
=================================================
*
	bool  NvAPILib::IsHDRMode (const RectI &windowRegion) C_NE___
	{
	}

/*
=================================================
	_Print
=================================================
*/
	void  NvAPILib::_Print () const
	{
		if ( not _initialized.load() )
			return;

		NvPhysicalGpuHandle	gpu_handles [NVAPI_MAX_PHYSICAL_GPUS];
		NvU32				gpu_count	= 0;

		NV_GPU_DISPLAYIDS	display_ids [8] = {};
		NvU32				disp_count	= 0;

		const NvU64			pid			= WindowsUtils::GetProcessID();

		NVAPI_CHECK_ERRV( NvAPI_EnumPhysicalGPUs( OUT gpu_handles, OUT &gpu_count ));
		CHECK_MSG( gpu_count == 1, "only one GPU is supported" );

		for (NvU32 gpu_i = 0; gpu_i < gpu_count; ++gpu_i)
		{
			//NvAPI_GPU_GetFullName
			//NvAPI_GPU_GetBoardInfo	- serial number
			//NvAPI_GPU_GetArchInfo
			//NvAPI_GPU_GetPerfDecreaseInfo
			//NvAPI_GPU_GetDynamicPstatesInfoEx

			NvU32	gpu_core_count;
			NVAPI_CHECK( NvAPI_GPU_GetGpuCoreCount( gpu_handles[gpu_i], OUT &gpu_core_count ));

			NvU32	vram_bus;
			NVAPI_CHECK( NvAPI_GPU_GetRamBusWidth( gpu_handles[gpu_i], OUT &vram_bus ));

			NvU32	pci_read_width;
			NVAPI_CHECK( NvAPI_GPU_GetCurrentPCIEDownstreamWidth( gpu_handles[gpu_i], OUT &pci_read_width ));

			NV_GPU_INFO	gpu_info;
			gpu_info.version = NV_GPU_INFO_VER;
			NVAPI_CHECK( NvAPI_GPU_GetGPUInfo( gpu_handles[gpu_i], OUT &gpu_info ));

			ZeroMem( display_ids );
			display_ids[0].version = NV_GPU_DISPLAYIDS_VER;
			disp_count = NvU32(CountOf( display_ids ));

			NVAPI_CHECK_ERRV( NvAPI_GPU_GetConnectedDisplayIds( gpu_handles[gpu_i], OUT display_ids, OUT &disp_count, 0 ));

			for (NvU32 disp_i = 0; disp_i < disp_count; ++disp_i)
			{
				auto	disp_id = display_ids[disp_i].displayId;

				NV_SCANOUT_INFORMATION	scan;
				scan.version = NV_SCANOUT_INFORMATION_VER;
				NVAPI_CHECK( NvAPI_GPU_GetScanoutConfigurationEx( disp_id, OUT &scan ));

				//NV_HDR_METADATA		hdr_meta;
				//hdr_meta.version = NV_HDR_METADATA_VER;
				//NVAPI_CHECK( NvAPI_Disp_GetSourceHdrMetadata( disp_id, OUT &hdr_meta, pid ));

				NV_COLOR_DATA	col_data = {};
				col_data.version	= NV_COLOR_DATA_VER;
				col_data.size		= sizeof(NV_COLOR_DATA);
				col_data.cmd		= NV_HDR_CMD_GET;
				NVAPI_CHECK( NvAPI_Disp_ColorControl( disp_id, OUT &col_data ));

				NV_HDR_COLOR_DATA	hdr_data;
				hdr_data.version = NV_HDR_COLOR_DATA_VER;
				hdr_data.cmd	 = NV_HDR_CMD_GET;
				NVAPI_CHECK( NvAPI_Disp_HdrColorControl( disp_id, OUT &hdr_data ));

				NV_DISPLAY_OUTPUT_MODE	disp_mode;
				NVAPI_CHECK( NvAPI_Disp_GetOutputMode( disp_id, OUT &disp_mode ));

				NV_HDR_TONEMAPPING_METHOD	tm;
				NVAPI_CHECK( NvAPI_Disp_GetHdrToneMapping( disp_id, OUT &tm ));

				NV_HDR_CAPABILITIES	hdr_cap;
				hdr_cap.version = NV_HDR_CAPABILITIES_VER;
				NVAPI_CHECK( NvAPI_Disp_GetHdrCapabilities( disp_id, OUT &hdr_cap ));

				NV_COLORSPACE_TYPE	cs;
				NVAPI_CHECK( NvAPI_Disp_GetSourceColorSpace( disp_id, OUT &cs, pid ));

				AE_LOGI( "ColorSpace: "s << ToString(int(cs)) );
			}
		}
	}

/*
=================================================
	FillHDRConfig
=================================================
*/
	bool  NvAPILib::FillHDRConfig (MutableArrayView<Monitor> monitors) C_NE___
	{
		CHECK_ERR( not monitors.empty() );

		if ( not _initialized.load() )
			return false;

		NvPhysicalGpuHandle	gpu_handles [NVAPI_MAX_PHYSICAL_GPUS];
		NvU32				gpu_count	= 0;

		NVAPI_CHECK_ERR( NvAPI_EnumPhysicalGPUs( OUT gpu_handles, OUT &gpu_count ));

		if ( gpu_count == 0 )
			return false;

		CHECK_MSG( gpu_count == 1, "only one GPU is supported" );


		NV_GPU_DISPLAYIDS	display_ids [PlatformConfig::MaxMonitors];
		NvU32				disp_count	= 0;

		for (auto& disp : display_ids)
			disp.version = NV_GPU_DISPLAYIDS_VER;
		disp_count = NvU32(CountOf( display_ids ));

		NVAPI_CHECK_ERR( NvAPI_GPU_GetConnectedDisplayIds( gpu_handles[0], OUT display_ids, OUT &disp_count, 0 ));

		if ( disp_count == 0 )
			return false;

		CHECK_ERR( monitors.size() >= disp_count );

		for (NvU32 i = 0; i < disp_count; ++i)
		{
			const auto	disp_id	= display_ids[i].displayId;
			Monitor*	monitor	= null;

			// find monitor
			NV_SCANOUT_INFORMATION	scan;
			scan.version = NV_SCANOUT_INFORMATION_VER;
			if ( NvAPI_GPU_GetScanoutConfigurationEx( disp_id, OUT &scan ) == NVAPI_OK )
			{
				RectI	disp_rect{	scan.sourceDesktopRect.sX,
									scan.sourceDesktopRect.sY,
									scan.sourceDesktopRect.sX + scan.sourceDesktopRect.sWidth,
									scan.sourceDesktopRect.sY + scan.sourceDesktopRect.sHeight };

				for (auto& mon : monitors)
				{
					if ( disp_rect.Intersects( mon.region.pixels ))
					{
						monitor = &mon;
						break;
					}
				}
			}
			if ( monitor == null )
				continue;

			NV_HDR_CAPABILITIES	hdr_cap;
			hdr_cap.version = NV_HDR_CAPABILITIES_VER;
			NVAPI_CHECK_ERR( NvAPI_Disp_GetHdrCapabilities( disp_id, OUT &hdr_cap ));

			const auto	RemapCol = [] (NvU16 x) { return float(x) / float(0xC350); };
			const auto	RemapLum = [] (NvU16 x) { return HDRConfig::Luminance_t{ float(x) / float(0xFFFF) }; };

			auto&	dst = monitor->hdr;
			dst.red.x	= RemapCol( hdr_cap.display_data.displayPrimary_x0 );
			dst.red.y	= RemapCol( hdr_cap.display_data.displayPrimary_y0 );
			dst.green.x	= RemapCol( hdr_cap.display_data.displayPrimary_x1 );
			dst.green.y	= RemapCol( hdr_cap.display_data.displayPrimary_y1 );
			dst.blue.x	= RemapCol( hdr_cap.display_data.displayPrimary_x2 );
			dst.blue.y	= RemapCol( hdr_cap.display_data.displayPrimary_y2 );
			dst.white.x	= RemapCol( hdr_cap.display_data.displayWhitePoint_x );
			dst.white.y	= RemapCol( hdr_cap.display_data.displayWhitePoint_y );
			dst.luminance.max = RemapLum( hdr_cap.display_data.desired_content_max_luminance ) * 65535.0f;
			dst.luminance.min = RemapLum( hdr_cap.display_data.desired_content_min_luminance ) * 6.55350f;
			dst.luminance.avr = RemapLum( hdr_cap.display_data.desired_content_max_frame_average_luminance ) * 65535.0f;
		}
		return true;
	}

/*
=================================================
	_CheckNvAPIError
=================================================
*/
	bool  NvAPILib::_CheckNvAPIError (const int err, const char* fnCall, const char* func, const SourceLoc &loc) C_NE___
	{
		if_likely( err == NVAPI_OK )
			return true;

	  #ifdef AE_ENABLE_LOGS
		TRY{
			NvAPI_ShortString	dec_str = {};
			NvAPI_GetErrorMessage( NvAPI_Status(err), OUT dec_str );

			String	msg;
			msg << "NvAPI error: " << dec_str << ", in " << fnCall << ", function: " << func;
			AE_LOGE( msg, loc );
		}
		CATCH_ALL()
	  #else
		Unused( fnCall, func, loc );
	  #endif

		return false;
	}

} // AE::App

# else
#	include "platform/Private/NvAPILib.h"

namespace AE::App
{

	NvAPILib::~NvAPILib ()										__NE___ {}
	bool  NvAPILib::Load ()										__NE___ { return false; }
	bool  NvAPILib::SetHDRMode (const RectI &, EColorSpace)		__NE___	{ return false; }
	bool  NvAPILib::IsHDRMode (const RectI &region)				C_NE___	{ return false; }
	bool  NvAPILib::FillHDRConfig (MutableArrayView<Monitor>)	C_NE___	{ return false; }

} // AE::App

# endif // AE_ENABLE_NVAPI
#endif // AE_PLATFORM_WINDOWS
