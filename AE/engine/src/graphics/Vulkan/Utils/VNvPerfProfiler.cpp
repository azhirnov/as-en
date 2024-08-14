// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# ifdef AE_ENABLE_NVPERF

#	include "nvperf_host.h"
#	include "nvperf_target.h"
#	include "nvperf_vulkan_host.h"
#	include "nvperf_vulkan_target.h"

#	include "graphics/Vulkan/Utils/VNvPerfProfiler.h"
#	include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{
namespace
{
/*
=================================================
	_NvPerfCheckError
=================================================
*/
	ND_ static bool  _NvPerfCheckError (NVPA_Status err, const char* nvcall, const char* func, const SourceLoc &loc) __NE___
	{
		if_likely( err == NVPA_STATUS_SUCCESS )
			return true;

	  #ifdef AE_ENABLE_LOGS
		TRY{
			String	msg = "NVPerf error: ";
			switch_enum( err )
			{
				case NVPA_STATUS_ERROR :							msg << "ERROR";							break;
				case NVPA_STATUS_INTERNAL_ERROR :					msg << "INTERNAL_ERROR";				break;
				case NVPA_STATUS_NOT_INITIALIZED :					msg << "NOT_INITIALIZED";				break;
				case NVPA_STATUS_NOT_LOADED :						msg << "NOT_LOADED";					break;
				case NVPA_STATUS_FUNCTION_NOT_FOUND :				msg << "FUNCTION_NOT_FOUND";			break;
				case NVPA_STATUS_NOT_SUPPORTED :					msg << "NOT_SUPPORTED";					break;
				case NVPA_STATUS_NOT_IMPLEMENTED :					msg << "NOT_IMPLEMENTED";				break;
				case NVPA_STATUS_INVALID_ARGUMENT :					msg << "INVALID_ARGUMENT";				break;
				case NVPA_STATUS_INVALID_METRIC_ID :				msg << "INVALID_METRIC_ID";				break;
				case NVPA_STATUS_DRIVER_NOT_LOADED :				msg << "DRIVER_NOT_LOADED";				break;
				case NVPA_STATUS_OUT_OF_MEMORY :					msg << "OUT_OF_MEMORY";					break;
				case NVPA_STATUS_INVALID_THREAD_STATE :				msg << "INVALID_THREAD_STATE";			break;
				case NVPA_STATUS_FAILED_CONTEXT_ALLOC :				msg << "FAILED_CONTEXT_ALLOC";			break;
				case NVPA_STATUS_UNSUPPORTED_GPU :					msg << "UNSUPPORTED_GPU";				break;
				case NVPA_STATUS_INSUFFICIENT_DRIVER_VERSION :		msg << "INSUFFICIENT_DRIVER_VERSION";	break;
				case NVPA_STATUS_OBJECT_NOT_REGISTERED :			msg << "OBJECT_NOT_REGISTERED";			break;
				case NVPA_STATUS_INSUFFICIENT_PRIVILEGE :			msg << "INSUFFICIENT_PRIVILEGE";		break;
				case NVPA_STATUS_INVALID_CONTEXT_STATE :			msg << "INVALID_CONTEXT_STATE";			break;
				case NVPA_STATUS_INVALID_OBJECT_STATE :				msg << "INVALID_OBJECT_STATE";			break;
				case NVPA_STATUS_RESOURCE_UNAVAILABLE :				msg << "RESOURCE_UNAVAILABLE";			break;
				case NVPA_STATUS_DRIVER_LOADED_TOO_LATE :			msg << "DRIVER_LOADED_TOO_LATE";		break;
				case NVPA_STATUS_INSUFFICIENT_SPACE :				msg << "INSUFFICIENT_SPACE";			break;
				case NVPA_STATUS_OBJECT_MISMATCH :					msg << "OBJECT_MISMATCH";				break;
				case NVPA_STATUS_VIRTUALIZED_DEVICE_NOT_SUPPORTED :	msg << "VIRTUALIZED_DEVICE_NOT_SUPPORTED";break;
				case NVPA_STATUS_PROFILING_NOT_ALLOWED :			msg << "PROFILING_NOT_ALLOWED";			break;
				case NVPA_STATUS_SUCCESS :
				case NVPA_STATUS__COUNT :
				default :											msg << "unknown";	break;
			}
			switch_end

			msg = msg + ", in " + nvcall + ", function: " + func;

			AE_LOGE( msg, loc.file, loc.line );
		}
		CATCH_ALL()
	  #else
		Unused( nvcall, func, loc );
	  #endif

		return false;
	}

#	define NVP_CHECK_ERR( ... )																							\
	{																													\
		auto	_err_ = (__VA_ARGS__);																					\
		if_unlikely( not _NvPerfCheckError( _err_, AE_TOSTRING(__VA_ARGS__), AE_FUNCTION_NAME, SourceLoc_Current() ))	\
			return Default;																								\
	}

} // namespace

