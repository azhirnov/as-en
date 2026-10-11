// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

#include "res_pack/asset_packer/Packer/ImagePacker.h"
#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

static const auto	c_Timeout = seconds{5*60};

/*
=================================================
	ToPixFmt
=================================================
*/
EPixelFormat  ToPixFmt (ECompFormat src)
{
	switch_enum( src )
	{
		#define CASE( _name_ )	case ECompFormat::_name_ :	return EPixelFormat::_name_;

		CASE( BC1_RGB8_UNorm )
		CASE( BC1_RGB8_A1_UNorm )
		CASE( BC3_RGBA8_UNorm )
		CASE( BC4_R8_SNorm )
		CASE( BC4_R8_UNorm )
		CASE( BC5_RG8_SNorm )
		CASE( BC5_RG8_UNorm )
		CASE( BC6H_RGB16F )
		CASE( BC6H_RGB16UF )
		CASE( BC7_RGBA8_UNorm )

		CASE( ETC2_RGB8_UNorm )
		CASE( ETC2_RGB8_A1_UNorm )
		CASE( ETC2_RGBA8_UNorm )

		CASE( EAC_R11_SNorm )
		CASE( EAC_R11_UNorm )
		CASE( EAC_RG11_SNorm )
		CASE( EAC_RG11_UNorm )

		CASE( ASTC_RGBA8_4x4 )
		CASE( ASTC_RGBA8_8x8 )

		#undef CASE

		case ECompFormat::_Count :			break;
		case ECompFormat::BC7_RGB8_UNorm :	return EPixelFormat::BC7_RGBA8_UNorm;
	}
	switch_end
	return Default;
}

/*
=================================================
	FixChannelCount
=================================================
*/
ND_ static uint  FixChannelCount (ECompFormat fmt, uint cnt)
{
	switch ( fmt )
	{
		case ECompFormat::BC7_RGB8_UNorm :	return 3;
	}
	return cnt;
}

/*
=================================================
	Allocate
=================================================
*/
bool  Executor::Images::Allocate (EPixelFormat compFmt, EPixelFormat decompFmt, const uint2 dim)
{
	using namespace Graphics;

	outCompressedImg	= Default;
	swDecompressedImg	= Default;
	hwDecompressedImg	= Default;
	_allocator.reset();

	auto&	fmt1_info	= EPixelFormat_GetInfo( compFmt );
	auto&	fmt2_info	= EPixelFormat_GetInfo( decompFmt );

	CHECK_ERR( fmt1_info.IsCompressed() );
	CHECK_ERR( not fmt2_info.IsCompressed() );

	CHECK_ERR( All( IsMultipleOf( dim, fmt1_info.TexBlockDim() )));
	CHECK_ERR( All( IsMultipleOf( dim, fmt2_info.TexBlockDim() )));

	Bytes	comp_size	= ImageUtils::ImageSize( uint3{dim,1}, fmt1_info.bitsPerBlock, fmt1_info.TexBlockDim() );
	Bytes	decomp_size	= ImageUtils::ImageSize( uint3{dim,1}, fmt2_info.bitsPerBlock, fmt2_info.TexBlockDim() );

	{
		auto*	alloc = new LinearAllocator<>{};
		_allocator.reset( alloc );
		alloc->SetBlockSize( comp_size + decomp_size * 2 + 128_b );
	}

	{
		void*	data = _allocator->Allocate(SizeAndAlign{ comp_size, 1_b });
		CHECK_ERR( data != null );
		outCompressedImg = ImageMemView{ data, comp_size, uint3{}, uint3{dim,1}, 0_b, 0_b, compFmt, EImageAspect::Color };
	}{
		void*	data = _allocator->Allocate(SizeAndAlign{ decomp_size, 1_b });
		CHECK_ERR( data != null );
		swDecompressedImg = ImageMemView{ data, decomp_size, uint3{}, uint3{dim,1}, 0_b, 0_b, decompFmt, EImageAspect::Color };
	}{
		void*	data = _allocator->Allocate(SizeAndAlign{ decomp_size, 1_b });
		CHECK_ERR( data != null );
		hwDecompressedImg = ImageMemView{ data, decomp_size, uint3{}, uint3{dim,1}, 0_b, 0_b, decompFmt, EImageAspect::Color };
	}
	return true;
}

/*
=================================================
	PerMode::operator +=
=================================================
*/
Executor::PerMode&  Executor::PerMode::operator += (const PerMode &rhs)
{
	this->maxError	= Max( this->maxError, rhs.maxError );
	this->avgError	= (this->avgError + rhs.avgError) * 0.5;
	this->blockCount += rhs.blockCount;
	return *this;
}

/*
=================================================
	CmpResult::operator +=
=================================================
*/
Executor::CmpResult&  Executor::CmpResult::operator += (const CmpResult &rhs)
{
	PerMode::operator+=( rhs );

	if ( this->histogram.empty() and rhs.histogram.empty() )
		return *this;

	histogram.resize( Max( histogram.size(), rhs.histogram.size() ));

	for (usize i = 0; i < histogram.size(); ++i)
	{
		if ( i >= rhs.histogram.size() )
			break;

		histogram[i] += rhs.histogram[i];
	}
	return *this;
}

