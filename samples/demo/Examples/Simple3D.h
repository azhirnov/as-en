// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

    //
    // Simple 3D Sample
    //

    class Simple3DSample final : public ISample
    {
    // types
    private:
        class DrawTask;
        class ProcessInputTask;
        class UploadTextureTask;

        using Camera                = FPVCameraTempl<float>;
        using CubeRenderer          = GeometryTools::CubeRenderer;
        using SphericalCubeRenderer = GeometryTools::SphericalCubeRenderer;


    // variables
    public:
        RenderTechPipelinesPtr      rtech;
        GfxMemAllocatorPtr          gfxAlloc;

        GraphicsPipelineID          ppln;
        Strong<DescriptorSetID>     descSet;
        const DescSetBinding        dsIndex     {0};

        CubeRenderer                cube1;
        SphericalCubeRenderer       cube2;
        Strong<BufferID>            uniformBuf;

        Atomic<bool>                uploaded    {false};
        StrongImageAndViewID        cubeMap;

        StrongImageAndViewID        depthBuf;

        Camera                      camera;

        const uint                  lod         = 9;
        const bool                  use_cube1   = false;


    // methods
    public:
        Simple3DSample () {}
        ~Simple3DSample () override;

        // ISample //
        bool            Init (PipelinePackID pack)                                          override;
        AsyncTask       Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps) override;
        AsyncTask       Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)                   override;
        InputModeName   GetInputMode ()                                                     const override;
    };


} // AE::Samples::Demo
