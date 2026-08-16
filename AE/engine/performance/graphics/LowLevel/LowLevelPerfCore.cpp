// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "LowLevelPerfCore.h"

#define ENABLE_SYNC_LOG		0

namespace AE::Graphics
{
namespace
{
	using namespace AE::Threading;

	static const char	c_CacheName []	= "PerfGraphics.PipelineCache.bin";
}

/*
=================================================
	constructor / destructor
=================================================
*/
	LowLevelPerfCore::LowLevelPerfCore ()
	{
		#ifdef AE_RESULT_PATH
			FileSystem::CreateDirectories( AE_RESULT_PATH );
		#endif
	}

	LowLevelPerfCore::~LowLevelPerfCore ()
	{
		_DestroyProfiler();
		_DestroyResources();
		_DestroyGraphics();
	}

/*
=================================================
	Run
=================================================
*/
	bool  LowLevelPerfCore::Run (FStorage_t assetStorage)
	{
		GraphicsCreateInfo	cfg;
		cfg.device.devFlags		= EDeviceFlags::SetStableClock;
		cfg.device.validation	= EDeviceValidation::Enabled;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		cfg.deviceAddr			= AE_RMG_IPv4;
	  #endif

		CHECK_ERR( _InitGraphics( cfg ));
		CHECK_ERR( _InitResources( assetStorage ));
		_InitProfiler();

		_NaNTest();
		_InstructionBenchmark();

		// TODO:
		//	- branching
		//	- texture access
		//	- buffer access
		//	- ray tracing ?
		//	- vertex cache ?
		//	- fast hash, noise

		return true;
	}

/*
=================================================
	_InitGraphics
=================================================
*/
	bool  LowLevelPerfCore::_InitGraphics (const GraphicsCreateInfo &cfg)
	{
		if_unlikely( _device.IsInitialized() )
			return true;

		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	ts_cfg;
		CHECK_ERR( Scheduler().Setup( ts_cfg ));

		CHECK_ERR( Networking::SocketService::Instance().Initialize() );

		RenderTaskScheduler::InstanceCtor::Create( _device );

	  #ifdef AE_ENABLE_VULKAN
		CHECK_ERR( _device.Init( cfg, Default ));

		#if ENABLE_SYNC_LOG
		{
			FlatHashMap<VkQueue, String>	qnames;

			for (auto& q : _device.GetQueues()) {
				qnames.emplace( q.handle, q.debugName );
			}
			VulkanSyncLog::Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
			VulkanSyncLog::Enable();
		}
		#endif

	  #elif defined(AE_ENABLE_METAL)
		CHECK_ERR( _device.Init( cfg ));

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		CHECK_ERR( _device.Init( cfg ));

		#if ENABLE_SYNC_LOG
			_device.EnableSyncLog( true );
		#endif

	  #else
	  #	error not implemented
	  #endif

		CHECK_ERR( _device.CheckConstantLimits() );
		CHECK_ERR( _device.CheckExtensions() );

		CHECK_ERR( GraphicsScheduler().Initialize( cfg ));

		_isHighPerf = (_device.AdapterType() == EGraphicsAdapterType::Discrete);

		#ifdef AE_PLATFORM_APPLE
		_isHighPerf = true;
		#endif

		return true;
	}

/*
=================================================
	_DestroyGraphics
=================================================
*/
	void  LowLevelPerfCore::_DestroyGraphics ()
	{
		if_unlikely( not _device.IsInitialized() )
			return;

		Unused( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));	// TODO ???

		RenderTaskScheduler::InstanceCtor::Destroy();

		#if ENABLE_SYNC_LOG
			VulkanSyncLog::Deinitialize( INOUT _device.EditDeviceFnTable() );
		#endif

		CHECK_ERRV( _device.DestroyLogicalDevice() );
		CHECK_ERRV( _device.DestroyInstance() );

		Networking::SocketService::Instance().Deinitialize();
		Scheduler().Release();
		TaskScheduler::InstanceCtor::Destroy();
	}

/*
=================================================
	_InitResources
=================================================
*/
	bool  LowLevelPerfCore::_InitResources (FStorage_t assetStorage)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		{
		  #ifdef AE_ENABLE_METAL
			RC<RStream>	file;
			CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Perf.Graphics.mtlPipelines.bin"} ));
		  #else
			RC<RStream>	file;
			CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Perf.Graphics.vkPipelines.bin"} ));
		  #endif

			PipelinePackDesc	desc;
			desc.stream = file;

			auto	pack_id = res_mngr.LoadPipelinePack( desc );
			CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));
		}

		_res.dim = _isHighPerf ? uint2{4 << 10} : uint2{2 << 10};

		_res.rt = res_mngr.CreateImage( ImageDesc{}.SetFormat( EPixelFormat::RGBA8_UNorm ).SetDimension( _res.dim )
													.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Transfer ));
		CHECK_ERR( _res.rt );

		_res.rtView = res_mngr.CreateImageView( ImageViewDesc{}, _res.rt );
		CHECK_ERR( _res.rtView );

		_res.img = res_mngr.CreateImage( ImageDesc{}.SetFormat( EPixelFormat::RGBA8_UNorm ).SetDimension( _res.dim )
													.SetUsage( EImageUsage::Storage | EImageUsage::Transfer ));
		CHECK_ERR( _res.img );

		_res.imgView = res_mngr.CreateImageView( ImageViewDesc{}, _res.img );
		CHECK_ERR( _res.imgView );

		_res.hostBuf = res_mngr.CreateBuffer( BufferDesc{}.SetSize( 16_b ).SetUsage( EBufferUsage::Transfer ).SetMemory( EMemoryType::HostCoherent ));
		CHECK_ERR( _res.hostBuf );

		{
			RC<RStream>		file = MakeRC<FileRStream>( c_CacheName );
			if ( not file->IsOpen() )
				file = null;

			_res.pplnCache = res_mngr.CreatePipelineCache( RVRef(file), Default );
			if ( not _res.pplnCache )
				_res.pplnCache = res_mngr.CreatePipelineCache();
		}

		return true;
	}

