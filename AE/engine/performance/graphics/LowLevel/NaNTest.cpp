// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "LowLevelPerfCore.h"

namespace AE::Graphics
{
#	include "vk_names.h"

namespace
{
	using namespace AE::Threading;

	static constexpr seconds	c_MaxTimeout	{30*60};
	static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};
	static const uint2			c_Dim			{8, 19};

/*
=================================================
	NanTestTask
=================================================
*/
	static RenderCoro  NanTestTask (ImageViewID view, GraphicsPipelineID ppln, Function<void (const ImageMemView &)> fn)
	{
		auto	self = RenderCoro_Get();

		DirectCtx::Graphics	ctx{ self };

		auto	rt = ctx.GetResourceManager().GetResource( view )->ImageId();

		ctx.AccumBarriers()
			.ResourceBarrier( rt, EResourceState::Unknown, EResourceState::ColorAttachment );

		// render pass
		{
			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"NaN.RP"}, c_Dim }
													.AddTarget( AttachmentName{"Color"}, view, RGBA32u{0} )
													.DefaultViewport() );

			dctx.BindPipeline( ppln );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		DirectCtx::Transfer	tctx { self, ctx.ReleaseCommandBuffer() };

		tctx.AccumBarriers()
			.ResourceBarrier( rt, EResourceState::ColorAttachment, EResourceState::CopySrc );

		tctx.ReadbackImage( rt, ReadbackImageDesc{} )
			.Then(	RVRef(fn),
					[] (Promise<ImageMemView> readOp, Function<void (const ImageMemView &)> fn) -> InlineCoro<>
					{
						auto mem = co_await readOp;
						fn( mem );
					});

		tctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( tctx );
	}

/*
=================================================
	ProcessResult
=================================================
*/
	template <typename T>
	static String  FValueToString (T x)
	{
		const int	c = std::fpclassify( x );
		switch ( c )
		{
			case FP_NAN :			return "nan";
			case FP_INFINITE :		return x < T(0) ? "-inf" : "inf";
			case FP_SUBNORMAL :		return "denorm";
			default :
			{
				if ( x >= MaxValue<T>() )	return "max";
				if ( x <= -MaxValue<T>() )	return "-max";

				String	s = ToString( x, 1 );
				s.pop_back();  s.pop_back();
				return s;
			}
		}
	}

	static void  ProcessResult (const ImageMemView &mem, StringView type, StringView devName)
	{
		CHECK( All( uint2{mem.Dimension()} == c_Dim ));

		Function< String (uint,uint) >	fn;
		RWImageMemView					img {mem};

		if ( type == "FP32" or type == "FP32_MED" or type == "FP32_LOW" )
		{
			fn = [&img] (uint x, uint y) -> String
			{{
				RGBA32u	pix;
				img.Load( uint3{x,y,0}, OUT pix );
				ASSERT( pix.g >= 1 and pix.g <= 3 );

				float	f = BitCast<float>( pix.r );
				return FValueToString( f );
			}};
		}
		else
		if ( type == "FP16" )
		{
			fn = [&img] (uint x, uint y) -> String
			{{
				RGBA32u	pix;
				img.Load( uint3{x,y,0}, OUT pix );
				ASSERT( pix.g == 4 );

				half	h = BitCast<half>( ushort(pix.r) );

				if ( h.IsNaN() )			return "nan";
				if ( h.IsInfinity() )		return h.IsNegative() ? "-inf" : "inf";
				if ( h == half::Max() )		return "max";
				if ( h == half::MaxNeg() )	return "-max";
				return FValueToString( h.Get() );
			}};
		}
		else
		if ( type == "FP64" )
		{
			fn = [&img] (uint x, uint y) -> String
			{{
				RGBA32u	pix;
				img.Load( uint3{x,y,0}, OUT pix );

				double	d = BitCast<packed_double2>( pix ).x;
				return FValueToString( d );
			}};
		}
		else
			RETURN_ERRV( "unknown type" );

		String	str;
		str << "* " << type << "\n\n";
		str << "\t| op \\ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |\n";
		str << "\t|---|---|---|---|---|---|---|---|---|\n";

		const StringView	ops [] = {
			"x", "Min(x,0)", "Min(0,x)", "Max(x,0)", "Max(0,x)", "Clamp(x,0,1)", "Clamp(x,-1,1)",
			"IsNaN", "IsInfinity", "bool(x)", "x != x", "Step(0,x)", "Step(x,0)",
			"Step(0,-x)", "Step(-x,0)", "SignOrZero(x)", "SignOrZero(-x)", "SmoothStep(x,0,1)",
			"Normalize(x)"
		};
		CHECK_ERRV( CountOf(ops) == c_Dim.y );

		for (uint y = 0; y < c_Dim.y; ++y)
		{
			str << "\t| " << ops[y];

			for (uint x = 0; x < c_Dim.x; ++x)
			{
				str << " | " << fn(x,y);
			}
			str << " |\n";
		}
		str << "\n";

		AE_LOGI( str );

		#ifdef AE_RESULT_PATH
		{
			StdFileWStream	file { Path{AE_RESULT_PATH} / (String{devName} << "-NaN.md"), StdFileWStream::EMode::OpenAppend };
			if ( file.IsOpen() )
				CHECK( file.Write( str ));
		}
		#endif
	}

} // namespace

