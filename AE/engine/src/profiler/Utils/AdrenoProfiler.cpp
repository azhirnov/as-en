// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Profiler.pch.h"
# include "profiler/Utils/AdrenoProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct AdrenoProfiler::Impl
	{
		ECounterSet		enabled;
		HWInfo			info;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	AdrenoProfiler::AdrenoProfiler ()									__NE___	{}
	AdrenoProfiler::~AdrenoProfiler ()									__NE___	{}

	bool  AdrenoProfiler::IsInitialized ()								C_NE___	{ return bool{_impl}; }
	void  AdrenoProfiler::Deinitialize ()								__NE___	{ _impl.reset( null ); }

	AdrenoProfiler::ECounterSet	AdrenoProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->enabled : Default; }
	AdrenoProfiler::HWInfo		AdrenoProfiler::GetHWInfo ()			C_NE___	{ return _impl ? _impl->info : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  AdrenoProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfAdreno_Initialize				msg;
		RC<Msg::ProfAdreno_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->enabled	= res->enabled;
			_impl->info		= res->info;
		}
		return res->ok;
	}

/*
=================================================
	Sample
=================================================
*/
	void  AdrenoProfiler::Sample (OUT Counters_t &result) C_NE___
	{
		result.clear();

		if ( not IsInitialized() ) return;

		Msg::ProfAdreno_Sample				msg;
		RC<Msg::ProfAdreno_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_ADRENO_PERFCOUNTER)
# include <fcntl.h>
# include <sys/ioctl.h>
# include <unistd.h>

extern "C"
{
	#define ADRENO_IOCTL_TYPE 0x09

	#define ADRENO_PROPERTY_DEVICE_INFO 0x1

	struct adreno_device_info {
		unsigned int device_id;
		unsigned int chip_id;
		unsigned int mmu_enabled;
		unsigned long gmem_gpu_base_address;
		unsigned int gpu_id;
		size_t gmem_sizebytes;
	};

	struct adreno_device_get_property {
		unsigned int type;
		void *value;
		size_t num_bytes;
	};

	#define ADRENO_IOCTL_DEVICE_GET_PROPERTY \
		_IOWR(ADRENO_IOCTL_TYPE, 0x2, struct adreno_device_get_property)

	struct adreno_counter_get {
		unsigned int group_id;
		unsigned int countable_selector;
		unsigned int regster_offset_low;
		unsigned int regster_offset_high;
		unsigned int __pad;
	};

	#define ADRENO_IOCTL_COUNTER_GET \
		_IOWR(ADRENO_IOCTL_TYPE, 0x38, struct adreno_counter_get)

	struct adreno_counter_put {
		unsigned int group_id;
		unsigned int countable_selector;
		unsigned int __pad[2];
	};

	#define ADRENO_IOCTL_COUNTER_PUT \
		_IOW(ADRENO_IOCTL_TYPE, 0x39, struct adreno_counter_put)

	struct hpc_gpu_adreno_ioctl_counter_read_counter_t {
		uint32_t group_id;
		uint32_t countable_selector;
		uint64_t value;
	};

	struct adreno_counter_read {
		struct hpc_gpu_adreno_ioctl_counter_read_counter_t *counters;
		unsigned int num_counters;
		unsigned int __pad[2];
	};

	#define ADRENO_IOCTL_COUNTER_READ \
		_IOWR(ADRENO_IOCTL_TYPE, 0x3B, struct adreno_counter_read)

} // extern "C"

# include "profiler/Utils/AdrenoProfiler.h"

