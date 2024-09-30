// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# ifdef AE_ENABLE_AMDGPUPERF

# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4668)
# endif

#	define DISABLE_GPA 0
#	include "base/Platforms/WindowsHeader.cpp.h"
#	include "gpu_performance_api/gpu_perf_api.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif

#	include "base/Defines/Undef.h"
#	include "graphics/Vulkan/Utils/VAMDPerfProfiler.h"
#	include "graphics/Vulkan/VDevice.h"

#	include "gpu_performance_api/gpu_perf_api_vk.h"

namespace AE::Graphics
{
/*
=================================================
	_LoggingCallback
=================================================
*/
namespace {
	static void  _LoggingCallback (GpaLoggingType type, const char* msg)
	{
		if ( AnyBits( type, kGpaLoggingError )) {
			AE_LOGE( "AMDPerf: "s << msg );
		}else{
			AE_LOGI( "AMDPerf: "s << msg );
		}
	}
}
/*
=================================================
	GpaCheckError
=================================================
*
namespace {
	ND_ static bool  GpaCheckError (const GpaStatus errCode, const char* fncall, const char* func, const SourceLoc &loc) __NE___
	{
		if_likely( errCode == kGpaStatusOk )
			return true;

	  #ifdef AE_ENABLE_LOGS
		TRY{
			String	msg;
			msg.reserve( 256 );
			msg << "AMD Perf error: ";

			switch_enum( errCode )
			{
				#define CASE( _code_ )	case _code_ :	msg += AE_TOSTRING( _code_ ); break;
				CASE( kGpaStatusResultNotReady )
				CASE( kGpaStatusErrorNullPointer )
				CASE( kGpaStatusErrorContextNotOpen )
				CASE( kGpaStatusErrorContextAlreadyOpen )
				CASE( kGpaStatusErrorIndexOutOfRange )
				CASE( kGpaStatusErrorCounterNotFound )
				CASE( kGpaStatusErrorAlreadyEnabled )
				CASE( kGpaStatusErrorNoCountersEnabled )
				CASE( kGpaStatusErrorNotEnabled )
				CASE( kGpaStatusErrorCommandListAlreadyEnded )
				CASE( kGpaStatusErrorCommandListAlreadyStarted )
				CASE( kGpaStatusErrorCommandListNotEnded )
				CASE( kGpaStatusErrorNotEnoughPasses )
				CASE( kGpaStatusErrorSampleNotStarted )
				CASE( kGpaStatusErrorSampleAlreadyStarted )
				CASE( kGpaStatusErrorSampleNotEnded )
				CASE( kGpaStatusErrorCannotChangeCountersWhenSampling )
				CASE( kGpaStatusErrorSessionNotFound )
				CASE( kGpaStatusErrorSampleNotFound )
				CASE( kGpaStatusErrorContextNotFound )
				CASE( kGpaStatusErrorCommandListNotFound )
				CASE( kGpaStatusErrorReadingSampleResult )
				CASE( kGpaStatusErrorVariableNumberOfSamplesInPasses )
				CASE( kGpaStatusErrorFailed )
				CASE( kGpaStatusErrorHardwareNotSupported )
				CASE( kGpaStatusErrorDriverNotSupported )
				CASE( kGpaStatusErrorApiNotSupported )
				CASE( kGpaStatusErrorInvalidParameter )
				CASE( kGpaStatusErrorLibLoadFailed )
				CASE( kGpaStatusErrorLibLoadMajorVersionMismatch )
				CASE( kGpaStatusErrorLibLoadMinorVersionMismatch )
				CASE( kGpaStatusErrorGpaNotInitialized )
				CASE( kGpaStatusErrorGpaAlreadyInitialized )
				CASE( kGpaStatusErrorSampleInSecondaryCommandList )
				CASE( kGpaStatusErrorIncompatibleSampleTypes )
				CASE( kGpaStatusErrorSessionAlreadyStarted )
				CASE( kGpaStatusErrorSessionNotStarted )
				CASE( kGpaStatusErrorSessionNotEnded )
				CASE( kGpaStatusErrorInvalidDataType )
				CASE( kGpaStatusErrorInvalidCounterEquation )
				CASE( kGpaStatusErrorTimeout )
				CASE( kGpaStatusErrorLibAlreadyLoaded )
				CASE( kGpaStatusErrorOtherSessionActive )
				CASE( kGpaStatusErrorException )
				CASE( kGpaStatusErrorInvalidCounterGroupData )
				CASE( kGpaStatusInternal )

				case kGpaStatusOk :
				default :	msg << "unknown (" << ToString(int(errCode)) << ')';  break;
			}
			switch_end

			msg << ", in " << fncall << ", function: " << func;
			AE_LOGE( msg, loc.file, loc.line );
		}
		CATCH_ALL()

	  #else
		Unused( fncall, func, loc );
	  #endif
		return false;
	}
}

/*
=================================================
	Impl
=================================================
*/
	struct VAMDPerfProfiler::Impl
	{
		GpaContextId		ctxId		= null;
		GpaFunctionTable	fnTable		= {};
		HWInfo				info;
	};

//-----------------------------------------------------------------------------


/*
=================================================
	constructor / destructor
=================================================
*/
	VAMDPerfProfiler::VAMDPerfProfiler () __NE___
	{}

