// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

    //
    // Metal Image immutable data
    //

    class MImage
    {
    // variables
    private:
        MetalImageRC            _image;
        ImageDesc               _desc;
        Strong<MemoryID>        _memoryId;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        MImage ()                                       __NE___ {}
        ~MImage ()                                      __NE___;

        ND_ bool  Create (MResourceManager &, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)  __NE___;
        ND_ bool  Create (const MResourceManager &, const MetalImageDesc &desc, StringView dbgName)                     __NE___;
            void  Destroy (MResourceManager &)                                                                          __NE___;

        ND_ MetalImage          Handle ()               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _image; }
        ND_ ImageDesc const&    Description ()          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc; }
        ND_ MemoryID            MemoryId ()             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
        ND_ bool                IsExclusiveSharing ()   C_NE___ { return false; }

        ND_ uint3 const         Dimension ()            C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.dimension; }
        ND_ uint const          Width ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.x; }
        ND_ uint const          Height ()               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.y; }
        ND_ uint const          Depth ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.z; }
        ND_ uint const          ArrayLayers ()          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.arrayLayers.Get(); }
        ND_ uint const          MipmapLevels ()         C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.maxLevel.Get(); }
        ND_ EPixelFormat        PixelFormat ()          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.format; }
        ND_ uint const          Samples ()              C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc.samples.Get(); }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })

        ND_ static bool  IsSupported (const MResourceManager &, const ImageDesc &desc)      __NE___;
        ND_ bool         IsSupported (const MResourceManager &, const ImageViewDesc &desc)  C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_METAL
