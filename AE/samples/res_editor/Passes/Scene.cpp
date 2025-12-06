// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Passes/Scene.h"
#include "Core/EditorUI.h"
#include "_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	Execute
=================================================
*/
	bool  SceneGraphicsPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( _scene );

		const uint2						dim			= _renderTargets[0].image->GetViewDesc().Dimension2();
		const auto&						instances	= _scene->_geomInstances;
		Array<ShaderDebugger::Result*>	dbg_result;
		LinearAllocator<>				allocator;
		SceneGraphicsSubpass const*		dbg_subpass	= null;

		for (auto& subpass : _subpasses)
		{
			if_unlikely( pd.dbg.IsEnabled( subpass.get() ))
			{
				DirectCtx::Transfer		tctx	{ pd.rtask, RVRef(pd.cmdbuf) };
				const uint2				coord	= pd.dbg.exactCoord.has_value() ?
													uint2{*pd.dbg.exactCoord} :
													uint2{pd.dbg.coord * float2(dim-1u)};

				dbg_result.resize( instances.size() );

				for (usize i = 0; i < instances.size(); ++i)
				{
					IGeomSource::DebugPrepareData	dd{ *subpass->_materials[i], tctx, pd.dbg, coord, allocator, _tempPplnToObjID, dbg_result[i] };
					instances[i].geometry->PrepareForDebugging( INOUT dd );
				}

				pd.cmdbuf	= tctx.ReleaseCommandBuffer();
				dbg_subpass	= subpass.get();

				//UIInteraction::Instance().SetShaderDebugCoord( coord );	// TODO
			}
		}

		DirectCtx::Graphics		ctx	{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };

		_BeginTimeQuery( ctx );

		for (uint it = 0, cnt = _GetRepeatCount(); it < cnt; ++it)
		{
			// state transition
			{
				for (auto& subpass : _subpasses)
				{
					for (usize i = 0; i < instances.size(); ++i)
					{
						auto&	mtr = subpass->_materials[i];
						if ( mtr != null )
							instances[i].geometry->StateTransition( *mtr, ctx );
					}

					subpass->_resources.SetStates( ctx, Default );
					ctx.ResourceState( subpass->_ubuffer, EResourceState::UniformRead | EResourceState::AllGraphicsShaders );
				}
				if ( cnt > 1 ) ctx.MemoryBarrier( EPipelineScope::All, EPipelineScope::All );	// disable overlapping, only for profiling!
				ctx.CommitBarriers();
			}

			// render pass
			{
				RenderPassDesc	rp_desc = _rpDesc;
				Scissors_t		scissors;
				Viewports_t		viewports;

				for (auto& rt : _renderTargets) {
					rp_desc.AddTarget( rt.name, rt.image->GetViewId(), rt.clear );
				}
				rp_desc.area = RectI{ int2{dim} };

				auto	dctx = ctx.BeginRenderPass( rp_desc, DebugLabel{_dbgName, _dbgColor} );

				// draw subpasses
				for (auto& subpass : _subpasses)
				{
					if ( not IsFirstElement( subpass, _subpasses ))
					{
						DirectCtx::Draw		temp = ctx.NextSubpass( dctx, DebugLabel{_dbgName, _dbgColor} );
						PlacementDelete( dctx );
						new(&dctx) DirectCtx::Draw{ RVRef(temp) };
					}

					// setup
					if ( not subpass->_viewports.empty() )
					{
						viewports.clear();
						for (auto& src : subpass->_viewports)
						{
							auto&	vp = viewports.emplace_back( src );
							vp.rect *= float2{dim};
						}
						dctx.SetViewports( viewports );
					}

					if ( not subpass->_scissors.empty() )
					{
						scissors.clear();
						for (auto& src : subpass->_scissors) {
							scissors.push_back( RectI{ src * float2{dim} });
						}
						dctx.SetScissors( scissors );
					}

					if ( subpass->_shadingRate )
						dctx.SetFragmentShadingRate( subpass->_shadingRate.rate, subpass->_shadingRate.primitiveOp, subpass->_shadingRate.textureOp );

					if ( not subpass->_wScaling.empty() )
						dctx.SetViewportWScaling( subpass->_wScaling );

					decltype(&IGeomSource::Draw)	draw_fn = null;
					switch_enum( subpass->_renderLayer )
					{
						case ERenderLayer::Opaque :
						case ERenderLayer::Translucent :	draw_fn = &IGeomSource::Draw;			break;
						case ERenderLayer::PostProcess :	draw_fn = &IGeomSource::PostProcess;	break;
						case ERenderLayer::_Count :			break;
					}
					switch_end

					if ( draw_fn == null )
						continue;

					// draw
					DescriptorSetID		ds = subpass->_descSets[ ctx.GetFrameId().Index() ];

					for (usize i = 0; i < instances.size(); ++i)
					{
						bool	has_dbg_result	= (dbg_subpass == subpass.get())	and
												  (not dbg_result.empty())			and
												  (dbg_result[i] != null)			and
												  it == 0;
						auto&	mtr				= subpass->_materials[i];

						if ( mtr == null )
							continue;

						CHECK_ERR( ((*instances[i].geometry).*draw_fn)( IGeomSource::DrawData{
										*mtr, dctx, ds,
										(has_dbg_result ? dbg_result[i]	: null),
										(has_dbg_result ? pd.dbg.mode	: Default),
										(has_dbg_result ? pd.dbg.stage	: Default)
									}));
					}
				}
				ctx.EndRenderPass( dctx );
			}
		}

		_EndTimeQuery( ctx );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}

