// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/NativeWindow.h"
# include "graphics/Public/SwapchainDesc.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{

    //
    // Metal Capture Scope (for Xcode debugger)
    //

    class MCaptureScope
    {
    // variables
    private:
        MetalCaptureScopeRC     _scope;


    // methods
    public:
        MCaptureScope () {}
        ~MCaptureScope () {}

        ND_ bool  Create ();

        void  Begin ();
        void  End ();
    };



    //
    // Metal Swapchain
    //

    class MSwapchain : public Noncopyable
    {
    // types
    protected:
        static constexpr uint   _ImageCacheSize     = 8;
        static constexpr uint   _ImageIndexBits     = 16;
        static constexpr uint   _MaxImageIndex      = (1u << _ImageIndexBits) - 1;

        using MtlImageCache_t       = StaticArray< MetalImage,              _ImageCacheSize >;
        using ImageIdCache_t        = StaticArray< StrongAtom<ImageID>,     _ImageCacheSize >;
        using ImageViewIdCache_t    = StaticArray< StrongAtom<ImageViewID>, _ImageCacheSize >;
        using ImagePresentQueue_t   = StaticArray< MetalImage,              GraphicsConfig::MaxFrames >;

        struct MutableIdxBits
        {
            uint    semaphoreId     : 3;
            uint    imageIdx        : _ImageIndexBits;

            MutableIdxBits () __NE___ : semaphoreId{0}, imageIdx{_MaxImageIndex} {}
        };


    // variables
    protected:
        MDevice const*                  _device         = null;
        mutable SharedMutex             _guard;

        Atomic< uint >                  _surfaceSize;
        BitAtomic< MutableIdxBits >     _indices;

        MtlImageCache_t                 _mtlImageCache;
        ImageIdCache_t                  _imageIdCache;
        ImageViewIdCache_t              _imageViewIdCache;

        ImagePresentQueue_t             _imageQueue;
        MetalSharedEventRC              _semaphore;

        MetalCADrawableRC               _mtlDrawable;
        MetalCALayerRC                  _mtlLayer;

        SwapchainDesc                   _desc;              // protected by '_guard'


    // methods
    protected:
        explicit MSwapchain ()                                              __NE___;

    public:
        ~MSwapchain ()                                                      __NE___;

            bool   GetSurfaceFormats (OUT FeatureSet::SurfaceFormatSet_t &) C_NE___;
        ND_ usize  GetSurfaceFormats (OUT SurfaceFormat*, usize count)      C_NE___;
        ND_ usize  GetPresentModes (OUT EPresentMode*, usize count)         C_NE___;

        ND_ bool  AcquireNextImage ()                                       __NE___;
        ND_ bool  Present (MQueuePtr queue)                                 __NE___;

        ND_ bool                        IsInitialized ()                    C_NE___ { SHAREDLOCK( _guard );  return bool(_mtlLayer) and _desc.format != Default; }

        ND_ MetalCALayer                GetMtlSurface ()                    C_NE___ { SHAREDLOCK( _guard );  return _mtlLayer; }

        ND_ uint2                       GetSurfaceSize ()                   C_NE___;
        ND_ uint                        GetSwapchainLength ()               C_NE___ { SHAREDLOCK( _guard );  return _desc.minImageCount; }

        ND_ SwapchainDesc               GetDescription ()                   C_NE___ { SHAREDLOCK( _guard );  return _desc; }

        ND_ uint                        GetCurretImageIndex ()              C_NE___ { return _indices.load().imageIdx; }
        ND_ bool                        IsImageAcquired ()                  C_NE___ { return GetCurretImageIndex() < _MaxImageIndex; }

        ND_ ImageAndViewID              GetCurrentImageAndViewID ()         C_NE___ { return GetImageAndViewID( GetCurretImageIndex() ); }
        ND_ ImageAndViewID              GetImageAndViewID (uint i)          C_NE___;

        // same as output params in 'AcquireNextImage()'
        ND_ MetalCmdBatchDependency     GetImageAvailableSemaphore ()       C_NE___;
        ND_ MetalCmdBatchDependency     GetRenderFinishedSemaphore ()       C_NE___;
    };



    //
    // Metal Swapchain Initializer
    //

    class MSwapchainInitializer final : public MSwapchain
    {
    private:
        static constexpr EImageUsage    _ImageUsage =
            EImageUsage::Transfer | EImageUsage::Sampled | EImageUsage::Storage |
            EImageUsage::ColorAttachment | EImageUsage::InputAttachment;


    // methods
    public:
        explicit MSwapchainInitializer ()                                                                       __NE___;

        ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = {})                                 __NE___;
            void  DestroySurface ()                                                                             __NE___;

        ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage) C_NE___;

        ND_ bool  ChooseColorFormat (INOUT EPixelFormat &colorFormat, INOUT EColorSpace &colorSpace)            C_NE___;

        ND_ bool  Create (const uint2&          viewSize,
                          const SwapchainDesc&  desc,
                          StringView            dbgName = {})                                                   __NE___;

            void  Destroy ()                                                                                    __NE___;

    private:
        ND_ bool  _Create (const SwapchainDesc& desc, StringView dbgName)                                       __NE___;

            void  _PrintInfo ()                                                                                 C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_METAL