/*
=================================================
	_NaNTest
=================================================
*/
	void  LowLevelPerfCore::_NaNTest ()
	{
		#ifdef AE_RESULT_PATH
			FileSystem::DeleteFile( Path{AE_RESULT_PATH} / (String{_device.GetDeviceName()} << "-NaN.md") );
		#endif

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	rt = res_mngr.CreateImage( ImageDesc{}.SetFormat( EPixelFormat::RG32U ).SetDimension( c_Dim )
													.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Transfer ));
		CHECK_ERRV( rt );

		auto	rt_view = res_mngr.CreateImageView( ImageViewDesc{}, rt );
		CHECK_ERRV( rt_view );

		// float32
		if ( auto pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::NaN_RT ))
		{
			const StringView	types [] = { "FP32", "FP32_MED" };
			for (auto type : types)
			{
				String	name = "NaN."s << type;

				GraphicsPipelineID	ppln = pipelines->GetGraphicsPipeline( PipelineName{name} );
				if ( not ppln )
				{
					AE_LOGW( "Skip pipeline '"s << name << "'" );
					continue;
				}

				_NaNTest2( rt_view, ppln, [t = String{type}, this] (const ImageMemView &mem) { ProcessResult( mem, t, _device.GetDeviceName() ); });
			}
		}

		// float16
		if ( auto pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::NaN_RT_ShaderFloatInt16 ))
		{
			GraphicsPipelineID	ppln = pipelines->GetGraphicsPipeline( RenderTechs::NaN_RT_ShaderFloatInt16.Graphics.NaN_FP16 );
			if ( ppln )
				_NaNTest2( rt_view, ppln, [this](const ImageMemView &mem) { ProcessResult( mem, "FP16", _device.GetDeviceName() ); });
			else
				AE_LOGW( "Skip pipeline 'NaN.FP16'" );
		}else
		if ( auto pipelines2 = res_mngr.LoadRenderTech( Default, RenderTechs::NaN_RT_ShaderFloat16 ))
		{
			GraphicsPipelineID	ppln = pipelines2->GetGraphicsPipeline( RenderTechs::NaN_RT_ShaderFloat16.Graphics.NaN_FP16 );
			if ( ppln )
				_NaNTest2( rt_view, ppln, [this](const ImageMemView &mem) { ProcessResult( mem, "FP16", _device.GetDeviceName() ); });
			else
				AE_LOGW( "Skip pipeline 'NaN.FP16'" );
		}else{
			AE_LOGW( "Skip render technique 'NaN.RT.ShaderFloatInt16'" );
		}

		// float64
		if ( auto pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::NaN_RT_ShaderFloatInt64 ))
		{
			GraphicsPipelineID	ppln = pipelines->GetGraphicsPipeline( RenderTechs::NaN_RT_ShaderFloatInt64.Graphics.NaN_FP64 );
			if ( ppln )
				_NaNTest2( rt_view, ppln, [this](const ImageMemView &mem) { ProcessResult( mem, "FP64", _device.GetDeviceName() ); } );
			else
				AE_LOGW( "Skip pipeline 'NaN.FP64'" );
		}else
		if ( auto pipelines2 = res_mngr.LoadRenderTech( Default, RenderTechs::NaN_RT_ShaderFloat64 ))
		{
			GraphicsPipelineID	ppln = pipelines2->GetGraphicsPipeline( RenderTechs::NaN_RT_ShaderFloat64.Graphics.NaN_FP64 );
			if ( ppln )
				_NaNTest2( rt_view, ppln, [this](const ImageMemView &mem) { ProcessResult( mem, "FP64", _device.GetDeviceName() ); } );
			else
				AE_LOGW( "Skip pipeline 'NaN.FP64'" );
		}else{
			AE_LOGW( "Skip render technique 'NaN.RT.ShaderFloatInt64'" );
		}


		res_mngr.ReleaseResources( rt, rt_view );
	}

/*
=================================================
	_NaNTest2
=================================================
*/
	bool  LowLevelPerfCore::_NaNTest2 (ImageViewID view, GraphicsPipelineID ppln, Function<void (const ImageMemView &)> fn)
	{
		auto&	rts = GraphicsScheduler();

		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto	batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0 );
		CHECK_ERR( batch );

		auto	task	= batch->Run( NanTestTask( view, ppln, RVRef(fn) ), Tuple{}, True{"submit"} );

		auto	end		= rts.EndFrame( Tuple{ task });

		CHECK_ERR( Scheduler().Wait( {end}, c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		return true;
	}


} // AE::Graphics
