// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/SphericalCube.h"
#include "res_editor/Passes/Renderer.h"

#include "res_editor/_data/cpp/types.h"
#include "res_editor/_ui_data/cpp/types.h"

namespace AE::ResEditor
{
    StaticAssert( sizeof(GeometryTools::SphericalCubeRenderer::Vertex) == sizeof(ShaderTypes::SphericalCubeVertex) );

/*
=================================================
    destructor
=================================================
*/
    SphericalCube::Material::~Material ()
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        res_mngr.ReleaseResourceArray( INOUT descSets );
        res_mngr.ReleaseResource( INOUT ubuffer );
    }

/*
=================================================
    GetDebugModeBits
=================================================
*/
    IGSMaterials::DebugModeBits  SphericalCube::Material::GetDebugModeBits () C_NE___
    {
        DebugModeBits   result;
        for (auto [mode, ppln] : pplnMap) {
            result.insert( mode );
        }
        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    SphericalCube::SphericalCube (Renderer &r, uint minLod, uint maxLod) __NE___ :
        IGeomSource{ r },
        _minLod{minLod}, _maxLod{maxLod}
    {}

/*
=================================================
    destructor
=================================================
*/
    SphericalCube::~SphericalCube ()
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        _cube.Destroy( res_mngr );
    }

/*
=================================================
    PrepareForDebugging
=================================================
*/
    void  SphericalCube::PrepareForDebugging (IGSMaterials &inMtr, DirectCtx::Transfer &ctx,
                                              const Debugger &dbg, OUT ShaderDebugger::Result &dbgStorage) __Th___
    {
        ASSERT( dbg.IsEnabled() );

        auto&   mtr = RefCast<Material>(inMtr);
        auto    it  = mtr.pplnMap.find( dbg.mode );

        if ( it == mtr.pplnMap.end() )
            return;

        Visit( it->second,
            [&] (GraphicsPipelineID ppln)   { CHECK( dbg.debugger->AllocForGraphics( OUT dbgStorage, ctx, ppln )); },
            [&] (MeshPipelineID ppln)       { CHECK( dbg.debugger->AllocForGraphics( OUT dbgStorage, ctx, ppln )); },
            [] (NullUnion)                  { CHECK_MSG( false, "pipeline is not defined" ); }
        );
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  SphericalCube::StateTransition (IGSMaterials &, DirectCtx::Graphics &ctx) __Th___
    {
        ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::UniformRead | EResourceState::AllGraphicsShaders );

        _resources.SetStates( ctx, EResourceState::AllGraphicsShaders );
    }

/*
=================================================
    Draw
=================================================
*/
    bool  SphericalCube::Draw (const DrawData &in) __Th___
    {
        auto&           ctx     = in.ctx;
        auto&           mtr     = RefCast<Material>(in.mtr);
        DescriptorSetID mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];
        PplnID_t        ppln;
        bool            use_debugger = false;

        const auto      BindPipeline = [&ctx] (const auto &pplnId)
        {{
            Visit( pplnId,
                [&ctx] (GraphicsPipelineID ppln)    { ctx.BindPipeline( ppln ); },
                [&ctx] (MeshPipelineID ppln)        { ctx.BindPipeline( ppln ); },
                [] (NullUnion)                      { CHECK_MSG( false, "pipeline is not defined" ); }
            );
        }};

        if ( in.IsDebuggerEnabled() )
        {
            auto    it = mtr.pplnMap.find( in.dbgMode );
            if ( it != mtr.pplnMap.end() )
            {
                use_debugger = true;
                ppln = it->second;
            }
        }
        if ( IsNullUnion( ppln ))
            ppln = mtr.pplnMap.find( IPass::EDebugMode::Unknown )->second;

        BindPipeline( ppln );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );
        if ( use_debugger ) ctx.BindDescriptorSet( in.dbgStorage->DSIndex(), in.dbgStorage->DescSet() );

        CHECK_ERR( _cube.Draw( ctx, _maxLod ));
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  SphericalCube::Update (const UpdateData &in) __Th___
    {
        auto&   ctx = in.ctx;
        auto&   mtr = RefCast<Material>(in.mtr);

        if_unlikely( not _cube.IsCreated() )
        {
            CHECK_ERR( _cube.Create( ctx.GetResourceManager(), ctx, _minLod, _maxLod, False{"tris"}, True{"cubeMap"}, Default, _GfxAllocator() ));

            auto&   rstate = RenderGraph().GetStateTracker();
            rstate.SetDefaultState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            rstate.SetDefaultState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );

            ctx.ResourceState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            ctx.ResourceState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );
        }

        // update uniform buffer
        {
            ShaderTypes::SphericalCubeMaterialUB    ub_data;
            ub_data.transform   = in.transform;
            ub_data.normalMat   = float3x3{in.transform}.Inverse();

            CHECK_ERR( ctx.UploadBuffer( mtr.ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
        }

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));
            CHECK_ERR( _resources.Bind( ctx.GetFrameId(), updater ));
            CHECK_ERR( updater.BindBuffer< ShaderTypes::SphericalCubeMaterialUB >( UniformName{"un_PerObject"}, mtr.ubuffer ));

            CHECK_ERR( updater.Flush() );
        }
        return true;
    }


} // AE::ResEditor
