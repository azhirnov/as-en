// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
namespace
{
	using Audio::ESampleFormat;
	using Audio::KBitPerSec;
	using FBytes = DefaultPhysicalQuantity< float >::Byte;

	static constexpr uint	c_RiffChunk			= 0x52494646;	// "RIFF"
	static constexpr uint	c_WaveChunk			= 0x57415645;	// "WAVE"
	static constexpr uint	c_FmtChunk			= 0x666d7420;	// "fmt "
	static constexpr uint	c_DataChunk			= 0x64617461;	// "data"
	static constexpr uint	c_PcmSubchunkSize	= 16;			// bytes
	static constexpr uint	c_PcmAudioFormat	= 1;


	struct WavePcmHeader
	{
		uint	chunkID			= c_RiffChunk;
		uint	chunkSize		= 0;
		uint	format			= c_WaveChunk;

		// format chunk
		uint	subchunk1ID		= c_FmtChunk;
		uint	subchunk1Size	= c_PcmSubchunkSize;
		uint	audioFormat		: 16;
		uint	numChannels		: 16;	// 1 - mono, 2 - stereo
		uint	sampleRate		= 0;
		uint	byteRate		= 0;
		uint	blockAlign		: 16;
		uint	bitsPerSample	: 16;

		// data chunk
		uint	subchunk2ID		= c_DataChunk;
		uint	subchunk2Size	= 0;

		WavePcmHeader () :
			audioFormat{c_PcmSubchunkSize}, numChannels{0},
			blockAlign{0}, bitsPerSample{0}
		{}
	};
	StaticAssert( sizeof(WavePcmHeader) == 44 );


} // namespace
} // AE::ResLoader