namespace AE::Profiler
{
namespace
{
	using ReadCounter = ::hpc_gpu_adreno_ioctl_counter_read_counter_t;

/*
=================================================
	UnpackGroupAndSelector_A5xx
----
	[A5xx](https://github.com/freedreno/envytools/blob/master/registers/adreno/a5xx.xml)
=================================================
*/
	ND_ static Pair<uint, uint>  UnpackGroupAndSelector_A5xx (const AdrenoProfiler::ECounter c) __NE___
	{
		using ECounter = AdrenoProfiler::ECounter;

		const auto	Unpack = [] (ushort packed)
		{{
			return Pair<uint, uint>{ packed >> 8, packed & 0xFF };
		}};

		if_likely( c < ECounter::_Count )
		{
			static const ushort		values [] = {

			// Command Parser
			//	0x00'xx

			// RBBM
				0x01'03,	// RBBM_RasterizerBusy
				0x01'0A,	// RBBM_VSCbusy
				0x01'0C,	// RBBM_UCHEbusy
				0x01'09,	// RBBM_VBIFbusy
				0x01'02,	// RBBM_TSEbusy

			// PC
				0x02'12,	// PC_DeadPrim
				0x02'13,	// PC_LivePrim
				0x02'15,	// PC_IA_Vertices
				0x02'16,	// PC_IA_Primitives
				0x02'1A,	// PC_VS_Invocations
				0x02'1E,	// PC_DrawCalls3D
				0x02'1F,	// PC_DrawCalls2D
				0x02'11,	// PC_VPCPrimitives

			// Vertex Fetch and Decode
				0x03'17,	// VFD_TotalVertices

			// High Level SeQuencer
			//	0x04'xx

			// Varying/Position Cache
				0x05'00,	// VPC_BusyCycles
				0x05'01,	// VPC_WorkingCycles

			// Triangle Setup Engine
			//	0x06'xx

			// Rasterizer
				0x07'04,	// RAS_SuperTiles
				0x07'05,	// RAS_8x4Tiles
				0x07'06,	// RAS_MaskgenActive
				0x07'07,	// RAS_FullyCoveredSuperTiles
				0x07'08,	// RAS_FullyCovered8x4Tiles
				0x07'09,	// RAS_PrimKilledInvisible

			// Unified L2 Cache
			//	0x08'xx

			// Texture Processor
			//	0x09'xx

			// Shader/Streaming Processor
			//	0x0A'xx

			// Render backend
				0x0B'0D,	// RB_ZRead
				0x0B'0E,	// RB_ZWrite
				0x0B'0F,	// RB_CRead
				0x0B'10,	// RB_CWrite
				0x0B'12,	// RB_Z_Pass
				0x0B'13,	// RB_Z_Fail
				0x0B'14,	// RB_S_Fail
				0x0B'18,	// RB_AliveCycles2D

			// VBIF
			//	0x0D'xx

			// Visibility Stream Compressor
				0x17'01,	// VSC_WorkingCycles

			// Cache and Compression Unit
				0x18'04,	// CCU_DepthBlocks
				0x18'05,	// CCU_ColorBlocks
				0x18'08,	// CCU_PartialBlockRead
				0x18'09,	// CCU_GMemRead
				0x18'0A,	// CCU_GMemWrite
				0x18'19,	// CCU_2DPixels

			// low resolution Z pass
				0x19'07,	// LRZ_Read
				0x19'08,	// LRZ_Write
				0x19'0B,	// LRZ_PrimKilledByMaskGen
				0x19'0C,	// LRZ_PrimKilledByLRZ
				0x19'0D,	// LRZ_PrimPassed
				0x19'10,	// LRZ_TileKilled
				0x19'11,	// LRZ_TotalPixel

			// CMP
			//	0x1A'xx

			};
			StaticAssert( CountOf(values) == uint(ECounter::_Count) );
			return Unpack( values[ uint(c) ]);
		}
		return { ~0u, 0 };
	}

/*
=================================================
	UnpackGroupAndSelector_A6xx
----
	[A6xx](https://github.com/freedreno/envytools/blob/master/registers/adreno/a6xx.xml)
=================================================
*/
	ND_ static Pair<uint, uint>  UnpackGroupAndSelector_A6xx (const AdrenoProfiler::ECounter c) __NE___
	{
		using ECounter = AdrenoProfiler::ECounter;

		const auto	Unpack = [] (ushort packed)
		{{
			return Pair<uint, uint>{ packed >> 8, packed & 0xFF };
		}};

		if_likely( c < ECounter::_Count )
		{
			static const ushort		values [] = {

			// Command Parser
			//	0x00'xx

			// RBBM
				0x01'03,	// RBBM_RasterizerBusy
				0x01'0A,	// RBBM_VSCbusy
				0x01'0C,	// RBBM_UCHEbusy
				0x01'09,	// RBBM_VBIFbusy
				0x01'02,	// RBBM_TSEbusy

			// PC
				0x02'12,	// PC_DeadPrim
				0x02'13,	// PC_LivePrim
				0x02'15,	// PC_IA_Vertices
				0x02'16,	// PC_IA_Primitives
				0x02'1A,	// PC_VS_Invocations
				0x02'1E,	// PC_DrawCalls3D
				0x02'1F,	// PC_DrawCalls2D
				0x02'11,	// PC_VPCPrimitives

			// Vertex Fetch and Decode
				0x03'17,	// VFD_TotalVertices

			// High Level SeQuencer
			//	0x04'xx

			// Varying/Position Cache
				0x05'00,	// VPC_BusyCycles
				0x05'01,	// VPC_WorkingCycles

			// Triangle Setup Engine
			//	0x06'xx

			// Rasterizer
				0x07'04,	// RAS_SuperTiles
				0x07'05,	// RAS_8x4Tiles
				0x07'06,	// RAS_MaskgenActive
				0x07'07,	// RAS_FullyCoveredSuperTiles
				0x07'08,	// RAS_FullyCovered8x4Tiles
				0x07'09,	// RAS_PrimKilledInvisible

			// Unified L2 Cache
			//	0x08'xx

			// Texture Processor
			//	0x09'xx

			// Shader/Streaming Processor
			//	0x0A'xx

			// Render backend
				0x0B'0D,	// RB_ZRead
				0x0B'0E,	// RB_ZWrite
				0x0B'0F,	// RB_CRead
				0x0B'10,	// RB_CWrite
				0x0B'12,	// RB_Z_Pass
				0x0B'13,	// RB_Z_Fail
				0x0B'14,	// RB_S_Fail
				0x0B'18,	// RB_AliveCycles2D

			// Visibility Stream Compressor
				0x17'01,	// VSC_WorkingCycles

			// Cache and Compression Unit
				0x18'04,	// CCU_DepthBlocks
				0x18'05,	// CCU_ColorBlocks
				0x18'08,	// CCU_PartialBlockRead
				0x18'09,	// CCU_GMemRead
				0x18'0A,	// CCU_GMemWrite
				0x18'19,	// CCU_2DPixels

			// low resolution Z pass
				0x19'07,	// LRZ_Read
				0x19'08,	// LRZ_Write
				0x19'0B,	// LRZ_PrimKilledByMaskGen
				0x19'0C,	// LRZ_PrimKilledByLRZ
				0x19'0D,	// LRZ_PrimPassed
				0x19'10,	// LRZ_TileKilled
				0x19'11,	// LRZ_TotalPixel

			// CMP
			//	0x1A'xx

			};
			StaticAssert( CountOf(values) == uint(ECounter::_Count) );
			return Unpack( values[ uint(c) ]);
		}
		return { ~0u, 0 };
	}

/*
=================================================
	UnpackGroupAndSelector
=================================================
*/
	ND_ static Pair<uint, uint>  UnpackGroupAndSelector (AdrenoProfiler::EGPUSeries series, const AdrenoProfiler::ECounter c) __NE___
	{
		switch_enum( series )
		{
			case AdrenoProfiler::EGPUSeries::A5xx :		return UnpackGroupAndSelector_A5xx( c );
			case AdrenoProfiler::EGPUSeries::A6xx :		return UnpackGroupAndSelector_A6xx( c );
			case AdrenoProfiler::EGPUSeries::A7xx :
            case AdrenoProfiler::EGPUSeries::Unknown :	break;
		}
		switch_end
		return { ~0u, 0 };
	}

# ifdef AE_ENABLE_LOGS
/*
=================================================
	GetGroupName
=================================================
*/
	ND_ static const char*  GetGroupName (AdrenoProfiler::EGPUSeries series, uint group)
	{
		Unused( series );
		switch ( group )
		{
			case 0 :	return "Command Parser";
			case 1 :	return "RBBM";
			case 2 :	return "PC";
			case 3 :	return "Vertex Fetch and Decode";
			case 4 :	return "High Level SeQuencer";
			case 5 :	return "Varying/Position Cache";
			case 6 :	return "Triangle Setup Engine";
			case 7 :	return "Rasterizer";
			case 8 :	return "Unified L2 Cache";
			case 9 :	return "Texture Processor";
			case 0xA :	return "Shader/Streaming Processor";
			case 0xB :	return "Render backend";
			case 0xD :	return "VBIF";
			case 0x17:	return "Visibility Stream Compressor";
			case 0x18:	return "Cache and Compression Unit";
			case 0x19:	return "Low Resolution Z pass";
			case 0x1A:	return "CMP";
		}
		return "???";
	}

/*
=================================================
	GetCounterName_A6xx
=================================================
*/
	ND_ static const char*  GetCounterName_A6xx (uint group, uint counter)
	{
		switch ( group )
		{
			case 0 :
			{
				switch ( counter )
				{
					case 0 :	return "always count";
					case 1 :	return "busy gfx core idle";
					case 2 :	return "busy cycles";
					case 3 :	return "num preemptions";
					case 4 :	return "preemption reaction delay";
					case 5 :	return "preemption switch out time";
					case 6 :	return "preemption switch IN time";
					case 7 :	return "dead draws IN bin render";
					case 8 :	return "predicated draws killed";
					case 9 :	return "mode switch";
					case 10 :	return "zpass done";
					case 11 :	return "context done";
					case 12 :	return "cache flush";
					case 13 :	return "long preemptions";
					case 14 :	return "sqe I cache starve";
					case 15 :	return "sqe idle";
					case 16 :	return "sqe pm4 starve RB IB";
					case 17 :	return "sqe pm4 starve sds";
					case 18 :	return "sqe mrb starve";
					case 19 :	return "sqe rrb starve";
					case 20 :	return "sqe vsd starve";
					case 21 :	return "vsd decode starve";
					case 22 :	return "sqe pipe out stall";
					case 23 :	return "sqe sync stall";
					case 24 :	return "sqe pm4 wfi stall";
					case 25 :	return "sqe sys wfi stall";
					case 26 :	return "sqe T4 exec";
					case 27 :	return "sqe load state exec";
					case 28 :	return "sqe save sds state";
					case 29 :	return "sqe draw exec";
					case 30 :	return "sqe ctxt reg bunch exec";
					case 31 :	return "sqe exec profiled";
					case 32 :	return "memory pool empty";
					case 33 :	return "memory pool sync stall";
					case 34 :	return "memory pool above thresh";
					case 35 :	return "ahb WR stall pre draws";
					case 36 :	return "ahb stall sqe gmu";
					case 37 :	return "ahb stall sqe WR other";
					case 38 :	return "ahb stall sqe RD other";
					case 39 :	return "cluster0 empty";
					case 40 :	return "cluster1 empty";
					case 41 :	return "cluster2 empty";
					case 42 :	return "cluster3 empty";
					case 43 :	return "cluster4 empty";
					case 44 :	return "cluster5 empty";
					case 45 :	return "pm4 data";
					case 46 :	return "pm4 headers";
					case 47 :	return "vbif read beats";
					case 48 :	return "vbif write beats";
					case 49 :	return "sqe instr counter";
				}
				break;
			}
			case 1 :
			{
				switch ( counter )
				{
					case 0 :	return "always count";
					case 1 :	return "always ON";
					case 2 :	return "TSE busy";
					case 3 :	return "RAS busy";
					case 4 :	return "PC dcall busy";
					case 5 :	return "PC vsd busy";
					case 6 :	return "status masked";
					case 7 :	return "com busy";
					case 8 :	return "dcom busy";
					case 9 :	return "vbif busy";
					case 10 :	return "VSC busy";
					case 11 :	return "tess busy";
					case 12 :	return "UCHE busy";
					case 13 :	return "HLSQ busy";
				}
				break;
			}
			case 2 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles VFD";
					case 3 :	return "stall cycles TSE";
					case 4 :	return "stall cycles VPC";
					case 5 :	return "stall cycles UCHE";
					case 6 :	return "stall cycles tess";
					case 7 :	return "stall cycles TSE only";
					case 8 :	return "stall cycles VPC only";
					case 9 :	return "pass1 TF stall cycles";
					case 10 :	return "starve cycles for index";
					case 11 :	return "starve cycles for tess factor";
					case 12 :	return "starve cycles for viz stream";
					case 13 :	return "starve cycles for position";
					case 14 :	return "starve cycles DI";
					case 15 :	return "vis streams loaded";
					case 16 :	return "instances";
					case 17 :	return "VPC primitives";
					case 18 :	return "dead prim";
					case 19 :	return "live prim";
					case 20 :	return "vertex hits";
					case 21 :	return "IA vertices";
					case 22 :	return "IA primitives";
					case 23 :	return "GS primitives";
					case 24 :	return "HS invocations";
					case 25 :	return "DS invocations";
					case 26 :	return "VS invocations";
					case 27 :	return "GS invocations";
					case 28 :	return "DS primitives";
					case 29 :	return "VPC pos data transaction";
					case 30 :	return "3D drawcalls";
					case 31 :	return "2D drawcalls";
					case 32 :	return "non drawcall global events";
					case 33 :	return "tess busy cycles";
					case 34 :	return "tess working cycles";
					case 35 :	return "tess stall cycles PC";
					case 36 :	return "tess starve cycles PC";
					case 37 :	return "TSE transaction";
					case 38 :	return "TSE vertex";
					case 39 :	return "tess PC UV trans";
					case 40 :	return "tess PC UV patches";
					case 41 :	return "tess factor trans";
				}
				break;
			}
			case 3 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "stall cycles VPC alloc";
					case 3 :	return "stall cycles SP info";
					case 4 :	return "stall cycles SP attr";
					case 5 :	return "starve cycles UCHE";
					case 6 :	return "rbuffer full";
					case 7 :	return "attr info fifo full";
					case 8 :	return "decoded attribute bytes";
					case 9 :	return "num attributes";
					case 10 :	return "upper shader fibers";
					case 11 :	return "lower shader fibers";
					case 12 :	return "mode 0 fibers";
					case 13 :	return "mode 1 fibers";
					case 14 :	return "mode 2 fibers";
					case 15 :	return "mode 3 fibers";
					case 16 :	return "mode 4 fibers";
					case 17 :	return "total vertices";
					case 18 :	return "vfdp stall cycles VFD";
					case 19 :	return "vfdp stall cycles VFD index";
					case 20 :	return "vfdp stall cycles VFD prog";
					case 21 :	return "vfdp starve cycles PC";
					case 22 :	return "vfdp VS stage waves";
				}
				break;
			}
			case 4 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "stall cycles SP state";
					case 3 :	return "stall cycles SP FS stage";
					case 4 :	return "UCHE latency cycles";
					case 5 :	return "UCHE latency count";
					case 6 :	return "FS stage 1X waves";
					case 7 :	return "FS stage 2X waves";
					case 8 :	return "quads";
					case 9 :	return "CS invocations";
					case 10 :	return "compute drawcalls";
					case 11 :	return "FS data wait programming";
					case 12 :	return "dual FS prog active";
					case 13 :	return "dual VS prog active";
					case 14 :	return "FS batch count zero";
					case 15 :	return "VS batch count zero";
					case 16 :	return "wave pending NO quad";
					case 17 :	return "wave pending NO prim base";
					case 18 :	return "stall cycles VPC";
					case 19 :	return "pixels";
					case 20 :	return "draw mode switch vsfs sync";
				}
				break;
			}
			case 5 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles UCHE";
					case 3 :	return "stall cycles VFD wack";
					case 4 :	return "stall cycles HLSQ prim alloc";
					case 5 :	return "stall cycles PC";
					case 6 :	return "stall cycles SP LM";
					case 7 :	return "starve cycles SP";
					case 8 :	return "starve cycles LRZ";
					case 9 :	return "PC primitives";
					case 10 :	return "SP components";
					case 11 :	return "stall cycles vpcram pos";
					case 12 :	return "LRZ assign primitives";
					case 13 :	return "RB visible primitives";
					case 14 :	return "LM transaction";
					case 15 :	return "streamout transaction";
					case 16 :	return "VS busy cycles";
					case 17 :	return "PS busy cycles";
					case 18 :	return "VS working cycles";
					case 19 :	return "PS working cycles";
					case 20 :	return "starve cycles RB";
					case 21 :	return "num vpcram read pos";
					case 22 :	return "wit full cycles";
					case 23 :	return "vpcram full cycles";
					case 24 :	return "LM full wait for intp end";
					case 25 :	return "num vpcram write";
					case 26 :	return "num vpcram read SO";
					case 27 :	return "num attr req LM";
				}
				break;
			}
			case 6 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "clipping cycles";
					case 2 :	return "stall cycles RAS";
					case 3 :	return "stall cycles LRZ baryplane";
					case 4 :	return "stall cycles LRZ zplane";
					case 5 :	return "starve cycles PC";
					case 6 :	return "input prim";
					case 7 :	return "input null prim";
					case 8 :	return "trival rej prim";
					case 9 :	return "clipped prim";
					case 10 :	return "zero area prim";
					case 11 :	return "faceness culled prim";
					case 12 :	return "zero pixel prim";
					case 13 :	return "output null prim";
					case 14 :	return "output visible prim";
					case 15 :	return "cinvocation";
					case 16 :	return "cprimitives";
					case 17 :	return "2D input prim";
					case 18 :	return "2D alive cycles";
					case 19 :	return "clip planes";
				}
				break;
			}
			case 7 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "supertile active cycles";
					case 2 :	return "stall cycles LRZ";
					case 3 :	return "starve cycles TSE";
					case 4 :	return "super tiles";
					case 5 :	return "8x4 tiles";
					case 6 :	return "maskgen active";
					case 7 :	return "fully covered super tiles";
					case 8 :	return "fully covered 8x4 tiles";
					case 9 :	return "prim killed invisible";
					case 10 :	return "supertile gen active cycles";
					case 11 :	return "LRZ intf working cycles";
					case 12 :	return "blocks";
				}
				break;
			}
			case 8 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles arbiter";
					case 2 :	return "vbif latency cycles";
					case 3 :	return "vbif latency samples";
					case 4 :	return "vbif read beats TP";
					case 5 :	return "vbif read beats VFD";
					case 6 :	return "vbif read beats HLSQ";
					case 7 :	return "vbif read beats LRZ";
					case 8 :	return "vbif read beats SP";
					case 9 :	return "read requests TP";
					case 10 :	return "read requests VFD";
					case 11 :	return "read requests HLSQ";
					case 12 :	return "read requests LRZ";
					case 13 :	return "read requests SP";
					case 14 :	return "write requests LRZ";
					case 15 :	return "write requests SP";
					case 16 :	return "write requests VPC";
					case 17 :	return "write requests VSC";
					case 18 :	return "evicts";
					case 19 :	return "bank req0";
					case 20 :	return "bank req1";
					case 21 :	return "bank req2";
					case 22 :	return "bank req3";
					case 23 :	return "bank req4";
					case 24 :	return "bank req5";
					case 25 :	return "bank req6";
					case 26 :	return "bank req7";
					case 27 :	return "vbif read beats ch0";
					case 28 :	return "vbif read beats ch1";
					case 29 :	return "gmem read beats";
					case 30 :	return "tph ref full";
					case 31 :	return "tph victim full";
					case 32 :	return "tph ext full";
					case 33 :	return "vbif stall write data";
					case 34 :	return "dcmp latency samples";
					case 35 :	return "dcmp latency cycles";
					case 36 :	return "vbif read beats PC";
					case 37 :	return "read requests PC";
					case 38 :	return "ram read req";
					case 39 :	return "ram write req";
				}
				break;
			}
			case 9 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "latency cycles";
					case 3 :	return "latency trans";
					case 4 :	return "flag cache request samples";
					case 5 :	return "flag cache request latency";
					case 6 :	return "L1 cacheline requests";
					case 7 :	return "L1 cacheline misses";
					case 8 :	return "SP TP trans";
					case 9 :	return "TP SP trans";
					case 10 :	return "output pixels";
					case 11 :	return "filter workload 16bit";
					case 12 :	return "filter workload 32bit";
					case 13 :	return "quads received";
					case 14 :	return "quads offset";
					case 15 :	return "quads shadow";
					case 16 :	return "quads array";
					case 17 :	return "quads gradient";
					case 18 :	return "quads 1D";
					case 19 :	return "quads 2D";
					case 20 :	return "quads buffer";
					case 21 :	return "quads 3D";
					case 22 :	return "quads cube";
					case 23 :	return "divergent quads received";
					case 24 :	return "prt non resident events";
					case 25 :	return "output pixels point";
					case 26 :	return "output pixels bilinear";
					case 27 :	return "output pixels mip";
					case 28 :	return "output pixels aniso";
					case 29 :	return "output pixels zero lod";
					case 30 :	return "flag cache requests";
					case 31 :	return "flag cache misses";
					case 32 :	return "L1 5 L2 requests";
					case 33 :	return "2D output pixels";
					case 34 :	return "2D output pixels point";
					case 35 :	return "2D output pixels bilinear";
					case 36 :	return "2D filter workload 16bit";
					case 37 :	return "2D filter workload 32bit";
					case 38 :	return "tpa2tpc trans";
					case 39 :	return "L1 misses astc 1tile";
					case 40 :	return "L1 misses astc 2tile";
					case 41 :	return "L1 misses astc 4tile";
					case 42 :	return "L1 5 L2 compress reqs";
					case 43 :	return "L1 5 L2 compress miss";
					case 44 :	return "L1 bank conflict";
					case 45 :	return "L1 5 miss latency cycles";
					case 46 :	return "L1 5 miss latency trans";
					case 47 :	return "quads constant multiplied";
					case 48 :	return "frontend working cycles";
					case 49 :	return "L1 tag working cycles";
					case 50 :	return "L1 data write working cycles";
					case 51 :	return "pre L1 decom working cycles";
					case 52 :	return "backend working cycles";
					case 53 :	return "flag cache working cycles";
					case 54 :	return "L1 5 cache working cycles";
					case 55 :	return "starve cycles SP";
					case 56 :	return "starve cycles UCHE";
				}
				break;
			}
			case 0xA :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "ALU working cycles";
					case 2 :	return "EFU working cycles";
					case 3 :	return "stall cycles VPC";
					case 4 :	return "stall cycles TP";
					case 5 :	return "stall cycles UCHE";
					case 6 :	return "stall cycles RB";
					case 7 :	return "non execution cycles";
					case 8 :	return "wave contexts";
					case 9 :	return "wave context cycles";
					case 10 :	return "FS stage wave cycles";
					case 11 :	return "FS stage wave samples";
					case 12 :	return "VS stage wave cycles";
					case 13 :	return "VS stage wave samples";
					case 14 :	return "FS stage duration cycles";
					case 15 :	return "VS stage duration cycles";
					case 16 :	return "wave ctrl cycles";
					case 17 :	return "wave load cycles";
					case 18 :	return "wave emit cycles";
					case 19 :	return "wave nop cycles";
					case 20 :	return "wave wait cycles";
					case 21 :	return "wave fetch cycles";
					case 22 :	return "wave idle cycles";
					case 23 :	return "wave end cycles";
					case 24 :	return "wave long sync cycles";
					case 25 :	return "wave short sync cycles";
					case 26 :	return "wave join cycles";
					case 27 :	return "LM load instructions";
					case 28 :	return "LM store instructions";
					case 29 :	return "LM atomics";
					case 30 :	return "GM load instructions";
					case 31 :	return "GM store instructions";
					case 32 :	return "GM atomics";
					case 33 :	return "VS stage tex instructions";
					case 34 :	return "VS stage EFU instructions";
					case 35 :	return "VS stage full ALU instructions";
					case 36 :	return "VS stage half ALU instructions";
					case 37 :	return "FS stage tex instructions";
					case 38 :	return "FS stage cflow instructions";
					case 39 :	return "FS stage EFU instructions";
					case 40 :	return "FS stage full ALU instructions";
					case 41 :	return "FS stage half ALU instructions";
					case 42 :	return "FS stage bary instructions";
					case 43 :	return "VS instructions";
					case 44 :	return "FS instructions";
					case 45 :	return "addr lock count";
					case 46 :	return "UCHE read trans";
					case 47 :	return "UCHE write trans";
					case 48 :	return "export VPC trans";
					case 49 :	return "export RB trans";
					case 50 :	return "pixels killed";
					case 51 :	return "icl1 requests";
					case 52 :	return "icl1 misses";
					case 53 :	return "HS instructions";
					case 54 :	return "DS instructions";
					case 55 :	return "GS instructions";
					case 56 :	return "CS instructions";
					case 57 :	return "GPR read";
					case 58 :	return "GPR write";
					case 59 :	return "FS stage half EFU instructions";
					case 60 :	return "VS stage half EFU instructions";
					case 61 :	return "LM bank conflicts";
					case 62 :	return "tex control working cycles";
					case 63 :	return "load control working cycles";
					case 64 :	return "flow control working cycles";
					case 65 :	return "LM working cycles";
					case 66 :	return "dispatcher working cycles";
					case 67 :	return "sequencer working cycles";
					case 68 :	return "low efficiency starved BY TP";
					case 69 :	return "starve cycles HLSQ";
					case 70 :	return "non execution LS cycles";
					case 71 :	return "working EU";
					case 72 :	return "any EU working";
					case 73 :	return "working EU FS stage";
					case 74 :	return "any EU working FS stage";
					case 75 :	return "working EU VS stage";
					case 76 :	return "any EU working VS stage";
					case 77 :	return "working EU CS stage";
					case 78 :	return "any EU working CS stage";
					case 79 :	return "GPR read prefetch";
					case 80 :	return "GPR read conflict";
					case 81 :	return "GPR write conflict";
					case 82 :	return "GM load latency cycles";
					case 83 :	return "GM load latency samples";
					case 84 :	return "executable waves";
				}
				break;
			}
			case 0xB :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles HLSQ";
					case 2 :	return "stall cycles fifo0 full";
					case 3 :	return "stall cycles fifo1 full";
					case 4 :	return "stall cycles fifo2 full";
					case 5 :	return "starve cycles SP";
					case 6 :	return "starve cycles LRZ tile";
					case 7 :	return "starve cycles CCU";
					case 8 :	return "starve cycles Z plane";
					case 9 :	return "starve cycles bary plane";
					case 10 :	return "Z workload";
					case 11 :	return "HLSQ active";
					case 12 :	return "Z read";
					case 13 :	return "Z write";
					case 14 :	return "C read";
					case 15 :	return "C write";
					case 16 :	return "total pass";
					case 17 :	return "Z pass";
					case 18 :	return "Z fail";
					case 19 :	return "S fail";
					case 20 :	return "blended fxp components";
					case 21 :	return "blended fp16 components";
					case 22 :	return "PS invocations";
					case 23 :	return "2D alive cycles";
					case 24 :	return "2D stall cycles a2d";
					case 25 :	return "2D starve cycles src";
					case 26 :	return "2D starve cycles SP";
					case 27 :	return "2D starve cycles dst";
					case 28 :	return "2D valid pixels";
					case 29 :	return "3D pixels";
					case 30 :	return "blender working cycles";
					case 31 :	return "zproc working cycles";
					case 32 :	return "cproc working cycles";
					case 33 :	return "sampler working cycles";
					case 34 :	return "stall cycles CCU color read";
					case 35 :	return "stall cycles CCU color write";
					case 36 :	return "stall cycles CCU depth read";
					case 37 :	return "stall cycles CCU depth write";
					case 38 :	return "stall cycles VPC";
					case 39 :	return "2D input trans";
					case 40 :	return "2D output RB dst trans";
					case 41 :	return "2D output RB src trans";
					case 42 :	return "blended fp32 components";
					case 43 :	return "color pix tiles";
					case 44 :	return "stall cycles CCU";
					case 45 :	return "early Z arb3 grant";
					case 46 :	return "late Z arb3 grant";
					case 47 :	return "early Z skip grant";
				}
				break;
			}
			case 0x17 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles UCHE";
					case 3 :	return "eot num";
					case 4 :	return "input tiles";
				}
				break;
			}
			case 0x18 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles RB depth return";
					case 2 :	return "stall cycles RB color return";
					case 3 :	return "starve cycles flag return";
					case 4 :	return "depth blocks";
					case 5 :	return "color blocks";
					case 6 :	return "depth block hit";
					case 7 :	return "color block hit";
					case 8 :	return "partial block read";
					case 9 :	return "gmem read";
					case 10 :	return "gmem write";
					case 11 :	return "depth read flag0 count";
					case 12 :	return "depth read flag1 count";
					case 13 :	return "depth read flag2 count";
					case 14 :	return "depth read flag3 count";
					case 15 :	return "depth read flag4 count";
					case 16 :	return "depth read flag5 count";
					case 17 :	return "depth read flag6 count";
					case 18 :	return "depth read flag8 count";
					case 19 :	return "color read flag0 count";
					case 20 :	return "color read flag1 count";
					case 21 :	return "color read flag2 count";
					case 22 :	return "color read flag3 count";
					case 23 :	return "color read flag4 count";
					case 24 :	return "color read flag5 count";
					case 25 :	return "color read flag6 count";
					case 26 :	return "color read flag8 count";
					case 27 :	return "2D RD req";
					case 28 :	return "2D WR req";
				}
				break;
			}
			case 0x19 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "starve cycles RAS";
					case 2 :	return "stall cycles RB";
					case 3 :	return "stall cycles VSC";
					case 4 :	return "stall cycles VPC";
					case 5 :	return "stall cycles flag prefetch";
					case 6 :	return "stall cycles UCHE";
					case 7 :	return "LRZ read";
					case 8 :	return "LRZ write";
					case 9 :	return "read latency";
					case 10 :	return "merge cache updating";
					case 11 :	return "prim killed BY maskgen";
					case 12 :	return "prim killed BY LRZ";
					case 13 :	return "visible prim after LRZ";
					case 14 :	return "full 8x8 tiles";
					case 15 :	return "partial 8x8 tiles";
					case 16 :	return "tile killed";
					case 17 :	return "total pixel";
					case 18 :	return "visible pixel after LRZ";
					case 19 :	return "fully covered tiles";
					case 20 :	return "partial covered tiles";
					case 21 :	return "feedback accept";
					case 22 :	return "feedback discard";
					case 23 :	return "feedback stall";
					case 24 :	return "stall cycles RB zplane";
					case 25 :	return "stall cycles RB bplane";
					case 26 :	return "stall cycles VC";
					case 27 :	return "RAS mask trans";
				}
				break;
			}
			case 0x1A :
			{
				switch ( counter )
				{
					case 0 :	return "cmpdecmp stall cycles arb";
					case 1 :	return "cmpdecmp vbif latency cycles";
					case 2 :	return "cmpdecmp vbif latency samples";
					case 3 :	return "cmpdecmp vbif read data CCU";
					case 4 :	return "cmpdecmp vbif write data CCU";
					case 5 :	return "cmpdecmp vbif read request";
					case 6 :	return "cmpdecmp vbif write request";
					case 7 :	return "cmpdecmp vbif read data";
					case 8 :	return "cmpdecmp vbif write data";
					case 9 :	return "cmpdecmp flag fetch cycles";
					case 10 :	return "cmpdecmp flag fetch samples";
					case 11 :	return "cmpdecmp depth write flag1 count";
					case 12 :	return "cmpdecmp depth write flag2 count";
					case 13 :	return "cmpdecmp depth write flag3 count";
					case 14 :	return "cmpdecmp depth write flag4 count";
					case 15 :	return "cmpdecmp depth write flag5 count";
					case 16 :	return "cmpdecmp depth write flag6 count";
					case 17 :	return "cmpdecmp depth write flag8 count";
					case 18 :	return "cmpdecmp color write flag1 count";
					case 19 :	return "cmpdecmp color write flag2 count";
					case 20 :	return "cmpdecmp color write flag3 count";
					case 21 :	return "cmpdecmp color write flag4 count";
					case 22 :	return "cmpdecmp color write flag5 count";
					case 23 :	return "cmpdecmp color write flag6 count";
					case 24 :	return "cmpdecmp color write flag8 count";
					case 25 :	return "cmpdecmp 2D stall cycles vbif req";
					case 26 :	return "cmpdecmp 2D stall cycles vbif WR";
					case 27 :	return "cmpdecmp 2D stall cycles vbif return";
					case 28 :	return "cmpdecmp 2D RD data";
					case 29 :	return "cmpdecmp 2D WR data";
					case 30 :	return "cmpdecmp vbif read data UCHE ch0";
					case 31 :	return "cmpdecmp vbif read data UCHE ch1";
					case 32 :	return "cmpdecmp 2D output trans";
					case 33 :	return "cmpdecmp vbif write data UCHE";
					case 34 :	return "cmpdecmp depth write flag0 count";
					case 35 :	return "cmpdecmp color write flag0 count";
					case 36 :	return "cmpdecmp color write flagalpha count";
					case 37 :	return "cmpdecmp 2D busy cycles";
					case 38 :	return "cmpdecmp 2D reorder starve cycles";
					case 39 :	return "cmpdecmp 2D pixels";
				}
				break;
			}
		}
		return "???";
	}

