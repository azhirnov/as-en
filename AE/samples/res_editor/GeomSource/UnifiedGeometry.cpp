// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/UnifiedGeometry.h"
#include "res_editor/Resources/Buffer.h"

#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    destructor
=================================================
*/
    UnifiedGeometry::Material::~Material ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        res_mngr.ReleaseResourceArray( INOUT descSets );
        res_mngr.ReleaseResource( INOUT ubuffer );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    UnifiedGeometry::UnifiedGeometry (Renderer &r) __Th___ :
        IGeomSource{ r }
    {
    }

    UnifiedGeometry::~UnifiedGeometry ()
    {}

/*
=================================================
    StateTransition
=================================================
*/
    void  UnifiedGeometry::StateTransition (IGSMaterials &, DirectCtx::Graphics &ctx) __Th___
    {
        _resources.SetStates( ctx, EResourceState::AllGraphicsShaders );

        for (usize i = 0; i < _drawCommands.size(); ++i)
        {
            Visit( _drawCommands[i],

                [&ctx] (const DrawCmd2 &) {},
                [&ctx] (const DrawIndexedCmd2 &src) {
                    ctx.ResourceState( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndexBuffer );
                },
                [&ctx] (const DrawMeshTasksCmd2 &) {},
                [&ctx] (const DrawIndirectCmd2 &src) {
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                },
                [&ctx] (const DrawIndexedIndirectCmd2 &src) {
                    ctx.ResourceState( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndexBuffer );
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                },
                [&ctx] (const DrawMeshTasksIndirectCmd2 &src) {
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                },
                [&ctx] (const DrawIndirectCountCmd2 &src) {
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                    ctx.ResourceState( src.countBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                },
                [&ctx] (const DrawIndexedIndirectCountCmd2 &src) {
                    ctx.ResourceState( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndexBuffer );
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                    ctx.ResourceState( src.countBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                },
                [&ctx] (const DrawMeshTasksIndirectCountCmd2 &src) {
                    ctx.ResourceState( src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                    ctx.ResourceState( src.countBufferPtr->GetBufferId( ctx.GetFrameId() ), EResourceState::IndirectBuffer );
                });
        }
    }

/*
=================================================
    Draw
=================================================
*/
    bool  UnifiedGeometry::Draw (const DrawData &in) __Th___
    {
        auto&               ctx         = in.ctx;
        auto&               mtr         = RefCast<Material>(in.mtr);
        DescriptorSetID     mtr_ds      = mtr.descSets[ ctx.GetFrameId().Index() ];
        Material::PplnID_t  prev_ppln;

        CHECK( _drawCommands.size() == mtr.pplns.size() );

        const auto      BindPipeline = [&ctx, &prev_ppln] (const auto &pplnId)
        {{
            if ( prev_ppln == pplnId )
                return;

            prev_ppln = pplnId;
            Visit( pplnId,
                [&ctx] (GraphicsPipelineID ppln)    { ctx.BindPipeline( ppln ); },
                [&ctx] (MeshPipelineID ppln)        { ctx.BindPipeline( ppln ); },
                [] (NullUnion)                      { CHECK_MSG( false, "pipeline is not defined" ); }
            );
        }};

        BindPipeline( mtr.pplns.front() );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

        for (usize i = 0; i < _drawCommands.size(); ++i)
        {
            BindPipeline( mtr.pplns[i] );

            Visit( _drawCommands[i],

                [&ctx] (const DrawCmd2 &src)
                {
                    Graphics::DrawCmd   cmd = src;
                    cmd.vertexCount     = src.dynVertexCount ? src.dynVertexCount->Get() : src.vertexCount;
                    cmd.instanceCount   = src.dynInstanceCount ? src.dynInstanceCount->Get() : src.instanceCount;
                    ctx.Draw( cmd );
                },

                [&ctx] (const DrawIndexedCmd2 &src)
                {
                    ctx.BindIndexBuffer( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), src.indexBufferOffset, src.indexType );

                    Graphics::DrawIndexedCmd    cmd = src;
                    cmd.indexCount      = src.dynIndexCount ? src.dynIndexCount->Get() : src.indexCount;
                    cmd.instanceCount   = src.dynInstanceCount ? src.dynInstanceCount->Get() : src.instanceCount;
                    ctx.DrawIndexed( cmd );
                },

                [&ctx] (const DrawMeshTasksCmd2 &src)
                {
                    uint3   task_count = src.dynTaskCount ? src.dynTaskCount->Get() : src.taskCount;
                    ctx.DrawMeshTasks( task_count );
                },

                [&ctx] (const DrawIndirectCmd2 &src)
                {
                    Graphics::DrawIndirectCmd   cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.drawCount       = src.dynDrawCount ? src.dynDrawCount->Get() : src.drawCount;
                    ctx.DrawIndirect( cmd );
                },

                [&ctx] (const DrawIndexedIndirectCmd2 &src)
                {
                    ctx.BindIndexBuffer( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), src.indexBufferOffset, src.indexType );

                    Graphics::DrawIndexedIndirectCmd    cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.drawCount       = src.dynDrawCount ? src.dynDrawCount->Get() : src.drawCount;
                    ctx.DrawIndexedIndirect( cmd );
                },

                [&ctx] (const DrawMeshTasksIndirectCmd2 &src)
                {
                    Graphics::DrawMeshTasksIndirectCmd  cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.drawCount       = src.dynDrawCount ? src.dynDrawCount->Get() : src.drawCount;
                    ctx.DrawMeshTasksIndirect( cmd );
                },

                [&ctx] (const DrawIndirectCountCmd2 &src)
                {
                    Graphics::DrawIndirectCountCmd  cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.countBuffer     = src.countBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.maxDrawCount    = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : src.maxDrawCount;
                    ctx.DrawIndirectCount( cmd );
                },

                [&ctx] (const DrawIndexedIndirectCountCmd2 &src)
                {
                    ctx.BindIndexBuffer( src.indexBufferPtr->GetBufferId( ctx.GetFrameId() ), src.indexBufferOffset, src.indexType );

                    Graphics::DrawIndexedIndirectCountCmd   cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.countBuffer     = src.countBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.maxDrawCount    = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : src.maxDrawCount;
                    ctx.DrawIndexedIndirectCount( cmd );
                },

                [&ctx] (const DrawMeshTasksIndirectCountCmd2 &src)
                {
                    Graphics::DrawMeshTasksIndirectCountCmd cmd = src;
                    cmd.indirectBuffer  = src.indirectBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.countBuffer     = src.countBufferPtr->GetBufferId( ctx.GetFrameId() );
                    cmd.maxDrawCount    = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : src.maxDrawCount;
                    ctx.DrawMeshTasksIndirectCount( cmd );
                });
        }

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  UnifiedGeometry::Update (const UpdateData &in) __Th___
    {
        auto&   ctx = in.ctx;
        auto&   mtr = RefCast<Material>(in.mtr);

        // update uniform buffer
        {
            ShaderTypes::UnifiedGeometryMaterialUB  ub_data;
            ub_data.transform   = in.transform;

            CHECK_ERR( ctx.UploadBuffer( mtr.ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
        }

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));
            CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));
            CHECK_ERR( updater.BindBuffer< ShaderTypes::UnifiedGeometryMaterialUB >( UniformName{"un_PerObject"}, mtr.ubuffer ));

            CHECK_ERR( updater.Flush() );
        }
        return true;
    }


} // AE::ResEditor
