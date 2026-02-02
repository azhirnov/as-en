// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Passes/RasterMip.h"
#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Core/EditorUI.h"
#include "_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	Execute
=================================================
*/
	bool  RasterMip::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( not _variables.empty() );

		ShaderDebugger::Result	dbg;
		GraphicsPipelineID		ppln;

		if ( pd.dbg.IsEnabled( this ))
		{
			auto	it = _pipelines.find( pd.dbg.mode );

			if ( it != _pipelines.end()							and
				 AnyBits( pd.dbg.stage, EShaderStages::Fragment ))
			{
				ppln = it->second;

				DirectCtx::Transfer		tctx{ pd.rtask, RVRef(pd.cmdbuf) };

				if ( pd.dbg.mode == EDebugMode::Asserts ){
					CHECK( pd.dbg.debugger->AllocForAsserts( OUT dbg, tctx, ppln ));
				}else{
					CHECK( pd.dbg.debugger->AllocForGraphics( OUT dbg, tctx, ppln ));
				}
				pd.cmdbuf = tctx.ReleaseCommandBuffer();
			}
		}

		if ( not dbg )
			ppln = _pipelines.find( IPass::EDebugMode::Unknown )->second;


		for (uint i = 0, cnt = _GetRepeatCount(); i < cnt; ++i)
		{
			DirectCtx::Graphics	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };
			auto&				ctx2 = ctx.GetBaseContext();

			DescriptorSetID		ds0	= _descSets[ ctx.GetFrameId().Index() ];

			if ( i == 0 )
			{
				_BeginTimeQuery( ctx );
				ctx.ResourceState( _ubuffer, EResourceState::UniformRead | EResourceState::FragmentShader );
			}

			_resources.SetStates( ctx, Default );
			ctx.CommitBarriers();

			// change resource state in RG here, but this state will be at the end of all render passes
			for (auto& var : _variables)
			{
				ImageID					img_id		= var.image->GetImageId();
				EResourceState			new_state	= EResourceState::ShaderSample | EResourceState::FragmentShader;
				EResourceState			old_state	= ctx.SetResourceState( img_id, new_state );
				ImageSubresourceRange	subres;

				subres.aspectMask	= EImageAspect::Color;
				subres.mipmapCount	= 1;

				if ( old_state != new_state )
					ctx2.ImageBarrier( img_id, old_state, new_state, subres );
			}
			ctx2.CommitBarriers();

			for (usize mip = 0; mip < _mipChainDS.size(); ++mip)
			{
				RenderPassDesc	rp_desc	= _rpDesc;
				const uint2		dim		= Max( _variables.front().image->GetImageDesc().Dimension2() >> (mip + 1), 1u );

				rp_desc.area = RectI{int2{dim}};
				rp_desc.AddViewport( rp_desc.area );

				for (auto& var : _variables)
				{
					rp_desc.AddTarget( var.outName, var.outViews[mip] );
				}
				ctx.CommitBarriers();

				auto	dctx = ctx2.BeginRenderPass( rp_desc );

				dctx.BindPipeline( ppln );
				dctx.BindDescriptorSet( _ds0Index, ds0 );
				if ( dbg ) dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

				dctx.BindDescriptorSet( _ds1Index, _mipChainDS[mip] );

				ShaderTypes::ComputeMipPC	pc;
				pc.invResolution	= 1.f / float2{dim};
				pc.resolution		= dim;

				dctx.PushConstant( _pcIndex, pc );

				dctx.Draw( 3 );

				ctx2.EndRenderPass( dctx );
			}

			if ( i+1 == cnt ) _EndTimeQuery( ctx );

			pd.cmdbuf = ctx.ReleaseCommandBuffer();
		}
		return true;
	}

