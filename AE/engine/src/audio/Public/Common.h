// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Audio.pch.h"

namespace AE::Audio
{
	using namespace AE::Base;

	using KBitPerSec	= DefaultPhysicalQuantity< float >::KibiBitPerSecond;
	using Seconds		= DefaultPhysicalQuantity< float >::Second;
	using Freq			= DefaultPhysicalQuantity< uint >::Frequency;
	using Pos3D			= PhysicalQuantitySIMDVec3< DefaultPhysicalQuantity<float>::Meter >;
	using Vel3D			= PhysicalQuantitySIMDVec3< DefaultPhysicalQuantity<float>::MeterPerSecond >;


	enum class ESoundFlags : uint
	{
		Enable3D		= 1 << 0,
		Async			= 1 << 1,
		MultiChannel	= 1 << 2,
		Unknown			= 0,
	};
	AE_BIT_OPERATORS( ESoundFlags );


	enum class EAudioFormat : ubyte
	{
		RAW			= 0,		// PCM WAVE
		OGG,
		Unknown		= 0xFF
	};


	enum class EAudioQuality : ubyte
	{
		Highest,
		High,
		Medium,
		Low,
		Lowest,
		Unknown		= Medium,
	};


	enum class ESampleFormat : ubyte
	{
		UInt8,				// 8-bit integer format
		UInt16,				// 16-bit integer format
		Float32,			// 32-bit floating point format
		Unknown		= 0xFF,
	};


	struct AudioDataDesc
	{
		KBitPerSec		bitrate;
		Seconds			duration;
		Freq			freq;						// samples per second
		Bytes			size;
		ESoundFlags		flags			= Default;
		ubyte			channels		= 0;		// 1 - mono, 2 - stereo
		ESampleFormat	sampleFormat	= Default;	// represent bit depth
		EAudioFormat	audioFormat		= Default;

		ND_ bool  IsValid ()	C_NE___;
	};

	inline bool  AudioDataDesc::IsValid () C_NE___
	{
		return	bitrate.GetNonScaled() > 0					and
				(duration.GetNonScaled() > 0) == (size > 0)	and
				freq.GetNonScaled() > 0						and
				channels > 0								and
				sampleFormat != Default						and
				audioFormat != Default;
	}

} // AE::Audio
