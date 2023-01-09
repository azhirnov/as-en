// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Feature
	//
	enum class EFeature : ubyte
	{
		Ignore		= 0,
		RequireFalse,
		RequireTrue,
		_Count
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
		//								|						GLSL							|						MSL						|
		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Basic ----				|						GL_KHR_shader_subgroup_basic	|												|
		Size,						//	|	uint gl_SubgroupSize								|	uint [[threads_per_simdgroup]]				|
		InvocationID,				//	|	uint gl_SubgroupInvocationID						|	uint [[thread_index_in_simdgroup]]			|
		Elect,						//	|	bool subgroupElect()								|	bool simd_is_first()						|
		Barrier,					//	|														|	void simdgroup_barrier(mem_flags)			|
		_Basic_Begin				= Size,
		_Basic_End					= Barrier,
		
		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Vote ----				|						GL_KHR_shader_subgroup_vote		|												|
		Any,						//	|	bool subgroupAny(bool value)						|	bool simd_any(bool value)					|
		All,						//	|	bool subgroupAll(bool value)						|	bool simd_all(bool value)					|
		AllEqual,					//	|	bool subgroupAllEqual(T value)						|	-											|
		_Vote_Begin					= Any,
		_Vote_End					= AllEqual,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Arithmetic ----			|					GL_KHR_shader_subgroup_arithmetic	|												|
		Add,						//	|	T subgroupAdd(T value)								|	T simd_sum(T value)							|
		Mul,						//	|	T subgroupMul(T value)								|	T simd_product(T value)						|
		Min,						//	|	T subgroupMin(T value)								|	T simd_min(T value)							|
		Max,						//	|	T subgroupMax(T value)								|	T simd_max(T value)							|
		And,						//	|	T subgroupAnd(T value)								|	T simd_and(T value)							|
		Or,							//	|	T subgroupOr(T value)								|	T simd_or(T value)							|
		Xor,						//	|	T subgroupXor(T value)								|	T simd_xor(T value)							|
		InclusiveMul,				//	|	T subgroupInclusiveMul(T value)						|	T simd_prefix_inclusive_product(T value)	|
		InclusiveAdd,				//	|	T subgroupInclusiveAdd(T value)						|	T simd_prefix_inclusive_sum(T value)		|
		InclusiveMin,				//	|	T subgroupInclusiveMin(T value)						|	-											|
		InclusiveMax,				//	|	T subgroupInclusiveMax(T value)						|	-											|
		InclusiveAnd,				//	|	T subgroupInclusiveAnd(T value)						|	-											|
		InclusiveOr,				//	|	T subgroupInclusiveOr(T value)						|	-											|
		InclusiveXor,				//	|	T subgroupInclusiveXor(T value)						|	-											|
		ExclusiveAdd,				//	|	T subgroupExclusiveAdd(T value)						|	T simd_prefix_exclusive_sum(T value)		|
		ExclusiveMul,				//	|	T subgroupExclusiveMul(T value)						|	T simd_prefix_exclusive_product (T value)	|
		ExclusiveMin,				//	|	T subgroupExclusiveMin(T value)						|	-											|
		ExclusiveMax,				//	|	T subgroupExclusiveMax(T value)						|	-											|
		ExclusiveAnd,				//	|	T subgroupExclusiveAnd(T value)						|	-											|
		ExclusiveOr,				//	|	T subgroupExclusiveOr(T value)						|	-											|
		ExclusiveXor,				//	|	T subgroupExclusiveXor(T value)						|	-											|
		_Arithmetic_Begin			= Add,
		_Arithmetic_End				= ExclusiveXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Ballot ----				|						GL_KHR_shader_subgroup_ballot	|												|
		Ballot,						//	|	uvec4 subgroupBallot(bool value)					|	simd_vote simd_ballot(bool value)			|
		Broadcast,					//	|	T subgroupBroadcast(T value, uint id)				|	T simd_broadcast(T value, ushort id)		|
		BroadcastFirst,				//	|	T subgroupBroadcastFirst(T value)					|	T simd_broadcast_first(T value)				|
		InverseBallot,				//	|	bool subgroupInverseBallot(uvec4 value)				|	-											|
		BallotBitExtract,			//	| bool subgroupBallotBitExtract(uvec4 value, uint index)|	-											|
		BallotBitCount,				//	|	uint subgroupBallotBitCount(uvec4 value)			|	-											|
		BallotInclusiveBitCount,	//	|	uint subgroupBallotInclusiveBitCount(uvec4 value)	|	-											|
		BallotExclusiveBitCount,	//	|	uint subgroupBallotExclusiveBitCount(uvec4 value)	|	-											|
		BallotFindLSB,				//	|	uint subgroupBallotFindLSB(uvec4 value)				|	-											|
		BallotFindMSB,				//	|	uint subgroupBallotFindMSB(uvec4 value)				|	-											|
		_Ballot_Begin				= Ballot,
		_Ballot_End					= BallotFindMSB,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Shuffle ----			|						GL_KHR_shader_subgroup_shuffle	|												|
		Shuffle,					//	|	T subgroupShuffle(T value, uint index)				|	T simd_shuffle(T value, ushort index)		|
		ShuffleXor,					//	|	T subgroupShuffleXor(T value, uint mask)			|	T simd_shuffle_xor(T value, ushort mask)	|
		_Shuffle_Begin				= Shuffle,
		_Shuffle_End				= ShuffleXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Shuffle Relative ----	|				GL_KHR_shader_subgroup_shuffle_relative	|												|
		ShuffleUp,					//	|	T subgroupShuffleUp(T value, uint delta)			|	T simd_shuffle_up(T value, ushort delta)	|
		ShuffleDown,				//	|	T subgroupShuffleDown(T value, uint delta)			|	T simd_shuffle_down(T value, ushort delta)	|
		_ShuffleRelative_Begin		= ShuffleUp,
		_ShuffleRelative_End		= ShuffleDown,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Clustered ----			|					GL_KHR_shader_subgroup_clustered	|												|
		ClusteredAdd,				//	|	T subgroupClusteredAdd(T value, uint clusterSize)	|	-											|
		ClusteredMul,				//	|	T subgroupClusteredMul(T value, uint clusterSize)	|	-											|
		ClusteredMin,				//	|	T subgroupClusteredMin(T value, uint clusterSize)	|	-											|
		ClusteredMax,				//	|	T subgroupClusteredMax(T value, uint clusterSize)	|	-											|
		ClusteredAnd,				//	|	T subgroupClusteredAnd(T value, uint clusterSize)	|	-											|
		ClusteredOr,				//	|	T subgroupClusteredOr(T value, uint clusterSize)	|	-											|
		ClusteredXor,				//	|	T subgroupClusteredXor(T value, uint clusterSize)	|	-											|
		_Clustered_Begin			= ClusteredAdd,
		_Clustered_End				= ClusteredXor,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Quad ----				|						GL_KHR_shader_subgroup_quad		|												|
		QuadBroadcast,				//	|	T subgroupQuadBroadcast(T value, uint id)			|	T quad_broadcast(T value, ushort id)		|
		QuadSwapHorizontal,			//	|	T subgroupQuadSwapHorizontal(T value)				|	-											|
		QuadSwapVertical,			//	|	T subgroupQuadSwapVertical(T value)					|	-											|
		QuadSwapDiagonal,			//	|	T subgroupQuadSwapDiagonal(T value)					|	-											|
		_Quad_Begin					= QuadBroadcast,
		_Quad_End					= QuadSwapDiagonal,

		//								|-------------------------------------------------------|-----------------------------------------------|
		// ---- Partitioned NV ----		|				GL_NV_shader_subgroup_partitioned		|												|
		//PartitionedAdd_NV,		//	|	T subgroupPartitionedAddNV(T value, uvec4 ballot)	|												|
		//PartitionedMin_NV,		//	|	T subgroupPartitionedMinNV(T value, uvec4 ballot)	|												|
		//PartitionedMax_NV,		//	|	T subgroupPartitionedMaxNV(T value, uvec4 ballot)	|												|
		//PartitionedAnd_NV,		//	|	T subgroupPartitionedAndNV(T value, uvec4 ballot)	|												|
		//PartitionedOr_NV,			//	|	T subgroupPartitionedOrNV(T value, uvec4 ballot)	|												|
		//PartitionedXor_NV,		//	|	T subgroupPartitionedXorNV(T value, uvec4 ballot)	|												|
		// TODO
		
		_Count
	};
	STATIC_ASSERT( uint(ESubgroupOperation::_Count) == 53 );


	//
	// Vendor ID
	//
	enum class EVendorID : uint
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
		_Count,
		Unknown		= _Count,
	};


	//
	// Graphics Device ID
	//
	enum class EGraphicsDeviceID : uint
	{
	#define AE_GRAPHICS_DEVICE_LIST( _visit_ ) \
		/*---- Adreno ----*/\
		_visit_( Adreno_500 )			\
		_visit_( Adreno_600 )			/* 605, 608, 610, 612, 615, 616, 618, 619L, 620		with dual channel LPDDR4							*/\
		_visit_( Adreno_600_QC4 )		/* 630, 640, 643, 675								with quad channel LPDDR4							*/\
		_visit_( Adreno_600_OC4 )		/* 680, 685, 690									with octa channel LPDDR4X							*/\
		_visit_( Adreno_600_QC5 )		/* 643, 650, 660									with quad channel LPDDR5							*/\
		_visit_( Adreno_700_SC3 )		/* 702												with single channel LPDDR3							*/\
		_visit_( Adreno_700_DC4_SC5 )	/* 730												with dual channel LPDDR4 or single channel LPDDR5	*/\
		\
		/*---- AMD ----*/\
		_visit_( AMD_GCN1 )				/* 520, 610									*/\
		_visit_( AMD_GCN2 )				/* PS4										*/\
		_visit_( AMD_GCN3 )				/* 530										*/\
		_visit_( AMD_GCN4 )				/* PS4 Pro, 540-590, 620-640				*/\
		_visit_( AMD_GCN5 )				/* RX Vega, Radeon VII						*/\
		_visit_( AMD_GCN5_APU )			/* Ryzen 2xxx - 5xxx APU					*/\
		_visit_( AMD_RDNA1 )			/* 5300 - 5700								*/\
		_visit_( AMD_RDNA2 )			/* N6: 6400 - 6500, N7: 6600 - 6900			*/\
		_visit_( AMD_RDNA2_APU )		/* PS5, Ryzen 6xxx - 7xxx APU, SteamDeck	*/\
		\
		/*---- Apple ----*/\
		_visit_( Apple_A8 )				\
		_visit_( Apple_A9_A10 )			\
		_visit_( Apple_A11 )			\
		_visit_( Apple_A12 )			\
		_visit_( Apple_A13 )			\
		_visit_( Apple_A14_M1 )			/* M1 Pro, M1 Max	*/\
		_visit_( Apple_A15_M2 )			\
		\
		/*---- Mali ----*/\
		_visit_( Mali_Midgard_Gen4 )	/* T820 ... T880	- vector	*/\
		_visit_( Mali_Bifrost_Gen1 )	/* G31, G51, G71	- scalar	*/\
		_visit_( Mali_Bifrost_Gen2 )	/* G52, G72						*/\
		_visit_( Mali_Bifrost_Gen3 )	/* G76							*/\
		_visit_( Mali_Valhall_Gen1 )	/* G57, G77						*/\
		_visit_( Mali_Valhall_Gen2 )	/* G68, G78						*/\
		_visit_( Mali_Valhall_Gen3 )	/* G310, G510, G610, G710		*/\
		\
		/*---- NVidia ----*/\
		_visit_( NV_Maxwell )			/* GTX 9xx, Titan X, Quadro Mxxxx						*/\
		_visit_( NV_Maxwell_Tegra )		/* Tegra X1, Nintendo Switch, Shield TV					*/\
		_visit_( NV_Pascal )			/* GTX 10xx, Titan X, Titan Xp, Quadro Pxxx				*/\
		_visit_( NV_Pascal_MX )			/* MX 1xx, GTX 10xx Max-Q								*/\
		_visit_( NV_Pascal_Tegra )		/* Tegra X2												*/\
		_visit_( NV_Volta )				/* Volta, Xavier, Titan V								*/\
		_visit_( NV_Turing_16 )			/* GTX 16xx, 16xx Max-Q, Quadro Txxx, Txxx				*/\
		_visit_( NV_Turing )			/* 20xx, Quadro RTX	x000								*/\
		_visit_( NV_Turing_MX )			/* MX230, MX250, MX330, MX350, MX450, MX550, MX570		*/\
		_visit_( NV_Ampere )			/* RTX 30xx, RTX Ax000									*/\
		_visit_( NV_Ampere_Orin )		/* Orin													*/\
		_visit_( NV_Ada )				/* RTX 40xx												*/\
		\
		/*---- Intel ----*/\
		_visit_( Intel_Gen9 )			/* UHD 620, 630									*/\
		_visit_( Intel_Gen11 )			/* Iris Plus, Core 10xxx						*/\
		_visit_( Intel_Gen12 )			/* UHD Graphics 7xx, Iris Xe, Core 11xxx		*/\
		\
		/*---- PowerVR ----*/\
		_visit_( PowerVR_Series8XE )	/* GE8100, GE8200, GE8300, GE8310, GE8430			*/\
		_visit_( PowerVR_Series8XEP )	/* GE8320, GE8325, GE8340							*/\
		_visit_( PowerVR_Series8XT )	/* GT8525, GT8540									*/\
		_visit_( PowerVR_Series9XE )	/* GE9000, GE9100, GE9115, GE9210, GE9215, GE9420	*/\
		\
		/*---- Other ----*/\
		_visit_( VeriSilicon )			/*  */\
		_visit_( V3D_4 )				/* Raspberry Pi 4 */\
		_visit_( SwiftShader )			/* emulation */\
		
		#define AE_GRAPHICS_DEVICE_VISIT( _name_ )				_name_,
		AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
		#undef AE_GRAPHICS_DEVICE_VISIT

		_Count,
		Unknown			= _Count,

		_Adreno_Begin	= Adreno_500,
		_Adreno_End		= Adreno_700_DC4_SC5,

		_AMD_Begin		= AMD_GCN1,
		_AMD_End		= AMD_RDNA2_APU,

		_Apple_Begin	= Apple_A8,
		_Apple_End		= Apple_A15_M2,

		_Mali_Begin		= Mali_Midgard_Gen4,
		_Mali_End		= Mali_Valhall_Gen3,

		_NV_Begin		= NV_Maxwell,
		_NV_End			= NV_Ada,

		_Intel_Begin	= Intel_Gen9,
		_Intel_End		= Intel_Gen12,

		_PowerVR_Begin	= PowerVR_Series8XE,
		_PowerVR_End	= PowerVR_Series9XE,

		_Other_Begin	= VeriSilicon,
	};

	STATIC_ASSERT( uint(EGraphicsDeviceID::_Adreno_Begin)	== 0 );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Adreno_Begin)	<  uint(EGraphicsDeviceID::_Adreno_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Adreno_End)+1	== uint(EGraphicsDeviceID::_AMD_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_AMD_Begin)		<  uint(EGraphicsDeviceID::_AMD_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_AMD_End)+1		== uint(EGraphicsDeviceID::_Apple_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Apple_Begin)	<  uint(EGraphicsDeviceID::_Apple_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Apple_End)+1	== uint(EGraphicsDeviceID::_Mali_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Mali_Begin)		<  uint(EGraphicsDeviceID::_Mali_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Mali_End)+1		== uint(EGraphicsDeviceID::_NV_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_NV_Begin)		<  uint(EGraphicsDeviceID::_NV_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_NV_End)+1		== uint(EGraphicsDeviceID::_Intel_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Intel_Begin)	<  uint(EGraphicsDeviceID::_Intel_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_Intel_End)+1	== uint(EGraphicsDeviceID::_PowerVR_Begin) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_PowerVR_Begin)	<  uint(EGraphicsDeviceID::_PowerVR_End) );
	STATIC_ASSERT( uint(EGraphicsDeviceID::_PowerVR_End)+1	== uint(EGraphicsDeviceID::_Other_Begin) );


} // AE::Graphics
