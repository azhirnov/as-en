// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Scene.h"
#include "res_editor/Passes/IPass.cpp.h"
#include "res_editor/EditorUI.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{
	
/*
=================================================
	Execute
=================================================
*/
	bool  SceneGraphicsPass::Execute (SyncPassData &pd) __NE___
	{
		CHECK_ERR( _scene );
		
		const uint2				dim			{_renderTargets[0].image->GetImageDesc().dimension};
		DirectCtx::Graphics		ctx			{ pd.rtask, RVRef(pd.cmdbuf) };
		const auto&				instances	= _scene->_geomInstances;
		
		// state transition
		{
			for (usize i = 0; i < instances.size(); ++i) {
				instances[i].geometry->StateTransition( *_materials[i], ctx );
			}
			_SetResStates( ctx.GetFrameId(), ctx, _resources );
			ctx.CommitBarriers();
		}
		
		// render pass
		{
			RenderPassDesc	rp_desc = _rpDesc;

			for (auto& rt : _renderTargets) {
				rp_desc.AddTarget( rt.name, rt.image->GetViewId(), rt.clear );
			}

			rp_desc.area = RectI{ int2{dim} };
			rp_desc.DefaultViewport();
			
			DescriptorSetID		ds		= _descSets[ ctx.GetFrameId().Index() ];
			auto				dctx	= ctx.BeginRenderPass( rp_desc, DebugLabel{_dbgName, _dbgColor} );
			
			// draw
			{
				for (usize i = 0; i < instances.size(); ++i) {
					instances[i].geometry->Draw( IGeomSource::DrawData{ *_materials[i], dctx, ds });
				}
			}

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
	bool  SceneGraphicsPass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __NE___
	{
		CHECK_ERR( _scene );
		CHECK_ERR( _controller );
		CHECK_ERR( not _renderTargets.empty() );

		for (auto& rt : _renderTargets) {
			rt.image->Resize( ctx );
		}
		_ResizeRes( ctx, _resources );

		// validate dimensions
		{
			const uint2		cur_dim = uint2{ _renderTargets.front().image->GetImageDesc().dimension };
			
			for (auto& rt : _renderTargets)
			{
				const uint2		dim = uint2{ rt.image->GetImageDesc().dimension };
				CHECK_ERR( All( cur_dim == dim ));
			}
		}

		// update uniform buffer
		{
			const auto&		rt		= *_renderTargets[0].image;
			const auto		desc	= rt.GetImageDesc();

			ShaderTypes::ScenePassUB	ub_data;
			
			ub_data.resolution	= float2{desc.dimension};
			ub_data.time		= pd.totalTime.count();
		//	ub_data.timeDelta	= pd.frameTime.count();
		//	ub_data.frame		= pd.frameId;
			
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
			CHECK_ERR( updater.BindBuffer< ShaderTypes::ScenePassUB >( UniformName{"passUB"}, _ubuffer ));
			CHECK_ERR( _BindRes( ctx.GetFrameId(), updater, _resources ));
			CHECK_ERR( updater.Flush() );
		}

		// update materials
		{
			const auto&	instances = _scene->_geomInstances;
			for (usize i = 0; i < instances.size(); ++i)
			{
				float3	pos = instances[i].position - _controller->GetPosition();

				instances[i].geometry->Update( IGeomSource::UpdateData{ *_materials[i], ctx, pos, *_controller, pd });
			}
		}

		return true;
	}
	
/*
=================================================
	destructor
=================================================
*/
	SceneGraphicsPass::~SceneGraphicsPass ()
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( INOUT _ubuffer );
	}
//-----------------------------------------------------------------------------



} // AE::ResEditor
