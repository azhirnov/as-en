// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/FeatureSetEnums.h"
#include "graphics_rhi/Public/BufferDesc.h"
#include "graphics_rhi/Public/IDs.h"

namespace AE::Graphics
{

	enum class ECoopMatrixComponentType : ubyte
	{
		Float16,
		Float32,
		Float64,
		BFloat16,		// requires 'cooperativeMatrix' or 'shaderBFloat16' features
		Float8_E4M3,	// requires 'cooperativeMatrix' or 'shaderFloat8' features
		Float8_E5M2,	// requires 'cooperativeMatrix' or 'shaderFloat8' features

		SInt8,
		SInt16,
		SInt32,
		SInt64,
		SInt8x4,		// packed to 32bit, requires 'cooperativeVector' feature

		UInt8,
		UInt16,
		UInt32,
		UInt64,
		UInt8x4,		// packed to 32bit, requires 'cooperativeVector' feature

		_Count,
		Unknown		= _Count
	};


	enum class ECoopVecMatrixLayout : ubyte
	{
		RowMajor,
		ColumnMajor,
		InferencingOptimal,
		TrainingOptimal,
		_Count,
		Unknown		= _Count
	};


	struct ConvertCoopMatrixCmd;
	struct ConvertCoopMatrixCmd2;
	struct ConvertCoopMatrixOnHost;



	//
	// Convert Cooperative Vector/Matrix Command
	//
	struct ConvertCoopMatrixCmd
	{
		Bytes						srcSize;
		DeviceAddress				srcAddress		= Default;		// EResourceState::CoopVecConvert_Read

		Bytes						dstSize;
		DeviceAddress				dstAddress		= Default;		// EResourceState::CoopVecConvert_Write

		uint						numRows			= 0;
		uint						numColumns		= 0;

		Bytes						srcStride;						// only if srcLayout = RowMajor or ColumnMajor
		Bytes						dstStride;						// only if dstLayout = RowMajor or ColumnMajor

		ECoopMatrixComponentType	srcType			= Default;
		ECoopMatrixComponentType	dstType			= Default;

		ECoopVecMatrixLayout		srcLayout		= Default;
		ECoopVecMatrixLayout		dstLayout		= Default;

		ConvertCoopMatrixCmd ()											__NE___	{}
		ConvertCoopMatrixCmd (const ConvertCoopMatrixCmd &)				__NE___	= default;
		explicit ConvertCoopMatrixCmd (const ConvertCoopMatrixCmd2 &)	__NE___;
		explicit ConvertCoopMatrixCmd (const ConvertCoopMatrixOnHost &)	__NE___;
	};


	struct ConvertCoopMatrixCmd2
	{
		Bytes						srcSize;
		Bytes						srcOffset;
		BufferID					srcBuffer;						// EResourceState::CoopVecConvert_Read

		Bytes						dstSize;
		Bytes						dstOffset;
		BufferID					dstBuffer;						// EResourceState::CoopVecConvert_Write

		uint						numRows			= 0;
		uint						numColumns		= 0;

		Bytes						srcStride;						// only if srcLayout = RowMajor or ColumnMajor
		Bytes						dstStride;						// only if dstLayout = RowMajor or ColumnMajor

		ECoopMatrixComponentType	srcType			= Default;
		ECoopMatrixComponentType	dstType			= Default;

		ECoopVecMatrixLayout		srcLayout		= Default;
		ECoopVecMatrixLayout		dstLayout		= Default;

		ConvertCoopMatrixCmd2 ()											__NE___	{}
		ConvertCoopMatrixCmd2 (const ConvertCoopMatrixCmd2 &)				__NE___	= default;
		explicit ConvertCoopMatrixCmd2 (const ConvertCoopMatrixCmd &)		__NE___;
		explicit ConvertCoopMatrixCmd2 (const ConvertCoopMatrixOnHost &)	__NE___;
	};



	//
	// Convert Cooperative Vector/Matrix On Host
	//
	struct ConvertCoopMatrixOnHost
	{
		Bytes						srcSize;
		const void *				srcData			= null;

		Bytes						dstSize;
		void *						dstData			= null;

		uint						numRows			= 0;
		uint						numColumns		= 0;

