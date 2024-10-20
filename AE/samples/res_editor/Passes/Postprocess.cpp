// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Postprocess.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	_GetDimension
=================================================
*/
	uint2  Postprocess::_GetDimension () C_NE___
	{
		if ( not _renderTargets.empty() )
			return _renderTargets.front().image->GetViewDesc().Dimension2();

		if ( _dynamicDim )
			return _dynamicDim->Dimension2();

		RETURN_ERR( "render pass dimension is not specified" );
	}

/*
=================================================
	Execute
=================================================
*/
	bool  Postprocess::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		ShaderDebugger::Result		dbg;
		GraphicsPipelineID			ppln;
		const uint2					dim	= _GetDimension();

		if ( pd.dbg.IsEnabled( this ))
		{
			auto	it = _pipelines.find( pd.dbg.mode );

			if ( it != _pipelines.end()							and
				 AnyBits( pd.dbg.stage, EShaderStages::Fragment ))
			{
				ppln = it->second;

				DirectCtx::Transfer		tctx{ pd.rtask, RVRef(pd.cmdbuf) };
				CHECK( pd.dbg.debugger->AllocForGraphics( OUT dbg, tctx, ppln, uint2{pd.dbg.coord * float2(dim-1u)} ));
				pd.cmdbuf = tctx.ReleaseCommandBuffer();
			}
		}

		if ( not dbg )
			ppln = _pipelines.find( IPass::EDebugMode::Unknown )->second;

		DirectCtx::Graphics		ctx{ pd.rtask, RVRef(pd.cmdbuf) };

		for (uint i = 0, cnt = _GetRepeatCount(); i < cnt; ++i)
		{
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
				for (auto& vp : rp_desc.viewports) {
					vp.rect *= float2{dim};
				}

				auto	dctx = ctx.BeginRenderPass( rp_desc, DebugLabel{_dbgName, _dbgColor} );

				dctx.BindPipeline( ppln );
				dctx.BindDescriptorSet( _dsIndex, ds );
				if ( dbg ) dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

				dctx.Draw( 3 );

				ctx.EndRenderPass( dctx );
			}
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
		// validate dimensions
		const uint2		cur_dim = _GetDimension();
		{
			for (auto& rt : _renderTargets)
			{
				const uint2		dim = rt.image->GetViewDesc().Dimension2();
				CHECK_ERR( All( cur_dim == dim ));
			}
		}

		// update uniform buffer
		{
			ShaderTypes::ShadertoyUB	ub_data;
			ub_data.resolution	= float3{ cur_dim, 1.f };
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
