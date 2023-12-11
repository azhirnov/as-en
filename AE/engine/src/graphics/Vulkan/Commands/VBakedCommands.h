// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

    //
    // Vulkan Baked Commands
    //

    class VBakedCommands final
    {
    // types
    public:
        using Header    = Graphics::_hidden_::SoftwareCmdBufBase::Header;


    // variables
    protected:
        void *      _root   = null;


    // methods
    public:
        VBakedCommands ()                                   __NE___ {}
        explicit VBakedCommands (void* root)                __NE___ : _root{ root } {}
        VBakedCommands (VBakedCommands &&other)             __NE___ : _root{ other._root }  { other._root = null; }

            void  Destroy ()                                __NE___ { Graphics::_hidden_::SoftwareCmdBufBase::Deallocate( _root );  _root = null; }
        ND_ bool  Execute (VulkanDeviceFn, VkCommandBuffer) C_NE___;
        ND_ bool  IsValid ()                                C_NE___ { return _root != null; }
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
