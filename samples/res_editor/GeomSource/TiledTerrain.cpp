// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/TiledTerrain.h"
#include "res_editor/Passes/Renderer.h"
#include "res_editor/Passes/PassGroup.h"
#include "res_editor/Resources/Image.h"

#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    destructor
=================================================
*/
    TiledTerrain::Material::~Material ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        res_mngr.ReleaseResources( INOUT ubuffer, INOUT descSet );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    TileKey::operator == / <
=================================================
*/
    bool  TiledTerrain::TileKey::operator == (const TileKey &rhs) C_NE___
    {
        return  lod == rhs.lod          and
                All( coord == rhs.coord );
    }
    bool  TiledTerrain::TileKey::operator <  (const TileKey &rhs) C_NE___
    {
        return  lod     != rhs.lod      ? lod       < rhs.lod       :
                coord.x != rhs.coord.x  ? coord.x   < rhs.coord.x   :
                                          coord.y   < rhs.coord.y;
    }
//-----------------------------------------------------------------------------



    //
    // Copy Layers Pass
    //
    class TiledTerrain::CopyLayersPass final : public IPass
    {
    // variables
    private:
        TiledTerrain &      _self;
    public:
        uint                dstLayer    = 0;

    // methods
    public:
        explicit CopyLayersPass (TiledTerrain &self) : _self{self} {}

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return "CopyLayersPass"; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
    };

/*
=================================================
    Execute
=================================================
*/
    bool  TiledTerrain::CopyLayersPass::Execute (SyncPassData &pd) __NE___
    {
        RG::DirectCtx::Transfer     ctx{ pd.rtask, RVRef(pd.cmdbuf) };

        for (auto& [name, dst_img_view, src_img] : _self._layers)
        {
            ctx.ResourceState( src_img->GetImageId(), EResourceState::CopySrc );
            ctx.ResourceState( dst_img_view.image, EResourceState::CopyDst );
        }
        ctx.CommitBarriers();

        for (auto& [name, dst_img_view, src_img] : _self._layers)
        {
            ImageCopy   range;
            range.srcOffset     = uint3{0};
            range.srcSubres     = { EImageAspect::Color, 0_mipmap, 0_layer, 1 };
            range.dstOffset     = uint3{0};
            range.dstSubres     = { EImageAspect::Color, 0_mipmap, ImageLayer{dstLayer}, 1 };
            range.extent        = src_img->GetImageDesc().dimension;

            ctx.CopyImage( src_img->GetImageId(), dst_img_view.image, {range} );

            // TODO: mipmaps
        }

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  TiledTerrain::CopyLayersPass::Update (TransferCtx_t &, const UpdatePassData &) __NE___
    {
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    TiledTerrain::TiledTerrain (Renderer &r, RC<PassGroup> generator, EMode mode, uint vertsPerEdge, RC<DynamicFloat4> dynRegion) __Th___ :
        IGeomSource{ r },
        _generator{RVRef(generator)}, _copyPass{ new CopyLayersPass{ *this }}, _dynRegion{RVRef(dynRegion)},
        _mode{mode}, _vertsPerEdge{vertsPerEdge}
    {
        CHECK_THROW( _generator );
        CHECK_THROW( _mode < EMode::_Count );

        _generator->AddPass( _copyPass );  // throw
    }

/*
=================================================
    destructor
=================================================
*/
    TiledTerrain::~TiledTerrain ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        for (auto& [name, img_view, img] : _layers) {
            res_mngr.ReleaseResources( img_view.image, img_view.view );
        }
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  TiledTerrain::StateTransition (IGSMaterials &, GraphicsCtx_t &ctx) __NE___
    {
        const auto  state = EResourceState::ShaderSample | EResourceState::AllGraphicsShaders;

        for (auto& [name, img_view, src_img] : _layers) {
            ctx.ResourceState( img_view.image, state );
        }
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  TiledTerrain::StateTransition (IGSMaterials &, RayTracingCtx_t &ctx) __NE___
    {
        const auto  state = EResourceState::ShaderSample | EResourceState::AllGraphicsShaders;

        for (auto& [name, img_view, src_img] : _layers) {
            ctx.ResourceState( img_view.image, state );
        }
    }

/*
=================================================
    Draw
=================================================
*/
    bool  TiledTerrain::Draw (const DrawData &in) __NE___
    {
        auto&   ctx = in.ctx;
        auto&   mtr = RefCast<Material>(in.mtr);

        ctx.BindPipeline( mtr.ppln );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr.descSet );

        _grid.Draw( ctx );
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  TiledTerrain::Update (const UpdateData &in) __NE___
    {
        auto&   ctx = in.ctx;
        //auto& mtr = Cast<Material>(in.mtr);

        if_unlikely( _grid.IsCreated() )
        {
            auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
            CHECK_ERR( _grid.Create( res_mngr, ctx, _GfxAllocator(), _vertsPerEdge, 3 ));
        }

        // TODO
        {
            const float scale   = 1.f;
            uint        layer   = 0;

            for (int y = -2; y <= 2; ++y)
            for (int x = -2; x <= 2; ++x)
            {
                if ( _tileMap.emplace( TileKey{ 0, x, y }, TileInfo{ in.pd.frameId, ubyte(layer) }).second )
                {
                    auto&   req = _requests.emplace_back();
                    req.layer   = layer;
                    req.min     = float2{x,  y  } * scale;
                    req.max     = float2{x+1,y+1} * scale;

                    ++layer;
                }
            }
        }

        for (; not _requests.empty();)
        {
            const LayerRequest  req = _requests.ExtractFront();
            _RequestUpdate( req );
        }
        return true;
    }

/*
=================================================
    _RequestUpdate
=================================================
*/
    void  TiledTerrain::_RequestUpdate (const LayerRequest &req)
    {
        _generator->RequestUpdate();
        _copyPass->dstLayer = req.layer;
        _dynRegion->Set( float4{ req.min, req.max });
    }


} // AE::ResEditor
