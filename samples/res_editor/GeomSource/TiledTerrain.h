// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/GeomSource/IGeomSource.h"
#include "geometry_tools/Grid/GridRenderer.h"

namespace AE::ResEditor
{

    //
    // Tiled/Chunked Terrain 2D/3D Geometry Source
    //

    class TiledTerrain final : public IGeomSource
    {
        friend class ScriptTiledTerrain;

    // types
    public:
        enum class EMode : uint
        {
            Tile2D,
            Chunk3D,
        //  Chunk3D_WithSkirt,
            _Count
        };

    private:
        using ImageLayers_t = Array< Tuple< UniformName, StrongImageAndViewID, RC<Image> >>;


        class Material final : public IGSMaterials
        {
        // variables
        public:
            RenderTechPipelinesPtr      rtech;

            GraphicsPipelineID          ppln;
            Strong<DescriptorSetID>     descSet;

            DescSetBinding              passDSIndex;
            DescSetBinding              mtrDSIndex;

            Strong<BufferID>            ubuffer;


        // methods
        public:
            Material () {}
            ~Material ();
        };

        class CopyLayersPass;

        static constexpr uint   LayerCount  = 128;


        struct TileKey
        {
            ubyte           lod         = 0;
            packed_short2   coord;

            TileKey () {}
            TileKey (int lod, int x, int y) : lod{ubyte(lod)}, coord{x,y} {}

            ND_ bool  operator == (const TileKey &rhs)  C_NE___;
            ND_ bool  operator <  (const TileKey &rhs)  C_NE___;
        };

        struct TileInfo
        {
            uint            frameId     = UMax;     // last update
            ubyte           layer       = UMax;
        };

        using TileMap_t = FixedMap< TileKey, TileInfo, LayerCount >;


        struct LayerRequest
        {
            uint        layer;
            float2      min;
            float2      max;
        };
        using Requests_t = RingBuffer< LayerRequest >;


    // variables
    private:
        GeometryTools::GridRenderer _grid;

        RC<PassGroup>               _generator;
        RC<CopyLayersPass>          _copyPass;
        RC<DynamicFloat4>           _dynRegion;

        ImageLayers_t               _layers;        // image2DArray

        TileMap_t                   _tileMap;
        Requests_t                  _requests;

        const EMode                 _mode;
        const uint                  _vertsPerEdge;



    // methods
    public:
        TiledTerrain (Renderer &r, RC<PassGroup>, EMode, uint vertsPerEdge, RC<DynamicFloat4> dynRegion) __Th___;
        ~TiledTerrain ();


    // IGeomSource //
        void  StateTransition (IGSMaterials &, GraphicsCtx_t &)     __NE_OV;
        void  StateTransition (IGSMaterials &, RayTracingCtx_t &)   __NE_OV;

        bool  Draw (const DrawData &)                               __NE_OV;
        bool  Update (const UpdateData &)                           __NE_OV;

    private:
        void  _RequestUpdate (const LayerRequest &req);
    };


} // AE::ResEditor
