// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Utils/VNvPerf.h"
# include "graphics/Vulkan/VDevice.h"

# ifdef AE_ENABLE_NVPERF
#   include "nvperf_host.h"
#   include "nvperf_target.h"
#   include "nvperf_vulkan_host.h"
#   include "nvperf_vulkan_target.h"

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
            String  msg = "NVPerf error: ";
            switch_enum( err )
            {
                case NVPA_STATUS_ERROR :                            msg << "ERROR";                         break;
                case NVPA_STATUS_INTERNAL_ERROR :                   msg << "INTERNAL_ERROR";                break;
                case NVPA_STATUS_NOT_INITIALIZED :                  msg << "NOT_INITIALIZED";               break;
                case NVPA_STATUS_NOT_LOADED :                       msg << "NOT_LOADED";                    break;
                case NVPA_STATUS_FUNCTION_NOT_FOUND :               msg << "FUNCTION_NOT_FOUND";            break;
                case NVPA_STATUS_NOT_SUPPORTED :                    msg << "NOT_SUPPORTED";                 break;
                case NVPA_STATUS_NOT_IMPLEMENTED :                  msg << "NOT_IMPLEMENTED";               break;
                case NVPA_STATUS_INVALID_ARGUMENT :                 msg << "INVALID_ARGUMENT";              break;
                case NVPA_STATUS_INVALID_METRIC_ID :                msg << "INVALID_METRIC_ID";             break;
                case NVPA_STATUS_DRIVER_NOT_LOADED :                msg << "DRIVER_NOT_LOADED";             break;
                case NVPA_STATUS_OUT_OF_MEMORY :                    msg << "OUT_OF_MEMORY";                 break;
                case NVPA_STATUS_INVALID_THREAD_STATE :             msg << "INVALID_THREAD_STATE";          break;
                case NVPA_STATUS_FAILED_CONTEXT_ALLOC :             msg << "FAILED_CONTEXT_ALLOC";          break;
                case NVPA_STATUS_UNSUPPORTED_GPU :                  msg << "UNSUPPORTED_GPU";               break;
                case NVPA_STATUS_INSUFFICIENT_DRIVER_VERSION :      msg << "INSUFFICIENT_DRIVER_VERSION";   break;
                case NVPA_STATUS_OBJECT_NOT_REGISTERED :            msg << "OBJECT_NOT_REGISTERED";         break;
                case NVPA_STATUS_INSUFFICIENT_PRIVILEGE :           msg << "INSUFFICIENT_PRIVILEGE";        break;
                case NVPA_STATUS_INVALID_CONTEXT_STATE :            msg << "INVALID_CONTEXT_STATE";         break;
                case NVPA_STATUS_INVALID_OBJECT_STATE :             msg << "INVALID_OBJECT_STATE";          break;
                case NVPA_STATUS_RESOURCE_UNAVAILABLE :             msg << "RESOURCE_UNAVAILABLE";          break;
                case NVPA_STATUS_DRIVER_LOADED_TOO_LATE :           msg << "DRIVER_LOADED_TOO_LATE";        break;
                case NVPA_STATUS_INSUFFICIENT_SPACE :               msg << "INSUFFICIENT_SPACE";            break;
                case NVPA_STATUS_OBJECT_MISMATCH :                  msg << "OBJECT_MISMATCH";               break;
                case NVPA_STATUS_VIRTUALIZED_DEVICE_NOT_SUPPORTED : msg << "VIRTUALIZED_DEVICE_NOT_SUPPORTED";break;
                case NVPA_STATUS_PROFILING_NOT_ALLOWED :            msg << "PROFILING_NOT_ALLOWED";         break;
                case NVPA_STATUS_SUCCESS :
                case NVPA_STATUS__COUNT :
                default :                                           msg << "unknown";   break;
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

#   define NVP_CHECK_ERR( ... )                                                                                         \
    {                                                                                                                   \
        auto    _err_ = (__VA_ARGS__);                                                                                  \
        if_unlikely( not _NvPerfCheckError( _err_, AE_TOSTRING(__VA_ARGS__), AE_FUNCTION_NAME, SourceLoc_Current() ))   \
            return Default;                                                                                             \
    }
}
/*
=================================================
    Impl
=================================================
*/
    struct VNvPerf::Impl
    {
    // functions
        decltype(&NVPW_InitializeHost)                                  InitializeHost                                  = null;
        decltype(&NVPW_InitializeTarget)                                InitializeTarget                                = null;

        decltype(&NVPW_Device_SetClockSetting)                          Device_SetClockSetting                          = null;
        decltype(&NVPW_Device_GetNames)                                 Device_GetNames                                 = null;

        decltype(&NVPW_MetricsEvaluator_GetMetricNames)                 MetricsEvaluator_GetMetricNames                 = null;
        decltype(&NVPW_MetricsEvaluator_GetMetricDimUnits)              MetricsEvaluator_GetMetricDimUnits              = null;

        decltype(&NVPW_VK_LoadDriver)                                   VK_LoadDriver                                   = null;
        decltype(&NVPW_VK_Device_GetDeviceIndex)                        VK_Device_GetDeviceIndex                        = null;

        decltype(&NVPW_VK_Profiler_GetRequiredInstanceExtensions)       VK_Profiler_GetRequiredInstanceExtensions       = null;
        decltype(&NVPW_VK_Profiler_GetRequiredDeviceExtensions)         VK_Profiler_GetRequiredDeviceExtensions         = null;
        decltype(&NVPW_VK_Profiler_IsGpuSupported)                      VK_Profiler_IsGpuSupported                      = null;

        decltype(&NVPW_VK_MetricsEvaluator_CalculateScratchBufferSize)  VK_MetricsEvaluator_CalculateScratchBufferSize  = null;
        decltype(&NVPW_VK_MetricsEvaluator_Initialize)                  VK_MetricsEvaluator_Initialize                  = null;


    // variables
        usize                   deviceId        = UMax;
        StringView              deviceName;
        StringView              chipName;

        NVPW_MetricsEvaluator*  evaluator       = null;
        Array<ubyte>            scratchBuffer;
    };

