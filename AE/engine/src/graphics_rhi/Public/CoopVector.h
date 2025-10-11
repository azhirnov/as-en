// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/FeatureSetEnums.h"

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
		Unknown		= 0xFF
	};


	enum class ECoopVecMatrixLayout : ubyte
	{
		RowMajor,
		ColumnMajor,
		InferencingOptimal,
		TrainingOptimal,
		_Count,
		Unknown		= 0xFF
	};



	//
	// Convert Cooperative Vector/Matrix Command
	//
	struct ConvertCoopMatrixCmd
	{
		Bytes						srcSize;
		DeviceAddress				srcAddress		= Default;		// EResourceState::ShaderAddress_Read | CoopVecConvertStage

		Bytes						dstSize;
		DeviceAddress				dstAddress		= Default;		// EResourceState::ShaderAddress_Write | CoopVecConvertStage

		uint						numRows			= 0;
		uint						numColumns		= 0;

		Bytes						srcStride;						// only if srcLayout = RowMajor or ColumnMajor
		Bytes						dstStride;						// only if dstLayout = RowMajor or ColumnMajor

		ECoopMatrixComponentType	srcType			= Default;
		ECoopMatrixComponentType	dstType			= Default;

		ECoopVecMatrixLayout		srcLayout		= Default;
		ECoopVecMatrixLayout		dstLayout		= Default;
	};
	
	struct ConvertCoopMatrixCmd2
	{
		Bytes						srcSize;
		Bytes						srcOffset;
		BufferID					srcBuffer;						// EResourceState::ShaderAddress_Read | CoopVecConvertStage

		Bytes						dstSize;
		Bytes						dstOffset;
		BufferID					dstBuffer;						// EResourceState::ShaderAddress_Write | CoopVecConvertStage

		uint						numRows			= 0;
		uint						numColumns		= 0;

		Bytes						srcStride;						// only if srcLayout = RowMajor or ColumnMajor
		Bytes						dstStride;						// only if dstLayout = RowMajor or ColumnMajor

		ECoopMatrixComponentType	srcType			= Default;
		ECoopMatrixComponentType	dstType			= Default;

		ECoopVecMatrixLayout		srcLayout		= Default;
		ECoopVecMatrixLayout		dstLayout		= Default;
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
		uint						m		= 0;
		uint						n		= 0;
		uint						k		= 0;

	// methods
		CoopMatrixConfig ()							__NE___ {}
		CoopMatrixConfig (const CoopMatrixConfig &)	__NE___ = default;
		CoopMatrixConfig (CoopMatrixConfig &&)		__NE___ = default;

		explicit CoopMatrixConfig (ECoopMatrixCfg)	__NE___;
		explicit operator ECoopMatrixCfg ()			C_NE___;
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
		CoopVectorConfig ()							__NE___ {}
		CoopVectorConfig (const CoopVectorConfig &)	__NE___ = default;
		CoopVectorConfig (CoopVectorConfig &&)		__NE___ = default;

		explicit CoopVectorConfig (ECoopVecCfg)		__NE___;
		explicit operator ECoopVecCfg ()			C_NE___;
	};

} // AE::Graphics