/*
=================================================
	Update
=================================================
*/
	bool  SceneGraphicsPass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( _scene );
		CHECK_ERR( not _renderTargets.empty() );

		// validate dimensions
		const uint2		cur_dim = _renderTargets.front().image->GetViewDesc().Dimension2();

		for (auto& rt : _renderTargets)
		{
			const uint2		dim = rt.image->GetViewDesc().Dimension2();

			if ( rt.name == AttachmentName{"ShadingRate"} or rt.name == AttachmentName{"FragmentDensity"} ){
				CHECK_ERR( All( dim < cur_dim ));
			}else{
				CHECK_ERR( All( dim == cur_dim ));
			}
		}

		for (auto& subpass : _subpasses)
		{
			subpass->_dimension = cur_dim;
			subpass->Update( ctx, pd );
		}

		_ReadTimeQuery( ctx.GetFrameId() );
		return true;
	}

/*
=================================================
	GetResourcesToResize
=================================================
*/
	void  SceneGraphicsPass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		for (auto& rt : _renderTargets) {
			if_unlikely( rt.image->RequireResize() )
				resources.push_back( rt.image );
		}

		for (auto& subpass : _subpasses) {
			subpass->GetResourcesToResize( INOUT resources );
		}

		// TODO: _scene->_geomInstances ?
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Update
=================================================
*/
	bool  SceneGraphicsSubpass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( _scene );

		// update uniform buffer
		{
			ShaderTypes::SceneGraphicsPassUB	ub_data;

			ub_data.resolution		= float2{_dimension};
			ub_data.invResolution	= 1.f / float2{_dimension};
			ub_data.time			= pd.totalTime.count();
			ub_data.timeDelta		= pd.frameTime.count();
			ub_data.frame			= pd.frameId;
			ub_data.seed			= pd.seed;
			ub_data.mouse			= float4{ pd.unormCursorPos.x, pd.unormCursorPos.y, float(pd.pressed), 0.f };
			ub_data.customKeys		= float2{ pd.customKeys[0], pd.customKeys[1] };
			ub_data.pixPerMm		= pd.pixPerMm;
			ub_data.mmPerPix		= pd.mmPerPix;

			if ( _controller )
				_controller->CopyTo( OUT ub_data.camera );

			_CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
			_CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

			CHECK_ERR( ctx.UploadBuffer( _ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
		}

		// update descriptors
		{
			DescriptorUpdater	updater;
			DescriptorSetID		ds		= _descSets[ ctx.GetFrameId().Index() ];

			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::SceneGraphicsPassUB >( UniformName{"un_PerPass"}, _ubuffer ));
			CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));
			CHECK_ERR( updater.Flush() );
		}

		// update materials
		{
			const auto&		instances = _scene->_geomInstances;

			for (usize i = 0; i < instances.size(); ++i)
			{
				auto&	mtr = _materials[i];
				if ( mtr != null )
					CHECK_ERR( instances[i].geometry->Update( IGeomSource::UpdateData{ *mtr, ctx, instances[i].transform, pd }));
			}
		}

		return true;
	}