/*
=================================================
	Update
=================================================
*/
	bool  RasterMip::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( not _variables.empty() );

		// validate dimensions
		const uint2		cur_dim = _variables.front().image->GetViewDesc().Dimension2();
		{
			for (auto& var : _variables)
			{
				const uint2		dim = var.image->GetViewDesc().Dimension2();
				CHECK_ERR( All( cur_dim == dim ));
			}
		}

		if_unlikely( Any( _lastDim != cur_dim ))
		{
			_lastDim = cur_dim;
			CHECK_ERR( _CreateMipChain() );
		}

		// update uniform buffer
		{
			ShaderTypes::ComputeMipUB	ub_data;
			ub_data.time		= pd.totalTime.count();
			ub_data.timeDelta	= pd.frameTime.count();
			ub_data.frame		= pd.frameId;
			ub_data.passFrameId	= _dynData.frame;
			ub_data.seed		= pd.seed;
			ub_data.mouse		= float4{ pd.unormCursorPos.x, pd.unormCursorPos.y, float(pd.pressed), 0.f };
			ub_data.customKeys	= float2{ pd.customKeys[0], pd.customKeys[1] };
			ub_data.pixPerMm	= pd.pixPerMm;
			ub_data.mmPerPix	= pd.mmPerPix;

			_CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
			_CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

			if ( _dynData.prevFrame != pd.frameId )
			{
				++_dynData.frame;
				_dynData.prevFrame = pd.frameId;
			}
			CHECK_ERR( ctx.UploadBuffer( _ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
		}

		// update descriptors
		{
			DescriptorUpdater	updater;
			DescriptorSetID		ds		= _descSets[ ctx.GetFrameId().Index() ];

			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::ComputeMipUB >( UniformName{"un_PerPass"}, _ubuffer ));
			CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));
			CHECK_ERR( updater.Flush() );
		}

		_ReadTimeQuery( ctx.GetFrameId() );
		return true;
	}

/*
=================================================
	GetResourcesToResize
=================================================
*/
	void  RasterMip::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		_resources.GetResourcesToResize( INOUT resources );
	}

/*
=================================================
	_CreateMipChain
=================================================
*/
	bool  RasterMip::_CreateMipChain ()
	{
		CHECK_ERR( not _variables.empty() );

		_DestroyMipChain();

		ASSERT( _mipChainDS.empty() );

		const uint	mip_count = ImageUtils::NumberOfMipmaps( uint3{_lastDim, 1} ) - 1;

		if ( mip_count == 0 or mip_count == UMax )
			return true;

		GraphicsPipelineID	ppln;
		{
			auto	it = _pipelines.find( EDebugMode::Unknown );
			CHECK_ERR( it != _pipelines.end() );
			ppln = it->second;
		}

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		_mipChainDS.resize( mip_count );
		CHECK_ERR( res_mngr.CreateDescriptorSets( OUT _ds1Index, OUT _mipChainDS.data(), _mipChainDS.size(),
												  ppln, DescriptorSetName{"ds1"}, null, "RasterMip" ));

		for (auto& var : _variables)
		{
			ASSERT( var.inViews.empty() );
			ASSERT( var.outViews.empty() );

			var.inViews.resize( mip_count );
			var.outViews.resize( mip_count );

			ImageID	img_id = var.image->GetImageId();

			for (uint i = 0; i < mip_count; ++i)
			{
				ImageViewDesc	view_desc;
				view_desc.baseMipmap	= MipmapLevel{i};
				view_desc.mipmapCount	= 1;

				var.inViews[i] = res_mngr.CreateImageView( view_desc, img_id );

				view_desc.baseMipmap++;

				var.outViews[i] = res_mngr.CreateImageView( view_desc, img_id );

				CHECK_ERR( var.inViews[i] and var.outViews[i] );
			}
		}

		DescriptorUpdater	updater;
		for (uint i = 0; i < mip_count; ++i)
		{
			CHECK_ERR( updater.Set( _mipChainDS[i], EDescUpdateMode::Partialy ));
			for (auto& var : _variables) {
				CHECK_ERR( updater.BindImage( var.inName,  var.inViews[i] ));
			}
			CHECK_ERR( updater.Flush() );
		}

		return true;
	}

/*
=================================================
	_DestroyMipChain
=================================================
*/
	void  RasterMip::_DestroyMipChain ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.ReleaseResourceArray( _mipChainDS );

		_mipChainDS.clear();

		for (auto& var : _variables)
		{
			for (auto& view : var.inViews)
				res_mngr.ReleaseResource( INOUT view );

			for (auto& view : var.outViews)
				res_mngr.ReleaseResource( INOUT view );

			var.inViews.clear();
			var.outViews.clear();
		}
	}

/*
=================================================
	destructor
=================================================
*/
	RasterMip::~RasterMip ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( _ubuffer );
		_DestroyMipChain();
	}


} // AE::ResEditor