/*
=================================================
	CmpResult::ToString
=================================================
*/
String  Executor::CmpResult::ToString () const
{
	String	str;
	str	<< "max err: "s << Base::ToString( maxError * 100.0, 2, Bool{maxError < 0.01} )
		<< "%, avg: " << Base::ToString( avgError * 100.0, 2, Bool{avgError < 0.01} )
		<< "%, blocks: " << Base::ToString( blockCount );

	if ( not histogram.empty() )
	{
		str << "\n| mode | max err % | avr % | blocks |";
		for (auto [his, i] : WithIndex( histogram ))
		{
			if ( his.blockCount == 0 )
				continue;

			str << "\n| " << Base::ToString( i )
				<< " | " << Base::ToString( his.maxError * 100.0, 2, Bool{maxError < 0.01} )
				<< " | " << Base::ToString( his.avgError * 100.0, 2, Bool{maxError < 0.01} )
				<< " | " << Base::ToString( his.blockCount ) << " |";
		}
	}
	return str;
}
//-----------------------------------------------------------------------------


namespace {
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
}

/*
=================================================
	ASTCDecode::_Load
=================================================
*/
Graphics::RenderCoro  Executor::ASTCDecode::_Load ()
{
	using namespace AE::Graphics;

	DirectCtx::Transfer  ctx {RenderCoro_Get()};

	auto&	res_mngr = GraphicsScheduler().GetResourceManager();

	const auto	LoadBuffer = [&res_mngr, &ctx] (const Path &path, EPixelFormat fmt, Strong<BufferID> &buf, Strong<BufferViewID> &view) -> bool
	{{
		CHECK_ERR( not buf and not view );

		Array<ubyte>	data;
		{
			FileRStream		file {path};
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Read( file.RemainingSize(), OUT data ));
		}

		buf = res_mngr.CreateBuffer( BufferDesc{ ArraySizeOf(data), EBufferUsage::Transfer | EBufferUsage::UniformTexel }, ToString(path.stem()) );
		CHECK_ERR( buf );

		view = res_mngr.CreateBufferView( BufferViewDesc{ fmt }, buf );
		CHECK_ERR( view );

		CHECK_ERR( ctx.UploadBuffer( buf, 0_b, ArraySizeOf(data), data.data(), EStagingHeapType::Dynamic ));
		return true;
	}};

	CHECK_CE( LoadBuffer( Path{AE_ASTC_TABLES} / "LUTRemainingBitsToEndpointQuantizer.bin", EPixelFormat::RGBA16U,	OUT buf0, OUT view0 ));
	CHECK_CE( LoadBuffer( Path{AE_ASTC_TABLES} / "LUTEndpointUnquantize.bin",				EPixelFormat::R8U,		OUT buf1, OUT view1 ));
	CHECK_CE( LoadBuffer( Path{AE_ASTC_TABLES} / "LUTWeightQuantizer.bin",					EPixelFormat::RGBA8U,	OUT buf2, OUT view2 ));
	CHECK_CE( LoadBuffer( Path{AE_ASTC_TABLES} / "LUTWeightUnquantize.bin",					EPixelFormat::R8U,		OUT buf3, OUT view3 ));
	CHECK_CE( LoadBuffer( Path{AE_ASTC_TABLES} / "LUTTritQuintDecode.bin",					EPixelFormat::R16U,		OUT buf4, OUT view4 ));

	// load image
	{
		FileRStream		file { Path{AE_ASTC_TABLES} / "LUTPartitionTable_128x128_4x4.aeimg" };
		CHECK_CE( file.IsOpen() );

		AssetPacker::ImagePacker::FileHeader	file_hdr;
		CHECK_CE( ImagePacker_ReadHeader( file, OUT file_hdr ));
		CHECK_CE( not file_hdr.fileName.IsDefined() );

		auto&	img_header	= file_hdr.imageHeader;

		img5 = res_mngr.CreateImage( img_header.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default );
		CHECK_CE( img5 );

		view5 = res_mngr.CreateImageView( ImageViewDesc{img_header.viewType}, img5 );
		CHECK_CE( view5 );

		CHECK_CE( img_header.arrayLayers == 1 );
		CHECK_CE( img_header.mipmaps == 1 );

		ctx.AccumBarriers()
			.ResourceBarrier( img5, EResourceState::Invalidate, EResourceState::CopyDst );

		// upload
		{
			UploadImageDesc	upload;
			upload.imageDim	= img_header.dimension;

			ImageMemView	dst_mem;
			ctx.UploadImage( img5, upload, OUT dst_mem );

			Array<ubyte>	data;
			CHECK_CE( file.Read( file.RemainingSize(), OUT data ));

			auto&	fmt_info	= EPixelFormat_GetInfo( img_header.format );
			Bytes	row_pitch	= AlignUp( ImageUtils::RowSize( img_header.dimension.x, fmt_info.bitsPerBlock, uint2{1} ), img_header.rowAlignPOT );
			Bytes	slice_pitch	= ImageUtils::SliceSize( img_header.dimension.y, row_pitch, uint2{1} );

			ImageMemView	src_mem{ data.data(), ArraySizeOf(data), uint3{}, uint3{img_header.dimension}, row_pitch, slice_pitch, img_header.format, EImageAspect::Color };
			CHECK_CE( dst_mem.CopyFrom( src_mem ));
		}

		ctx.AccumBarriers()
			.ResourceBarrier( img5, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::AllShaderStages );
	}

	RenderCoro_Execute( ctx );
	co_return;
}

