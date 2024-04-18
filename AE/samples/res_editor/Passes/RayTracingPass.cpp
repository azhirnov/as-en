// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/RayTracingPass.h"
#include "res_editor/Resources/ResourceArray.cpp.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	Dimension
=================================================
*/
	uint3  RayTracingPass::Iteration::Dimension () const
	{
		uint3	res;
		Visit( dim,
			[&res] (const uint3 &src)			{ res = src; },
			[&res] (const RC<DynamicDim> &src)	{ res = src->Dimension3(); },
			[&res] (const RC<DynamicUInt> &src)	{ res.x = src->Get(); },
			[&res] (const RC<DynamicUInt3> &src){ res = src->Get(); });

		return Max( res, 1u );
	}

/*
=================================================
	FindMaxConstDimension
=================================================
*/
	uint3  RayTracingPass::Iteration::FindMaxConstDimension (ArrayView<Iteration> arr)
	{
		uint3	max_dim;

		for (auto& item : arr)
		{
			const uint3	dim = item.Dimension();

			if ( (dim.x > max_dim.x)												or
				 (dim.x == max_dim.x and dim.y > max_dim.y)							or
				 (dim.x == max_dim.x and dim.y == max_dim.y and dim.z > max_dim.z)	)
				max_dim = dim;
		}
		return max_dim;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Execute
=================================================
*/
	bool  RayTracingPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( not _resources.Empty() );
		CHECK_ERR( not _iterations.empty() );

		ShaderDebugger::Result	dbg;
		RayTracingPipelineID	ppln;
		RTShaderBindingID		sbt;

		if ( pd.dbg.IsEnabled( this ))
		{
			auto	it = _pipelines.find( pd.dbg.mode );

			if ( it != _pipelines.end()							and
				 AnyBits( pd.dbg.stage, EShaderStages::AllRayTracing ))
			{
				// TODO: dispatch indirect?
				const uint2		dim	= uint2{Iteration::FindMaxConstDimension( _iterations )};

				ppln = it->second.Get<0>();
				sbt  = it->second.Get<1>();

				DirectCtx::Transfer		tctx{ pd.rtask, RVRef(pd.cmdbuf) };
				CHECK( pd.dbg.debugger->AllocForRayTracing( OUT dbg, tctx, ppln, uint3{uint2{pd.dbg.coord * float2{dim} + 0.5f}, 0u }));
				pd.cmdbuf = tctx.ReleaseCommandBuffer();
			}
		}

		if ( not dbg )
		{
			auto	it = _pipelines.find( IPass::EDebugMode::Unknown );
			ppln = it->second.Get<0>();
			sbt  = it->second.Get<1>();
		}

		DirectCtx::RayTracing	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };
		DescriptorSetID			ds	= _descSets[ ctx.GetFrameId().Index() ];

		_resources.SetStates( ctx, Default );
		ctx.ResourceState( _ubuffer, EResourceState::UniformRead | EResourceState::RayTracingShaders );
		ctx.CommitBarriers();

		ctx.BindPipeline( ppln );
		ctx.BindDescriptorSet( _dsIndex, ds );
		if ( dbg ) ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

		// from Vulkan specs:
		// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#ray-tracing-pipeline-stack
		{
			const int	max_ray_recursion	= _maxRayRecursion ? _maxRayRecursion->Get() : 31;
			const uint	max_call_recursion	= _maxCallRecursion ? _maxCallRecursion->Get() : 2;

			const Bytes	stack_size =
					uint(Min( 1, max_ray_recursion )) * Bytes{Max( _closestHitStackMax, _missStackMax, _intersectionStackMax, _anyHitStackMax )} +
					uint(Max( 0, max_ray_recursion-1 )) * Bytes{Max( _closestHitStackMax, _missStackMax )} +
					Max( 2u, max_call_recursion ) * Bytes{_callableStackMax} +
					_rayGenStackMax;

			ctx.SetStackSize( stack_size );
		}

		for (const auto& it : _iterations)
		{
			if ( it.indirect ){
				ctx.TraceRaysIndirect( sbt, it.indirect->GetBufferId( ctx.GetFrameId() ), it.indirectOffset );
			}else{
				ctx.TraceRays( it.Dimension(), sbt );
			}

			if ( not IsLastElement( it, _iterations ))
			{
				ctx.ExecutionBarrier( EPipelineScope::RayTracing, EPipelineScope::RayTracing );
				ctx.CommitBarriers();
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
	bool  RayTracingPass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( not _resources.Empty() );
		CHECK_ERR( not _iterations.empty() );

		// update uniform buffer
		{
			ShaderTypes::RayTracingPassUB	ub_data;
			ub_data.time		= pd.totalTime.count();
			ub_data.timeDelta	= pd.frameTime.count();
			ub_data.frame		= _dynData.frame;
			ub_data.seed		= pd.seed;
			ub_data.mouse		= pd.pressed ? float4{ pd.unormCursorPos.x, pd.unormCursorPos.y, 1.f, 0.f } : float4{-1.0e+20f};
			ub_data.customKeys	= pd.customKeys[0];

			if ( _controller )
				_controller->CopyTo( OUT ub_data.camera );

			_CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
			_CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

			++_dynData.frame;
			CHECK_ERR( ctx.UploadBuffer( _ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
		}

		// update descriptors
		{
			DescriptorUpdater	updater;
			DescriptorSetID		ds		= _descSets[ ctx.GetFrameId().Index() ];

			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::RayTracingPassUB >( UniformName{"un_PerPass"}, _ubuffer ));
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
	void  RayTracingPass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		_resources.GetResourcesToResize( INOUT resources );
	}

/*
=================================================
	destructor
=================================================
*/
	RayTracingPass::~RayTracingPass ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( _ubuffer );
	}


} // AE::ResEditor