/*
=================================================
	_DestroyResources
=================================================
*/
	void  LowLevelPerfCore::_DestroyResources ()
	{
		if_unlikely( not _device.IsInitialized() )
			return;

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		CHECK( res_mngr.SerializePipelineCache( _res.pplnCache, MakeRC<FileWStream>( c_CacheName )));

		res_mngr.ReleaseResources(
			_res.rt, _res.rtView,
			_res.img, _res.imgView,
			_res.hostBuf, _res.pplnCache );
	}

/*
=================================================
	_InitProfiler
=================================================
*/
	void  LowLevelPerfCore::_InitProfiler ()
	{
		{
			using ECounter = Profiler::MaliProfiler::ECounter;

			Unused( _profilers.mali.Initialize( EnumSet<ECounter>{ ECounter::GPUActiveCy } ));
		}{
			using ECounter = Profiler::AdrenoProfiler::ECounter;

			Unused( _profilers.adreno.Initialize( EnumSet<ECounter>{ ECounter::PC_DrawCalls3D } ));
		}{
			using ECounter = Profiler::PowerVRProfiler::ECounter;

			Unused( _profilers.pvr.Initialize( EnumSet<ECounter>{ ECounter::GPU_ClockSpeed } ));
		}
	}

/*
=================================================
	_DestroyProfiler
=================================================
*/
	void  LowLevelPerfCore::_DestroyProfiler ()
	{
		_profilers.mali.Deinitialize();
		_profilers.adreno.Deinitialize();
		_profilers.pvr.Deinitialize();
	}

/*
=================================================
	_ResetProfilers
=================================================
*/
	void  LowLevelPerfCore::_ResetProfilers () const
	{
		if ( _profilers.mali.IsInitialized() )
		{
			Profiler::MaliProfiler::Counters_t			counters;
			float										invdt;
			_profilers.mali.Sample( OUT counters, INOUT invdt );
		}

		if ( _profilers.adreno.IsInitialized() )
		{
			Profiler::AdrenoProfiler::Counters_t		counters;
			float										invdt;
			_profilers.adreno.Sample( OUT counters, INOUT invdt );
		}

		if ( _profilers.pvr.IsInitialized() )
		{
			Profiler::PowerVRProfiler::TimeScopeArr_t	timings;
			_profilers.pvr.ReadTimingData( OUT timings );

			Profiler::PowerVRProfiler::Counters_t		counters;
			float										invdt;
			_profilers.pvr.Sample( OUT counters, INOUT invdt );
		}
	}

/*
=================================================
	_ReadProfilers
=================================================
*/
	void  LowLevelPerfCore::_ReadProfilers (OUT nanosecondsd* time, usize count) const
	{
		ThreadUtils::MilliSleep( milliseconds{500} );

		if ( _profilers.mali.IsInitialized() )
		{
			Profiler::MaliProfiler::Counters_t		counters;
			float									invdt;
			_profilers.mali.Sample( OUT counters, INOUT invdt );
		}

		if ( _profilers.adreno.IsInitialized() )
		{
			Profiler::AdrenoProfiler::Counters_t		counters;
			float										invdt;
			_profilers.adreno.Sample( OUT counters, INOUT invdt );
		}

		if ( _profilers.pvr.IsInitialized() )
		{
			Profiler::PowerVRProfiler::Counters_t		counters;
			float										invdt;
			_profilers.pvr.Sample( OUT counters, INOUT invdt );

			Profiler::PowerVRProfiler::TimeScopeArr_t	timings;
			_profilers.pvr.ReadTimingData( OUT timings );

			usize i = 0;
			for (usize j = timings.size()-1; i < count and j < timings.size(); --j)
			{
				if ( timings[j].pass == Profiler::PowerVRProfiler::EPass::Renderer )
				{
					time[i+0] = timings[j].begin;
					time[i+1] = timings[j].end;		i += 2;
				}
			}

			CHECK( i > 0 );

			for (; i < count; ++i)
				time[i] = nanosecondsd{0.0};
		}
	}

