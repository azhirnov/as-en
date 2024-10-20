// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Surface Format
	//
	enum class ESurfaceFormat : ubyte
	{
		BGRA8_sRGB_nonlinear,			// BGRA8_UNorm,		sRGB_nonlinear
		RGBA8_sRGB_nonlinear,			// RGBA8_UNorm,		sRGB_nonlinear
		BGRA8_BT709_nonlinear,			// BGRA8_UNorm,		BT709_nonlinear

		RGBA16F_Extended_sRGB_linear,	// RGBA16F,			Extended_sRGB_linear
		RGBA16F_sRGB_nonlinear,			// RGBA16F,			sRGB_nonlinear
		RGBA16F_BT709_nonlinear,		// RGBA16F,			BT709_nonlinear
		RGBA16F_HDR10_ST2084,			// RGBA16F,			HDR10_ST2084
		RGBA16F_BT2020_linear,			// RGBA16F,			BT2020_linear

		RGB10A2_sRGB_nonlinear,			// RGB10_A2_UNorm,	sRGB_nonlinear
		RGB10A2_HDR10_ST2084,			// RGB10_A2_UNorm,	HDR10_ST2084

		_Count,
		Unknown	= 0xFF,
	};



	//
	// Subgroup Types
	//
	enum class ESubgroupTypes : ubyte
	{
		Unknown		= 0,
		Float32		= 1 << 0,
		Int32		= 1 << 1,

		// extended
		Int8		= 1 << 2,
		Int16		= 1 << 3,
		Int64		= 1 << 4,
		Float16		= 1 << 5,

		_Last,
		All			= ((_Last - 1) << 1) - 1
	};
	AE_BIT_OPERATORS( ESubgroupTypes );


	//
	// Subgroup Operation
	//
	enum class ESubgroupOperation : uint
	{
		//
		// Inclusive suffix - function includes invocations from 0 to `InvocationID.
		// Exclusive suffix - function includes invocations from 0 to `InvocationID` but current invocation is not included.

		//								|						GLSL							|						MSL						|
		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Basic ----				|						   GL_KHR_shader_subgroup_basic |												|
		IndexAndSize,				//	|	uint gl_SubgroupSize								|	uint [[threads_per_simdgroup]]				| - size of the subgroup
									//	|	uint gl_SubgroupInvocationID						|	uint [[thread_index_in_simdgroup]]			| - invocation index in the subgroup
									//	|														|	uint [[thread_index_in_quadgroup]]			| - invocation index in the guadgroup
									//	|	uint gl_SubgroupID									|	uint [[simdgroup_index_in_threadgroup]]		| - subgroup index in workgroup
									//	|														|	uint [[quadgroup_index_in_threadgroup]]		| - quadgroup index in workgroup
									//	|	uint gl_NumSubgroups								|	uint [[simdgroups_per_threadgroup]]			| - num subgroups per workgroup
									//	|														|	uint [[quadgroups_per_threadgroup]]			| - num quadgroups per workgroup
									//	|-------------------------------------------------------|-----------------------------------------------|
		Elect,						//	|	bool subgroupElect()								|	bool simd_is_first()						| - exactly one invocation will return true
		Barrier,					//	|	void subgroupBarrier()								|	void simdgroup_barrier(mem_none)			| - execution barrier
									//	|	void subgroupMemoryBarrier()						|	void simdgroup_barrier(all)					| - wait for the completion of accesses to all memory types
									//	|	void subgroupMemoryBarrierBuffer()					|	void simdgroup_barrier(mem_device)			| - wait for the completion of accesses to buffer memory
									//	|	void subgroupMemoryBarrierShared()					|	void simdgroup_barrier(mem_threadgroup)		| - wait for the completion of accesses to shared memory
									//	|	void subgroupMemoryBarrierImage()					|	void simdgroup_barrier(mem_texture)			| - wait for the completion of accesses to image memory
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Basic_Begin				= IndexAndSize,
		_Basic_End					= Barrier,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Vote ----				|							GL_KHR_shader_subgroup_vote |												|
		Any,						//	|	bool subgroupAny(bool value)						|	bool simd_any(bool value)					| - returns true if any active invocation has 'value == true'
		All,						//	|	bool subgroupAll(bool value)						|	bool simd_all(bool value)					| - returns true if all active invocation have 'value == true'
		AllEqual,					//	|	bool subgroupAllEqual(T value)						|	-											| - returns true if all active invocation have a 'value' that is equal
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Vote_Begin					= Any,
		_Vote_End					= AllEqual,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Arithmetic ----			|					  GL_KHR_shader_subgroup_arithmetic |												|
		Add,						//	|	T subgroupAdd(T value)								|	T simd_sum(T value)							| - returns the summation of all active invocation provided 'value's
		Mul,						//	|	T subgroupMul(T value)								|	T simd_product(T value)						| - returns the multiplication of all active invocation-provided 'value's.
		Min,						//	|	T subgroupMin(T value)								|	T simd_min(T value)							| - returns the minimum <value> of all active invocation-provided 'value's.
		Max,						//	|	T subgroupMax(T value)								|	T simd_max(T value)							| - returns the maximum <value> of all active invocation-provided 'value's.
		And,						//	|	T subgroupAnd(T value)								|	T simd_and(T value)							| - returns the bitwise AND of all active invocation provided 'value's.
		Or,							//	|	T subgroupOr(T value)								|	T simd_or(T value)							| - returns the bitwise OR of all active invocation provided 'value's.
		Xor,						//	|	T subgroupXor(T value)								|	T simd_xor(T value)							| - returns the bitwise XOR of all active invocation provided 'value's.
		InclusiveMul,				//	|	T subgroupInclusiveMul(T value)						|	T simd_prefix_inclusive_product(T value)	| - returns an inclusive scan operation that is the multiplication of all active invocation-provided 'value's.
		InclusiveAdd,				//	|	T subgroupInclusiveAdd(T value)						|	T simd_prefix_inclusive_sum(T value)		| - returns an inclusive scan operation that is the summation of all active invocation-provided 'value's.
		InclusiveMin,				//	|	T subgroupInclusiveMin(T value)						|	-											| - returns an inclusive scan operation that is the minimum <value> of all active invocation-provided 'value's.
		InclusiveMax,				//	|	T subgroupInclusiveMax(T value)						|	-											| - returns an inclusive scan operation that is the maximum <value> of all active invocation-provided 'value's.
		InclusiveAnd,				//	|	T subgroupInclusiveAnd(T value)						|	-											| - returns an inclusive scan operation that is the bitwise AND of all active invocation-provided 'value's.
		InclusiveOr,				//	|	T subgroupInclusiveOr(T value)						|	-											| - returns an inclusive scan operation that is the bitwise OR of all active invocation-provided 'value's.
		InclusiveXor,				//	|	T subgroupInclusiveXor(T value)						|	-											| - returns an inclusive scan operation that is the bitwise XOR of all active invocation-provided 'value's.
		ExclusiveAdd,				//	|	T subgroupExclusiveAdd(T value)						|	T simd_prefix_exclusive_sum(T value)		| - returns an exclusive scan operation that is the summation of all active invocation-provided 'value's.
		ExclusiveMul,				//	|	T subgroupExclusiveMul(T value)						|	T simd_prefix_exclusive_product(T value)	| - returns an exclusive scan operation that is the multiplication of all active invocation-provided 'value's.
		ExclusiveMin,				//	|	T subgroupExclusiveMin(T value)						|	-											| - returns an exclusive scan operation that is the minimum <value> of all active invocation-provided 'value's.
		ExclusiveMax,				//	|	T subgroupExclusiveMax(T value)						|	-											| - returns an exclusive scan operation that is the maximum <value> of all active invocation-provided 'value's.
		ExclusiveAnd,				//	|	T subgroupExclusiveAnd(T value)						|	-											| - returns an exclusive scan operation that is the bitwise AND of all active invocation-provided 'value's.
		ExclusiveOr,				//	|	T subgroupExclusiveOr(T value)						|	-											| - returns an exclusive scan operation that is the bitwise OR of all active invocation-provided 'value's.
		ExclusiveXor,				//	|	T subgroupExclusiveXor(T value)						|	-											| - returns an exclusive scan operation that is the bitwise XOR of all active invocation-provided 'value's.
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Arithmetic_Begin			= Add,
		_Arithmetic_End				= ExclusiveXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Ballot ----				|						  GL_KHR_shader_subgroup_ballot |												|
		Ballot,						//	|	uint4 subgroupBallot(bool value)					|	simd_vote simd_ballot(bool value)			|
		Broadcast,					//	|	T subgroupBroadcast(T value, uint id)				|	T simd_broadcast(T value, ushort id)		|
		BroadcastFirst,				//	|	T subgroupBroadcastFirst(T value)					|	T simd_broadcast_first(T value)				|
		InverseBallot,				//	|	bool subgroupInverseBallot(uint4 value)				|	-											|
		BallotBitExtract,			//	| bool subgroupBallotBitExtract(uint4 value, uint index)|	-											|
		BallotBitCount,				//	|	uint subgroupBallotBitCount(uint4 value)			|	-											|
		BallotInclusiveBitCount,	//	|	uint subgroupBallotInclusiveBitCount(uint4 value)	|	-											|
		BallotExclusiveBitCount,	//	|	uint subgroupBallotExclusiveBitCount(uint4 value)	|	-											|
		BallotFindLSB,				//	|	uint subgroupBallotFindLSB(uint4 value)				|	-											|
		BallotFindMSB,				//	|	uint subgroupBallotFindMSB(uint4 value)				|	-											|
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Ballot_Begin				= Ballot,
		_Ballot_End					= BallotFindMSB,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Shuffle ----			|						 GL_KHR_shader_subgroup_shuffle |												|
		Shuffle,					//	|	T subgroupShuffle(T value, uint index)				|	T simd_shuffle(T value, ushort index)		|
		ShuffleXor,					//	|	T subgroupShuffleXor(T value, uint mask)			|	T simd_shuffle_xor(T value, ushort mask)	|
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Shuffle_Begin				= Shuffle,
		_Shuffle_End				= ShuffleXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Shuffle Relative ----	|				GL_KHR_shader_subgroup_shuffle_relative |												|
		ShuffleUp,					//	|	T subgroupShuffleUp(T value, uint delta)			|	T simd_shuffle_up(T value, ushort delta)	|
		ShuffleDown,				//	|	T subgroupShuffleDown(T value, uint delta)			|	T simd_shuffle_down(T value, ushort delta)	|
									//	'-------------------------------------------------------'-----------------------------------------------'
		_ShuffleRelative_Begin		= ShuffleUp,
		_ShuffleRelative_End		= ShuffleDown,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Clustered ----			|					   GL_KHR_shader_subgroup_clustered |												|
		ClusteredAdd,				//	|	T subgroupClusteredAdd(T value, uint clusterSize)	|	-											|
		ClusteredMul,				//	|	T subgroupClusteredMul(T value, uint clusterSize)	|	-											|
		ClusteredMin,				//	|	T subgroupClusteredMin(T value, uint clusterSize)	|	-											|
		ClusteredMax,				//	|	T subgroupClusteredMax(T value, uint clusterSize)	|	-											|
		ClusteredAnd,				//	|	T subgroupClusteredAnd(T value, uint clusterSize)	|	-											|
		ClusteredOr,				//	|	T subgroupClusteredOr(T value, uint clusterSize)	|	-											|
		ClusteredXor,				//	|	T subgroupClusteredXor(T value, uint clusterSize)	|	-											|
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Clustered_Begin			= ClusteredAdd,
		_Clustered_End				= ClusteredXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Quad ----				|							GL_KHR_shader_subgroup_quad |												|
		QuadBroadcast,				//	|	T subgroupQuadBroadcast(T value, uint id)			|	T quad_broadcast(T value, ushort id)		|
		QuadSwapHorizontal,			//	|	T subgroupQuadSwapHorizontal(T value)				|	-											|
		QuadSwapVertical,			//	|	T subgroupQuadSwapVertical(T value)					|	-											|
		QuadSwapDiagonal,			//	|	T subgroupQuadSwapDiagonal(T value)					|	-											|
									//	'-------------------------------------------------------'-----------------------------------------------'
		_Quad_Begin					= QuadBroadcast,
		_Quad_End					= QuadSwapDiagonal,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Partitioned NV ----		|					  GL_NV_shader_subgroup_partitioned |	-											|
		//PartitionedAdd_NV,		//	|	T subgroupPartitionedAddNV(T value, uint4 ballot)	|	-											|
		//PartitionedMin_NV,		//	|	T subgroupPartitionedMinNV(T value, uint4 ballot)	|	-											|
		//PartitionedMax_NV,		//	|	T subgroupPartitionedMaxNV(T value, uint4 ballot)	|	-											|
		//PartitionedAnd_NV,		//	|	T subgroupPartitionedAndNV(T value, uint4 ballot)	|	-											|
		//PartitionedOr_NV,			//	|	T subgroupPartitionedOrNV(T value, uint4 ballot)	|	-											|
		//PartitionedXor_NV,		//	|	T subgroupPartitionedXorNV(T value, uint4 ballot)	|	-											|
									//	'-------------------------------------------------------'-----------------------------------------------'
		// TODO

		_Count
	};
	StaticAssert( uint(ESubgroupOperation::_Count) == 52 );


	//
	// GPU Vendor
	//
	enum class EGPUVendor : uint
	{
		AMD,
		NVidia,
		Intel,
		ARM,			// Mali GPU
		Qualcomm,		// Adreno GPU
		ImgTech,		// PowerVR GPU
		Microsoft,
		Apple,
		Mesa,
		Broadcom,
		Samsung,
		VeriSilicon,
		Huawei,
		_Count,
		Unknown		= _Count,
	};


	//
	// Graphics Device ID
	//
	enum class EGraphicsDeviceID : uint
	{
	#define AE_GRAPHICS_DEVICE_LIST( _visit_ )\
		/*---- Adreno ----*/\
		_visit_( Adreno_500 )			\
		_visit_( Adreno_600 )			/* 													- VRS		*/\
		_visit_( Adreno_700 )			/* 																*/\
		\
		/*---- AMD ----*/\
		_visit_( AMD_GCN1 )				/* R5-R9, RX 520, 610, HD 7970									*/\
		_visit_( AMD_GCN2 )				/* PS4, XBox One X												*/\
		_visit_( AMD_GCN3 )				/* RX 530														*/\
		_visit_( AMD_GCN4 )				/* PS4 Pro, RX 540-590, RX 620-640								*/\
		_visit_( AMD_GCN5 )				/* RX Vega, Radeon VII											*/\
		_visit_( AMD_GCN5_APU )			/* Ryzen 2xxx - 5xxx APU, Ryzen 7x30 APU						*/\
		_visit_( AMD_RDNA1 )			/* RX 5300 - 5700												*/\
		_visit_( AMD_RDNA2 )			/* N6: RX 6400 - 6500, N7: RX 6600 - 6900		- RT, MS, VRS	*/\
		_visit_( AMD_RDNA2_APU )		/* PS5, SteamDeck, Ryzen 6xxx / 7x20 / 7x35 / 7x45 APU 			*/\
		_visit_( AMD_RDNA3 )			/* N5/N6: 7600 - 7900, Radeon 7xxM								*/\
		_visit_( AMD_RDNA3_APU )		/* Ryzen 7x40 APU, Z1											*/\
		_visit_( AMD_RDNA4 )			/*																*/\
		\
		/*---- Apple ----*/\
		_visit_( Apple_A8 )				/*										- Apple2				*/\
		_visit_( Apple_A9_A10 )			/*				(Hurricane / Zephyr)	- Apple3				*/\
		_visit_( Apple_A11 )			/*				(Monsoon / Mistral)		- Apple4	- TS		*/\
		_visit_( Apple_A12 )			/* A12X, A12Z	(Vortex / Tempest)		- Apple5				*/\
		_visit_( Apple_A13 )			/*				(Lightning / Thunder)	- Apple6	- RT		*/\
		_visit_( Apple_A14_M1 )			/* all M1		(Firestorm / Icestorm)	- Apple7	- MS		*/\
		_visit_( Apple_A15_M2 )			/* all M2		(Avalanche / Blizzard)	- Apple8				*/\
		_visit_( Apple_A16 )			/*				(Everest / Sawtooth)	- Apple8				*/\
		_visit_( Apple_A17_M3 )			/* A17, M3, M4	( )						- Apple9	- HwRT		*/\
		\
		/*---- Mali ----*/\
		_visit_( Mali_Midgard_Gen2 )	/* T622, T624, T628, T678										*/\
		_visit_( Mali_Midgard_Gen3 )	/* T720, T760													*/\
		_visit_( Mali_Midgard_Gen4 )	/* T820 ... T880						- vector				*/\
		_visit_( Mali_Bifrost_Gen1 )	/* G31, G51, G71						- scalar, warp 4		*/\
		_visit_( Mali_Bifrost_Gen2 )	/* G52, G72														*/\
		_visit_( Mali_Bifrost_Gen3 )	/* G76									- warp 8				*/\
		_visit_( Mali_Valhall_Gen1 )	/* G57, G77								- warp 16				*/\
		_visit_( Mali_Valhall_Gen2 )	/* G68, G78								- 2 queues				*/\
		_visit_( Mali_Valhall_Gen3 )	/* G310, G510, G610, G710				- 3 queues				*/\
		_visit_( Mali_Valhall_Gen4 )	/* G615, G715							- VRS, RT				*/\
		_visit_( Mali_5thGen_Gen1 )		/* G620, G720							- DVS					*/\
		_visit_( Mali_5thGen_Gen2 )		/* G625, G725													*/\
		\
		/*---- NVidia ----*/\
		_visit_( NV_Maxwell )			/* GTX 9xx, Titan X, Quadro Mxxxx, MX130						*/\
		_visit_( NV_Maxwell_Tegra )		/* Tegra X1, Nintendo Switch, Shield TV							*/\
		_visit_( NV_Pascal )			/* GTX 10xx, Titan X, Titan Xp, Quadro Pxxx						*/\
		_visit_( NV_Pascal_MX )			/* MX 150, GTX 10xx Max-Q, MX2xx, MX3xx							*/\
		_visit_( NV_Pascal_Tegra )		/* Tegra X2														*/\
		_visit_( NV_Volta )				/* Volta, Xavier, Titan V										*/\
		_visit_( NV_Turing_16 )			/* GTX 16xx, 16xx Max-Q, Quadro Txxx			- MS, VRS		*/\
		_visit_( NV_Turing )			/* 20xx, Quadro RTX x000						- RT, MS		*/\
		_visit_( NV_Turing_MX )			/* MX4xx, MX550													*/\
		_visit_( NV_Ampere )			/* RTX 30xx, RTX Ax000, MX570									*/\
		_visit_( NV_Ampere_Orin )		/* Orin															*/\
		_visit_( NV_Ada )				/* RTX 40xx														*/\
		_visit_( NV_Blackwell )			/* RTX 50xx														*/\
		\
		/*---- Intel ----*/\
		_visit_( Intel_Gen7 )			/* 																*/\
		_visit_( Intel_Gen8 )			/* 																*/\
		_visit_( Intel_Gen9 )			/* HD 510..550, HD 610..650, UHD 610..650						*/\
		_visit_( Intel_Gen11 )			/* Iris Plus, Core 10xxx		(Ice Lake)						*/\
		_visit_( Intel_Gen12 )			/* UHD Graphics 7xx, Iris Xe, Core 11xxx						*/\
		_visit_( Intel_Gen12_7 )		/* Arc 3/5/7									- RT, MS, VRS	*/\
		\
		/*---- PowerVR ----*/\
		_visit_( PowerVR_Series8 )		/* GE8xxx														*/\
		_visit_( PowerVR_Series9 )		/* GE9xxx														*/\
		_visit_( PowerVR_SeriesA )		/* AXE, AXM														*/\
		_visit_( PowerVR_SeriesB )		/* BXE, BXM														*/\
		\
		/*---- Other ----*/\
		_visit_( VeriSilicon )			/*																*/\
		/*_visit_( V3D_4 )				/ * Raspberry Pi 4												*/\
		/*_visit_( V3D_6 )				/ * Raspberry Pi 5												*/\
		_visit_( SwiftShader )			/* emulation													*/\
		/*_visit_( Huawei_Maleoon910 )	/ *												- TS, VRS		*/\

		#define AE_GRAPHICS_DEVICE_VISIT( _name_ )				_name_,
		AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
		#undef AE_GRAPHICS_DEVICE_VISIT

		_Count,
		Unknown			= _Count,

		_Adreno_Begin	= Adreno_500,
		_Adreno_End		= Adreno_700,

		_AMD_Begin		= AMD_GCN1,
		_AMD_End		= AMD_RDNA4,

		_Apple_Begin	= Apple_A8,
		_Apple_End		= Apple_A17_M3,

		_Mali_Begin		= Mali_Midgard_Gen2,
		_Mali_End		= Mali_5thGen_Gen2,

		_NV_Begin		= NV_Maxwell,
		_NV_End			= NV_Blackwell,

		_Intel_Begin	= Intel_Gen7,
		_Intel_End		= Intel_Gen12_7,

		_PowerVR_Begin	= PowerVR_Series8,
		_PowerVR_End	= PowerVR_SeriesB,

		_Other_Begin	= VeriSilicon,
	};

	StaticAssert( uint(EGraphicsDeviceID::_Adreno_Begin)	== 0 );
	StaticAssert( uint(EGraphicsDeviceID::_Adreno_Begin)	<  uint(EGraphicsDeviceID::_Adreno_End) );
	StaticAssert( uint(EGraphicsDeviceID::_Adreno_End)+1	== uint(EGraphicsDeviceID::_AMD_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_AMD_Begin)		<  uint(EGraphicsDeviceID::_AMD_End) );
	StaticAssert( uint(EGraphicsDeviceID::_AMD_End)+1		== uint(EGraphicsDeviceID::_Apple_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_Apple_Begin)		<  uint(EGraphicsDeviceID::_Apple_End) );
	StaticAssert( uint(EGraphicsDeviceID::_Apple_End)+1		== uint(EGraphicsDeviceID::_Mali_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_Mali_Begin)		<  uint(EGraphicsDeviceID::_Mali_End) );
	StaticAssert( uint(EGraphicsDeviceID::_Mali_End)+1		== uint(EGraphicsDeviceID::_NV_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_NV_Begin)		<  uint(EGraphicsDeviceID::_NV_End) );
	StaticAssert( uint(EGraphicsDeviceID::_NV_End)+1		== uint(EGraphicsDeviceID::_Intel_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_Intel_Begin)		<  uint(EGraphicsDeviceID::_Intel_End) );
	StaticAssert( uint(EGraphicsDeviceID::_Intel_End)+1		== uint(EGraphicsDeviceID::_PowerVR_Begin) );
	StaticAssert( uint(EGraphicsDeviceID::_PowerVR_Begin)	<  uint(EGraphicsDeviceID::_PowerVR_End) );
	StaticAssert( uint(EGraphicsDeviceID::_PowerVR_End)+1	== uint(EGraphicsDeviceID::_Other_Begin) );


} // AE::Graphics
