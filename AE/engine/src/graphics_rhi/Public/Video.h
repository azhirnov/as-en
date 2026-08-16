// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/VideoEnums.h"
#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/Queue.h"
#include "graphics_rhi/Public/ImageLayer.h"
#include "graphics_rhi/Public/SamplerDesc.h"

namespace AE::Graphics
{

	//
	// Video Profile
	//
	struct VideoProfile
	{
	// types

		// [EVideoCodecMode]_[EVideoCodec]

		struct Decode_H264
		{
			EStdVideoH264ProfileIdc			stdProfileIdc	= Default;
			EVideoDecodeH264PictureLayout	pictureLayout	= Default;
		};

		struct Encode_H264
		{
			EStdVideoH264ProfileIdc			stdProfileIdc	= Default;
		};

		struct Decode_H265
		{
			EStdVideoH265ProfileIdc			stdProfileIdc	= Default;
		};

		struct Encode_H265
		{
			EStdVideoH265ProfileIdc			stdProfileIdc	= Default;
		};

		struct Decode_AV1
		{
		};

		struct Encode_AV1
		{
		};

		struct Decode_VP9
		{
		};

		using Specialization_t	= Union< NullUnion,
										 Decode_H264, Encode_H264,
										 Decode_H265, Encode_H265,
										 Decode_AV1,  Encode_AV1,
										 Decode_VP9
										>;

	// variables
		EVideoCodecMode				mode				= Default;
		EVideoCodec					codec				= Default;
		EVideoChromaSubsampling		chromaSubsampling	= Default;		// Vulkan: bitset, Android - ?, MacOS - ?
		EVideoComponentBitDepth		lumaBitDepth		= Default;		// Vulkan: bitset, Android - ?, MacOS - ?
		EVideoComponentBitDepth		chromaBitDepth		= Default;		// Vulkan: bitset, Android - ?, MacOS - ?
		Specialization_t			spec;


	// methods
		VideoProfile ()				__NE___	{}

		ND_ bool	IsDefined ()	C_NE___;
	};

	using VideoProfileList = FixedArray< VideoProfile, 4 >;



	//
	// Video Session description
	//
	struct VideoSessionDesc
	{
		uint2					maxCodedExtent				{~0u};			// 0 - min, UMax - max
		EQueueType				queue						= Default;		// VideoEncode or VideoDecode
		EPixelFormat			pictureFormat				= Default;
		VideoProfile			profile;
		EMemoryType				memType						= EMemoryType::DeviceLocal;

		// DPB
		EPixelFormat			referencePictureFormat		= Default;
		uint					maxDpbSlots					= 0;
		uint					maxActiveReferencePictures	= 0;


		VideoSessionDesc ()		__NE___ {}
	};



	//
	// Video Buffer description
	//
	struct VideoBufferDesc
	{
	// variables
		Bytes					size;
		EBufferUsage			usage			= Default;
		EBufferOpt				options			= Default;
		EVideoBufferUsage		videoUsage		= Default;
		EMemoryType				memType			= EMemoryType::DeviceLocal;
		EQueueMask				queues			= Default;
		VideoProfileList		profiles;


	// methods
		VideoBufferDesc ()									__NE___	{}

		// Will remove unsupported combinations
		void  Validate ()									__NE___;

		ND_ bool  IsExclusiveSharing ()						C_NE___	{ return queues == Default; }
	};



	//
	// Video Image description
	//
	struct VideoImageDesc
	{
	// variables
		VideoImageDim_t			dimension;					// 0 - min, UMax - max
		ImageLayer				arrayLayers		= 1_layer;
		EImageUsage				usage			= Default;
		EPixelFormat			format			= Default;	// keep default to auto-detect
		EVideoImageUsage		videoUsage		= Default;
		EMemoryType				memType			= EMemoryType::DeviceLocal;
		EQueueMask				queues			= Default;
		EImageOpt				options			= Default;
		SamplerName				ycbcrConversion;
		PipelinePackID			ycbcrConvPack;				// pipeline pack where 'ycbcrConversion' sampler is stored
		VideoProfileList		profiles;


	// methods
		VideoImageDesc ()												__NE___	{}

		// Will remove unsupported combinations
		void  Validate ()												__NE___;

		ND_ bool		IsExclusiveSharing ()							C_NE___	{ return queues == Default; }
		ND_ uint3		Dimension ()									C_NE___	{ return uint3{ dimension, 1u }; }
		ND_ uint2		Dimension2 ()									C_NE___	{ return uint2{ dimension }; }

		VideoImageDesc&  SetDimension (const uint2 &value)				__NE___	{ dimension = CheckCast{value};			return *this; }
		VideoImageDesc&  SetDimension (uint w, uint h)					__NE___	{ return SetDimension( uint2{w,h} ); }
		VideoImageDesc&  SetOptions (EImageOpt value)					__NE___	{ options		= value;				return *this; }
		VideoImageDesc&  SetUsage (EImageUsage v1, EVideoImageUsage v2)	__NE___	{ usage = v1;	videoUsage = v2;		return *this; }
		VideoImageDesc&  SetUsage (EImageUsage value)					__NE___	{ usage			= value;				return *this; }
		VideoImageDesc&  SetVideoUsage (EVideoImageUsage value)			__NE___	{ videoUsage	= value;				return *this; }
		VideoImageDesc&  SetFormat (EPixelFormat value)					__NE___	{ format		= value;				return *this; }
		VideoImageDesc&  SetArrayLayers (uint value)					__NE___	{ arrayLayers	= ImageLayer{value};	return *this; }
		VideoImageDesc&  SetQueues (EQueueMask value)					__NE___	{ queues		= value;				return *this; }
		VideoImageDesc&  SetMemory (EMemoryType value)					__NE___	{ memType		= value;				return *this; }
		VideoImageDesc&  SetYcbcrConversion (SamplerName::Ref value)	__NE___	{ ycbcrConversion = value;				return *this; }
	};


} // AE::Graphics
