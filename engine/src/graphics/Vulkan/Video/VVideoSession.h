// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

    //
    // Vulkan Video Session immutable data
    //

    class VVideoSession final
    {
    // types
    private:
        using VideoStorageArr_t = IGfxMemAllocator::VideoStorageArr_t;


    // variables
    private:
        VkVideoSessionKHR               _session    = Default;
        VkVideoSessionParametersKHR     _params     = Default;

        VideoSessionDesc                _desc;

        GfxMemAllocatorPtr              _memAllocator;
        VideoStorageArr_t               _memStorages;

        DEBUG_ONLY( DebugName_t         _debugName; )
        DRC_ONLY(   RWDataRaceCheck     _drCheck;   )


    // methods
    public:
        VVideoSession ()                                __NE___ {}
        ~VVideoSession ()                               __NE___;

        ND_ bool  Create (VResourceManager &, const VideoSessionDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)   __NE___;
            void  Destroy (VResourceManager &)                                                                                  __NE___;

        ND_ VkVideoSessionKHR           Session ()      C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _session; }
        ND_ VkVideoSessionParametersKHR Parameters ()   C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _params; }
        ND_ VideoSessionDesc const&     Description ()  C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })


        ND_ static bool  Validate (const VDevice &dev, INOUT VideoSessionDesc &desc)            __NE___;
        ND_ static bool  IsSupported (const VResourceManager &, const VideoSessionDesc &desc)   __NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