/*
=================================================
	GetCounterName_A5xx
=================================================
*/
	ND_ static const char*  GetCounterName_A5xx (uint group, uint counter)
	{
		switch ( group )
		{
			case 0 :
			{
				switch ( counter )
				{
					case 0 :	return "always count";
					case 1 :	return "busy gfx core idle";
					case 2 :	return "busy cycles";
					case 3 :	return "pfp idle";
					case 4 :	return "pfp busy working";
					case 5 :	return "pfp stall cycles any";
					case 6 :	return "pfp starve cycles any";
					case 7 :	return "pfp icache miss";
					case 8 :	return "pfp icache hit";
					case 9 :	return "pfp match pm4 pkt profile";
					case 10 :	return "ME busy working";
					case 11 :	return "ME idle";
					case 12 :	return "ME starve cycles any";
					case 13 :	return "ME fifo empty pfp idle";
					case 14 :	return "ME fifo empty pfp busy";
					case 15 :	return "ME fifo full ME busy";
					case 16 :	return "ME fifo full ME non working";
					case 17 :	return "ME stall cycles any";
					case 18 :	return "ME icache miss";
					case 19 :	return "ME icache hit";
					case 20 :	return "num preemptions";
					case 21 :	return "preemption reaction delay";
					case 22 :	return "preemption switch out time";
					case 23 :	return "preemption switch IN time";
					case 24 :	return "dead draws IN bin render";
					case 25 :	return "predicated draws killed";
					case 26 :	return "mode switch";
					case 27 :	return "zpass done";
					case 28 :	return "context done";
					case 29 :	return "cache flush";
					case 30 :	return "long preemptions";
				}
				break;
			}
			case 1 :
			{
				switch ( counter )
				{
					case 0 :	return "always count";
					case 1 :	return "always ON";
					case 2 :	return "TSE busy";
					case 3 :	return "RAS busy";
					case 4 :	return "PC dcall busy";
					case 5 :	return "PC vsd busy";
					case 6 :	return "status masked";
					case 7 :	return "com busy";
					case 8 :	return "dcom busy";
					case 9 :	return "VBIF busy";
					case 10 :	return "VSC busy";
					case 11 :	return "tess busy";
					case 12 :	return "UCHE busy";
					case 13 :	return "HLSQ busy";
				}
				break;
			}
			case 2 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles VFD";
					case 3 :	return "stall cycles TSE";
					case 4 :	return "stall cycles VPC";
					case 5 :	return "stall cycles UCHE";
					case 6 :	return "stall cycles tess";
					case 7 :	return "stall cycles TSE only";
					case 8 :	return "stall cycles VPC only";
					case 9 :	return "pass1 TF stall cycles";
					case 10 :	return "starve cycles for index";
					case 11 :	return "starve cycles for tess factor";
					case 12 :	return "starve cycles for viz stream";
					case 13 :	return "starve cycles for position";
					case 14 :	return "starve cycles DI";
					case 15 :	return "vis streams loaded";
					case 16 :	return "instances";
					case 17 :	return "VPC primitives";
					case 18 :	return "dead prim";
					case 19 :	return "live prim";
					case 20 :	return "vertex hits";
					case 21 :	return "IA vertices";
					case 22 :	return "IA primitives";
					case 23 :	return "GS primitives";
					case 24 :	return "HS invocations";
					case 25 :	return "DS invocations";
					case 26 :	return "VS invocations";
					case 27 :	return "GS invocations";
					case 28 :	return "DS primitives";
					case 29 :	return "VPC pos data transaction";
					case 30 :	return "3D drawcalls";
					case 31 :	return "2D drawcalls";
					case 32 :	return "non drawcall global events";
					case 33 :	return "tess busy cycles";
					case 34 :	return "tess working cycles";
					case 35 :	return "tess stall cycles PC";
					case 36 :	return "tess starve cycles PC";
				}
				break;
			}
			case 3 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "stall cycles VPC alloc";
					case 3 :	return "stall cycles miss VB";
					case 4 :	return "stall cycles miss Q";
					case 5 :	return "stall cycles SP info";
					case 6 :	return "stall cycles SP attr";
					case 7 :	return "stall cycles vfdp VB";
					case 8 :	return "stall cycles vfdp Q";
					case 9 :	return "decoder packer stall";
					case 10 :	return "starve cycles UCHE";
					case 11 :	return "rbuffer full";
					case 12 :	return "attr info fifo full";
					case 13 :	return "decoded attribute bytes";
					case 14 :	return "num attributes";
					case 15 :	return "instructions";
					case 16 :	return "upper shader fibers";
					case 17 :	return "lower shader fibers";
					case 18 :	return "mode 0 fibers";
					case 19 :	return "mode 1 fibers";
					case 20 :	return "mode 2 fibers";
					case 21 :	return "mode 3 fibers";
					case 22 :	return "mode 4 fibers";
					case 23 :	return "total vertices";
					case 24 :	return "num attr miss";
					case 25 :	return "1 burst req";
					case 26 :	return "vfdp stall cycles VFD";
					case 27 :	return "vfdp stall cycles VFD index";
					case 28 :	return "vfdp stall cycles VFD prog";
					case 29 :	return "vfdp starve cycles PC";
					case 30 :	return "vfdp VS stage 32 waves";
				}
				break;
			}
			case 4 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "stall cycles SP state";
					case 3 :	return "stall cycles SP FS stage";
					case 4 :	return "UCHE latency cycles";
					case 5 :	return "UCHE latency count";
					case 6 :	return "FS stage 32 waves";
					case 7 :	return "FS stage 64 waves";
					case 8 :	return "quads";
					case 9 :	return "SP state copy trans FS stage";
					case 10 :	return "SP state copy trans VS stage";
					case 11 :	return "TP state copy trans FS stage";
					case 12 :	return "TP state copy trans VS stage";
					case 13 :	return "CS invocations";
					case 14 :	return "compute drawcalls";
				}
				break;
			}
			case 5 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles UCHE";
					case 3 :	return "stall cycles VFD wack";
					case 4 :	return "stall cycles HLSQ prim alloc";
					case 5 :	return "stall cycles PC";
					case 6 :	return "stall cycles SP LM";
					case 7 :	return "pos export stall cycles";
					case 8 :	return "starve cycles SP";
					case 9 :	return "starve cycles LRZ";
					case 10 :	return "PC primitives";
					case 11 :	return "SP components";
					case 12 :	return "SP LM primitives";
					case 13 :	return "SP LM components";
					case 14 :	return "SP LM dwords";
					case 15 :	return "streamout components";
					case 16 :	return "grant phases";
				}
				break;
			}
			case 6 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "clipping cycles";
					case 2 :	return "stall cycles RAS";
					case 3 :	return "stall cycles LRZ baryplane";
					case 4 :	return "stall cycles LRZ zplane";
					case 5 :	return "starve cycles PC";
					case 6 :	return "input prim";
					case 7 :	return "input null prim";
					case 8 :	return "trival rej prim";
					case 9 :	return "clipped prim";
					case 10 :	return "zero area prim";
					case 11 :	return "faceness culled prim";
					case 12 :	return "zero pixel prim";
					case 13 :	return "output null prim";
					case 14 :	return "output visible prim";
					case 15 :	return "cinvocation";
					case 16 :	return "cprimitives";
					case 17 :	return "2D input prim";
					case 18 :	return "2D alive clcles";
				}
				break;
			}
			case 7 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "supertile active cycles";
					case 2 :	return "stall cycles LRZ";
					case 3 :	return "starve cycles TSE";
					case 4 :	return "super tiles";
					case 5 :	return "8x4 tiles";
					case 6 :	return "maskgen active";
					case 7 :	return "fully covered super tiles";
					case 8 :	return "fully covered 8x4 tiles";
					case 9 :	return "prim killed invisible";
				}
				break;
			}
			case 8 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles VBIF";
					case 2 :	return "VBIF latency cycles";
					case 3 :	return "VBIF latency samples";
					case 4 :	return "VBIF read beats TP";
					case 5 :	return "VBIF read beats VFD";
					case 6 :	return "VBIF read beats HLSQ";
					case 7 :	return "VBIF read beats LRZ";
					case 8 :	return "VBIF read beats SP";
					case 9 :	return "read requests TP";
					case 10 :	return "read requests VFD";
					case 11 :	return "read requests HLSQ";
					case 12 :	return "read requests LRZ";
					case 13 :	return "read requests SP";
					case 14 :	return "write requests LRZ";
					case 15 :	return "write requests SP";
					case 16 :	return "write requests VPC";
					case 17 :	return "write requests VSC";
					case 18 :	return "evicts";
					case 19 :	return "bank req0";
					case 20 :	return "bank req1";
					case 21 :	return "bank req2";
					case 22 :	return "bank req3";
					case 23 :	return "bank req4";
					case 24 :	return "bank req5";
					case 25 :	return "bank req6";
					case 26 :	return "bank req7";
					case 27 :	return "VBIF read beats ch0";
					case 28 :	return "VBIF read beats ch1";
					case 29 :	return "gmem read beats";
					case 30 :	return "flag count";
				}
				break;
			}
			case 9 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles UCHE";
					case 2 :	return "latency cycles";
					case 3 :	return "latency trans";
					case 4 :	return "flag cache request samples";
					case 5 :	return "flag cache request latency";
					case 6 :	return "L1 cacheline requests";
					case 7 :	return "L1 cacheline misses";
					case 8 :	return "SP TP trans";
					case 9 :	return "TP SP trans";
					case 10 :	return "output pixels";
					case 11 :	return "filter workload 16bit";
					case 12 :	return "filter workload 32bit";
					case 13 :	return "quads received";
					case 14 :	return "quads offset";
					case 15 :	return "quads shadow";
					case 16 :	return "quads array";
					case 17 :	return "quads gradient";
					case 18 :	return "quads 1D";
					case 19 :	return "quads 2D";
					case 20 :	return "quads buffer";
					case 21 :	return "quads 3D";
					case 22 :	return "quads cube";
					case 23 :	return "state cache requests";
					case 24 :	return "state cache misses";
					case 25 :	return "divergent quads received";
					case 26 :	return "bindless state cache requests";
					case 27 :	return "bindless state cache misses";
					case 28 :	return "prt non resident events";
					case 29 :	return "output pixels point";
					case 30 :	return "output pixels bilinear";
					case 31 :	return "output pixels mip";
					case 32 :	return "output pixels aniso";
					case 33 :	return "output pixels zero lod";
					case 34 :	return "flag cache requests";
					case 35 :	return "flag cache misses";
					case 36 :	return "L1 5 L2 requests";
					case 37 :	return "2D output pixels";
					case 38 :	return "2D output pixels point";
					case 39 :	return "2D output pixels bilinear";
					case 40 :	return "2D filter workload 16bit";
					case 41 :	return "2D filter workload 32bit";
				}
				break;
			}
			case 0xA :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "ALU working cycles";
					case 2 :	return "EFU working cycles";
					case 3 :	return "stall cycles VPC";
					case 4 :	return "stall cycles TP";
					case 5 :	return "stall cycles UCHE";
					case 6 :	return "stall cycles RB";
					case 7 :	return "scheduler non working";
					case 8 :	return "wave contexts";
					case 9 :	return "wave context cycles";
					case 10 :	return "FS stage wave cycles";
					case 11 :	return "FS stage wave samples";
					case 12 :	return "VS stage wave cycles";
					case 13 :	return "VS stage wave samples";
					case 14 :	return "FS stage duration cycles";
					case 15 :	return "VS stage duration cycles";
					case 16 :	return "wave ctrl cycles";
					case 17 :	return "wave load cycles";
					case 18 :	return "wave emit cycles";
					case 19 :	return "wave nop cycles";
					case 20 :	return "wave wait cycles";
					case 21 :	return "wave fetch cycles";
					case 22 :	return "wave idle cycles";
					case 23 :	return "wave end cycles";
					case 24 :	return "wave long sync cycles";
					case 25 :	return "wave short sync cycles";
					case 26 :	return "wave join cycles";
					case 27 :	return "LM load instructions";
					case 28 :	return "LM store instructions";
					case 29 :	return "LM atomics";
					case 30 :	return "GM load instructions";
					case 31 :	return "GM store instructions";
					case 32 :	return "GM atomics";
					case 33 :	return "VS stage tex instructions";
					case 34 :	return "VS stage cflow instructions";
					case 35 :	return "VS stage EFU instructions";
					case 36 :	return "VS stage full ALU instructions";
					case 37 :	return "VS stage half ALU instructions";
					case 38 :	return "FS stage tex instructions";
					case 39 :	return "FS stage cflow instructions";
					case 40 :	return "FS stage EFU instructions";
					case 41 :	return "FS stage full ALU instructions";
					case 42 :	return "FS stage half ALU instructions";
					case 43 :	return "FS stage bary instructions";
					case 44 :	return "VS instructions";
					case 45 :	return "FS instructions";
					case 46 :	return "addr lock count";
					case 47 :	return "UCHE read trans";
					case 48 :	return "UCHE write trans";
					case 49 :	return "export VPC trans";
					case 50 :	return "export RB trans";
					case 51 :	return "pixels killed";
					case 52 :	return "icl1 requests";
					case 53 :	return "icl1 misses";
					case 54 :	return "icl0 requests";
					case 55 :	return "icl0 misses";
					case 56 :	return "HS instructions";
					case 57 :	return "DS instructions";
					case 58 :	return "GS instructions";
					case 59 :	return "CS instructions";
					case 60 :	return "GPR read";
					case 61 :	return "GPR write";
					case 62 :	return "LM ch0 requests";
					case 63 :	return "LM ch1 requests";
					case 64 :	return "LM bank conflicts";
				}
				break;
			}
			case 0xB :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles CCU";
					case 2 :	return "stall cycles HLSQ";
					case 3 :	return "stall cycles fifo0 full";
					case 4 :	return "stall cycles fifo1 full";
					case 5 :	return "stall cycles fifo2 full";
					case 6 :	return "starve cycles SP";
					case 7 :	return "starve cycles LRZ tile";
					case 8 :	return "starve cycles CCU";
					case 9 :	return "starve cycles Z plane";
					case 10 :	return "starve cycles bary plane";
					case 11 :	return "Z workload";
					case 12 :	return "HLSQ active";
					case 13 :	return "Z read";
					case 14 :	return "Z write";
					case 15 :	return "C read";
					case 16 :	return "C write";
					case 17 :	return "total pass";
					case 18 :	return "Z pass";
					case 19 :	return "Z fail";
					case 20 :	return "S fail";
					case 21 :	return "blended fxp components";
					case 22 :	return "blended fp16 components";
					case 23 :	return "reserved";
					case 24 :	return "2D alive cycles";
					case 25 :	return "2D stall cycles a2d";
					case 26 :	return "2D starve cycles src";
					case 27 :	return "2D starve cycles SP";
					case 28 :	return "2D starve cycles dst";
					case 29 :	return "2D valid pixels";
				}
				break;
			}
			case 0xD :
			{
				switch ( counter )
				{
					case 0 :	return "axi read requests ID 0";
					case 1 :	return "axi read requests ID 1";
					case 2 :	return "axi read requests ID 2";
					case 3 :	return "axi read requests ID 3";
					case 4 :	return "axi read requests ID 4";
					case 5 :	return "axi read requests ID 5";
					case 6 :	return "axi read requests ID 6";
					case 7 :	return "axi read requests ID 7";
					case 8 :	return "axi read requests ID 8";
					case 9 :	return "axi read requests ID 9";
					case 10 :	return "axi read requests ID 10";
					case 11 :	return "axi read requests ID 11";
					case 12 :	return "axi read requests ID 12";
					case 13 :	return "axi read requests ID 13";
					case 14 :	return "axi read requests ID 14";
					case 15 :	return "axi read requests ID 15";
					case 16 :	return "axi0 read requests total";
					case 17 :	return "axi1 read requests total";
					case 18 :	return "axi2 read requests total";
					case 19 :	return "axi3 read requests total";
					case 20 :	return "axi read requests total";
					case 21 :	return "axi write requests ID 0";
					case 22 :	return "axi write requests ID 1";
					case 23 :	return "axi write requests ID 2";
					case 24 :	return "axi write requests ID 3";
					case 25 :	return "axi write requests ID 4";
					case 26 :	return "axi write requests ID 5";
					case 27 :	return "axi write requests ID 6";
					case 28 :	return "axi write requests ID 7";
					case 29 :	return "axi write requests ID 8";
					case 30 :	return "axi write requests ID 9";
					case 31 :	return "axi write requests ID 10";
					case 32 :	return "axi write requests ID 11";
					case 33 :	return "axi write requests ID 12";
					case 34 :	return "axi write requests ID 13";
					case 35 :	return "axi write requests ID 14";
					case 36 :	return "axi write requests ID 15";
					case 37 :	return "axi0 write requests total";
					case 38 :	return "axi1 write requests total";
					case 39 :	return "axi2 write requests total";
					case 40 :	return "axi3 write requests total";
					case 41 :	return "axi write requests total";
					case 42 :	return "axi total requests";
					case 43 :	return "axi read data beats ID 0";
					case 44 :	return "axi read data beats ID 1";
					case 45 :	return "axi read data beats ID 2";
					case 46 :	return "axi read data beats ID 3";
					case 47 :	return "axi read data beats ID 4";
					case 48 :	return "axi read data beats ID 5";
					case 49 :	return "axi read data beats ID 6";
					case 50 :	return "axi read data beats ID 7";
					case 51 :	return "axi read data beats ID 8";
					case 52 :	return "axi read data beats ID 9";
					case 53 :	return "axi read data beats ID 10";
					case 54 :	return "axi read data beats ID 11";
					case 55 :	return "axi read data beats ID 12";
					case 56 :	return "axi read data beats ID 13";
					case 57 :	return "axi read data beats ID 14";
					case 58 :	return "axi read data beats ID 15";
					case 59 :	return "axi0 read data beats total";
					case 60 :	return "axi1 read data beats total";
					case 61 :	return "axi2 read data beats total";
					case 62 :	return "axi3 read data beats total";
					case 63 :	return "axi read data beats total";
					case 64 :	return "axi write data beats ID 0";
					case 65 :	return "axi write data beats ID 1";
					case 66 :	return "axi write data beats ID 2";
					case 67 :	return "axi write data beats ID 3";
					case 68 :	return "axi write data beats ID 4";
					case 69 :	return "axi write data beats ID 5";
					case 70 :	return "axi write data beats ID 6";
					case 71 :	return "axi write data beats ID 7";
					case 72 :	return "axi write data beats ID 8";
					case 73 :	return "axi write data beats ID 9";
					case 74 :	return "axi write data beats ID 10";
					case 75 :	return "axi write data beats ID 11";
					case 76 :	return "axi write data beats ID 12";
					case 77 :	return "axi write data beats ID 13";
					case 78 :	return "axi write data beats ID 14";
					case 79 :	return "axi write data beats ID 15";
					case 80 :	return "axi0 write data beats total";
					case 81 :	return "axi1 write data beats total";
					case 82 :	return "axi2 write data beats total";
					case 83 :	return "axi3 write data beats total";
					case 84 :	return "axi write data beats total";
					case 85 :	return "axi data beats total";
				}
				break;
			}
			case 0x17 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "working cycles";
					case 2 :	return "stall cycles UCHE";
					case 3 :	return "eot num";
				}
				break;
			}
			case 0x18 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "stall cycles RB depth return";
					case 2 :	return "stall cycles RB color return";
					case 3 :	return "starve cycles flag return";
					case 4 :	return "depth blocks";
					case 5 :	return "color blocks";
					case 6 :	return "depth block hit";
					case 7 :	return "color block hit";
					case 8 :	return "partial block read";
					case 9 :	return "gmem read";
					case 10 :	return "gmem write";
					case 11 :	return "depth read flag0 count";
					case 12 :	return "depth read flag1 count";
					case 13 :	return "depth read flag2 count";
					case 14 :	return "depth read flag3 count";
					case 15 :	return "depth read flag4 count";
					case 16 :	return "color read flag0 count";
					case 17 :	return "color read flag1 count";
					case 18 :	return "color read flag2 count";
					case 19 :	return "color read flag3 count";
					case 20 :	return "color read flag4 count";
					case 21 :	return "2D busy cycles";
					case 22 :	return "2D RD req";
					case 23 :	return "2D WR req";
					case 24 :	return "2D reorder starve cycles";
					case 25 :	return "2D pixels";
				}
				break;
			}
			case 0x19 :
			{
				switch ( counter )
				{
					case 0 :	return "busy cycles";
					case 1 :	return "starve cycles RAS";
					case 2 :	return "stall cycles RB";
					case 3 :	return "stall cycles VSC";
					case 4 :	return "stall cycles VPC";
					case 5 :	return "stall cycles flag prefetch";
					case 6 :	return "stall cycles UCHE";
					case 7 :	return "LRZ read";
					case 8 :	return "LRZ write";
					case 9 :	return "read latency";
					case 10 :	return "merge cache updating";
					case 11 :	return "prim killed BY maskgen";
					case 12 :	return "prim killed BY LRZ";
					case 13 :	return "visible prim after LRZ";
					case 14 :	return "full 8x8 tiles";
					case 15 :	return "partial 8x8 tiles";
					case 16 :	return "tile killed";
					case 17 :	return "total pixel";
					case 18 :	return "visible pixel after LRZ";
				}
				break;
			}
			case 0x1A :
			{
				switch ( counter )
				{
					case 0 :	return "cmpdecmp stall cycles VBIF";
					case 1 :	return "cmpdecmp VBIF latency cycles";
					case 2 :	return "cmpdecmp VBIF latency samples";
					case 3 :	return "cmpdecmp VBIF read data CCU";
					case 4 :	return "cmpdecmp VBIF write data CCU";
					case 5 :	return "cmpdecmp VBIF read request";
					case 6 :	return "cmpdecmp VBIF write request";
					case 7 :	return "cmpdecmp VBIF read data";
					case 8 :	return "cmpdecmp VBIF write data";
					case 9 :	return "cmpdecmp flag fetch cycles";
					case 10 :	return "cmpdecmp flag fetch samples";
					case 11 :	return "cmpdecmp depth write flag1 count";
					case 12 :	return "cmpdecmp depth write flag2 count";
					case 13 :	return "cmpdecmp depth write flag3 count";
					case 14 :	return "cmpdecmp depth write flag4 count";
					case 15 :	return "cmpdecmp color write flag1 count";
					case 16 :	return "cmpdecmp color write flag2 count";
					case 17 :	return "cmpdecmp color write flag3 count";
					case 18 :	return "cmpdecmp color write flag4 count";
					case 19 :	return "cmpdecmp 2D stall cycles VBIF req";
					case 20 :	return "cmpdecmp 2D stall cycles VBIF WR";
					case 21 :	return "cmpdecmp 2D stall cycles VBIF return";
					case 22 :	return "cmpdecmp 2D RD data";
					case 23 :	return "cmpdecmp 2D WR data";
				}
				break;
			}
		}
		return "???";
	}

