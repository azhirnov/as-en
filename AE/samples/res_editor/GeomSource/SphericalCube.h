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
        using PplnID_t      = Union< NullUnion, GraphicsPipelineID, MeshPipelineID >;
        using PipelineMap_t = FixedMap< EDebugMode, PplnID_t, uint(EDebugMode::_Count) >;

        class Material final : public IGSMaterials
        {
        // variables
        public:
            RenderTechPipelinesPtr      rtech;

            PipelineMap_t               pplnMap;
            PerFrameDescSet_t           descSets;

            DescSetBinding              passDSIndex;
            DescSetBinding              mtrDSIndex;

            Strong<BufferID>            ubuffer;


        // methods
        public:
            Material ()     __NE___ {}
            ~Material ();

            DebugModeBits  GetDebugModeBits ()  C_NE_OV;
        };


    // variables
    private:
        GeometryTools::SphericalCubeRenderer    _cube;
        ResourceArray                           _resources;

        const uint                              _minLod     = 0;
        const uint                              _maxLod     = 0;


    // methods
    public:
        SphericalCube (Renderer &r, uint minLod, uint maxLod)                       __NE___;
        ~SphericalCube ();


    // IGeomSource //
        void  PrepareForDebugging (IGSMaterials &, DirectCtx::Transfer &,
                                   const Debugger &, OUT ShaderDebugger::Result &)  __Th_OV;
        void  StateTransition (IGSMaterials &, DirectCtx::Graphics &)               __Th_OV;
        using IGeomSource::StateTransition;

        bool  Draw (const DrawData &)                                               __Th_OV;
        bool  Update (const UpdateData &)                                           __Th_OV;
    };


} // AE::ResEditor