		Bytes						srcStride;						// only if srcLayout = RowMajor or ColumnMajor
		Bytes						dstStride;						// only if dstLayout = RowMajor or ColumnMajor

		ECoopMatrixComponentType	srcType			= Default;
		ECoopMatrixComponentType	dstType			= Default;

		ECoopVecMatrixLayout		srcLayout		= Default;
		ECoopVecMatrixLayout		dstLayout		= Default;

		ConvertCoopMatrixOnHost ()											__NE___	{}
		ConvertCoopMatrixOnHost (const ConvertCoopMatrixOnHost &)			__NE___	= default;
		explicit ConvertCoopMatrixOnHost (const ConvertCoopMatrixCmd &)		__NE___;
		explicit ConvertCoopMatrixOnHost (const ConvertCoopMatrixCmd2 &)	__NE___;
	};



	//
	// Cooperative Matrix Config
	//
	struct CoopMatrixConfig
	{
	// variables
		ECoopMatrixComponentType	a		= Default;
		ECoopMatrixComponentType	b		= Default;
		ECoopMatrixComponentType	c		= Default;
		ECoopMatrixComponentType	res		= Default;
		ubyte						m		= 0;
		ubyte						n		= 0;
		ubyte						k		= 0;
		bool						satAccum = false;

	// methods
		CoopMatrixConfig ()											__NE___ {}
		CoopMatrixConfig (const CoopMatrixConfig &)					__NE___ = default;
		CoopMatrixConfig (CoopMatrixConfig &&)						__NE___ = default;

		__Cx__ CoopMatrixConfig (ECoopMatrixComponentType AB,
								 ECoopMatrixComponentType CR,
								 uint M, uint N, uint K,
								 bool satAccum = false)				__NE___ : a{AB}, b{AB}, c{CR}, res{CR}, m{ubyte(M)}, n{ubyte(N)}, k{ubyte(K)}, satAccum{satAccum} {}

		explicit CoopMatrixConfig (ECoopMatrixCfg)					__NE___;
		explicit operator ECoopMatrixCfg ()							C_NE___	{ return ToECoopMatrixCfg(); }

		CoopMatrixConfig&	operator = (const CoopMatrixConfig &)	__NE___	= default;
		ND_ bool			operator == (const CoopMatrixConfig &)	C_NE___ = default;

		ND_ ECoopMatrixCfg  ToECoopMatrixCfg ()						C_NE___;

	  #ifdef AE_ENABLE_LOGS
		ND_ String  ToString ()										C_Th___;
	  #endif
	};


	//
	// Cooperative Vector Config
	//
	struct CoopVectorConfig
	{
	// variables
		ECoopMatrixComponentType	inputType				= Default;
		ECoopMatrixComponentType	inputInterpretation		= Default;
		ECoopMatrixComponentType	matrixInterpretation	= Default;
		ECoopMatrixComponentType	biasInterpretation		= Default;
		ECoopMatrixComponentType	resultType				= Default;
		bool						transpose				= false;

	// methods
		CoopVectorConfig ()														__NE___ {}
		CoopVectorConfig (const CoopVectorConfig &)								__NE___ = default;
		CoopVectorConfig (CoopVectorConfig &&)									__NE___ = default;

		__Cx__ CoopVectorConfig (ECoopMatrixComponentType inputType,
								 ECoopMatrixComponentType inputAndMatrixInterpretation,
								 ECoopMatrixComponentType biasAndResultInterpretation,
								 bool transpose)								__NE___ :
			inputType{inputType}, inputInterpretation{inputAndMatrixInterpretation},
			matrixInterpretation{inputAndMatrixInterpretation}, biasInterpretation{biasAndResultInterpretation},
			resultType{biasAndResultInterpretation}, transpose{transpose} {}

		explicit CoopVectorConfig (ECoopVecCfg)									__NE___;
		explicit operator ECoopVecCfg ()										C_NE___	{ return ToECoopVecCfg(); }

		CoopVectorConfig&	operator = (const CoopVectorConfig &)				__NE___	= default;
		ND_ bool			operator == (const CoopVectorConfig &)				C_NE___ = default;

		ND_ ECoopVecCfg		ToECoopVecCfg ()									C_NE___;

	  #ifdef AE_ENABLE_LOGS
		ND_ String  ToString ()													C_Th___;
	  #endif
	};

} // AE::Graphics
