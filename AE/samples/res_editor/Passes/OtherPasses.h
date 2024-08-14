// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/Buffer.h"

namespace AE::ResEditor
{

	//
	// Present pass
	//

	class Present final : public IPass
	{
	// types
	private:
		struct VideoInfo
		{
			const float		frameRate	= 60.f;
			const float		frameTime	= 1.f / frameRate;
			secondsd		duration;
			uint			frameCount	= 0;
		};


	// variables
	private:
		const Array<RC<Image>>		_src;
		const RC<DynamicDim>		_dynSize;
		const RC<DynamicUInt>		_filterMode;

		VideoInfo					_videoInfo;
		AtomicRC<IVideoEncoder>		_videoEncoder;


	// methods
	public:
		explicit Present (Array<RC<Image>> src, StringView dbgName, RC<DynamicDim> dynSize, RC<DynamicUInt> filterMode) __NE___ :
			IPass{dbgName}, _src{RVRef(src)}, _dynSize{dynSize}, _filterMode{filterMode} {}

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Present; }
		AsyncTask	PresentAsync (const PresentPassData &)				__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}

	private:
		ND_ static RenderTaskCoro  _Blit (RC<Present> self, IOutputSurface &);

		static CoroTask  _SaveScreenshot (RC<ResLoader::IntermImage> image, EImageFormat fmt, bool);

		RC<IVideoEncoder>  _CreateEncoder (float bitrate, EVideoFormat, EVideoCodec, EVideoColorPreset) const;
	};



	//
	// Debug View
	//

	class DebugView final : public IPass
	{
	// types
	public:
		enum class EFlags : uint
		{
			Copy		= 0,
			NoCopy,
			Histogram,
			LinearDepth,
			Stencil,
			_Count
		};

	private:
		class IAdditionalPass
		{
		public:
			virtual ~IAdditionalPass () {}
			virtual bool  Execute (const Image &src, const Image &copy, SyncPassData &) C_Th___ = 0;
		};


		//
		// Histogram
		//
		class Histogram final : public IAdditionalPass
		{
		private:
			RenderTechPipelinesPtr	_rtech;

			ComputePipelineID		_ppln1;
			ComputePipelineID		_ppln2;
			GraphicsPipelineID		_ppln3;
			uint2					_ppln1LS;
			uint					_ppln2LS;

			DescSetBinding			_ppln1DSIdx;
			PerFrameDescSet_t		_ppln1DS;		// 1 & 2

			DescSetBinding			_ppln3DSIdx;
			PerFrameDescSet_t		_ppln3DS;

			Strong<BufferID>		_ssb;

		public:
			Histogram (Renderer* renderer, const Image &src, const Image &copy)		__Th___;
			~Histogram ();

			bool  Execute (const Image &src, const Image &copy, SyncPassData &)		C_Th_OV;
		};


		//
		// Linear Depth
		//
		class LinearDepth final : public IAdditionalPass
		{
		private:
			RenderTechPipelinesPtr	_rtech;

			GraphicsPipelineID		_ppln;
			PushConstantIndex		_pcIdx;
			DescSetBinding			_pplnDSIdx;
			PerFrameDescSet_t		_pplnDS;

		public:
			LinearDepth (const Image &src, const Image &copy)						__Th___;
			~LinearDepth ();

			bool  Execute (const Image &src, const Image &copy, SyncPassData &)		C_Th_OV;
		};


		//
		// Stencil View
		//
		class StencilView final : public IAdditionalPass
		{
		private:
			RenderTechPipelinesPtr	_rtech;

			GraphicsPipelineID		_ppln;
			DescSetBinding			_pplnDSIdx;
			PerFrameDescSet_t		_pplnDS;

		public:
			StencilView (const Image &src, const Image &copy)						__Th___;
			~StencilView ();

			bool  Execute (const Image &src, const Image &copy, SyncPassData &)		C_Th_OV;
		};


	// variables
	private:
		RC<Image>				_src;
		RC<Image>				_copy;		// make a copy if '_src' will be changed
		RC<Image>				_view;		// view of '_src' or '_copy' which will be displayed

		const uint				_index;
		const EFlags			_flags;

		Unique<IAdditionalPass>	_pass;


	// methods
	public:
		explicit DebugView (RC<Image> src, uint idx, EFlags flags,
							ImageLayer layer, MipmapLevel mipmap,
							Renderer* renderer, StringView dbgName)			__Th___;
		~DebugView ();

		ND_ RC<Image>	GetImage ()											const	{ return _copy; }

	// IPass //
		EPassType		GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool			Execute (SyncPassData &)							__Th_OV;
		void			GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}

	private:
			void  _InitHistogram ()											__Th___;

		ND_ bool  _CopyImage (SyncPassData &pd)								const;
		ND_ bool  _CalcHistogram (SyncPassData &pd)							const;
	};



	//
	// Generate Mipmaps pass
	//

	class GenerateMipmapsPass final : public IPass
	{
	// variables
	private:
		RC<Image>	_image;


	// methods
	public:
		explicit GenerateMipmapsPass (RC<Image> image, StringView dbgName) __NE___ :
			IPass{dbgName}, _image{RVRef(image)} {}

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool		Execute (SyncPassData &)							__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}
	};



	//
	// Copy Image pass
	//

	class CopyImagePass final : public IPass
	{
	// variables
	private:
		RC<Image>		_srcImage;
		RC<Image>		_dstImage;
		uint3			_dim;
		EImageAspect	_aspect;


	// methods
	public:
		explicit CopyImagePass (RC<Image> src, RC<Image> dst, StringView dbgName) __Th___;

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool		Execute (SyncPassData &)							__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}
	};



	//
	// Clear Image pass
	//

	class ClearImagePass final : public IPass
	{
	// types
	public:
		using ClearValue_t	= Union< RGBA32f, RGBA32u, RGBA32i >;


	// variables
	private:
		RC<Image>			_image;
		const ClearValue_t	_value;


	// methods
	public:
		explicit ClearImagePass (RC<Image> image, ClearValue_t value, StringView dbgName) __NE___ :
			IPass{dbgName}, _image{RVRef(image)}, _value{value} {}

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool		Execute (SyncPassData &)							__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}
	};



	//
	// Clear Buffer pass
	//

	class ClearBufferPass final : public IPass
	{
	// variables
	private:
		RC<Buffer>		_buffer;
		const Bytes		_offset;
		const Bytes		_size;
		const uint		_value;


	// methods
	public:
		explicit ClearBufferPass (RC<Buffer> buffer, Bytes offset, Bytes size, uint value, StringView dbgName) __NE___ :
			IPass{dbgName}, _buffer{RVRef(buffer)}, _offset{offset}, _size{size}, _value{value} {}

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool		Execute (SyncPassData &)							__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV	{}
	};


} // AE::ResEditor
