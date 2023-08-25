// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/SphericalCube.h"
#include "res_editor/Passes/Renderer.h"

#include "res_editor/_data/cpp/types.h"
#include "res_editor/_ui_data/cpp/types.h"

namespace AE::ResEditor
{
    STATIC_ASSERT( sizeof(GeometryTools::SphericalCubeRenderer::Vertex) == sizeof(ShaderTypes::SphericalCubeVertex) );

/*
=================================================
    destructor
=================================================
*/
    SphericalCube::Material::~Material ()
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
    SphericalCube::SphericalCube (Renderer &r, uint minLod, uint maxLod, RC<DynamicFloat> tessLevel) __Th___ :
        IGeomSource{ r },
        _tessLevel{ RVRef(tessLevel) },
        _minLod{minLod}, _maxLod{maxLod}
    {}

/*
=================================================
    destructor
=================================================
*/
    SphericalCube::~SphericalCube ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        _cube.Destroy( res_mngr );
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  SphericalCube::StateTransition (IGSMaterials &, GraphicsCtx_t &ctx) __NE___
    {
        _resources.SetStates( ctx, EResourceState::FragmentShader );
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  SphericalCube::StateTransition (IGSMaterials &, RayTracingCtx_t &ctx) __NE___
    {
        _resources.SetStates( ctx, EResourceState::RayTracingShaders );
    }

/*
=================================================
    Draw
=================================================
*/
    bool  SphericalCube::Draw (const DrawData &in) __NE___
    {
        auto&           ctx     = in.ctx;
        auto&           mtr     = RefCast<Material>(in.mtr);
        DescriptorSetID mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

        const auto      BindPipeline = [&ctx] (const auto &pplnId)
        {{
            Visit( pplnId,
                [&ctx] (GraphicsPipelineID ppln)    { ctx.BindPipeline( ppln ); },
                [&ctx] (MeshPipelineID ppln)        { ctx.BindPipeline( ppln ); },
                [] (NullUnion)                      { CHECK_MSG( false, "pipeline is not defined" ); }
            );
        }};

        BindPipeline( mtr.ppln );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

        CHECK_ERR( _cube.Draw( ctx, _maxLod ));
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  SphericalCube::Update (const UpdateData &in) __NE___
    {
        auto&   ctx = in.ctx;
        auto&   mtr = RefCast<Material>(in.mtr);

        if_unlikely( not _cube.IsCreated() )
        {
            CHECK_ERR( _cube.Create( ctx.GetResourceManager(), ctx, _minLod, _maxLod, False{}, Default, _GfxAllocator() ));

            auto&   rstate = RenderGraph().GetStateTracker();
            rstate.SetDefaultState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            rstate.SetDefaultState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );

            ctx.ResourceState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            ctx.ResourceState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );
        }

        // update uniform buffer
        {
            ShaderTypes::SphericalCubeMaterialUB    ub_data;
            ub_data.transform   = float4x4::Translated( in.position );
            ub_data.tessLevel   = _tessLevel ? _tessLevel->Get() : 1.f;

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
