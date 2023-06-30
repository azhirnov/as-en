// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/ComputePass.h"
#include "res_editor/Passes/IPass.cpp.h"
#include "res_editor/EditorUI.h"
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
        uint3   dim;
        Visit( count,
            [&dim] (const uint3 &src)           { dim = src; },
            [&dim] (const RC<DynamicDim> &src)  { dim = src->Dimension3(); },
            [&dim] (const RC<DynamicUInt> &src) { dim.x = src->Get(); },
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
        uint3   dim;
        Visit( count,
            [&dim] (const uint3 &src)           { dim = src; },
            [&dim] (const RC<DynamicDim> &src)  { dim = src->Dimension3(); },
            [&dim] (const RC<DynamicUInt> &src) { dim.x = src->Get(); },
            [&dim] (const RC<DynamicUInt3> &src){ dim = src->Get(); });

        return Max( isGroups ? dim * localSize : dim, 1u );
    }

/*
=================================================
    FindDynamicThreadCount
=================================================
*/
    RC<DynamicDim>  ComputePass::Iteration::FindDynamicThreadCount (ArrayView<Iteration> arr)
    {
        for (auto& item : arr)
        {
            if ( not item.isGroups and HoldsAlternative< RC<DynamicDim> >( item.count ))
                return *UnionGet< RC<DynamicDim> >( item.count );
        }
        return null;
    }

/*
=================================================
    FindMaxConstThreadCount
=================================================
*/
    uint3  ComputePass::Iteration::FindMaxConstThreadCount (ArrayView<Iteration> arr, const uint3 &localSize)
    {
        uint3   max_dim;

        for (auto& item : arr)
        {
            const uint3 dim = item.ThreadCount( localSize );

            if ( (dim.x > max_dim.x)                                                or
                 (dim.x == max_dim.x and dim.y > max_dim.y)                         or
                 (dim.x == max_dim.x and dim.y == max_dim.y and dim.z > max_dim.z)  )
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
    bool  ComputePass::Execute (SyncPassData &pd) __NE___
    {
        CHECK_ERR( not _resources.empty() );
        CHECK_ERR( not _iterations.empty() );

        ShaderDebugger::Result  dbg;
        ComputePipelineID       ppln;
        const uint2             dim     = uint2{_iterations.front().ThreadCount( _localSize )};

        if ( pd.dbg.IsEnabled( this ))
        {
            auto    it = _pipelines.find( pd.dbg.mode );

            if ( it != _pipelines.end()                         and
                 AnyBits( pd.dbg.stage, EShaderStages::Compute ))
            {
                ppln = it->second;

                RG::DirectCtx::Transfer     tctx{ pd.rtask, RVRef(pd.cmdbuf) };
                CHECK( pd.dbg.debugger->AllocForCompute( OUT dbg, tctx, ppln, uint3{uint2{pd.dbg.coord * float2{dim} + 0.5f}, 0u }));
                pd.cmdbuf = tctx.ReleaseCommandBuffer();
            }
        }

        if ( not dbg )
            ppln = _pipelines.find( IPass::EDebugMode::Unknown )->second;

        DirectCtx::Compute  ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, _dbgColor} };
        DescriptorSetID     ds  = _descSets[ ctx.GetFrameId().Index() ];

        _SetResStates( ctx.GetFrameId(), ctx, _resources );
        ctx.CommitBarriers();

        ctx.BindPipeline( ppln );
        ctx.BindDescriptorSet( _dsIndex, ds );
        if ( dbg ) ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );

        for (auto it : _iterations)
        {
            const uint3     group_count = it.GroupCount( _localSize );

            ctx.Dispatch( group_count );

            ctx.ExecutionBarrier( EPipelineScope::Compute, EPipelineScope::Compute );
            ctx.CommitBarriers();
        }

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  ComputePass::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __NE___
    {
        CHECK_ERR( not _resources.empty() );
        CHECK_ERR( not _iterations.empty() );

        _ResizeRes( ctx, _resources );

        // update uniform buffer
        {
            ShaderTypes::ComputePassUB  ub_data;
            ub_data.time        = pd.totalTime.count();
            ub_data.timeDelta   = pd.frameTime.count();
            ub_data.frame       = _dynData.frame;
            ub_data.mouse       = pd.pressed ? float4{ pd.cursorPos.x, pd.cursorPos.y, 0.f, 0.f } : float4{-1.0e+20f};

            if ( _controller )
                _controller->CopyTo( OUT ub_data.camera );

            _CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
            _CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

            ++_dynData.frame;
            CHECK_ERR( ctx.UploadBuffer( _ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
        }

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     ds      = _descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
            CHECK_ERR( updater.BindBuffer< ShaderTypes::ComputePassUB >( UniformName{"ub"}, _ubuffer ));
            CHECK_ERR( _BindRes( ctx.GetFrameId(), updater, _resources ));
            CHECK_ERR( updater.Flush() );
        }

        return true;
    }

/*
=================================================
    destructor
=================================================
*/
    ComputePass::~ComputePass ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        res_mngr.ReleaseResourceArray( INOUT _descSets );
        res_mngr.ReleaseResource( _ubuffer );
    }


} // AE::ResEditor