/*
=================================================
	_SortResults
=================================================
*/
	void  LowLevelPerfCore::_SortResults ()
	{
		ResultsMap_t	map;
		map.reserve( 64 );

		for (auto& [type, results] : _results)
		{
			if ( results.empty() )
				continue;

			map.clear();
			for (usize i = 0; i < results.size(); ++i) {
				map.insert_or_assign( results[i].name, i );
			}

			for (const auto& c : _complex)
			{
				auto	lhs_it	= map.find( c.lhs );
				auto	rhs_it	= map.find( c.rhs );

				if ( lhs_it == map.end() or (rhs_it == map.end() and not c.rhs.empty()) )
				{
					//AE_LOGI( "Skip '"s << type << "-" << c.name << "'" );
					continue;
				}

				BenchResult			res;
				BenchResult&		lhs		= results[ lhs_it->second ];
				const BenchResult	rhs		= rhs_it == map.end() ? BenchResult{} : results[ rhs_it->second ];

				res.dtAvr	= (lhs.dtAvr * c.lScale) - (rhs.dtAvr * c.rScale);
				res.dtMin	= (lhs.dtMin * c.lScale) - (rhs.dtMin * c.rScale);
				res.dtMax	= (lhs.dtMax * c.lScale) - (rhs.dtMax * c.rScale);
				res.name	= c.name;

				res.dtMin	= Max( res.dtMin, nanosecondsd{0.0} );
				res.dtMax	= Max( res.dtMax, res.dtMin );

				if ( res.dtAvr < rhs.dtAvr )
				{
					res.dtAvr = rhs.dtAvr;
					AE_LOGI( "Value of '"s << type << "-" << c.name << "' is too small" );
				}

				ASSERT( res.dtMin <= res.dtMax );

				lhs.dtAvr = -1.0;	// disable

				map.insert_or_assign( res.name, results.size() );
				results.push_back( res );
			}

			std::sort(	results.begin(), results.end(),
						[](auto& lhs, auto& rhs)
						{
							return	Equal( lhs.dtAvr, rhs.dtAvr, 5_pct ) ?
										lhs.name < rhs.name :
										lhs.dtAvr < rhs.dtAvr;
						});
		}
	}

/*
=================================================
	_PrintResults
=================================================
*/
	void  LowLevelPerfCore::_PrintResults (StringView name, CustomPrintFn_t fn) const
	{
		double	global_min_dt	= MaxValue<double>();
		usize	max_len			= 0;

		Array<Pair< StringView, AllResults_t const* >>	arr;

		for (auto& [type, results] : _results)
		{
			for (auto& res : results)
			{
				if ( res.dtAvr <= 0.0 )
					continue;

				global_min_dt	= Min( res.dtAvr, global_min_dt );
				max_len			= Max( max_len, res.name.size() );
			}
			arr.emplace_back( type, &results );
		}

		std::sort( arr.begin(), arr.end(), [](auto& lhs, auto& rhs) { return StringLessThan( lhs.first, rhs.first ); });

		max_len			+= 4;
		global_min_dt	= Max( global_min_dt, 0.0 );

		String	str;
		for (auto& [type, results] : arr)
		{
			if ( results->empty() )
				continue;

			double	min_dt = MaxValue<double>();
			for (const auto& res : *results)
			{
				if ( res.dtAvr > 0.0 )
				{
					min_dt = res.dtAvr;
					break;
				}
			}

			str << "\ntype: " << type;

			for (const auto& res : *results)
			{
				if ( res.dtAvr <= 0.0 )
					continue;

				double	per_type	= Max( 0.0, 1.0 + (res.dtAvr - min_dt) / min_dt );
				double	global		= Max( 0.0, 1.0 + (res.dtAvr - global_min_dt) / global_min_dt );

				str << "\n  ";
				usize	pos	= str.size();

				str << res.name << ':';
				Parser::Align( INOUT str, pos, max_len, ' ' );

				pos = str.size();
				str << ToString( global, 1 );
				Parser::Align( INOUT str, pos, 8, ' ' );

				pos = str.size();
				str << ToString( per_type, 1 );
				Parser::Align( INOUT str, pos, 8, ' ' );

				ASSERT( res.dtMin <= res.dtMax );

				pos = str.size();
				str << " (" << ToString( res.dtMin ) << " / " << ToString( res.dtMax ) << ") ";
				Parser::Align( INOUT str, pos, 32, ' ' );

				str << ToStringSfx( (1.0e+9 / res.dtAvr) ) << "ops/s";
			}

			str << "\n----------";
		}

		if ( fn != null )
		{
			str << fn( _results );
		}

		AE_LOGI( str );

		#ifdef AE_RESULT_PATH
		{
			FileWStream	file { Path{AE_RESULT_PATH} / (String{_device.GetDeviceName()} << '-' << name << ".txt") };
			if ( file.IsOpen() )
				CHECK( file.Write( str ));
		}
		#endif
	}

/*
=================================================
	_Reset
=================================================
*/
	void  LowLevelPerfCore::_Reset ()
	{
		_complex.clear();
		_results.clear();
	}


} // AE::Graphics
