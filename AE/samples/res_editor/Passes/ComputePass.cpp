// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/ComputePass.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	GroupCount
=================================================
*/
	uint3  ComputePass::Iteration::GroupCount (const uint3 &localSize) const
	{
		uint3	dim;
		Visit( count,
			[&dim] (const uint3 &src)			{ dim = src; },
			[&dim] (const RC<DynamicDim> &src)	{ dim = src->Dimension3(); },
			[&dim] (const RC<DynamicUInt> &src)	{ dim.x = src->Get(); },
			[&dim] (const RC<DynamicUInt2> &src){ dim = uint3{src->Get(), 1u}; },
			[&dim] (const RC<DynamicUInt3> &src){ dim = src->Get(); });

		return Max( isGroups ? dim : DivCeil( dim, localSize ), 1u );
	}

/*
=================================================
	ThreadCount
=================================================
*/
	uint3  ComputePass::Iteration::ThreadCount (const uint3 &localSize) const
	{
		uint3	dim;
		Visit( count,
			[&dim] (const uint3 &src)			{ dim = src; },
			[&dim] (const RC<DynamicDim> &src)	{ dim = src->Dimension3(); },
			[&dim] (const RC<DynamicUInt> &src)	{ dim.x = src->Get(); },
			[&dim] (const RC<DynamicUInt2> &src){ dim = uint3{src->Get(), 1u}; },
			[&dim] (const RC<DynamicUInt3> &src){ dim = src->Get(); });

		return Max( isGroups ? dim * localSize : dim, 1u );
	}

/*
=================================================
	FindMaxConstThreadCount
=================================================
*/
	uint3  ComputePass::Iteration::FindMaxConstThreadCount (ArrayView<Iteration> arr, const uint3 &localSize)
	{
		uint3	max_dim;

		for (auto& item : arr)
		{
			const uint3	dim = item.ThreadCount( localSize );

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
	bool  ComputePass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		CHECK_ERR( not _resources.Empty() );
		CHECK_ERR( not _iterations.empty() );

		ShaderDebugger::Result	dbg;
		ComputePipelineID		ppln;

		if ( pd.dbg.IsEnabled( this ))
		{
			auto	it = _pipelines.find( pd.dbg.mode );

			if ( it != _pipelines.end()							and
				 AnyBits( pd.dbg.stage, EShaderStages::Compute ))
			{
				// TODO: dispatch indirect?
				const uint2		dim	= uint2{Iteration::FindMaxConstThreadCount( _iterations, _localSize )};

				ppln = it->second;

				DirectCtx::Transfer		tctx{ pd.rtask, RVRef(pd.cmdbuf) };
				CHECK( pd.dbg.debugger->AllocForCompute( OUT dbg, tctx, ppln, uint3{uint2{pd.dbg.coord * float2{dim} + 0.5f}, 0u }));
				pd.cmdbuf = tctx.ReleaseCommandBuffer();
			}
		}

		if ( not dbg )
			ppln = _pipelines.find( IPass::EDebugMode::Unknown )->second;

		DirectCtx::Compute	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };
		DescriptorSetID		ds	= _descSets[ ctx.GetFrameId().Index() ];

		_resources.SetStates( ctx, Default );
		ctx.CommitBarriers();

		ctx.BindPipeline( ppln );
		ctx.BindDescriptorSet( _dsIndex, ds );
		if ( dbg ) ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

		ShaderTypes::ComputePassPC	pc;
		pc.dispatchIndex = 0;

		for (const auto& it : _iterations)
		{
			ctx.PushConstant( _pcIndex, pc );
			pc.dispatchIndex++;

			if ( it.indirect ){
				ctx.DispatchIndirect( it.indirect->GetBufferId( ctx.GetFrameId() ), it.indirectOffset );
			}else{
				ctx.Dispatch( it.GroupCount( _localSize ));
			}

			if ( not IsLastElement( it, _iterations ))
			{
				ctx.ExecutionBarrier( EPipelineScope::Compute, EPipelineScope::Compute );
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
	bool  ComputePass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
	{
		CHECK_ERR( not _resources.Empty() );
		CHECK_ERR( not _iterations.empty() );

		// update uniform buffer
		{
			ShaderTypes::ComputePassUB	ub_data;
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
			CHECK_ERR( updater.BindBuffer< ShaderTypes::ComputePassUB >( UniformName{"un_PerPass"}, _ubuffer ));
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
	void  ComputePass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		_resources.GetResourcesToResize( INOUT resources );
	}

/*
=================================================
	destructor
=================================================
*/
	ComputePass::~ComputePass ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT _descSets );
		res_mngr.ReleaseResource( _ubuffer );
	}


} // AE::ResEditor