	VAMDPerfProfiler::~VAMDPerfProfiler () __NE___
	{}

/*
=================================================
	Load
=================================================
*/
	bool  VAMDPerfProfiler::Load () __NE___
	{
		CHECK_ERR( not IsLoaded() );

	  #ifdef AE_PLATFORM_WINDOWS
		Unused( _module.Load( "GPUPerfAPIVK-x64.dll" ));
	  #else
		Unused( _module.Load( "libGPUPerfAPIVK.so" ));
	  #endif

		if ( not _module )
			return false;

		Unique<Impl>	impl {new Impl{}};

		decltype(&::GpaGetFuncTable)	get_fn_table;
		CHECK_ERR( _module.GetProcAddr( "GpaGetFuncTable", OUT get_fn_table ));

		impl->fnTable.major_version	= GPA_FUNCTION_TABLE_MAJOR_VERSION_NUMBER;
		impl->fnTable.minor_version	= GPA_FUNCTION_TABLE_MINOR_VERSION_NUMBER;

		CHECK_ERR( get_fn_table( OUT &impl->fnTable ) == kGpaStatusOk );

		_impl = RVRef(impl);
		return true;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  VAMDPerfProfiler::Initialize (const VDevice &dev, EDeviceFlags devFlags) __NE___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( not IsInitialized() );

		if ( _Initialize( dev, devFlags ))
			return true;

		Deinitialize();
		return false;
	}