/*
=================================================
	GetCounterName
=================================================
*/
	ND_ static const char*  GetCounterName (AdrenoProfiler::EGPUSeries series, uint group, uint counter)
	{
		switch_enum( series )
		{
			case AdrenoProfiler::EGPUSeries::A5xx :		return GetCounterName_A5xx( group, counter );
			case AdrenoProfiler::EGPUSeries::A6xx :		return GetCounterName_A6xx( group, counter );
            case AdrenoProfiler::EGPUSeries::A7xx :
            case AdrenoProfiler::EGPUSeries::Unknown :	break;
		}
		switch_end
		return "???";
	}

# endif // AE_ENABLE_LOGS
} // namespace


/*
=================================================
	Impl
=================================================
*/
	struct AdrenoProfiler::Impl
	{
	// variables
		int					_gpuDevice	= -1;
		HWInfo				_hwInfo;
		uint				_valueId	: 1;

		ECounterSet			_enabledCounters;

		Array<ReadCounter>	_counters;
		Array<ulong>		_values [2];


	// methods
		Impl ()									__NE___ : _valueId{0} {}
		~Impl ()								__NE___;

		ND_ bool  QueryCounters ()				__NE___;

		ND_ auto  GetCurPrevValues ()			C_NE___ -> Pair< const ulong*, const ulong* >;

			void  PrintCounters ()				C_NE___;
	};


