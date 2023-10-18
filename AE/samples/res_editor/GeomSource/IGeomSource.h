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
        using UpdatePassData    = IPass::UpdatePassData;

        struct UpdateData
        {
            IGSMaterials &          mtr;
            DirectCtx::Transfer &   ctx;
            float4x4 const&         transform;
            UpdatePassData const&   pd;
        };

        struct UpdateRTData
        {
            DescriptorUpdater &     updater;
        };

        struct DrawData
        {
            IGSMaterials &          mtr;
            DirectCtx::Draw &       ctx;
            DescriptorSetID         passDS;
        };

        enum class ERTGeometryType
        {
            Opaque,
            OpaqueDualSided,
            Translucent,
            Volumetric,
            _Count
        };


    // variables
    private:
        Renderer &      _renderer;


    // methods
    protected:
        explicit IGeomSource (Renderer &r) : _renderer{r} {}

    public:
            virtual void  StateTransition (IGSMaterials &, DirectCtx::Graphics &)   __Th___ = 0;
            virtual void  StateTransition (DirectCtx::RayTracing &)                 __Th___ {}

        ND_ virtual bool  Draw (const DrawData &)                                   __Th___ = 0;
        ND_ virtual bool  PostProcess (const DrawData &)                            __Th___ { return false; }
        ND_ virtual bool  Update (const UpdateData &)                               __Th___ = 0;
        ND_ virtual bool  RTUpdate (const UpdateRTData &)                           __Th___ { return false; }

        ND_ Renderer&           _Renderer ()                                        const   { return _renderer; }
        ND_ DataTransferQueue&  _DtTrQueue ()                                       const;
        ND_ GfxMemAllocatorPtr  _GfxAllocator ()                                    const;
    };


} // AE::ResEditor
