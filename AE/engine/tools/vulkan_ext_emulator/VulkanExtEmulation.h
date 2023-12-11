// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Used to emulate some Vulkan extensions to use Vk 1.2+ on Android and other platforms.
*/

#pragma once

#include "graphics/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

    //
    // Vulkan Extension Emulation
    //

    class VulkanExtEmulation
    {
    public:
        explicit VulkanExtEmulation (VulkanDeviceFnTable* fnTable)      __NE___;
        ~VulkanExtEmulation ()                                          __NE___;

        void  OnInitialize ()                                           __NE___;    // after  VulkanLoader::Initialize()
        void  OnLoadInstance (VkInstance instance, Version2 instVer)    __NE___;    // after  VulkanLoader::LoadInstance()
        void  OnUnload ()                                               __NE___;    // before VulkanLoader::Unload()
    };

} // AE::Graphics
