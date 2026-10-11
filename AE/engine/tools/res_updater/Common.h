// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/Base.h"
#include "graphics_rhi/Public/ResourceEnums.h"

using namespace AE;
using namespace AE::Base;
using namespace AE::Graphics;

struct ConvertImageConfig
{
	bool			genMips			= false;
	EPixelFormat	intermFormat	= Default;
	EPixelFormat	compFormat		= Default;
	uint			baseMip			= 0;
};

struct ImageDesc2
{
	EImage			type			= Default;
	EPixelFormat	format			= Default;
//	EImageAspect	aspect			= EImageAspect::Color;
	uint3			dim;
	uint			arrayLayers		= 1;
	uint			mipmaps			= 1;
	bool			perLayerLayout	= true;		// if true: iterate each array layer{ each mip{ ... } }, otherwise: each mip{ each array layer{ ... } }
	Bytes			rowPitch;
	Bytes			slicePitch;
};


extern bool  ConvertAEImage (RDataSource &src, WDataSource &dst);
extern bool  ConvertAnyImage (const Path &src, const Path &dst, const ConvertImageConfig &cfg = Default);
extern bool  ConvertImagesInFolder (const Path &srcFolder, const Path &dstFolder, StringView dstExt, const ConvertImageConfig &cfg = Default);
extern bool  ConvertBinToImage (const Path &src, const Path &dst, const ImageDesc2 &desc);
