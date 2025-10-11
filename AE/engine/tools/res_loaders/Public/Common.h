// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Base.h"

#include "audio/Public/Common.h"

#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/ImageUtils.h"
#include "graphics_rhi/Public/ImageMemView.h"
#include "graphics_rhi/Public/MipmapLevel.h"
#include "graphics_rhi/Public/ImageLayer.h"
#include "graphics_rhi/Public/ImageDesc.h"
#include "graphics_rhi/Public/SamplerDesc.h"
#include "graphics_rhi/Public/RenderStateEnums.h"
#include "graphics_rhi/Public/VertexEnums.h"
#include "graphics_rhi/Public/VertexDesc.h"

#include "graphics_rhi/Private/EnumUtils.h"

namespace AE::ResLoader
{
	using namespace AE::Base;


	//
	// Image Format
	//
	enum class EImageFormat : uint
	{
		Unknown			= 0,
		DDS,			// .dds
		BMP,			// .bmp
		JPG,			// .jpg .jpe .jpeg
		PCX,			// .pcx
		PNG,			// .png
		TGA,			// .tga
		TIF,			// .tif .tiff
		PSD,			// .psd
		RadianceHDR,	// .hdr
		OpenEXR,		// .exr
		KTX,			// .ktx
		AEImg,			// .aeimg
		_Count
	};

	ND_ EImageFormat	PathToImageFileFormat (const Path &path)	__NE___;
	ND_ StringView		ImageFileFormatToExt (EImageFormat)			__NE___;



	//
	// Model Format
	//
	enum class EModelFormat : uint
	{
		Unknown		= 0,
		GLTF,		// .gltf
		CSV,		// .csv - export from RenderDoc
		_Count
	};

	ND_ EModelFormat	PathToModelFormat (const Path &path)		__NE___;
	ND_ StringView		ModelFormatToExt (EModelFormat)				__NE___;


	//
	// Audio Format
	//
	using AE::Audio::EAudioFormat;

	ND_ EAudioFormat	PathToSoundFileFormat (const Path &path)	__NE___;
	ND_ StringView		SoundFormatToExt (EAudioFormat)				__NE___;

	using AE::Audio::AudioDataDesc;


} // AE::ResLoader