/*
=================================================
	ASTCDecode::Load
=================================================
*/
bool  Executor::ASTCDecode::Load ()
{
	using namespace AE::Threading;
	using namespace AE::Graphics;

	auto&	rts	= GraphicsScheduler();

	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame, EThread::Background };
	CHECK_ERR( rts.WaitNextFrame( threads, c_Timeout ));
	CHECK_ERR( rts.BeginFrame() );

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run( _Load(), Tuple{}, True{"Last"}, {"load ASTC decode tables"} );

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, c_Timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, c_Timeout ));

	return true;
}

/*
=================================================
	ASTCDecode::Release
=================================================
*/
void  Executor::ASTCDecode::Release ()
{
	auto&	res_mngr = GraphicsScheduler().GetResourceManager();

	res_mngr.ReleaseResources( buf0, view0 );
	res_mngr.ReleaseResources( buf1, view1 );
	res_mngr.ReleaseResources( buf2, view2 );
	res_mngr.ReleaseResources( buf3, view3 );
	res_mngr.ReleaseResources( buf4, view4 );
	res_mngr.ReleaseResources( img5, view5 );
}
//-----------------------------------------------------------------------------



/*
=================================================
	Initialize
=================================================
*/
bool  Executor::Initialize ()
{
	using namespace Threading;
	using namespace Graphics;

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	VDeviceInitializer::InstanceCreateInfo	inst_ci;
	inst_ci.appName			= "BlockCompressionTest";
	inst_ci.instanceLayers	= _vulkan.GetRecommendedInstanceLayers();

	CHECK_ERR( _vulkan.CreateInstance( inst_ci ));

	_vulkan.CreateDebugCallback( VDeviceInitializer::c_DefaultDebugMessageSeverity,
								 VDeviceInitializer::c_DefaultDebugMessageTypes,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOGW(rep.message);  CHECK(not rep.isError); });

	CHECK_ERR( _vulkan.ChooseHighPerformanceDevice() );
	CHECK_ERR( _vulkan.CreateDefaultQueue() );
	CHECK_ERR( _vulkan.CreateLogicalDevice() );

	CHECK_ERR( _vulkan.IsInitialized() );
	CHECK_ERR( _vulkan.CheckConstantLimits() );
	CHECK_ERR( _vulkan.CheckExtensions() );

	RenderTaskScheduler::InstanceCtor::Create( _vulkan );
	CHECK_ERR( GraphicsScheduler().Initialize( Default ));

	CHECK_ERR( _DetectIntermFormat() );

	CHECK_ERR( _astcDecode.Load() );

	try {
		_CompileRTech( [this]()
			{
				_CompileDecodeAllBC();
				_CompileEncodeAllBC();

				_CompileDecodeAllETC();
				_CompileEncodeAllETC();

				_CompileDecodeAllASTC();
				_CompileEncodeAllASTC();
			});
	}
	catch(...)
	{
		PipelineCompiler::ObjectStorage::SetInstance( null );
		return false;
	}
	return true;
}

/*
=================================================
	Deinitialize
=================================================
*/
void  Executor::Deinitialize ()
{
	using namespace Threading;
	using namespace Graphics;

	_astcDecode.Release();

	_rtech  = null;
	_packId = Default;

	RenderTaskScheduler::InstanceCtor::Destroy();

	CHECK_ERRV( _vulkan.DestroyLogicalDevice() );
	CHECK_ERRV( _vulkan.DestroyInstance() );
	CHECK_ERRV( not _vulkan.IsInitialized() );

	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();
}

/*
=================================================
	_DetectIntermFormat
=================================================
*/
bool  Executor::_DetectIntermFormat ()
{
	using namespace Graphics;

	auto&	res_mngr = GraphicsScheduler().GetResourceManager();

	const EPixelFormat	formats[] = { EPixelFormat::RGBA32F, EPixelFormat::RGBA16F };

	ImageDesc	desc;
	desc.SetDimension( 32, 32 );
	desc.SetUsage( EImageUsage::Transfer | EImageUsage::Storage );

	for (auto fmt : formats)
	{
		desc.SetFormat( fmt );
		if ( res_mngr.IsSupported( desc ))
		{
			_intermPixFmt = fmt;
			return true;
		}
	}
	return false;
}

