// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/GraphicsRHI.h"
using namespace AE;
using namespace AE::Base;

using Graphics::EPixelFormat;
using Graphics::ImageMemView;
using Graphics::EImageAspect;


class Executor
{
// types
public:
	struct Images
	{
	public:
	//	ImageMemView	inputImg;
		ImageMemView	outCompressedImg;
		ImageMemView	swDecompressedImg;
		ImageMemView	hwDecompressedImg;
	private:
		RC<IAllocator>	_allocator;

	public:
		Images () {}
		~Images () {}	// allocator will free memory

		ND_ bool  Allocate (EPixelFormat compFmt, EPixelFormat decompFmt, uint2 dim);
		ND_ bool  IsEmpty () const	{ return _allocator == null; }
	};

	struct CmpResult
	{
		double	maxError	= 0.0;
		double	avgError	= 0.0;

		CmpResult&	operator += (const CmpResult &);
		ND_ String	ToString () const;
	};

private:
	using ImageID		= Graphics::ImageID;
	using ImageViewID	= Graphics::ImageViewID;


// variables
private:
	Graphics::VDeviceInitializer						_vulkan;

	Graphics::RenderTechPipelinesPtr					_rtech;
	Graphics::GAutorelease<Graphics::PipelinePackID>	_packId;

	const uint2											_wgSize			{8};
	EPixelFormat										_intermPixFmt	= Default;


// methods
public:
	Executor () : _vulkan{True{}} {}

	ND_ bool  Initialize ();
		void  Deinitialize ();

	// decode to 'swDecompressedImg' and 'hwDecompressedImg'
	ND_ bool  Decode (const ImageMemView &compressedInput, INOUT Images &output, Bool requireHW = True{});

	// encode to 'outCompressedImg'
	ND_ bool  Encode (const ImageMemView &uncompressedInput, EPixelFormat dstFmt, OUT Images &output);

	ND_ EPixelFormat  FindNonCompressed (EPixelFormat compFmt) const;

	ND_ CmpResult  Compare (const ImageMemView &in, const ImageMemView &ref, EPixelFormat actualFmt) const;

	ND_ bool  SupportsBC () const;
	ND_ bool  SupportsETC () const;
	ND_ bool  SupportsASTC () const;
	ND_ bool  SupportsASTC_HDR () const;

	ND_ EPixelFormat				GetIntermediateFormat ()	const { return _intermPixFmt; }
	ND_ Graphics::VDevice const&	GetDevice()					const { return _vulkan; }


private:
	ND_ bool  _DetectIntermFormat ();

	ND_ bool  _SwDecode (const ImageMemView &compressedInput, ImageMemView &dstImgMem);
	ND_ bool  _HwDecode (const ImageMemView &compressedInput, ImageMemView &dstImgMem);
	ND_ bool  _SwEncode (const ImageMemView &uncompressedInput, ImageMemView &dstImgMem);

	void  _CompileCS (StringView source, StringView pipeName,
					  Function<void (/*DescriptorSetLayout*/AnyTypeRef)> updateLayout)	__Th___;

	void  _CompileBCDecode (EPixelFormat compFmt, StringView decodeFnName)				__Th___;
	void  _CompileETCDecode (EPixelFormat compFmt, StringView decodeFnName)				__Th___;
	void  _CompileETCEncode (EPixelFormat compFmt, StringView decodeFnName)				__Th___;

	void  _CompileRTech (Function<void()> withCompiler)									__Th___;

private:
	void  _CompileDecodeAllBC ()		__Th___;
	void  _CompileEncodeAllBC ()		__Th___;

	void  _CompileDecodeBC1 ()			__Th___;
	void  _CompileDecodeBC4S ()			__Th___;
	void  _CompileDecodeBC4U ()			__Th___;
	void  _CompileDecodeBC5S ()			__Th___;
	void  _CompileDecodeBC5U ()			__Th___;
	void  _CompileDecodeBC6S ()			__Th___;
	void  _CompileDecodeBC6U ()			__Th___;
	void  _CompileDecodeBC7 ()			__Th___;
	void  _CompileHwDecodeBC ()			__Th___;

	void  _CompileDecodeETC_RGB8 ()		__Th___;
	void  _CompileDecodeETC_RGB8A1 ()	__Th___;
	void  _CompileDecodeETC_RGBA8 ()	__Th___;
	void  _CompileDecodeEAC_R11U ()		__Th___;
	void  _CompileDecodeEAC_R11S ()		__Th___;
	void  _CompileDecodeEAC_RG11U ()	__Th___;
	void  _CompileDecodeEAC_RG11S ()	__Th___;
	void  _CompileHwDecodeETC ()		__Th___;

private:
	void  _CompileDecodeAllETC ()		__Th___;
	void  _CompileEncodeAllETC ()		__Th___;

	void  _CompileEncodeBC1 ()			__Th___;
	void  _CompileEncodeBC4 ()			__Th___;
	void  _CompileEncodeBC5 ()			__Th___;
	void  _CompileEncodeBC6 ()			__Th___;
	void  _CompileEncodeBC7 ()			__Th___;

	void  _CompileEncodeEAC_R11U ()		__Th___;
	void  _CompileEncodeETC_RGB8 ()		__Th___;
};

ND_ Array<ubyte>  RandomByteArray (Bytes size, uint seed);
ND_ Array<float>  RandomFloatArray (usize count);

#include "../tests/shared/UnitTest_Shared.h"