/*
=================================================
    constructor / destructor
=================================================
*/
    VNvPerf::VNvPerf () __NE___
    {}

    VNvPerf::~VNvPerf () __NE___
    {}

/*
=================================================
    Load
=================================================
*/
    bool  VNvPerf::Load () __NE___
    {
        CHECK_ERR( not IsLoaded() );

      #ifdef AE_PLATFORM_WINDOWS
        CHECK_ERR( _module.Load( "nvperf_grfx_host.dll" ));
      #else
        CHECK_ERR( _module.Load( "nvperf_grfx_host.so" ));
      #endif

        _impl.reset( new Impl{} );

        CHECK_ERR( _module.GetProcAddr( "NVPW_InitializeHost",                                  OUT _impl->InitializeHost ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_InitializeTarget",                                OUT _impl->InitializeTarget ));

        CHECK_ERR( _module.GetProcAddr( "NVPW_Device_SetClockSetting",                          OUT _impl->Device_SetClockSetting ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_Device_GetNames",                                 OUT _impl->Device_GetNames ));

        CHECK_ERR( _module.GetProcAddr( "NVPW_MetricsEvaluator_GetMetricNames",                 OUT _impl->MetricsEvaluator_GetMetricNames ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_MetricsEvaluator_GetMetricDimUnits",              OUT _impl->MetricsEvaluator_GetMetricDimUnits ));

        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_LoadDriver",                                   OUT _impl->VK_LoadDriver ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_Device_GetDeviceIndex",                        OUT _impl->VK_Device_GetDeviceIndex ));

        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_Profiler_GetRequiredInstanceExtensions",       OUT _impl->VK_Profiler_GetRequiredInstanceExtensions ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_Profiler_GetRequiredDeviceExtensions",         OUT _impl->VK_Profiler_GetRequiredDeviceExtensions ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_Profiler_IsGpuSupported",                      OUT _impl->VK_Profiler_IsGpuSupported ));

        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_MetricsEvaluator_CalculateScratchBufferSize",  OUT _impl->VK_MetricsEvaluator_CalculateScratchBufferSize ));
        CHECK_ERR( _module.GetProcAddr( "NVPW_VK_MetricsEvaluator_Initialize",                  OUT _impl->VK_MetricsEvaluator_Initialize ));

        return true;
    }

/*
=================================================
    Initialize
=================================================
*/
    bool  VNvPerf::Initialize (const VDevice &dev) __NE___
    {
        CHECK_ERR( not IsInitialized() );

        if ( _Initialize( dev ))
            return true;

        Deinitialize();
        return false;
    }

    bool  VNvPerf::_Initialize (const VDevice &dev) __NE___
    {
        CHECK_ERR( IsLoaded() );
        CHECK_ERR( dev.GetVkInstance() != Default );
        CHECK_ERR( dev.GetVkPhysicalDevice() != Default );
        CHECK_ERR( dev.GetVkDevice() != Default );

        const auto  NVIDIA_VENDOR_ID = 0x10de;
        if ( dev.GetVProperties().properties.vendorID != NVIDIA_VENDOR_ID )
            return false;

        {
            NVPW_InitializeHost_Params  init_host_params    = {};
            init_host_params.structSize = NVPW_InitializeHost_Params_STRUCT_SIZE;
            NVP_CHECK_ERR( _impl->InitializeHost( &init_host_params ));
        }{
            NVPW_InitializeTarget_Params    init_target_params  = {};
            init_target_params.structSize   = NVPW_InitializeTarget_Params_STRUCT_SIZE;
            NVP_CHECK_ERR( _impl->InitializeTarget( &init_target_params ));
        }{
            NVPW_VK_LoadDriver_Params       load_driver_params = {};
            load_driver_params.structSize   = NVPW_VK_LoadDriver_Params_STRUCT_SIZE;
            load_driver_params.instance     = dev.GetVkInstance();
            NVP_CHECK_ERR( _impl->VK_LoadDriver( &load_driver_params ));
        }{
            NVPW_VK_Device_GetDeviceIndex_Params    get_dev_id = {};
            get_dev_id.structSize               = NVPW_VK_Device_GetDeviceIndex_Params_STRUCT_SIZE;
            get_dev_id.instance                 = dev.GetVkInstance();
            get_dev_id.physicalDevice           = dev.GetVkPhysicalDevice();
            get_dev_id.device                   = dev.GetVkDevice();
            get_dev_id.sliIndex                 = 0;
            get_dev_id.pfnGetInstanceProcAddr   = BitCast<void*>( _var_vkGetInstanceProcAddr );
            get_dev_id.pfnGetDeviceProcAddr     = BitCast<void*>( _var_vkGetDeviceProcAddr );

            NVP_CHECK_ERR( _impl->VK_Device_GetDeviceIndex( INOUT &get_dev_id ));
            _impl->deviceId = get_dev_id.deviceIndex;
        }{
            NVPW_VK_Profiler_IsGpuSupported_Params  is_supported = {};
            is_supported.structSize     = NVPW_VK_Profiler_IsGpuSupported_Params_STRUCT_SIZE;
            is_supported.deviceIndex    = _impl->deviceId;
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
            NVPW_Device_GetNames_Params     get_names = {};
            get_names.structSize    = NVPW_Device_GetNames_Params_STRUCT_SIZE;
            get_names.deviceIndex   = _impl->deviceId;
            NVP_CHECK_ERR( _impl->Device_GetNames( INOUT &get_names ));
            _impl->deviceName   = get_names.pDeviceName;
            _impl->chipName     = get_names.pChipName;
        }

        InitializeMetrics();

        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  VNvPerf::Deinitialize () __NE___
    {
        if ( IsInitialized() )
            SetStableClockState( false );

        _module.Unload();
        _impl.reset( null );
    }

/*
=================================================
    IsInitialized
=================================================
*/
    bool  VNvPerf::IsInitialized () C_NE___
    {
        return  IsLoaded()              and
                _impl != null           and
                _impl->deviceId != UMax;
    }

/*
=================================================
    GetInstanceExtensions
=================================================
*/
    bool  VNvPerf::GetInstanceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
    {
        CHECK_ERR( IsLoaded() );
        CHECK_ERR( dev.GetVkInstance() == Default );

        const auto  ver = dev.GetInstanceVersion();

        NVPW_VK_Profiler_GetRequiredInstanceExtensions_Params   params = {};
        params.structSize   = NVPW_VK_Profiler_GetRequiredInstanceExtensions_Params_STRUCT_SIZE;
        params.apiVersion   = VK_MAKE_VERSION( ver.major, ver.minor, 0 );

        NVP_CHECK_ERR( _impl->VK_Profiler_GetRequiredInstanceExtensions( INOUT &params ));
        CHECK_ERR( params.isOfficiallySupportedVersion );

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
    bool  VNvPerf::GetDeviceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
    {
        CHECK_ERR( IsLoaded() );
        CHECK_ERR( dev.GetVkPhysicalDevice() != Default );
        CHECK_ERR( dev.GetVkDevice() == Default );

        const auto  ver = dev.GetDeviceVersion();

        NVPW_VK_Profiler_GetRequiredDeviceExtensions_Params     params = {};
        params.structSize               = NVPW_VK_Profiler_GetRequiredDeviceExtensions_Params_STRUCT_SIZE;
        params.apiVersion               = VK_MAKE_VERSION( ver.major, ver.minor, 0 );
        params.instance                 = dev.GetVkInstance();
        params.physicalDevice           = dev.GetVkPhysicalDevice();
        params.pfnGetInstanceProcAddr   = BitCast<void*>( _var_vkGetInstanceProcAddr );

        NVP_CHECK_ERR( _impl->VK_Profiler_GetRequiredDeviceExtensions( INOUT &params ));
        CHECK_ERR( params.isOfficiallySupportedVersion );

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
    bool  VNvPerf::SetStableClockState (bool stable) C_NE___
    {
        CHECK_ERR( IsInitialized() );

        NVPW_Device_SetClockSetting_Params  params;
        params.structSize   = NVPW_Device_SetClockSetting_Params_STRUCT_SIZE;
        params.pPriv        = null;
        params.deviceIndex  = _impl->deviceId;
        params.clockSetting = stable ? NVPW_DEVICE_CLOCK_SETTING_LOCK_TO_RATED_TDP : NVPW_DEVICE_CLOCK_SETTING_DEFAULT;

        NVP_CHECK_ERR( _impl->Device_SetClockSetting( &params ));
        return true;
    }

/*
=================================================
    GetDeviceName / GetChipName
=================================================
*/
    StringView  VNvPerf::GetDeviceName () C_NE___
    {
        CHECK_ERR( IsInitialized() );
        return _impl->deviceName;
    }

    StringView  VNvPerf::GetChipName () C_NE___
    {
        CHECK_ERR( IsInitialized() );
        return _impl->chipName;
    }


} // AE::Graphics
//-----------------------------------------------------------------------------

# else

namespace AE::Graphics
{
    struct VNvPerf::Impl {};

    VNvPerf::VNvPerf ()                                                                 __NE___ {}
    VNvPerf::~VNvPerf ()                                                                __NE___ {}

    bool  VNvPerf::Load ()                                                              __NE___ { return false; }

    bool  VNvPerf::Initialize (const VDevice &)                                         __NE___ { return false; }
    void  VNvPerf::Deinitialize ()                                                      __NE___ {}
    bool  VNvPerf::IsInitialized ()                                                     C_NE___ { return false; }

    bool  VNvPerf::GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)  __Th___ { return false; }
    bool  VNvPerf::GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)    __Th___ { return false; }

    bool  VNvPerf::SetStableClockState (bool stable)                                    C_NE___ { return false; }

    bool  VNvPerf::InitializeMetrics ()                                                 __NE___ { return false; }

} // AE::Graphics

# endif // AE_ENABLE_NVPERF
#endif // AE_ENABLE_VULKAN
