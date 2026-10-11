// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/GraphicsRHI.h"
using namespace AE;
using namespace AE::Base;

using Graphics::EPixelFormat;
using Graphics::ImageMemView;
using Graphics::EImageAspect;

enum class ECompFormat
{
	BC1_RGB8_UNorm,
	BC1_RGB8_A1_UNorm,
//	BC2_RGBA8_UNorm,
	BC3_RGBA8_UNorm,
	BC4_R8_SNorm,
	BC4_R8_UNorm,
	BC5_RG8_SNorm,
	BC5_RG8_UNorm,
	BC6H_RGB16F,
	BC6H_RGB16UF,
	BC7_RGBA8_UNorm,
	BC7_RGB8_UNorm,		// mode 0-3 without alpha

	ETC2_RGB8_UNorm,
	ETC2_RGB8_A1_UNorm,
	ETC2_RGBA8_UNorm,	// alpha as EAC_R11_UNorm

	EAC_R11_SNorm,
	EAC_R11_UNorm,
	EAC_RG11_SNorm,
	EAC_RG11_UNorm,

	ASTC_RGBA8_4x4,
	ASTC_RGBA8_8x8,

	_Count
};

ND_ EPixelFormat  ToPixFmt (ECompFormat);


class Executor
{
// types
public:
	struct Images
	{
	public:
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

	struct PerMode
	{
		double	maxError	= 0.0;
		double	avgError	= 0.0;
		uint	blockCount	= 0;

		PerMode&	operator += (const PerMode &);
	};

	struct CmpResult : PerMode
	{
		Array<PerMode>	histogram;

		CmpResult&	operator += (const CmpResult &);
		ND_ String	ToString () const;
	};

private:
	using ImageID		= Graphics::ImageID;
	using ImageViewID	= Graphics::ImageViewID;
	using BufferID		= Graphics::BufferID;
	using BufferViewID	= Graphics::BufferViewID;

	using GetModeFn_t = int (*) (const RGBA32u &block);

	struct ASTCDecode
	{
		Strong<BufferID>		buf0;
		Strong<BufferID>		buf1;
		Strong<BufferID>		buf2;
		Strong<BufferID>		buf3;
		Strong<BufferID>		buf4;
		Strong<ImageID>			img5;

		Strong<BufferViewID>	view0;
		Strong<BufferViewID>	view1;
		Strong<BufferViewID>	view2;
		Strong<BufferViewID>	view3;
		Strong<BufferViewID>	view4;
		Strong<ImageViewID>		view5;

		ND_ bool  Load ();
			void  Release ();

	private:
		ND_ Graphics::RenderCoro  _Load ();
	};


// variables
private:
	Graphics::VDeviceInitializer						_vulkan;

	Graphics::RenderTechPipelinesPtr					_rtech;
	Graphics::GAutorelease<Graphics::PipelinePackID>	_packId;

	ASTCDecode											_astcDecode;

	static inline const uint2							_wgSize			{8};
	EPixelFormat										_intermPixFmt	= Default;


// methods
public:
	Executor () : _vulkan{True{}} {}

	ND_ bool  Initialize ();
		void  Deinitialize ();

	// decode to 'swDecompressedImg' and 'hwDecompressedImg'
	ND_ bool  Decode (const ImageMemView &compressedInput, INOUT Images &output, Bool requireHW = True{});

	// encode to 'outCompressedImg'
	ND_ bool  Encode (const ImageMemView &uncompressedInput, ECompFormat dstFmt, OUT Images &output);

	ND_ EPixelFormat  FindNonCompressed (EPixelFormat compFmt) const;

	ND_ CmpResult  Compare (const ImageMemView &in, const ImageMemView &ref,
							ECompFormat actualFmt, const ImageMemView *comp = null) const;

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
	ND_ bool  _SwEncode (const ImageMemView &uncompressedInput, ImageMemView &dstImgMem, ECompFormat compFmt);

	void  _CompileCS (StringView source, StringView pipeName,
					  Function<void (/*DescriptorSetLayout*/AnyTypeRef)> updateLayout)	__Th___;

	void  _CompileBCDecode (ECompFormat compFmt, StringView decodeFnName)				__Th___;
	void  _CompileBCEncode (ECompFormat compFmt, StringView encodeFnName)				__Th___;
	void  _CompileETCDecode (ECompFormat compFmt, StringView decodeFnName)				__Th___;
	void  _CompileETCEncode (ECompFormat compFmt, StringView encodeFnName)				__Th___;
	void  _CompileASTCDecode (ECompFormat compFmt, StringView decodeFnName)				__Th___;
	void  _CompileASTCEncode (ECompFormat compFmt, StringView encodeFnName)				__Th___;

	void  _CompileRTech (Function<void()> withCompiler)									__Th___;

private:
	void  _CompileDecodeAllBC ()		__Th___;
	void  _CompileDecodeAllETC ()		__Th___;
	void  _CompileDecodeAllASTC ()		__Th___;

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

	void  _CompileDecodeASTC ()			__Th___;
	void  _CompileHwDecodeASTC ()		__Th___;

private:
	void  _CompileEncodeAllBC ()		__Th___;
	void  _CompileEncodeAllETC ()		__Th___;
	void  _CompileEncodeAllASTC ()		__Th___;

	void  _CompileEncodeBC1 ()			__Th___;
	void  _CompileEncodeBC4 ()			__Th___;
	void  _CompileEncodeBC5 ()			__Th___;
	void  _CompileEncodeBC6 ()			__Th___;
	void  _CompileEncodeBC7 ()			__Th___;

	void  _CompileEncodeEAC_R11U ()		__Th___;
	void  _CompileEncodeETC_RGB8 ()		__Th___;

	void  _CompileEncodeASTC4x4 ()		__Th___;

private:
	static int  _GetModeETC2 (const RGBA32u &block);
	static int  _GetModeETC2A1 (const RGBA32u &block);
	static int  _GetModeETC2_EAC (const RGBA32u &block);

	static int  _GetModeASTC (const RGBA32u &block, uint2 blockSize);
	static int  _GetModeASTC4x4 (const RGBA32u &block);

	static int  _GetModeBC6 (const RGBA32u &block);
	static int  _GetModeBC7 (const RGBA32u &block);
};

ND_ Array<ubyte>  RandomByteArray (Bytes size, uint seed);

#include "../tests/shared/UnitTest_Shared.h"
