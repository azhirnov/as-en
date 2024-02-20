// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

    //
    // Camera 3D Sample
    //

    class Camera3DSample final : public ISample
    {
    // types
    private:
        class DrawTask;
        class ProcessInputTask;
        class UploadTextureTask;

        using Camera                = TFPVCamera<float>;
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
        Camera3DSample ()                                                                   __NE___ {}
        ~Camera3DSample ()                                                                  __NE_OV;

        // ISample //
        bool            Init (PipelinePackID, IApplicationTS)                               __NE_OV;
        AsyncTask       Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE_OV;
        AsyncTask       Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)                   __NE_OV;
        InputModeName   GetInputMode ()                                                     C_NE_OV;
    };


} // AE::Samples::Demo
