// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Passes/IPass.h"
#include "res_editor/Dynamic/DynamicDimension.h"

namespace AE::ResEditor
{

    //
    // Geometry Source Materials interface
    //
    class IGSMaterials : public EnableRC<IGSMaterials>
    {
    // types
    protected:
        using PerFrameDescSet_t = StaticArray< Strong<DescriptorSetID>, GraphicsConfig::MaxFrames >;


    // methods
    public:
    };



    //
    // Geometry Source interface
    //
    class IGeomSource : public EnableRC<IGeomSource>
    {
    // types
    public:
        using GraphicsCtx_t     = DirectCtx::Graphics;
        using RayTracingCtx_t   = DirectCtx::RayTracing;
        using UpdatePassData    = IPass::UpdatePassData;

        struct UpdateData
        {
            IGSMaterials &          mtr;
            DirectCtx::Transfer &   ctx;
            float3                  position;
            IController const&      controller;
            UpdatePassData const&   pd;
        };

        struct UpdateRTData
        {
            IGSMaterials &          mtr;
            DirectCtx::ASBuild &    ctx;
        };

        struct DrawData
        {
            IGSMaterials &          mtr;
            DirectCtx::Draw &       ctx;
            DescriptorSetID         passDS;
            float3                  position;
        };


    // variables
    private:
        Renderer &      _renderer;


    // methods
    protected:
        explicit IGeomSource (Renderer &r) : _renderer{r} {}

    public:
        virtual void  StateTransition (IGSMaterials &, GraphicsCtx_t &)     __NE___ = 0;
        virtual void  StateTransition (IGSMaterials &, RayTracingCtx_t &)   __NE___ = 0;

        virtual bool  Draw (const DrawData &)                               __NE___ = 0;
        virtual bool  Update (const UpdateData &)                           __NE___ = 0;
        virtual bool  RTUpdate (const UpdateRTData &)                       __NE___ { return false; }

        ND_ Renderer&           _Renderer ()                                const   { return _renderer; }
        ND_ ResourceQueue&      _ResQueue ()                                const;
        ND_ GfxMemAllocatorPtr  _GfxAllocator ()                            const;
    };


} // AE::ResEditor