/*
=================================================
	Impl dtor
=================================================
*/
	AdrenoProfiler::Impl::~Impl () __NE___
	{
		if ( _gpuDevice >= 0 )
			::close( _gpuDevice );
	}

/*
=================================================
	Impl::QueryCounters
=================================================
*/
	bool  AdrenoProfiler::Impl::QueryCounters () __NE___
	{
		::adreno_counter_read	read = {};
		read.num_counters	= uint(_counters.size());
		read.counters		= _counters.data();

		if_unlikely( ::ioctl( _gpuDevice, ADRENO_IOCTL_COUNTER_READ, INOUT &read ) < 0 )
			return false;

		ulong*	dst = _values[_valueId].data();

		for (usize i = 0; i < _counters.size(); ++i)
			dst[i] = _counters[i].value;

		++_valueId;
		return true;
	}

/*
=================================================
	Impl::GetCurPrevValues
=================================================
*/
	auto  AdrenoProfiler::Impl::GetCurPrevValues () C_NE___ -> Pair< const ulong*, const ulong* >
	{
		return MakePair( _values[ (_valueId+1)&1 ].data(),
						 _values[  _valueId      ].data() );
	}

/*
=================================================
	Impl::PrintCounters
=================================================
*/
	void  AdrenoProfiler::Impl::PrintCounters () C_NE___
	{
	  #ifdef AE_ENABLE_LOGS
		const uint	max_group	= 0x22;
		const uint	max_counter	= 255;
		String		str			= "Adreno perf counters:\n";

		for (uint g = 0; g <= max_group; ++g)
		{
			str << "| - | " << GetGroupName( _hwInfo.series, g ) << " | - |\n";

			for (uint c = 0; c <= max_counter; ++c)
			{
				::adreno_counter_get	cnt = {};
				cnt.group_id			= g;
				cnt.countable_selector	= c;

				if ( ::ioctl( _gpuDevice, ADRENO_IOCTL_COUNTER_GET, INOUT &cnt ) >= 0 )
				{
					str << "| " << ToString(g) << " / " << ToString(c) << " | " << GetCounterName( _hwInfo.series, g, c ) << " |\n";
				}
			}
		}

		AE_LOGI( str );
	  #endif
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor / destructor
=================================================
*/
	AdrenoProfiler::AdrenoProfiler () __NE___
	{}

	AdrenoProfiler::~AdrenoProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  AdrenoProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( counterSet.Any() );

		auto	impl = MakeUnique<Impl>();

		impl->_gpuDevice = ::open( "/dev/kgsl-3d0", O_RDWR );
		if ( impl->_gpuDevice < 0 )
			return false;

		// get ID
		{
			::adreno_device_info			dev_info	= {};
			::adreno_device_get_property	prop		= {};

			prop.type		= ADRENO_PROPERTY_DEVICE_INFO;
			prop.value		= &dev_info;
			prop.num_bytes	= sizeof(dev_info);

			if_unlikely( ::ioctl( impl->_gpuDevice, ADRENO_IOCTL_DEVICE_GET_PROPERTY, INOUT &prop ) < 0 )
				return false;

			uint	chip_id		= dev_info.chip_id;
			uint	core_id		= (chip_id >> 24) & 0xFF;
			uint	major_id	= (chip_id >> 16) & 0xFF;
			uint	minor_id	= (chip_id >>  8) & 0xFF;
			impl->_hwInfo.gpuId = core_id * 100 + major_id * 10 + minor_id;

			impl->_hwInfo.gmemSize = Bytes{dev_info.gmem_sizebytes};
		}

		// choose GPU series
		{
			if ( (impl->_hwInfo.gpuId >= 600 and impl->_hwInfo.gpuId < 700) or impl->_hwInfo.gpuId == 702 )
				impl->_hwInfo.series = EGPUSeries::A6xx;
			else
			if ( impl->_hwInfo.gpuId >= 700 )
				impl->_hwInfo.series = EGPUSeries::A7xx;
			else
			if ( impl->_hwInfo.gpuId >= 500 and impl->_hwInfo.gpuId < 600 )
				impl->_hwInfo.series = EGPUSeries::A5xx;
		}

		const auto	Activate = [&impl] (ECounter c) -> bool
		{{
			auto [group, selector] = UnpackGroupAndSelector( impl->_hwInfo.series, c );

			ReadCounter		read;
			read.group_id			= group;
			read.countable_selector	= selector;
			read.value				= 0;

			::adreno_counter_get	cnt = {};
			cnt.group_id			= group;
			cnt.countable_selector	= selector;

			if_unlikely( ::ioctl( impl->_gpuDevice, ADRENO_IOCTL_COUNTER_GET, INOUT &cnt ) < 0 )
			{
				AE_LOG_DBG( "Adreno counter x"s << ToString<16>( (group<<8) | selector ) << " is not supported" );
				return false;
			}

			impl->_counters.push_back( read );
			return true;
		}};

		for (ECounter c : counterSet)
		{
			if ( Activate( c ))
				impl->_enabledCounters.insert( c );
		}

		if ( impl->_counters.empty() )
			return false;

		CHECK( impl->_enabledCounters.BitCount() == impl->_counters.size() );

		for (auto& values : impl->_values)
			values.resize( impl->_counters.size() );

		// query initial values
		CHECK_ERR( impl->QueryCounters() );

		_impl = RVRef(impl);

		//impl->PrintCounters();

		AE_LOGI( "Started Adreno GPU profiler" );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  AdrenoProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  AdrenoProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	AdrenoProfiler::ECounterSet  AdrenoProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_enabledCounters;
	}