/*
=================================================
	Decode
=================================================
*/
bool  Executor::Decode (const ImageMemView &compressedInput, INOUT Images &output, Bool requireHW)
{
	CHECK_ERR( _rtech );

	if ( output.IsEmpty() ){
		CHECK_ERR( output.Allocate( compressedInput.Format(), _intermPixFmt, compressedInput.Dimension2() ));
	}else{
		CHECK_ERR( output.swDecompressedImg.Format() == _intermPixFmt );
		CHECK_ERR( output.hwDecompressedImg.Format() == _intermPixFmt );
	}

	CHECK_ERR( _HwDecode( compressedInput, output.hwDecompressedImg ) or not requireHW );
	CHECK_ERR( _SwDecode( compressedInput, output.swDecompressedImg ));
	return true;
}

/*
=================================================
	_HwDecode
=================================================
*/
bool  Executor::_HwDecode (const ImageMemView &compressedInput, ImageMemView &dstImgMem)
{
	using namespace Threading;
	using namespace Graphics;

	CHECK_ERR( _intermPixFmt == dstImgMem.Format() );

	const bool	is_bc	= EPixelFormat_IsBC( compressedInput.Format() );
	const bool	is_etc	= EPixelFormat_IsETC( compressedInput.Format() ) or EPixelFormat_IsEAC( compressedInput.Format() );
	const bool	is_astc	= EPixelFormat_IsASTC_LDR( compressedInput.Format() );
	const bool	is_astch = EPixelFormat_IsASTC_HDR( compressedInput.Format() );

	if ( is_bc )	{ if (not SupportsBC()) return false; }			else
	if ( is_etc )	{ if (not SupportsETC()) return false; }		else
	if ( is_astc )	{ if (not SupportsASTC()) return false; }		else
	if ( is_astch )	{ if (not SupportsASTC_HDR()) return false; }	else
					RETURN_ERR( "unknown format" );

	const String name	= "HwDec"s <<
							(is_bc ? "BC" :
							(is_etc ? "ETC" :
							(is_astc ? "ASTC" :
							(is_astch ? "ASTCHDR" : "-"))));

	auto&	rts			= GraphicsScheduler();
	auto&	res_mngr	= rts.GetResourceManager();

	auto	pipe_id		= _rtech->GetComputePipeline( PipelineName{name} );
	CHECK_ERR( pipe_id );

	const uint2		dim = compressedInput.Dimension2();

	GAutorelease	ds = res_mngr.CreateDescriptorSet( pipe_id, DescriptorSetName{name + ".ds"} ).Get<0>();
	CHECK_ERR( ds );

	GAutorelease	src_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( dim ).SetUsage( EImageUsage::Sampled | EImageUsage::Transfer )
														.SetFormat( compressedInput.Format() ),
													"hw decode input" );
	CHECK_ERR( src_img );

	GAutorelease	src_view = res_mngr.CreateImageView( ImageViewDesc{}, src_img );
	CHECK_ERR( src_view );

	GAutorelease	dst_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( dim ).SetUsage( EImageUsage::Storage | EImageUsage::Transfer )
														.SetFormat( dstImgMem.Format() ),
													"hw decode output" );
	CHECK_ERR( dst_img );

	GAutorelease	dst_view = res_mngr.CreateImageView( ImageViewDesc{}, dst_img );
	CHECK_ERR( dst_view );

	{
		DescriptorUpdater	updater;
		updater.Set( ds, EDescUpdateMode::Partialy );
		updater.BindImage( UniformName{"d_InputTex"},  src_view );
		updater.BindImage( UniformName{"d_OutputImg"}, dst_view );
		updater.Flush();
	}


	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame, EThread::Background };
	CHECK_ERR( rts.WaitNextFrame( threads, c_Timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[] (ComputePipelineID pipeId, ImageID srcImg, ImageID dstImg, const ImageMemView &compressedInput, DescriptorSetID ds,
						ImageMemView &dstImgMem, AsyncTask &readOp) -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( srcImg, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( dst_mem.CopyFrom( compressedInput ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::CopyDst,		EResourceState::ShaderSample | EResourceState::ComputeShader )
								.ResourceBarrier( dstImg, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipeId );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( dstImgMem.Dimension2(), _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dstImg, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							readOp = ctx.ReadbackImage( dstImg, Default ).IfFullyRead(
										dstImgMem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}( pipe_id, src_img, dst_img, compressedInput, ds, INOUT dstImgMem, OUT read_op ),
					Tuple{},
					True{"Last"},
					{"Hw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, c_Timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, c_Timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, c_Timeout ));

	return true;
}

/*
=================================================
	_SwDecode
=================================================
*/
bool  Executor::_SwDecode (const ImageMemView &compressedInput, ImageMemView &dstImgMem)
{
	using namespace Threading;
	using namespace Graphics;

	CHECK_ERR( _intermPixFmt == dstImgMem.Format() );

	auto&	rts			= GraphicsScheduler();
	auto&	res_mngr	= rts.GetResourceManager();

	String	pipe_name	= "SwDec."s << ToString( compressedInput.Format() );

	auto	pipe_id		= _rtech->GetComputePipeline( PipelineName{pipe_name} );
	CHECK_ERR( pipe_id );

	const uint2		dim = compressedInput.Dimension2();

	GAutorelease	ds = res_mngr.CreateDescriptorSet( pipe_id, DescriptorSetName{pipe_name + ".ds"} ).Get<0>();
	CHECK_ERR( ds );

	const auto&		src_fmt		= EPixelFormat_GetInfo( compressedInput.Format() );
	const auto		block_fmt	= src_fmt.BytesPerBlock() == 4*2 ? EPixelFormat::RG32U :
								 (src_fmt.BytesPerBlock() == 4*4 ? EPixelFormat::RGBA32U : Default);

	GAutorelease	src_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( dim / src_fmt.TexBlockDim() )
														.SetUsage( EImageUsage::Storage | EImageUsage::Transfer )
														.SetFormat( block_fmt ),
													"sw decode input" );
	CHECK_ERR( src_img );

	GAutorelease	src_view = res_mngr.CreateImageView( ImageViewDesc{}, src_img );
	CHECK_ERR( src_view );

	GAutorelease	dst_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( dim )
														.SetUsage( EImageUsage::Storage | EImageUsage::Transfer )
														.SetFormat( dstImgMem.Format() ),
													"sw decode output" );
	CHECK_ERR( dst_img );

	GAutorelease	dst_view = res_mngr.CreateImageView( ImageViewDesc{}, dst_img );
	CHECK_ERR( dst_view );

	{
		DescriptorUpdater	updater;
		updater.Set( ds, EDescUpdateMode::Partialy );
		updater.BindImage( UniformName{"d_InputBlocks"},	src_view );
		updater.BindImage( UniformName{"d_OutputImg"},		dst_view );

		if ( EPixelFormat_IsASTC( compressedInput.Format() ))
		{
			updater.BindTexelBuffer( UniformName{"LUTRemainingBitsToEndpointQuant"},	_astcDecode.view0 );
			updater.BindTexelBuffer( UniformName{"LUTEndpointUnquantize"},				_astcDecode.view1 );
			updater.BindTexelBuffer( UniformName{"LUTWeightQuantizer"},					_astcDecode.view2 );
			updater.BindTexelBuffer( UniformName{"LUTWeightUnquantize"},				_astcDecode.view3 );
			updater.BindTexelBuffer( UniformName{"LUTTritQuintDecode"},					_astcDecode.view4 );
			updater.BindImage(		 UniformName{"LUTPartitionTable"},					_astcDecode.view5 );
		}
		updater.Flush();
	}


	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame, EThread::Background };
	CHECK_ERR( rts.WaitNextFrame( threads, c_Timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[] (ComputePipelineID pipeId, ImageID srcImg, ImageID dstImg, const ImageMemView &compressedInput, DescriptorSetID ds,
						ImageMemView &dstImgMem, AsyncTask &readOp) -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( srcImg, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( compressedInput.CopyTo( dst_mem.AsBufferView() ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::CopyDst,		EResourceState::ShaderStorage_Read | EResourceState::ComputeShader )
								.ResourceBarrier( dstImg, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipeId );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( compressedInput.Dimension2(), _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dstImg, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							readOp = ctx.ReadbackImage( dstImg, Default ).IfFullyRead(
										dstImgMem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}( pipe_id, src_img, dst_img, compressedInput, ds, INOUT dstImgMem, OUT read_op ),
					Tuple{},
					True{"Last"},
					{"Sw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, c_Timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, c_Timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, c_Timeout ));

	return true;
}

/*
=================================================
	Encode
=================================================
*/
bool  Executor::Encode (const ImageMemView &uncompressedInput, ECompFormat dstFmt, OUT Images &output)
{
	CHECK_ERR( _rtech );
	CHECK_ERR( output.Allocate( ToPixFmt( dstFmt ), _intermPixFmt, uncompressedInput.Dimension2() ));

	CHECK_ERR( _SwEncode( uncompressedInput, output.outCompressedImg, dstFmt ));
	return true;
}

/*
=================================================
	_SwEncode
=================================================
*/
bool  Executor::_SwEncode (const ImageMemView &uncompressedInput, ImageMemView &dstImgMem, ECompFormat compFmt)
{
	using namespace Threading;
	using namespace Graphics;

	CHECK_ERR( _intermPixFmt == uncompressedInput.Format() );

	auto&	rts			= GraphicsScheduler();
	auto&	res_mngr	= rts.GetResourceManager();

	String	pipe_name	= "SwEnc."s << ToString( compFmt );

	auto	pipe_id		= _rtech->GetComputePipeline( PipelineName{pipe_name} );
	CHECK_ERR( pipe_id );

	const uint2		dim = uncompressedInput.Dimension2();

	GAutorelease	ds = res_mngr.CreateDescriptorSet( pipe_id, DescriptorSetName{pipe_name + ".ds"} ).Get<0>();
	CHECK_ERR( ds );

	const auto&		dst_fmt		= EPixelFormat_GetInfo( dstImgMem.Format() );
	const auto		block_fmt	= dst_fmt.BytesPerBlock() == 4*2 ? EPixelFormat::RG32U :
								 (dst_fmt.BytesPerBlock() == 4*4 ? EPixelFormat::RGBA32U : Default);
	ImageMemView	tmp_mem		{dstImgMem.AsBufferView(), uint3{}, uint3{dim / dst_fmt.TexBlockDim(),1}, dstImgMem.RowPitch(), dstImgMem.SlicePitch(), block_fmt, dstImgMem.Aspect()};

	GAutorelease	src_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( dim )
														.SetUsage( EImageUsage::Storage | EImageUsage::Transfer )
														.SetFormat( _intermPixFmt ),
													"sw encode input" );
	CHECK_ERR( src_img );

	GAutorelease	src_view = res_mngr.CreateImageView( ImageViewDesc{}, src_img );
	CHECK_ERR( src_view );

	GAutorelease	dst_img = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( tmp_mem.Dimension2() )
														.SetUsage( EImageUsage::Storage | EImageUsage::Transfer )
														.SetFormat( tmp_mem.Format() ),
													"sw encode output" );
	CHECK_ERR( dst_img );

	GAutorelease	dst_view = res_mngr.CreateImageView( ImageViewDesc{}, dst_img );
	CHECK_ERR( dst_view );

	{
		DescriptorUpdater	updater;
		updater.Set( ds, EDescUpdateMode::Partialy );
		updater.BindImage( UniformName{"d_InputImg"},		src_view );
		updater.BindImage( UniformName{"d_OutputBlocks"},	dst_view );
		updater.Flush();
	}


	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame, EThread::Background };
	CHECK_ERR( rts.WaitNextFrame( threads, c_Timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[] (ComputePipelineID pipeId, ImageID srcImg, ImageID dstImg, const ImageMemView &uncompressedInput, DescriptorSetID ds,
						ImageMemView &tempMem, AsyncTask &readOp) -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( srcImg, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( uncompressedInput.CopyTo( dst_mem.AsBufferView() ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( srcImg, EResourceState::CopyDst,		EResourceState::ShaderStorage_Read | EResourceState::ComputeShader )
								.ResourceBarrier( dstImg, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipeId );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( uncompressedInput.Dimension2(), _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dstImg, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							readOp = ctx.ReadbackImage( dstImg, Default ).IfFullyRead(
										tempMem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}( pipe_id, src_img, dst_img, uncompressedInput, ds, INOUT tmp_mem, OUT read_op ),
					Tuple{},
					True{"Last"},
					{"Sw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, c_Timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, c_Timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, c_Timeout ));

	return true;
}

/*
=================================================
	Compare
=================================================
*/
Executor::CmpResult  Executor::Compare (const ImageMemView &input, const ImageMemView &reference, ECompFormat actualFmt, const ImageMemView *compBlocks) const
{
	using namespace Graphics;

	CHECK_ERR( All( input.Dimension2() == reference.Dimension2() ));
	CHECK_ERR( input.Parts().size() == 1 );
	CHECK_ERR( reference.Parts().size() == 1 );

	const auto&		in_data		= input.Parts().front();
	const auto&		ref_data	= reference.Parts().front();
	const uint2		dim			= input.Dimension2();
	const auto		actual_fmt2	= ToPixFmt( actualFmt );
	const auto&		fmt_info	= EPixelFormat_GetInfo( actual_fmt2 );
	const uint2		block_cnt	= dim / fmt_info.TexBlockDim();
	const uint2		block_dim	= fmt_info.TexBlockDim();
	const uint		channels	= FixChannelCount( actualFmt, fmt_info.channels );
	CmpResult		res;

	res.blockCount = Area( block_cnt );

	CHECK_ERR( in_data.size == ref_data.size );

	if_likely( MemEqual( in_data.ptr, ref_data.ptr, in_data.size ))
		return res;

	GetModeFn_t		get_mode = null;
	if ( compBlocks != null )
	{
		CHECK_ERR( compBlocks->Format() == actual_fmt2 );
		switch ( actualFmt )
		{
			case ECompFormat::ETC2_RGB8_UNorm :		get_mode = _GetModeETC2; break;
			case ECompFormat::ETC2_RGB8_A1_UNorm :	get_mode = _GetModeETC2A1; break;
			case ECompFormat::ETC2_RGBA8_UNorm :	get_mode = _GetModeETC2_EAC; break;

			case ECompFormat::ASTC_RGBA8_4x4 :		get_mode = _GetModeASTC4x4; break;

			case ECompFormat::BC6H_RGB16F :
			case ECompFormat::BC6H_RGB16UF :		get_mode = _GetModeBC6; break;

			case ECompFormat::BC7_RGB8_UNorm :
			case ECompFormat::BC7_RGBA8_UNorm :		get_mode = _GetModeBC7;	break;
		}
	}

	// find error
	RWImageMemView	in_rw		{input};
	RWImageMemView	ref_rw		{reference};
	const double4	ch_mask		{bool4{ channels > 0, channels > 1, channels > 2, channels > 3 }};

	for (uint block_y = 0; block_y < block_cnt.y; ++block_y)
	for (uint block_x = 0; block_x < block_cnt.x; ++block_x)
	{
		double	max_block_err	= 0.0;
		double	avr_block_err	= 0.0;

		for (uint y = 0; y < block_dim.y; ++y)
		for (uint x = 0; x < block_dim.x; ++x)
		{
			RGBA32f	in_col, ref_col;
			uint2	coord = uint2{block_x, block_y} * block_dim + uint2{x, y};

			in_rw .Load( uint3{ coord, 0 }, OUT in_col );
			ref_rw.Load( uint3{ coord, 0 }, OUT ref_col );

			double4	diff	= double4{ Abs( float4{in_col} - float4{ref_col} )} * ch_mask;
			double	err		= 0.0;

			err = Max( err, diff.r );
			err = Max( err, diff.g );
			err = Max( err, diff.b );
			err = Max( err, diff.a );

			max_block_err = Max( max_block_err, err );
			avr_block_err += diff.r + diff.g + diff.b + diff.a;
		}

		res.maxError = Max( res.maxError, max_block_err );
		res.avgError += avr_block_err;

		if ( compBlocks != null and get_mode != null )
		{
			RWImageMemView	comp_rw{*compBlocks};
			RGBA32u			block;

			comp_rw.Load( uint3{block_x, block_y, 0}, OUT block );

			int	mode = get_mode( block );
			if ( mode >= 0 )
			{
				avr_block_err /= double(channels) * double(Area( block_dim ));

				res.histogram.resize( Max( mode+1u, res.histogram.size() ));
				res.histogram[ mode ] += PerMode{ max_block_err, avr_block_err, 1u };
			}
		}
	}

	res.avgError /= double(channels) * double(Area( dim ));
	return res;
}

/*
=================================================
	FindNonCompressed
----
	same as EPixelFormat_ToNoncompressed() but will check compatibility
=================================================
*/
EPixelFormat  Executor::FindNonCompressed (EPixelFormat srcFormat) const
{
	using namespace Graphics;
	using EType = PixelFormatInfo::EType;

	const auto&		src_fmt	= EPixelFormat_GetInfo( srcFormat );
	CHECK_ERR( src_fmt.IsCompressed() );
	CHECK_ERR( not src_fmt.IsSRGB() );

	const uint		src_bpc			= Max( 8u, src_fmt.UncompressedBitsPerChannel() );
	const uint		src_channels	= src_fmt.channels;
	auto&			res_mngr		= GraphicsScheduler().GetResourceManager();

	constexpr auto	float_mask		= EType::SFloat | EType::UFloat;
	constexpr auto	int_mask		= EType::UNorm | EType::SNorm | EType::SInt | EType::UInt | EType::BGR | EType::sRGB;

	ImageDesc	desc;
	desc.SetDimension( 32, 32 );
	desc.SetUsage( EImageUsage::Transfer | EImageUsage::Sampled );

	// check support for compressed format
	{
		desc.format = srcFormat;
		if ( not res_mngr.IsSupported( desc ))
			return Default;
	}

	FixedArray< EPixelFormat, 16 >	compatible;

	for (uint i = 0; i < uint(EPixelFormat::_Count); ++i)
	{
		if ( AnyEqual( EPixelFormat(i), EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB5_A1_UNorm,
										EPixelFormat::RGB10_A2U, EPixelFormat::RGB9F_E5 ))
			continue;

		const auto&		fmt = EPixelFormat_GetInfo( EPixelFormat(i) );

		if ( fmt.IsCompressed() or fmt.IsMultiPlanar() )
			break;

		if ( fmt.HasDepthOrStencil() )
			break;

		if ( not (fmt.channels == src_channels or (fmt.channels == 4 and src_channels == 3)) )
			continue;

		if ( not AllBits( src_fmt.valueType, fmt.valueType, int_mask ))
			continue;

		if ( AnyBits( src_fmt.valueType, float_mask ) != AnyBits( fmt.valueType, float_mask ))
			continue;

		if ( fmt.BitsPerChannel() < src_bpc )
			continue;

		desc.format = EPixelFormat(i);
		if ( res_mngr.IsSupported( desc ))
		{
			if ( fmt.BitsPerChannel() == src_bpc and fmt.channels == src_channels )
				compatible.insert( 0u, desc.format );
			else
				compatible.push_back( desc.format );
		}

		if ( compatible.IsFull() )
			break;
	}

	CHECK_ERR( not compatible.empty() );

	// sort
	EPixelFormat	best_match = compatible.front();

	for (auto e : compatible)
	{
		const auto&		fmt = EPixelFormat_GetInfo( e );

		if ( fmt.BitsPerChannel() == src_bpc and fmt.channels == src_channels )
			return e;

		if ( fmt.BitsPerChannel() == src_bpc )
			best_match = e;
	}

	return best_match;
}

/*
=================================================
	_CompileRTech
=================================================
*/
void  Executor::_CompileRTech (Function<void()> withCompiler) __Th___
{
	using namespace AE::PipelineCompiler;

	CHECK_THROW( not _packId );
	CHECK_THROW( not _rtech );
	CHECK_THROW( withCompiler );

	// init pipeline compiler
	ObjectStorage	obj_storage;
	PipelineStorage	ppln_storage;
	{
		Array<Path>		include_dirs = { AE_SHADER_PATH, AE_SHADER_PATH2 };

		obj_storage.pplnStorage			= &ppln_storage;
		obj_storage.defaultFeatureSet	= "DefaultFS";

		obj_storage.spirvCompiler		= MakeUnique<SpirvCompiler>( include_dirs );
		obj_storage.spirvCompiler->SetDefaultResourceLimits();

		ObjectStorage::SetInstance( &obj_storage );

		ScriptFeatureSetPtr	fs = ScriptFeatureSet::Create( obj_storage.defaultFeatureSet );
		fs->fs = GraphicsScheduler().GetFeatureSet();

		PipelineCompiler::ScriptConfig	cfg;
		cfg.SetShaderVersion( EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) |
							  EShaderVersion::_GLSL_SPIRV );

		cfg.SetDefaultLayout( EStructLayout::Std140 );
		cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );

		cfg.SetTarget( ECompilationTarget::Vulkan );

		cfg.SetPipelineOptions( EPipelineOpt::Optimize );
		cfg.SetShaderOptions( EShaderOpt::Optimize );
	}
	{
		RenderTechniquePtr	rtech = RenderTechnique::Create( "rtech" );
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}
	}

	withCompiler();

	RC<RStream>	stream;
	{
		CHECK_THROW( obj_storage.Build() );
		CHECK_THROW( obj_storage.BuildRenderTechniques() );

		auto	mem = MakeRC<ArrayWStream>();

		PipelinePackOffsets		offsets	= {};
		CHECK_THROW( obj_storage.SavePack( *mem, true, OUT offsets ));

		auto	mem2 = MakeRC<ArrayWDataSource>( mem->ReleaseData() );
				mem  = null;

		CHECK_THROW( mem2->Write( Sizeof(PackOffsets_Name), offsets ));

		obj_storage.Clear();

		stream = MakeRC<ArrayRStream>( mem2->ReleaseData() );
	}

	// load from stream
	{
		using namespace Graphics;
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		PipelinePackDesc	desc;
		desc.stream = stream;

		_packId = res_mngr.LoadPipelinePack( desc );
		CHECK_THROW( _packId );

		_rtech = res_mngr.LoadRenderTech( _packId, RenderTechName{"rtech"} );
		CHECK_THROW( _rtech );
	}

	ObjectStorage::SetInstance( null );
}

/*
=================================================
	_CompileCS
=================================================
*/
void  Executor::_CompileCS (StringView source, StringView pipeName,
							Function<void (AnyTypeRef)> updateLayout) __Th___
{
	using namespace AE::PipelineCompiler;

	DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( String{pipeName} << ".dsl" );
	updateLayout( AnyTypeRef{ ds_layout });

	PipelineLayoutPtr		ppln_layout = PipelineLayout::Create( String{pipeName} << ".pl" );
	ppln_layout->AddDSLayout2( String{pipeName} << ".ds", 0, String{pipeName} << ".dsl" );

	ComputePipelinePtr		ppln_templ = ComputePipelineScriptBinding::Create( String{pipeName} );
	ppln_templ->Disable();
	ppln_templ->SetLayout2( ppln_layout );

	{
		ScriptShaderPtr		sh{ new ScriptShader{}};
		sh->SetSource( EShader::Compute, String{source} );
		sh->options = EShaderOpt::Optimize;
		sh->SetComputeLocalSize2( _wgSize.x, _wgSize.y );

		ppln_templ->SetShader( sh );
	}
	{
		ComputePipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( String{pipeName} );
		ppln_spec->Disable();
		ppln_spec->AddToRenderTech( "rtech", "Compute" );
		ppln_spec->SetOptions( EPipelineOpt::Optimize );

		// if successfully compiled
		ppln_spec->Enable();
	}
}

/*
=================================================
	Supports*
=================================================
*/
bool  Executor::SupportsBC () const
{
	return _vulkan.GetVProperties().features.textureCompressionBC;
}

bool  Executor::SupportsETC () const
{
	return _vulkan.GetVProperties().features.textureCompressionETC2;
}

bool  Executor::SupportsASTC () const
{
	return _vulkan.GetVProperties().features.textureCompressionASTC_LDR;
}

bool  Executor::SupportsASTC_HDR () const
{
	return _vulkan.GetVProperties().astcHdrFeats.textureCompressionASTC_HDR;
}

/*
=================================================
	RandomByteArray
=================================================
*/
Array<ubyte>  RandomByteArray (Bytes size, uint seed)
{
	Random			rnd;
	Array<ubyte>	arr;

	arr.resize(usize{ size });

	for (usize i = 0; i+4 < arr.size(); i += 4)
	{
		uint	u = rnd.Uniform<uint>() ^ seed;
		memcpy( OUT arr.data() + i, &u, sizeof(u) );
		seed += uint(i);
	}
	return arr;
}