#define NVPERF_FUNC( _visitor_ )\
	_visitor_( InitializeHost )\
	_visitor_( InitializeTarget )\
	\
	_visitor_( Device_SetClockSetting )\
	_visitor_( Device_GetNames )\
	\
	_visitor_( MetricsEvaluator_GetMetricNames )\
	_visitor_( MetricsEvaluator_GetMetricDimUnits )\
	\
	_visitor_( VK_LoadDriver )\
	_visitor_( VK_Device_GetDeviceIndex )\
	\
	_visitor_( VK_Profiler_GetRequiredInstanceExtensions )\
	_visitor_( VK_Profiler_GetRequiredDeviceExtensions )\
	_visitor_( VK_Profiler_IsGpuSupported )\
	\
	_visitor_( VK_MetricsEvaluator_CalculateScratchBufferSize )\
	_visitor_( VK_MetricsEvaluator_Initialize )\

/*
=================================================
	Impl
=================================================
*/
	struct VNvPerfProfiler::Impl
	{
		usize					deviceId		= UMax;
		HWInfo					info;

		NVPW_MetricsEvaluator*	evaluator		= null;
		Array<ubyte>			scratchBuffer;

		#define NVPERF_DECL( _name_ )	decltype(&NVPW_##_name_)  _name_ = null;
		NVPERF_FUNC( NVPERF_DECL )
		#undef NVPERF_DECL
	};

/*
=================================================
	constructor / destructor
=================================================
*/
	VNvPerfProfiler::VNvPerfProfiler () __NE___
	{}

	VNvPerfProfiler::~VNvPerfProfiler () __NE___
	{}