/*
=================================================
	GetResourcesToResize
=================================================
*/
	void  SceneGraphicsSubpass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		_resources.GetResourcesToResize( INOUT resources );
	}

/*
=================================================
	destructor
=================================================
*/
	SceneGraphicsSubpass::~SceneGraphicsSubpass ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( INOUT _ubuffer );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Execute
=================================================
*/
	bool  SceneRayTracingPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( _scene );

		// TODO: shader debugger

		for (uint i = 0, cnt = _GetRepeatCount(); i < cnt; ++i)
		{
			DirectCtx::RayTracing	ctx			{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };
			const uint				fid			= ctx.GetFrameId().Index();
			const auto&				instances	= _scene->_geomInstances;

			if ( i == 0 ) _BeginTimeQuery( ctx );

			// state transition
			{
				for (auto& inst : instances) {
					inst.geometry->StateTransition( ctx );
				}
				_resources.SetStates( ctx, Default );
				ctx.ResourceState( _ubuffer, EResourceState::UniformRead | EResourceState::RayTracingShaders );
				if ( cnt > 1 ) ctx.MemoryBarrier( EPipelineScope::All, EPipelineScope::All );	// disable overlapping, only for profiling!
				ctx.CommitBarriers();
			}

			ctx.BindPipeline( _pipeline );
			ctx.BindDescriptorSet( _passDSIndex, _passDescSets[fid] );
			ctx.BindDescriptorSet( _objDSIndex,  _objDescSets[fid] );

			for (const auto& it : _iterations)
			{
				if ( it.indirect ){
					ctx.TraceRaysIndirect( _sbt, it.indirect->GetBufferId( fid ), it.indirectOffset );
				}else{
					ctx.TraceRays( it.Dimension(), _sbt );
				}

				if ( not IsLastElement( it, _iterations ))
				{
					ctx.ExecutionBarrier( EPipelineScope::RayTracing, EPipelineScope::RayTracing );
					ctx.CommitBarriers();
				}
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
	bool  SceneRayTracingPass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( _scene );

		// update uniform buffer
		{
			ShaderTypes::SceneRayTracingPassUB	ub_data;

			ub_data.time		= pd.totalTime.count();
			ub_data.timeDelta	= pd.frameTime.count();
			ub_data.frame		= pd.frameId;
			ub_data.seed		= pd.seed;
			ub_data.customKeys	= float2{ pd.customKeys[0], pd.customKeys[1] };
			ub_data.pixPerMm	= pd.pixPerMm;
			ub_data.mmPerPix	= pd.mmPerPix;

			if ( _controller )
				_controller->CopyTo( OUT ub_data.camera );

			_CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
			_CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

			CHECK_ERR( ctx.UploadBuffer( _ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
		}

		// update descriptors
		{
			DescriptorUpdater	updater;
			DescriptorSetID		ds	= _passDescSets[ ctx.GetFrameId().Index() ];

			// per pass
			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::SceneRayTracingPassUB >( UniformName{"un_PerPass"}, _ubuffer ));
			CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));

			// per object
			ds = _objDescSets[ ctx.GetFrameId().Index() ];
			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));

			// update objects
			for (auto& inst : _scene->_geomInstances)
			{
				CHECK_ERR( inst.geometry->RTUpdate( IGeomSource::UpdateRTData{ updater }));
			}
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
	void  SceneRayTracingPass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		_resources.GetResourcesToResize( INOUT resources );

		// TODO: _scene->_geomInstances ?
	}

/*
=================================================
	destructor
=================================================
*/
	SceneRayTracingPass::~SceneRayTracingPass ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _passDescSets );
		res_mngr.ReleaseResourceArray( INOUT _objDescSets );
		res_mngr.ReleaseResource( INOUT _ubuffer );
	}


} // AE::ResEditor
