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
    SphericalCube::SphericalCube (Renderer &r, uint minLod, uint maxLod) __Th___ :
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
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        _cube.Destroy( res_mngr );
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  SphericalCube::StateTransition (IGSMaterials &inMtr, GraphicsCtx_t &ctx) __NE___
    {
        auto&   mtr = Cast<Material>(inMtr);

        for (auto& [un, tex] : mtr.textures)
        {
            const auto  new_state = EResourceState::ShaderSample | EResourceState::FragmentShader;
            ctx.ResourceState( tex->GetImageId(), new_state );
        }
    }

/*
=================================================
    Draw
=================================================
*/
    bool  SphericalCube::Draw (const DrawData &in) __NE___
    {
        auto&           ctx     = in.ctx;
        auto&           mtr     = Cast<Material>(in.mtr);
        DescriptorSetID mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

        ctx.BindPipeline( mtr.ppln );
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
        auto&   mtr = Cast<Material>(in.mtr);

        if_unlikely( not _cube.IsCreated() )
        {
            CHECK_ERR( _cube.Create( ctx.GetResourceManager(), ctx, _minLod, _maxLod, false, _GfxAllocator() ));

            auto&   rstate = FrameGraph().GetStateTracker();
            rstate.SetDefaultState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            rstate.SetDefaultState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );

            ctx.ResourceState( _cube.VertexBufferId(), EResourceState::VertexBuffer );
            ctx.ResourceState( _cube.IndexBufferId(),  EResourceState::IndexBuffer );
        }

        // update uniform buffer
        {
            ShaderTypes::SphericalCubeMaterialUB    ub_data;
            ub_data.transform   = float4x4::Translate( in.position );

            CHECK_ERR( ctx.UploadBuffer( mtr.ubuffer, 0_b, Sizeof(ub_data), &ub_data ));
        }

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));

            for (auto& [un, tex] : mtr.textures) {
                CHECK_ERR( updater.BindImage( un, tex->GetViewId() ));
            }
            CHECK_ERR( updater.BindBuffer< ShaderTypes::SphericalCubeMaterialUB >( UniformName{"mtrUB"}, mtr.ubuffer ));

            CHECK_ERR( updater.Flush() );
        }
        return true;
    }


} // AE::ResEditor