/*
=================================================
	Load
=================================================
*/
	bool  VNvPerfProfiler::Load () __NE___
	{
		CHECK_ERR( not IsLoaded() );

	  #ifdef AE_PLATFORM_WINDOWS
		Unused( _module.Load( "nvperf_grfx_host.dll" ));
	  #else
		Unused( _module.Load( "nvperf_grfx_host.so" ));
	  #endif

		if ( not _module )
			return false;

		Unique<Impl>	impl	{new Impl{}};
		bool			loaded	= true;

		#define NVPERF_LOAD( _name_ )	loaded &= _module.GetProcAddr( "NVPW_" #_name_, OUT OUT impl->_name_ );
		NVPERF_FUNC( NVPERF_LOAD )
		#undef NVPERF_LOAD

		CHECK_ERR( loaded );

		_impl = RVRef(impl);
		return true;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  VNvPerfProfiler::Initialize (const VDevice &dev) __NE___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( not IsInitialized() );

		if ( _Initialize( dev ))
			return true;

		Deinitialize();
		return false;
	}

	bool  VNvPerfProfiler::_Initialize (const VDevice &dev) __NE___
	{
		CHECK_ERR( dev.GetVkInstance() != Default );
		CHECK_ERR( dev.GetVkPhysicalDevice() != Default );
		CHECK_ERR( dev.GetVkDevice() != Default );

		const auto	NVIDIA_VENDOR_ID = 0x10de;
		if ( dev.GetVProperties().properties.vendorID != NVIDIA_VENDOR_ID )
			return false;

		{
			NVPW_InitializeHost_Params	init_host_params	= {};
			init_host_params.structSize	= NVPW_InitializeHost_Params_STRUCT_SIZE;
			NVP_CHECK_ERR( _impl->InitializeHost( &init_host_params ));
		}{
			NVPW_InitializeTarget_Params	init_target_params	= {};
			init_target_params.structSize	= NVPW_InitializeTarget_Params_STRUCT_SIZE;
			NVP_CHECK_ERR( _impl->InitializeTarget( &init_target_params ));
		}{
			NVPW_VK_LoadDriver_Params		load_driver_params = {};
			load_driver_params.structSize	= NVPW_VK_LoadDriver_Params_STRUCT_SIZE;
			load_driver_params.instance		= dev.GetVkInstance();
			NVP_CHECK_ERR( _impl->VK_LoadDriver( &load_driver_params ));
		}{
			NVPW_VK_Device_GetDeviceIndex_Params	get_dev_id = {};
			get_dev_id.structSize				= NVPW_VK_Device_GetDeviceIndex_Params_STRUCT_SIZE;
			get_dev_id.instance					= dev.GetVkInstance();
			get_dev_id.physicalDevice			= dev.GetVkPhysicalDevice();
			get_dev_id.device					= dev.GetVkDevice();
			get_dev_id.sliIndex					= 0;
			get_dev_id.pfnGetInstanceProcAddr	= BitCast<void*>( _var_vkGetInstanceProcAddr );
			get_dev_id.pfnGetDeviceProcAddr		= BitCast<void*>( _var_vkGetDeviceProcAddr );

			NVP_CHECK_ERR( _impl->VK_Device_GetDeviceIndex( INOUT &get_dev_id ));
			_impl->deviceId = get_dev_id.deviceIndex;
		}{
			NVPW_VK_Profiler_IsGpuSupported_Params	is_supported = {};
			is_supported.structSize		= NVPW_VK_Profiler_IsGpuSupported_Params_STRUCT_SIZE;
			is_supported.deviceIndex	= _impl->deviceId;
			NVP_CHECK_ERR( _impl->VK_Profiler_IsGpuSupported( &is_supported ));

			if ( not is_supported.isSupported )
			{
				if ( is_supported.gpuArchitectureSupportLevel != NVPW_GPU_ARCHITECTURE_SUPPORT_LEVEL_SUPPORTED ){
					AE_LOG_DBG( "NvPerf: GPU architecture is not supported" );
				}else
				if ( is_supported.cmpSupportLevel == NVPW_CMP_SUPPORT_LEVEL_UNSUPPORTED ){
					AE_LOG_DBG( "NvPerf: Cryptomining GPUs (NVIDIA CMP) are not supported" );
				}else
					AE_LOG_DBG( "NvPerf: GPU is not supported" );
			}
		}{
			NVPW_Device_GetNames_Params		get_names = {};
			get_names.structSize	= NVPW_Device_GetNames_Params_STRUCT_SIZE;
			get_names.deviceIndex	= _impl->deviceId;
			NVP_CHECK_ERR( _impl->Device_GetNames( INOUT &get_names ));
			_impl->info.deviceName	= get_names.pDeviceName;
			_impl->info.chipName	= get_names.pChipName;
		}

		InitializeMetrics();

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  VNvPerfProfiler::Deinitialize () __NE___
	{
		if ( IsInitialized() )
			SetStableClockState( false );

		_module.Unload();
		_impl.reset();
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  VNvPerfProfiler::IsInitialized () C_NE___
	{
		return	IsLoaded()				and
				_impl != null			and
				_impl->deviceId != UMax;
	}

/*
=================================================
	GetInstanceExtensions
=================================================
*/
	bool  VNvPerfProfiler::GetInstanceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( dev.GetVkInstance() == Default );

		const auto	ver = dev.GetInstanceVersion();

		NVPW_VK_Profiler_GetRequiredInstanceExtensions_Params	params = {};
		params.structSize	= NVPW_VK_Profiler_GetRequiredInstanceExtensions_Params_STRUCT_SIZE;
		params.apiVersion	= VK_MAKE_VERSION( ver.major, ver.minor, 0 );

		NVP_CHECK_ERR( _impl->VK_Profiler_GetRequiredInstanceExtensions( INOUT &params ));
		CHECK_ERR( params.isOfficiallySupportedVersion != 0 );

		for (uint i = 0; i < params.numInstanceExtensionNames; ++i) {
			extensions.push_back( params.ppInstanceExtensionNames[i] );  // throw
		}
		return true;
	}

/*
=================================================
	GetDeviceExtensions
=================================================
*/
	bool  VNvPerfProfiler::GetDeviceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( dev.GetVkPhysicalDevice() != Default );
		CHECK_ERR( dev.GetVkDevice() == Default );

		const auto	ver = dev.GetDeviceVersion();

		NVPW_VK_Profiler_GetRequiredDeviceExtensions_Params		params = {};
		params.structSize				= NVPW_VK_Profiler_GetRequiredDeviceExtensions_Params_STRUCT_SIZE;
		params.apiVersion				= VK_MAKE_VERSION( ver.major, ver.minor, 0 );
		params.instance					= dev.GetVkInstance();
		params.physicalDevice			= dev.GetVkPhysicalDevice();
		params.pfnGetInstanceProcAddr	= BitCast<void*>( _var_vkGetInstanceProcAddr );

		NVP_CHECK_ERR( _impl->VK_Profiler_GetRequiredDeviceExtensions( INOUT &params ));
		CHECK_ERR( params.isOfficiallySupportedVersion != 0 );

		for (uint i = 0; i < params.numDeviceExtensionNames; ++i) {
			extensions.push_back( params.ppDeviceExtensionNames[i] );  // throw
		}
		return true;
	}