	bool  VAMDPerfProfiler::_Initialize (const VDevice &dev, const EDeviceFlags devFlags) __NE___
	{
		if ( NoBits( devFlags, EDeviceFlags::_AmdApiMask ))
			return false;

		const auto	AMD_VENDOR_ID = 0x01002;
		if ( dev.GetVProperties().properties.vendorID != AMD_VENDOR_ID )
			return false;

		// check extensions
		{
			bool	has_ext = true;

			for (const char* ext : {AMD_GPA_REQUIRED_INSTANCE_EXTENSION_NAME_LIST}) {
				has_ext &= dev.HasInstanceExtension( ext );
			}
			for (const char* ext : {AMD_GPA_REQUIRED_DEVICE_EXTENSION_NAME_LIST}) {
				has_ext &= dev.HasDeviceExtension( ext );
			}

			if ( not has_ext )
				return false;
		}

		// enable logger
		{
			GpaLoggingType	log_type = GpaLoggingType::kGpaLoggingError;

		  #ifdef AE_DEBUG
			log_type = GpaLoggingType( GpaLoggingType::kGpaLoggingAll | GpaLoggingType::kGpaLoggingDebugAll );
		  #endif

			CHECK_ERR( _impl->fnTable.GpaRegisterLoggingCallback( log_type, &_LoggingCallback ) == kGpaStatusOk );
		}

		CHECK_ERR( _impl->fnTable.GpaInitialize( kGpaInitializeDefaultBit ) == kGpaStatusOk );

		// create context
		{
			GpaVkContextOpenInfo	ctx_info	= {};
			GpaOpenContextFlags		flags		= 0;

			ctx_info.instance			= dev.GetVkInstance();
			ctx_info.physical_device	= dev.GetVkPhysicalDevice();
			ctx_info.device				= dev.GetVkDevice();

			if ( AnyBits( devFlags, EDeviceFlags::EnablePerfCounters ))
				flags |= kGpaOpenContextEnableHardwareCountersBit;

			if ( AnyBits( devFlags, EDeviceFlags::SetStableClock ))
				flags |= kGpaOpenContextClockModeMinEngineBit;

			if ( AnyBits( devFlags, EDeviceFlags::SetStableMemClock ))
				flags |= kGpaOpenContextClockModeMinMemoryBit;

			CHECK_ERR( _impl->fnTable.GpaOpenContext( &ctx_info, flags, OUT &_impl->ctxId ) == kGpaStatusOk );
		}

		// device info
		{
			char			device_name [255];
			const char*		device_name_ptr = device_name;

			if ( _impl->fnTable.GpaGetDeviceName( _impl->ctxId, OUT &device_name_ptr ) == kGpaStatusOk )
				_impl->info.deviceName = device_name_ptr;

			_impl->fnTable.GpaGetDeviceAndRevisionId( _impl->ctxId, OUT &_impl->info.deviceId, OUT &_impl->info.revisionId );

			GpaHwGeneration	gen;
			if ( _impl->fnTable.GpaGetDeviceGeneration( _impl->ctxId, OUT &gen ) == kGpaStatusOk )
			{
				switch_enum( gen )
				{
					case kGpaHwGenerationGfx6 :		_impl->info.gen = EHwGeneration::GCN1;		break;
					case kGpaHwGenerationGfx7 :		_impl->info.gen = EHwGeneration::GCN2;		break;
					case kGpaHwGenerationGfx8 :		_impl->info.gen = EHwGeneration::GCN3_GCN4;	break;
					case kGpaHwGenerationGfx9 :		_impl->info.gen = EHwGeneration::GCN5;		break;
					case kGpaHwGenerationGfx10 :	_impl->info.gen = EHwGeneration::RDNA1;		break;
					case kGpaHwGenerationGfx103 :	_impl->info.gen = EHwGeneration::RDNA2;		break;
					case kGpaHwGenerationGfx11 :	_impl->info.gen = EHwGeneration::RDNA3;		break;
					case kGpaHwGenerationCdna :		_impl->info.gen = EHwGeneration::CDNA1;		break;
					case kGpaHwGenerationCdna2 :	_impl->info.gen = EHwGeneration::CDNA2;		break;
					case kGpaHwGenerationCdna3 :	_impl->info.gen = EHwGeneration::CDNA3;		break;

					case kGpaHwGenerationNone :
					case kGpaHwGenerationNvidia :
					case kGpaHwGenerationIntel :
					case kGpaHwGenerationLast :
					default :						break;
				}
				switch_end
			}
		}

		//_PrintCounters();

		return true;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  VAMDPerfProfiler::IsInitialized () C_NE___
	{
		return	IsLoaded()				and
				_impl != null			and
				_impl->ctxId != null;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  VAMDPerfProfiler::Deinitialize () __NE___
	{
		if ( _impl )
		{
			if ( _impl->ctxId != null )
			{
				//_impl->fnTable.GpaCloseContext( _impl->ctxId );
				_impl->ctxId = null;
			}

			_impl->fnTable.GpaDestroy();
			_impl->fnTable = {};

			_impl.reset();
		}
		_module.Unload();
	}

/*
=================================================
	_PrintCounters
=================================================
*/
	void  VAMDPerfProfiler::_PrintCounters ()
	{
		String	str;

		if ( uint count = 0;
			 _impl->fnTable.GpaGetNumCounters( _impl->ctxId, OUT &count ) == kGpaStatusOk )
		{
			for (uint i = 0; i < count; ++i)
			{
				const char*	name	= null;
				const char*	group	= null;
				const char*	desc	= null;

				auto	name_stat	= _impl->fnTable.GpaGetCounterName( _impl->ctxId, i, OUT &name );
				auto	group_stat	= _impl->fnTable.GpaGetCounterGroup( _impl->ctxId, i, OUT &group );
				auto	desc_stat	= _impl->fnTable.GpaGetCounterDescription( _impl->ctxId, i, OUT &desc );

				if ( name_stat == kGpaStatusOk and group_stat == kGpaStatusOk and desc_stat == kGpaStatusOk )
				{
					str << "| " << name << " | " << group << " | " << desc << " |\n";
				}
			}
		}
		AE_LOGI( str );
	}

/*
=================================================
	GetInstanceExtensions
=================================================
*/
	bool  VAMDPerfProfiler::GetInstanceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( dev.GetVkInstance() == Default );

		for (const char* ext : {AMD_GPA_REQUIRED_INSTANCE_EXTENSION_NAME_LIST}) {
			extensions.push_back( ext );
		}
		return true;
	}

/*
=================================================
	GetDeviceExtensions
=================================================
*/
	bool  VAMDPerfProfiler::GetDeviceExtensions (const VDevice &dev, INOUT Array<const char*> &extensions) __Th___
	{
		CHECK_ERR( IsLoaded() );
		CHECK_ERR( dev.GetVkPhysicalDevice() != Default );
		CHECK_ERR( dev.GetVkDevice() == Default );

		for (const char* ext : {AMD_GPA_REQUIRED_DEVICE_EXTENSION_NAME_LIST}) {
			extensions.push_back( ext );
		}
		for (const char* ext : {AMD_GPA_OPTIONAL_DEVICE_EXTENSION_NAME_LIST}) {
			extensions.push_back( ext );
		}
		return true;
	}

} // AE::Graphics
//-----------------------------------------------------------------------------
# else // not AE_ENABLE_AMDGPUPERF

#	include "graphics/Vulkan/Utils/VAMDPerfProfiler.h"

namespace AE::Graphics
{
	struct VAMDPerfProfiler::Impl {};

	VAMDPerfProfiler::VAMDPerfProfiler ()														__NE___	{}
	VAMDPerfProfiler::~VAMDPerfProfiler ()														__NE___	{}

	bool  VAMDPerfProfiler::IsInitialized ()													C_NE___	{ return false; }
	bool  VAMDPerfProfiler::Load ()																__NE___	{ return false; }
	bool  VAMDPerfProfiler::Initialize (const VDevice &, EDeviceFlags)							__NE___	{ return false; }
	void  VAMDPerfProfiler::Deinitialize ()														__NE___	{}

	bool  VAMDPerfProfiler::GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___	{ return false; }
	bool  VAMDPerfProfiler::GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___	{ return false; }

} // AE::Graphics

# endif // AE_ENABLE_AMDGPUPERF
#endif // AE_ENABLE_VULKAN
