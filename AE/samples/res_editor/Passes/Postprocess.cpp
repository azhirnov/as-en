// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Postprocess.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	Execute
=================================================
*/
	bool  Postprocess::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( not _renderTargets.empty() );

		ShaderDebugger::Result		dbg;
		GraphicsPipelineID			ppln;
		const uint2					dim	{_renderTargets[0].image->GetViewDimension()};

		if ( pd.dbg.IsEnabled( this ))
		{
			auto	it = _pipelines.find( pd.dbg.mode );

			if ( it != _pipelines.end()							and
				 AnyBits( pd.dbg.stage, EShaderStages::Fragment ))
			{
				ppln = it->second;

				DirectCtx::Transfer		tctx{ pd.rtask, RVRef(pd.cmdbuf) };
				CHECK( pd.dbg.debugger->AllocForGraphics( OUT dbg, tctx, ppln, uint2{pd.dbg.coord * float2{dim} + 0.5f} ));
				pd.cmdbuf = tctx.ReleaseCommandBuffer();
			}
		}

		if ( not dbg )
			ppln = _pipelines.find( IPass::EDebugMode::Unknown )->second;

		DirectCtx::Graphics		ctx{ pd.rtask, RVRef(pd.cmdbuf) };

		_resources.SetStates( ctx, Default );
		ctx.ResourceState( _ubuffer, EResourceState::UniformRead | EResourceState::FragmentShader );
		ctx.CommitBarriers();

		// render pass
		{
			DescriptorSetID		ds		= _descSets[ ctx.GetFrameId().Index() ];
			RenderPassDesc		rp_desc	= _rpDesc;

			for (auto& rt : _renderTargets) {
				rp_desc.AddTarget( rt.name, rt.image->GetViewId(), rt.clear );
			}

			rp_desc.area = RectI{ int2{dim} };
			rp_desc.DefaultViewport( _depthRange.x, _depthRange.y );

			auto	dctx = ctx.BeginRenderPass( rp_desc, DebugLabel{_dbgName, _dbgColor} );

			dctx.BindPipeline( ppln );
			dctx.BindDescriptorSet( _dsIndex, ds );
			if ( dbg ) dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}

/*
=================================================
	Update
=================================================
*/
	bool  Postprocess::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( not _renderTargets.empty() );

		// validate dimensions
		const uint2		cur_dim = uint2{ _renderTargets.front().image->GetViewDimension() };
		{
			for (auto& rt : _renderTargets)
			{
				const uint2		dim = uint2{ rt.image->GetViewDimension() };
				CHECK_ERR( All( cur_dim == dim ));
			}
		}

		// update uniform buffer
		{
			ShaderTypes::ShadertoyUB	ub_data;
			ub_data.resolution	= float3{cur_dim.x, cur_dim.y, 1};
			ub_data.time		= pd.totalTime.count();
			ub_data.timeDelta	= pd.frameTime.count();
			ub_data.frame		= pd.frameId;
			ub_data.passFrameId	= _dynData.frame;
			ub_data.seed		= pd.seed;
			ub_data.mouse		= pd.pressed ? float4{ pd.unormCursorPos.x, pd.unormCursorPos.y, 1.f, 0.f } : float4{-MaxValue<float>()};
			ub_data.customKeys	= pd.customKeys[0];
			ub_data.pixToMm		= pd.pixToMm;

			if ( _controller )
				_controller->CopyTo( OUT ub_data.camera );

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
			CHECK_ERR( updater.BindBuffer< ShaderTypes::ShadertoyUB >( UniformName{"un_PerPass"}, _ubuffer ));
			CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));
			CHECK_ERR( updater.Flush() );
		}

		return true;
	}

/*
=================================================
	GetResourcesToResize
=================================================
*/
	void  Postprocess::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		for (auto& rt : _renderTargets) {
			if ( rt.image->RequireResize() )
				resources.push_back( rt.image );
		}

		_resources.GetResourcesToResize( INOUT resources );
	}

/*
=================================================
	destructor
=================================================
*/
	Postprocess::~Postprocess ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( _ubuffer );
	}


} // AE::ResEditor
