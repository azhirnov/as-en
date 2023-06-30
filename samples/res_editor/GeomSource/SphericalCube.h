// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/GeomSource/IGeomSource.h"
#include "geometry_tools/SphericalCube/SphericalCubeRenderer.h"

namespace AE::ResEditor
{

    //
    // Spherical Cube Geometry Source
    //

    class SphericalCube final : public IGeomSource
    {
        friend class ScriptSphericalCube;

    // types
    private:
        class Material final : public IGSMaterials
        {
        // variables
        public:
            RenderTechPipelinesPtr      rtech;

            GraphicsPipelineID          ppln;
            PerFrameDescSet_t           descSets;

            DescSetBinding              passDSIndex;
            DescSetBinding              mtrDSIndex;

            Textures_t                  textures;
            Strong<BufferID>            ubuffer;


        // methods
        public:
            Material () {}
            ~Material ();
        };


    // variables
    private:
        GeometryTools::SphericalCubeRenderer    _cube;

        const uint                              _minLod     = 0;
        const uint                              _maxLod     = 0;


    // methods
    public:
        SphericalCube (Renderer &r, uint minLod, uint maxLod)       __Th___;
        ~SphericalCube ();


    // IGeomSource //
        void  StateTransition (IGSMaterials &, GraphicsCtx_t &)     __NE_OV;
        bool  Draw (const DrawData &)                               __NE_OV;
        bool  Update (const UpdateData &)                           __NE_OV;
    };


} // AE::ResEditor