/*
=================================================
	GetHWInfo
=================================================
*/
	AdrenoProfiler::HWInfo  AdrenoProfiler::GetHWInfo () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_hwInfo;
	}

/*
=================================================
	Sample
=================================================
*/
	void  AdrenoProfiler::Sample (OUT Counters_t &outCounters) C_NE___
	{
		outCounters.clear();

		if ( not _impl )
			return;  // not initialized

		CHECK_ERRV( _impl->QueryCounters() );

		auto	[curr, prev] = _impl->GetCurPrevValues();
		usize	i = 0;

		outCounters.reserve( _impl->_counters.size() );

		for (ECounter c : _impl->_enabledCounters)
		{
			ulong   d = (curr[i] > prev[i] ? curr[i] - prev[i] : 0);
			outCounters.emplace( c, d );
			++i;
		}
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // AE_ENABLE_ADRENO_PERFCOUNTER and not AE_ENABLE_REMOTE_GRAPHICS

# include "profiler/Utils/AdrenoProfiler.h"
# include "profiler/Remote/RemoteAdrenoProfiler.h"

namespace AE::Profiler
{
	struct AdrenoProfiler::Impl
	{
		RC<AdrenoProfilerClient>	client;

		Impl (RC<AdrenoProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	AdrenoProfiler::AdrenoProfiler ()									__NE___	{}
	AdrenoProfiler::~AdrenoProfiler ()									__NE___	{}

	bool  AdrenoProfiler::Initialize (const ECounterSet &cs)			__NE___	{ return _impl and _impl->client->Initialize( cs ); }
	bool  AdrenoProfiler::IsInitialized ()								C_NE___	{ return _impl and _impl->client->IsInitialized(); }

	AdrenoProfiler::ECounterSet	AdrenoProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }
	AdrenoProfiler::HWInfo		AdrenoProfiler::GetHWInfo ()			C_NE___	{ return _impl ? _impl->client->GetHWInfo() : Default; }

	void  AdrenoProfiler::Sample (OUT Counters_t &result)				C_NE___	{ if (_impl) return _impl->client->Sample( OUT result ); }


	bool  AdrenoProfiler::InitClient (RC<AdrenoProfilerClient> client)	__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  AdrenoProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler
#endif // AE_ENABLE_ADRENO_PERFCOUNTER
//-----------------------------------------------------------------------------