/*
=================================================
	SetStableClockState
=================================================
*/
	bool  VNvPerfProfiler::SetStableClockState (bool stable) C_NE___
	{
		CHECK_ERR( IsInitialized() );

		NVPW_Device_SetClockSetting_Params	params;
		params.structSize	= NVPW_Device_SetClockSetting_Params_STRUCT_SIZE;
		params.pPriv		= null;
		params.deviceIndex	= _impl->deviceId;
		params.clockSetting	= stable ? NVPW_DEVICE_CLOCK_SETTING_LOCK_TO_RATED_TDP : NVPW_DEVICE_CLOCK_SETTING_DEFAULT;

		NVP_CHECK_ERR( _impl->Device_SetClockSetting( &params ));
		return true;
	}

/*
=================================================
	GetHWInfo
=================================================
*/
	VNvPerfProfiler::HWInfo  VNvPerfProfiler::GetHWInfo () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->info;
	}

/*
=================================================
	InitializeMetrics
=================================================
*/
	bool  VNvPerfProfiler::InitializeMetrics () __NE___
	{
		CHECK_ERR( IsInitialized() );
		CHECK_ERR( _impl->evaluator == null );

		Bytes	scratch_buffer_size;
		{
			NVPW_VK_MetricsEvaluator_CalculateScratchBufferSize_Params	params = {};
			params.structSize	= NVPW_VK_MetricsEvaluator_CalculateScratchBufferSize_Params_STRUCT_SIZE;
			params.pChipName	= _impl->info.chipName.data();
			NVP_CHECK_ERR( _impl->VK_MetricsEvaluator_CalculateScratchBufferSize( INOUT &params ));
			scratch_buffer_size = Bytes{params.scratchBufferSize};
		}
		NOTHROW_ERR( _impl->scratchBuffer.resize( usize(scratch_buffer_size) ));
		{
			NVPW_VK_MetricsEvaluator_Initialize_Params	params = {};
			params.structSize			= NVPW_VK_MetricsEvaluator_Initialize_Params_STRUCT_SIZE;
			params.pScratchBuffer		= _impl->scratchBuffer.data();
			params.scratchBufferSize	= _impl->scratchBuffer.size();
			params.pChipName			= _impl->info.chipName.data();
			NVP_CHECK_ERR( _impl->VK_MetricsEvaluator_Initialize( INOUT &params ));
			_impl->evaluator = params.pMetricsEvaluator;
		}

		// enumerate counters
		/*{
			struct MetricAttribute
			{
				NVPW_MetricType		metricType;
				NVPW_RollupOp		rollupOp;
				NVPW_Submetric		submetric;
			};
			const MetricAttribute	metricAttributes[] = {
				{NVPW_METRIC_TYPE_COUNTER},
				{NVPW_METRIC_TYPE_RATIO},
				{NVPW_METRIC_TYPE_THROUGHPUT}
			/ *	{ NVPW_METRIC_TYPE_COUNTER,		NVPW_ROLLUP_OP_SUM,		NVPW_SUBMETRIC_NONE },
				{ NVPW_METRIC_TYPE_COUNTER,		NVPW_ROLLUP_OP_AVG,		NVPW_SUBMETRIC_NONE },
				{ NVPW_METRIC_TYPE_COUNTER,		NVPW_ROLLUP_OP_MAX,		NVPW_SUBMETRIC_NONE },
				{ NVPW_METRIC_TYPE_COUNTER,		NVPW_ROLLUP_OP_MIN,		NVPW_SUBMETRIC_NONE },
				{ NVPW_METRIC_TYPE_RATIO,		NVPW_ROLLUP_OP_AVG,		NVPW_SUBMETRIC_RATIO },
				{ NVPW_METRIC_TYPE_RATIO,		NVPW_ROLLUP_OP_AVG,		NVPW_SUBMETRIC_MAX_RATE },
				{ NVPW_METRIC_TYPE_RATIO,		NVPW_ROLLUP_OP_AVG,		NVPW_SUBMETRIC_PCT },* /
			};

			for (auto& attr : metricAttributes)
			{
				NVPW_MetricsEvaluator_GetMetricNames_Params	metric_names = {};
				metric_names.structSize			= NVPW_MetricsEvaluator_GetMetricNames_Params_STRUCT_SIZE;
				metric_names.pMetricsEvaluator	= _impl->evaluator;
				metric_names.metricType			= uint8_t(attr.metricType);
				NVP_CHECK_ERR( _impl->MetricsEvaluator_GetMetricNames( INOUT &metric_names ));

				for (usize i = 0; i < metric_names.numMetrics; ++i)
				{
					const auto	counter_name	= StringView{metric_names.pMetricNames + metric_names.pMetricNameBeginIndices[i] };

					if ( HasSubString( counter_name, "Triage" ))
						continue;	// already duplicate

					AE_LOGI( counter_name );
				}
				AE_LOGI( "-----" );
			}
		}*/

		/*
			TODO: use counters:
				pcie__read_bytes, pcie__write_bytes	- host memory access

				sm__throughput				- shaders %
				l1tex__throughput			- L1 texture cache %
				lts__throughput				- L2 cache %
				dram__throughput			- DRAM %
				raster__throughput			- raster %
				prop__throughput			- pre-rasterization %
				zrop__throughput			- depth test %
				crop__throughput			- color blend %

				idc__request_hit_rate		- Indexed Constants, IDC cache hit rate
				l1tex__t_sector_hit_rate	- Local, Global, Texture, Surface - L1 TEX Cache Hit-Rate %
				lts__t_sector_hit_rate		- All Cached Memory - L2 Cache Hit-Rate %

				fe__output_ops_type_bundle_cmd_go_idle	- Stalls - Wait For Idle Commands
				fe__pixel_shader_barriers				- Stalls - Pixel Shader Barriers

				sm__cycles_active				- SM Active Cycles
				tpc__cycles_active_shader_3d	- SM Active Cycles - 3D
				sm__cycles_active_shader_cs		- SM Active Cycles - Compute
				sm__issue_active				- SM Instruction Issue Cycles
				sm__warps_active				- Warp Occupancy (per SM)
		*/
		return true;
	}


} // AE::Graphics
//-----------------------------------------------------------------------------
# else // not AE_ENABLE_NVPERF

#	include "graphics/Vulkan/Utils/VNvPerfProfiler.h"

namespace AE::Graphics
{
	struct VNvPerfProfiler::Impl {};

	VNvPerfProfiler::VNvPerfProfiler ()															__NE___ {}
	VNvPerfProfiler::~VNvPerfProfiler ()														__NE___ {}

	bool  VNvPerfProfiler::Load ()																__NE___ { return false; }

	bool  VNvPerfProfiler::Initialize (const VDevice &)											__NE___	{ return false; }
	void  VNvPerfProfiler::Deinitialize ()														__NE___ {}
	bool  VNvPerfProfiler::IsInitialized ()														C_NE___ { return false; }

	bool  VNvPerfProfiler::GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___	{ return false; }
	bool  VNvPerfProfiler::GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___	{ return false; }

	bool  VNvPerfProfiler::SetStableClockState (bool stable)									C_NE___	{ return false; }

	bool  VNvPerfProfiler::InitializeMetrics ()													__NE___	{ return false; }

} // AE::Graphics

# endif // AE_ENABLE_NVPERF
#endif // AE_ENABLE_VULKAN
