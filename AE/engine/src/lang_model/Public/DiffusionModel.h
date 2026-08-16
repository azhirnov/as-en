// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread safe: no
*/

#pragma once

#include "lang_model/Public/Types.h"

namespace AE::LangModel
{

	//
	// Image Sequence
	//
	class IImageSequence : public EnableRC<IImageSequence>
	{
	// interface
	public:
		ND_ virtual bool  GetImage (uint index, OUT ImageRef &)		C_NE___ = 0;
		ND_ virtual bool  GetImage (uint index, OUT ImageStorage &)	C_NE___ = 0;

		struct Description
		{
			uint2			dim;
			uint			count		= 0;
			EPixelFormat	format		= Default;
			Bytes			rowPitch;
			Bytes			size;
		};
		ND_ virtual Description  GetDescription ()					C_NE___ = 0;
	};



	//
	// Diffusion Model interface
	//
	class IDiffusionModel : public EnableRC<IDiffusionModel>
	{
	// interface
	public:
		ND_ virtual RC<IImageSequence>  Generate (const ImageGenParams &)				__NE___	= 0;

		ND_ virtual RC<IImageSequence>  Generate (const VideoGenParams &)				__NE___	= 0;

		ND_ virtual ModelInfo  GetModelInfo ()											__NE___ = 0;

		ND_ virtual EImplementation  GetImplementationType ()							C_NE___ = 0;
	};



	//
	// Upscaler Context
	//
	class IUpscalerContext : public EnableRC<IUpscalerContext>
	{
	// interface
	public:
	};


} // AE::LangModel
