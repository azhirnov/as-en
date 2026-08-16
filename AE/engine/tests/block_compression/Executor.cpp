// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

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
	CmpResult::operator +=
=================================================
*/
Executor::CmpResult&  Executor::CmpResult::operator += (const CmpResult &rhs)
{
	this->maxError	= Max( this->maxError, rhs.maxError );
	this->avgError	= (this->avgError + rhs.avgError) * 0.5;
	return *this;
}

/*
=================================================
	CmpResult::ToString
=================================================
*/
String  Executor::CmpResult::ToString () const
{
	return "max err: "s << Base::ToString( maxError, 2, Bool{maxError < 0.01f} ) << "%, avg: " << Base::ToString( avgError, 2, Bool{avgError < 0.01f} ) << "%";
}

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

	try {
		_CompileRTech( [this]()
			{
				_CompileDecodeAllBC();
				_CompileEncodeAllBC();

				_CompileDecodeAllETC();
				_CompileEncodeAllETC();

				//_CompileDecodeAllASTC();
				//_CompileEncodeAllASTC();
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

	const auto	timeout = seconds{10};

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
	CHECK_ERR( rts.WaitNextFrame( threads, timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[&] () -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( src_img, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( dst_mem.CopyFrom( compressedInput ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::CopyDst,		EResourceState::ShaderSample | EResourceState::ComputeShader )
								.ResourceBarrier( dst_img, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipe_id );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( dim, _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dst_img, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							read_op = ctx.ReadbackImage( dst_img, Default ).IfFullyRead(
										dstImgMem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}(),
					Tuple{},
					True{"Last"},
					{"Hw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, timeout ));

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

	const auto	timeout = seconds{10};

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
		updater.Flush();
	}


	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame, EThread::Background };
	CHECK_ERR( rts.WaitNextFrame( threads, timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[&] () -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( src_img, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( compressedInput.CopyTo( dst_mem.AsBufferView() ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::CopyDst,		EResourceState::ShaderStorage_Read | EResourceState::ComputeShader )
								.ResourceBarrier( dst_img, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipe_id );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( dim, _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dst_img, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							read_op = ctx.ReadbackImage( dst_img, Default ).IfFullyRead(
										dstImgMem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}(),
					Tuple{},
					True{"Last"},
					{"Sw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, timeout ));

	return true;
}

/*
=================================================
	Encode
=================================================
*/
bool  Executor::Encode (const ImageMemView &uncompressedInput, EPixelFormat dstFmt, OUT Images &output)
{
	CHECK_ERR( _rtech );
	CHECK_ERR( output.Allocate( dstFmt, _intermPixFmt, uncompressedInput.Dimension2() ));

	CHECK_ERR( _SwEncode( uncompressedInput, output.outCompressedImg ));
	return true;
}

/*
=================================================
	_SwEncode
=================================================
*/
bool  Executor::_SwEncode (const ImageMemView &uncompressedInput, ImageMemView &dstImgMem)
{
	using namespace Threading;
	using namespace Graphics;

	CHECK_ERR( _intermPixFmt == uncompressedInput.Format() );

	const auto	timeout = seconds{10};

	auto&	rts			= GraphicsScheduler();
	auto&	res_mngr	= rts.GetResourceManager();

	String	pipe_name	= "SwEnc."s << ToString( dstImgMem.Format() );

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
	CHECK_ERR( rts.WaitNextFrame( threads, timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[&] () -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::Invalidate, EResourceState::CopyDst );

							ImageMemView	dst_mem;
							ctx.UploadImage( src_img, UploadImageDesc{}, OUT dst_mem );
							CHECK_CE( uncompressedInput.CopyTo( dst_mem.AsBufferView() ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.ResourceBarrier( src_img, EResourceState::CopyDst,		EResourceState::ShaderStorage_Read | EResourceState::ComputeShader )
								.ResourceBarrier( dst_img, EResourceState::Invalidate,	EResourceState::ShaderStorage_Write | EResourceState::ComputeShader );

							ctx.BindPipeline( pipe_id );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( DivCeil( dim, _wgSize ));

							ctx.AccumBarriers()
								.ResourceBarrier( dst_img, EResourceState::ShaderStorage_Write | EResourceState::ComputeShader,  EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							read_op = ctx.ReadbackImage( dst_img, Default ).IfFullyRead(
										tmp_mem,
										[] (Promise<ImageMemView> readRes, CoSafe<ImageMemView&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											CHECK_CE( output->CopyFrom( *view ));
										});

							RenderCoro_Execute( ctx );
						}
					}(),
					Tuple{},
					True{"Last"},
					{"Sw Decode task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, timeout ));

	return true;
}

/*
=================================================
	Compare
=================================================
*/
Executor::CmpResult  Executor::Compare (const ImageMemView &input, const ImageMemView &reference, EPixelFormat actualFmt) const
{
	using namespace Graphics;

	CHECK_ERR( All( input.Dimension2() == reference.Dimension2() ));
	CHECK_ERR( input.Parts().size() == 1 );
	CHECK_ERR( reference.Parts().size() == 1 );

	const auto&		in_data		= input.Parts().front();
	const auto&		ref_data	= reference.Parts().front();
	const uint2		dim			= input.Dimension2();

	CHECK_ERR( in_data.size == ref_data.size );

	if_likely( MemEqual( in_data.ptr, ref_data.ptr, in_data.size ))
		return CmpResult{};

	// find error
	RWImageMemView	in_rw		{input};
	RWImageMemView	ref_rw		{reference};
	const auto&		fmt_info	= EPixelFormat_GetInfo( actualFmt );
	const double4	ch_mask		{bool4{ fmt_info.channels > 0, fmt_info.channels > 1, fmt_info.channels > 2, fmt_info.channels > 3 }};
	CmpResult		res;

	for (uint y = 0; y < dim.y; ++y)
	{
		for (uint x = 0; x < dim.x; ++x)
		{
			RGBA32f	in_col, ref_col;

			in_rw .Load( uint3{ x, y, 0 }, OUT in_col );
			ref_rw.Load( uint3{ x, y, 0 }, OUT ref_col );

			double4	diff	= double4{ Abs( float4{in_col} - float4{ref_col} )} * ch_mask;
			double	err		= 0.0;

			err = Max( err, diff.r );
			err = Max( err, diff.g );
			err = Max( err, diff.b );
			err = Max( err, diff.a );

			res.maxError = Max( res.maxError, err );
			res.avgError += diff.r + diff.g + diff.b + diff.a;
		}
	}

	res.avgError /= double(fmt_info.channels) * double(dim.x * dim.y);
	res.maxError *= 100.0;
	res.avgError *= 100.0;

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

/*
=================================================
	RandomFloatArray
=================================================
*/
Array<float>  RandomFloatArray (usize count)
{
	Random			rnd;
	Array<float>	arr;

	arr.resize( count );

	for (usize i = 0; i < arr.size(); ++i)
	{
		arr[i] = rnd.Uniform<float>( 0.0f, 1.f );
	}
	return arr;
}
