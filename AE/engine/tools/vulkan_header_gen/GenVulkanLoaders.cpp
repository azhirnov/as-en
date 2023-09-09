// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include <map>
#include "Generator.h"

namespace AE::Vulkan
{

/*
=================================================
    GenVulkanLoaders
=================================================
*/
    bool Generator::GenVulkanLoaders (const Path &outputFolder, Version2 minVer) const
    {
        struct Group
        {
            FixedArray< String, 16 >    inline_funcs;
            FixedArray< String, 16 >    func_pointers;
            FixedArray< String, 16 >    func_address;
            FixedArray< String, 16 >    dummy_func;
            FixedArray< String, 16 >    loader;
            String                      filename;
        };

        StaticArray< Group, 3 >     groups;
        const auto                  feats       = _GetFeatures( minVer );
        HashSet<StringView>         include_fn  = { "vkGetRayTracingShaderGroupHandlesKHR" };
        HashSet<StringView>         exclude_fn  = { "vkGetInstanceProcAddr" };


        for (auto& g : groups)
        {
            g.inline_funcs  .resize( _fileData.size() );
            g.func_pointers .resize( _fileData.size() );
            g.func_address  .resize( _fileData.size() );
            g.dummy_func    .resize( _fileData.size() );
            g.loader        .resize( _fileData.size() );
        }

        groups[0].filename = "fn_vulkan_lib.h";
        groups[1].filename = "fn_vulkan_inst.h";
        groups[2].filename = "fn_vulkan_dev.h";

        for (auto& fn : _funcs)
        {
            if ( not EndsWith( fn.data.name, "KHR" ) and
                 not EndsWith( fn.data.name, "EXT" ))
            {
                if ( _funcs.contains( SearchableFunc{ String{fn.data.name} + "KHR" }))
                    exclude_fn.insert( fn.data.name );

                if ( _funcs.contains( SearchableFunc{ String{fn.data.name} + "EXT" }))
                    exclude_fn.insert( fn.data.name );
            }
        }

        // Vulkan 1.1 core
        #if 1
          // VK_KHR_get_physical_device_properties2
            exclude_fn.erase( "vkGetPhysicalDeviceFeatures2" );
            exclude_fn.erase( "vkGetPhysicalDeviceProperties2" );
            exclude_fn.erase( "vkGetPhysicalDeviceFormatProperties2" );
            exclude_fn.erase( "vkGetPhysicalDeviceImageFormatProperties2" );
            exclude_fn.erase( "vkGetPhysicalDeviceQueueFamilyProperties2" );
            exclude_fn.erase( "vkGetPhysicalDeviceMemoryProperties2" );
            exclude_fn.erase( "vkGetPhysicalDeviceSparseImageFormatProperties2" );

          // VK_KHR_maintenance1
            exclude_fn.erase( "vkTrimCommandPool" );

          // VK_KHR_maintenance3
            exclude_fn.erase( "vkGetDescriptorSetLayoutSupport" );

          // VK_KHR_bind_memory2
            exclude_fn.erase( "vkBindBufferMemory2" );
            exclude_fn.erase( "vkBindImageMemory2" );

          // VK_KHR_get_memory_requirements2
            exclude_fn.erase( "vkGetImageMemoryRequirements2" );
            exclude_fn.erase( "vkGetBufferMemoryRequirements2" );
            exclude_fn.erase( "vkGetImageSparseMemoryRequirements2" );

          // VK_KHR_sampler_ycbcr_conversion
        //  exclude_fn.erase( "vkCreateSamplerYcbcrConversion" );
        //  exclude_fn.erase( "vkDestroySamplerYcbcrConversion" );

          // VK_KHR_descriptor_update_template
        //  exclude_fn.erase( "vkCreateDescriptorUpdateTemplate" );
        //  exclude_fn.erase( "vkDestroyDescriptorUpdateTemplate" );
        //  exclude_fn.erase( "vkUpdateDescriptorSetWithTemplate" );
        #endif

        // Vulkan 1.2 core
        #if 0
          // VK_KHR_draw_indirect_count
            exclude_fn.erase( "vkCmdDrawIndirectCount" );
            exclude_fn.erase( "vkCmdDrawIndexedIndirectCount" );

          // VK_KHR_create_renderpass2
            exclude_fn.erase( "vkCreateRenderPass2" );
            exclude_fn.erase( "vkCmdBeginRenderPass2" );
            exclude_fn.erase( "vkCmdNextSubpass2" );
            exclude_fn.erase( "vkCmdEndRenderPass2" );

          // VK_KHR_timeline_semaphore
            exclude_fn.erase( "vkGetSemaphoreCounterValue" );
            exclude_fn.erase( "vkWaitSemaphores" );
            exclude_fn.erase( "vkSignalSemaphore" );

          // VK_EXT_host_query_reset
            exclude_fn.erase( "vkResetQueryPool" );
        #endif

        // Vulkan 1.3 core
        #if 0
          // VK_KHR_buffer_device_address
            exclude_fn.erase( "vkGetBufferDeviceAddress" );
            exclude_fn.erase( "vkGetBufferOpaqueCaptureAddress" );
            exclude_fn.erase( "vkGetDeviceMemoryOpaqueCaptureAddress" );

          // VK_KHR_synchronization2
            exclude_fn.erase( "vkCmdPipelineBarrier2" );
            exclude_fn.erase( "vkCmdResetEvent2" );
            exclude_fn.erase( "vkCmdSetEvent2" );
            exclude_fn.erase( "vkCmdWaitEvents2" );
            exclude_fn.erase( "vkCmdWriteTimestamp2" );
            exclude_fn.erase( "vkQueueSubmit2" );

          // VK_KHR_maintenance4
            exclude_fn.erase( "vkGetDeviceBufferMemoryRequirements" );
            exclude_fn.erase( "vkGetDeviceImageMemoryRequirements" );
            exclude_fn.erase( "vkGetDeviceImageSparseMemoryRequirements" );
        #endif

        for (auto& fn : _funcs)
        {
            // select group
            Group*  gr      = null;
            bool    is_dev  = false;

            switch ( fn.data.scope )
            {
                case EFuncScope::Library :      gr = &groups[0];                        break;
                case EFuncScope::Instance :     gr = &groups[1];                        break;
                case EFuncScope::Device :       gr = &groups[2];    is_dev = true;      break;
                default :                       RETURN_ERR( "" );
            }

            if ( exclude_fn.contains( fn.data.name ))
                continue;

            if ( not (fn.data.extension.empty() or feats.enabledExt.contains( fn.data.extension )) )
            {
                if ( not include_fn.contains( fn.data.name ))
                    continue;
            }

            // add function pointer decl
            {
                String& src = gr->func_pointers[ fn.data.fileIndex ];

                src << "\textern PFN_" << fn.data.name << "  _var_" << fn.data.name << ";\n";
            }

            // add function pointer
            {
                String& src = gr->func_address[ fn.data.fileIndex ];

                src << "\tPFN_" << fn.data.name << "  _var_" << fn.data.name << " = null;\n"; //"&Dummy_" << fn.data.name << ";\n";
            }

            // add function loader
            {
                String& src = gr->loader[ fn.data.fileIndex ];

                src << "\tLoad( OUT " << (is_dev ? "table." : "") << "_var_" << fn.data.name << ", \"" << fn.data.name << "\","
                    << " Dummy_" << fn.data.name << " );\n";
            }


            // add dummy functions
            {
                String& src = gr->dummy_func[ fn.data.fileIndex ];

                src << "\tVKAPI_ATTR ";

                // add result type
                for (auto& type : fn.data.result.type) {
                    src << type << " ";
                }

                // add name
                src << "VKAPI_CALL Dummy_" << fn.data.name << " (";

                // add arguments to function declaration
                for (auto& arg : fn.data.args)
                {
                    src << (&arg != fn.data.args.data() ? ", " : "");

                    for (auto& type : arg.type) {
                        src << type << " ";
                    }
                }

                // add body
                src << ")\t\t\t{"
                    << "  VK_LOG( \"used dummy function '" << fn.data.name << "'\" );"
                    << "  return";

                if ( fn.data.result.type.size() == 1 and
                     fn.data.result.type.front() == "void" )
                {}
                else
                {
                    bool                                is_pointer      = false;
                    StructMap_t::const_iterator         struct_info     = _structs.end();
                    EnumMap_t::const_iterator           enum_info       = _enums.end();
                    BitfieldMap_t::const_iterator       bitfield_info   = _bitfields.end();
                    ResourceTypes_t::const_iterator     res_info        = _resourceTypes.end();
                    BasicTypeMap_t::const_iterator      basic_info      = _basicTypes.end();

                    for (auto& type : fn.data.result.type)
                    {
                        if ( not (struct_info   != _structs.end()       or
                                  enum_info     != _enums.end()         or
                                  bitfield_info != _bitfields.end()     or
                                  res_info      != _resourceTypes.end() or
                                  basic_info    != _basicTypes.end()    ))
                        {
                            struct_info     = _structs.find( SearchableStruct{type} );
                            enum_info       = _enums.find( SearchableEnum{type} );
                            bitfield_info   = _bitfields.find( SearchableBitfield{type} );
                            res_info        = _resourceTypes.find( type );
                            basic_info      = _basicTypes.find( type );
                        }
                        is_pointer |= (type == "*");
                    }

                    if ( is_pointer )
                        src << " null";
                    else
                    if ( res_info != _resourceTypes.end() )
                        src << " " << res_info->second.typeName << "(0)";
                    else
                    if ( enum_info != _enums.end() )
                        src << " " << enum_info->data.fields.back().name;
                    else
                    if ( bitfield_info != _bitfields.end() )
                        src << " " << bitfield_info->data.name << "(0)";
                    else
                    if ( basic_info != _basicTypes.end() )
                        src << " " << basic_info->first << "(0)";
                    else
                    if ( fn.data.result.type.size() == 1 and
                         fn.data.result.type[0] == "PFN_vkVoidFunction" )
                    {
                        src << " null";
                    }
                    else
                        RETURN_ERR( "unknown type" );
                }

                src << ";  }\n";
            }


            // add inline function decl
            {
                String& src = gr->inline_funcs[ fn.data.fileIndex ];
                src << "\t";

                if ( fn.data.result.type.size() > 1         or
                     fn.data.result.type.front() != "void" )
                {
                    src << "ND_ ";
                }
                else
                    src << "\t";

                src << "VKAPI_ATTR inline ";

                // add result type
                for (auto& type : fn.data.result.type) {
                    src << type << " ";
                }

                // add name
                src << fn.data.name << " (";

                // add arguments to function declaration
                for (auto& arg : fn.data.args)
                {
                    src << (&arg != fn.data.args.data() ? ", " : "");

                    String  suffix;
                    bool    has_suffix = false;

                    for (auto& type : arg.type)
                    {
                        if ( type == "[" )
                            has_suffix = true;

                        if ( has_suffix )
                            suffix << type;
                        else
                            src << type << " ";
                    }
                    src << arg.name << suffix;
                }

                src << ")" << (is_dev ? " const" : "")
                    << "\t\t\t\t\t\t\t\t{ ";

                if (false) {
                    src << "VK_LOG( \"" << fn.data.name << "\" );  ";
                }

                src << "return " << (is_dev ? "_table->" : "") << "_var_" << fn.data.name << "( ";

                // add arguments to function call
                for (auto& arg : fn.data.args)
                {
                    src << (&arg != fn.data.args.data() ? ", " : "") << arg.name;
                }
                src << " ); }\n";
            }
        }


        // save to file
        for (auto& gr : groups)
        {
            String  src;

            src << "\n#ifdef VKLOADER_STAGE_DECLFNPOINTER\n";
            for (usize i = 0; i < _fileData.size(); ++i)
            {
                if ( gr.func_pointers[i].empty() ) continue;
                if ( i > 0 )                src << "\n\n# ifdef " << _fileData[i].macro << "\n";
                src << gr.func_pointers[i];
                if ( i > 0 )                src << "# endif // " << _fileData[i].macro << "\n";
            }
            src << "#endif // VKLOADER_STAGE_DECLFNPOINTER\n\n";


            src << "\n#ifdef VKLOADER_STAGE_FNPOINTER\n";
            for (usize i = 0; i < _fileData.size(); ++i)
            {
                if ( gr.func_address[i].empty() ) continue;
                if ( i > 0 )                src << "\n\n# ifdef " << _fileData[i].macro << "\n";
                src << gr.func_address[i];
                if ( i > 0 )                src << "# endif // " << _fileData[i].macro << "\n";
            }
            src << "#endif // VKLOADER_STAGE_FNPOINTER\n\n";


            src << "\n#ifdef VKLOADER_STAGE_INLINEFN\n";
            for (usize i = 0; i < _fileData.size(); ++i)
            {
                if ( gr.inline_funcs[i].empty() ) continue;
                if ( i > 0 )                src << "\n\n# ifdef " << _fileData[i].macro << "\n";
                src << gr.inline_funcs[i];
                if ( i > 0 )                src << "# endif // " << _fileData[i].macro << "\n";
            }
            src << "#endif // VKLOADER_STAGE_INLINEFN\n\n";


            src << "\n#ifdef VKLOADER_STAGE_DUMMYFN\n";
            for (usize i = 0; i < _fileData.size(); ++i)
            {
                if ( gr.dummy_func[i].empty() ) continue;
                if ( i > 0 )                src << "\n\n# ifdef " << _fileData[i].macro << "\n";
                src << gr.dummy_func[i];
                if ( i > 0 )                src << "# endif // " << _fileData[i].macro << "\n";
            }
            src << "#endif // VKLOADER_STAGE_DUMMYFN\n\n";


            src << "\n#ifdef VKLOADER_STAGE_GETADDRESS\n";
            for (usize i = 0; i < _fileData.size(); ++i)
            {
                if ( gr.loader[i].empty() ) continue;
                if ( i > 0 )                src << "\n\n# ifdef " << _fileData[i].macro << "\n";
                src << gr.loader[i];
                if ( i > 0 )                src << "# endif // " << _fileData[i].macro << "\n";
            }
            src << "#endif // VKLOADER_STAGE_GETADDRESS\n\n";

          #if not AE_PRIVATE_USE_TABS
            src = Parser::TabsToSpaces( src );
          #endif

            FileSystem::CreateDirectory( outputFolder );
            CHECK_ERR( FileSystem::IsDirectory( outputFolder ));

            FileWStream file{ Path{outputFolder}.append( gr.filename ) };
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( file.Write( src ));
        }

        return true;
    }

/*
=================================================
    _GetFeatures
=================================================
*/
    Generator::FeatureSet  Generator::_GetFeatures (Version2 minVer) const
    {
        constexpr Version2  NoVer   = Version2::Max();  // only as extension

        const FeatureInfo   instanceFeatures[] =
        {
            { "surface",                        VK_KHR_SURFACE_EXTENSION_NAME,                              NoVer,  {1,0},  {} },

        // surface //
            { "surfaceCaps2",                   VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,           NoVer,  {1,0},  {VK_KHR_SURFACE_EXTENSION_NAME} },
            { "swapchainColorspace",            VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_SURFACE_EXTENSION_NAME} },

        // display //
            { "display",                        VK_KHR_DISPLAY_EXTENSION_NAME,                              NoVer,  {1,0},  {VK_KHR_SURFACE_EXTENSION_NAME} },
            { "directModeDisplay",              VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_DISPLAY_EXTENSION_NAME} },
            { "displayProps2",                  VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME,             NoVer,  {1,0},  {VK_KHR_DISPLAY_EXTENSION_NAME} },

        // debugging //
            { "debugReport",                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,                         NoVer,  {1,0},  {} },   // deprecated, but still present on Android
            { "debugMarker",                    VK_EXT_DEBUG_MARKER_EXTENSION_NAME,                         NoVer,  {1,0},  {} },   // deprecated, but still present on Android
            { "debugUtils",                     VK_EXT_DEBUG_UTILS_EXTENSION_NAME,                          NoVer,  {1,0},  {} },
            { "validationFlags",                VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME,                  NoVer,  {1,0},  {} },

            { "deviceGroupCreation",            VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,                {1,1},  {1,0},  {} },
            { "deviceProps2",                   VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,     {1,1},  {1,0},  {} },
        };

        const FeatureInfo   deviceFeatures[] =
        {
        // 1.1 //
            { "memRequirements2",               VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,            {1,1},  {1,0},  {} },
            { "bindMemory2",                    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,                        {1,1},  {1,0},  {} },
            { "dedicatedAllocation",            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,                 {1,1},  {1,0},  {VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME} },
            { "descriptorUpdateTemplate",       VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,           {1,1},  {1,0},  {} },
            { "maintenance1",                   VK_KHR_MAINTENANCE1_EXTENSION_NAME,                         {1,1},  {1,0},  {} },
            { "maintenance2",                   VK_KHR_MAINTENANCE2_EXTENSION_NAME,                         {1,1},  {1,0},  {} },
            { "maintenance3",                   VK_KHR_MAINTENANCE3_EXTENSION_NAME,                         {1,1},  {1,0},  {VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME} },
            { "deviceGroup",                    VK_KHR_DEVICE_GROUP_EXTENSION_NAME,                         {1,1},  {1,0},  {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME} },
            { "storageBufferClass",             VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,         {1,1},  {1,0},  {} },
            { "subgroup",                       {},                                                         {1,1},  {1,1},  {} },
            { "relaxedBlockLayout",             VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,                 {1,1},  {1,0},  {} },

        // optional in 1.1 //
            { "multiview",                      VK_KHR_MULTIVIEW_EXTENSION_NAME,                            NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "variablePointers",               VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,                    NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME} },
            { "samplerYcbcrConversion",         VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,             NoVer,  {1,0},  {VK_KHR_MAINTENANCE1_EXTENSION_NAME, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderDrawParams",               VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,               NoVer,  {1,0},  {} },
            { "storage16bit",                   VK_KHR_16BIT_STORAGE_EXTENSION_NAME,                        NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME} },

        // 1.2 //
            { "renderPass2",                    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,                  {1,2},  {1,0},  {VK_KHR_MULTIVIEW_EXTENSION_NAME, VK_KHR_MAINTENANCE2_EXTENSION_NAME} },
            { "shaderFloatControls",            VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,                {1,2},  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "spirv14",                        VK_KHR_SPIRV_1_4_EXTENSION_NAME,                            {1,2},  {1,1},  {VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME} },
            { "imageFormatList",                VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,                    {1,2},  {1,0},  {} },
            { "driverProperties",               VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,                    {1,2},  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },

        // optional in 1.2 //
            { "storage8bits",                   VK_KHR_8BIT_STORAGE_EXTENSION_NAME,                         NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME} },
            { "shaderAtomicInt64",              VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderFloat16Int8",              VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "samplerMirrorClamp",             VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,         NoVer,  {1,0},  {} },
            { "drawIndirectCount",              VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,                  NoVer,  {1,0},  {} },
            { "descriptorIndexing",             VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_MAINTENANCE3_EXTENSION_NAME} },
            { "samplerFilterMinmax",            VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "scalarBlockLayout",              VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "imagelessFramebuffer",           VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_MAINTENANCE2_EXTENSION_NAME, VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME} },
            { "subgroupExtendedTypes",          VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,       NoVer,  {1,1},  {} },
            { "hostQueryReset",                 VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,                     NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "timelineSemaphore",              VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,                   NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "bufferDeviceAddress",            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,                NoVer,  {1,1},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "memoryModel",                    VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,                  NoVer,  {1,0},  {} },
            { "uniformBufferStandardLayout",    VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,       NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderViewportIndexLayer",       VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,          NoVer,  {1,0},  {} },

        // 1.3 //
            { "copyCmds2",                      VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,                      {1,3},  {1,0},  {} },
            { "formatFaet2",                    VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,               {1,3},  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "texelBufferAlignment",           VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,               {1,3},  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },

        // optional 1.3 //
            { "maintenance4",                   VK_KHR_MAINTENANCE_4_EXTENSION_NAME,                        NoVer,  {1,1},  {} },
            { "synchronization2",               VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,                    NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "zeroInitializeWorkgroupMem",     VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME,     NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "inlineUniformBlock",             VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,                 NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_MAINTENANCE3_EXTENSION_NAME} },
            { "subgroupSizeControl",            VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,                NoVer,  {1,1},  {} },
            { "astcHdr",                        VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME,         NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderTerminateInvocation",      VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME,          NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            // TODO: VK_EXT_ycbcr_2plane_444_formats
            // VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME


        // extensions //
            { "swapchain",                      VK_KHR_SWAPCHAIN_EXTENSION_NAME,                            NoVer,  {1,0},  {VK_KHR_SURFACE_EXTENSION_NAME} },
            { "displaySwapchain",               VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME,                    NoVer,  {1,0},  {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DISPLAY_EXTENSION_NAME} },
            { "depthRangeUnrestricted",         VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME,             NoVer,  {1,0},  {} },
        //  { "pushDescriptor",                 VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,                      NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "blendOpExt",                     VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME,             NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "memoryPriority",                 VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,                      NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "vertexDivisor",                  VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,             NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "depthClip",                      VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME,                    NoVer,  {1,0},  {} },
            { "portabilitySubset",              VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,                   NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "loadOpNone",                     VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME,                   NoVer,  {1,0},  {} },
            { "pagebleDeviceLocalMemory",       VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME,         NoVer,  {1,0},  {VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME} },
            { "sampleLocations",                VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME,                     NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "fragmentBarycentric",            VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME,          NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "meshShader",                     VK_EXT_MESH_SHADER_EXTENSION_NAME,                          NoVer,  {1,1},  {VK_KHR_SPIRV_1_4_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "rasterOrderAttachment",          VK_EXT_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME,NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "maintenance5",                   VK_KHR_MAINTENANCE_5_EXTENSION_NAME,                        NoVer,  {1,1},  {VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME} },

        // shaders //
            { "fragShaderInterlock",            VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME,            NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderClock",                    VK_KHR_SHADER_CLOCK_EXTENSION_NAME,                         NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderAtomicFloat",              VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderAtomicFloat2",             VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME,                NoVer,  {1,0},  {VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME} },
            { "shaderSubgroupUniformControlFlow",VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME,NoVer,  {1,1},  {} },
            { "workgroupMemExplicitLayout",     VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME,     NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderImageAtomicInt64",         VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,            NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },

        // queue priority //
            { "queueGlobalPriority",            VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME,                      NoVer,  {1,0},  {} },

        // debugging & profiling //
            { "performanceQuery",               VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME,                    NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "calibratedTimestamps",           VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "toolingInfo",                    VK_EXT_TOOLING_INFO_EXTENSION_NAME,                         NoVer,  {1,0},  {} },
            { "memoryBudget",                   VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,                        NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME},   EPropsType::Memory },
            { "memoryReport",                   VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME,                 NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },

        // shading rate //
            { "fragShadingRate",                VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME} },
        //  { "fragDensityMap",                 VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME,                 NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "fragDensityMap2",                VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME,               NoVer,  {1,0},  {VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME} },

        // ray tracing //
            { "pipelineLibrary",                VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,                     NoVer,  {1,0},  {} },
            { "deferredHostOps",                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,             NoVer,  {1,0},  {} },
            { "accelerationStructure",          VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,               NoVer,  {1,1},  {VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME} },
            { "rayTracingPipeline",             VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,                 NoVer,  {1,1},  {VK_KHR_SPIRV_1_4_EXTENSION_NAME, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME} },
            { "rayQuery",                       VK_KHR_RAY_QUERY_EXTENSION_NAME,                            NoVer,  {1,1},  {VK_KHR_SPIRV_1_4_EXTENSION_NAME, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME} },
            { "rayTracingMaintenance1",         VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,            NoVer,  {1,1},  {VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME} },
            // VK_KHR_ray_tracing_position_fetch
            // VK_NV_displacement_micromap
            // VK_NV_ray_tracing_invocation_reorder

        // dynamic state //
        //  { "extendedDynamicState",           VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,               NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "extendedDynamicState2",          VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,             NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "vertexInputDynamicState",        VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,           NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "colorWriteEnable",               VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME,                   NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },

        // texture compression //
            { "astcDecodeMode",                 VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME,                     NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "imageCompressionCtrl",           VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME,            NoVer,  {1,0},  {} },
            { "swapchainCompressionCtrl",       VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME,  NoVer,  {1,0},  {VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME} },

        // video //
            { "videoQueue",                     VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,                          NoVer,  {1,1},  {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME} },
            { "videoEncodeQueue",               VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,                   NoVer,  {1,1},  {VK_KHR_VIDEO_QUEUE_EXTENSION_NAME} },
            { "videoDecodeQueue",               VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,                   NoVer,  {1,1},  {VK_KHR_VIDEO_QUEUE_EXTENSION_NAME} },
            { "videoDecodeH264",                VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,                    NoVer,  {1,1},  {VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME} },
            { "videoDecodeH265",                VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,                    NoVer,  {1,1},  {VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME} },
            { "videoEncodeH264",                VK_EXT_VIDEO_ENCODE_H264_EXTENSION_NAME,                    NoVer,  {1,1},  {VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME} },
            { "videoEncodeH265",                VK_EXT_VIDEO_ENCODE_H265_EXTENSION_NAME,                    NoVer,  {1,1},  {VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME} },

        // NVidia //
        //  { "rayTracingNV",                   VK_NV_RAY_TRACING_EXTENSION_NAME,                           NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME} },
        //  { "meshShaderNV",                   VK_NV_MESH_SHADER_EXTENSION_NAME,                           NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "imageFootprintNV",               VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME,                NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "fragmentBarycentricNV",          VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME,           NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "compShaderDerivativesNV",        VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,            NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
        //  { "rayTracingMotionBlurNV",         VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME,               NoVer,  {1,0},  {VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME} },
            { "deviceGeneratedCmdsNV",          VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME,             NoVer,  {1,1},  {} },
            { "cooperativeMatrixNV",            VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME,                    NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderSMBuiltinsNV",             VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME,                    NoVer,  {1,1},  {} },
        //  { "dedicatedAllocImageAliasingNV",  VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME,   NoVer,  {1,0},  {VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME} },
        //  { "framebufferMixedSamplesNV",      VK_NV_FRAMEBUFFER_MIXED_SAMPLES_EXTENSION_NAME,             NoVer,  {1,0},  {} },
        //  { "coverageReductionModeNV",        VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME,               NoVer,  {1,0},  {VK_NV_FRAMEBUFFER_MIXED_SAMPLES_EXTENSION_NAME} },
        //  { "geometryShaderPassthroughNV",    VK_NV_GEOMETRY_SHADER_PASSTHROUGH_EXTENSION_NAME,           NoVer,  {1,0},  {} },
        //  { "representativeFragmentTestNV",   VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME,          NoVer,  {1,0},  {} },
        //  { "viewportArrayNV",                VK_NV_VIEWPORT_ARRAY_2_EXTENSION_NAME,                      NoVer,  {1,0},  {} },
        //  { "viewportSwizzleNV",              VK_NV_VIEWPORT_SWIZZLE_EXTENSION_NAME,                      NoVer,  {1,0},  {} },

        // AMD //
        //  { "coherentMemoryAMD",              VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,               NoVer,  {1,0},  {} },
        //  { "displayNativeHdrAMD",            VK_AMD_DISPLAY_NATIVE_HDR_EXTENSION_NAME,                   NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME} },
        //  { "rasterizationOrderAMD",          VK_AMD_RASTERIZATION_ORDER_EXTENSION_NAME,                  NoVer,  {1,0},  {} },
            { "shaderCorePropsAMD",             VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME,               NoVer,  {1,0},  {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME} },
            { "shaderCorePropsAMD2",            VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME,             NoVer,  {1,0},  {VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME} },

        // ARM //
            { "shaderCoreBuiltinsARM",          VK_ARM_SHADER_CORE_BUILTINS_EXTENSION_NAME,                 NoVer,  {1,0},  {} },

        // Huawei //
            { "subpassShadingHW",               VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME,                   NoVer,  {1,0},  {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME} },

        // Qualcomm //
        //  { "renderPassShaderResolve",        VK_QCOM_RENDER_PASS_SHADER_RESOLVE_EXTENSION_NAME,          NoVer,  {1,0},  {} },
            // VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME
            // VK_QCOM_RENDER_PASS_TRANSFORM_EXTENSION_NAME
            // VK_QCOM_ROTATED_COPY_COMMANDS_EXTENSION_NAME
            // VK_QCOM_multiview_per_view_render_areas

        // present //
            { "incrementalPresent",             VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME,                  NoVer,  {1,0},  {VK_KHR_SWAPCHAIN_EXTENSION_NAME} },
            { "presentId",                      VK_KHR_PRESENT_ID_EXTENSION_NAME,                           NoVer,  {1,0},  {VK_KHR_SWAPCHAIN_EXTENSION_NAME} },
            { "presentWait",                    VK_KHR_PRESENT_WAIT_EXTENSION_NAME,                         NoVer,  {1,0},  {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PRESENT_ID_EXTENSION_NAME} }
        };


        usize   max_name_len = 0;
        for (auto& feat : instanceFeatures) {
            max_name_len = Max( max_name_len, feat.shortName.size() );
        }
        for (auto& feat : deviceFeatures) {
            max_name_len = Max( max_name_len, feat.shortName.size() );
        }
        max_name_len = (max_name_len & 1 ? max_name_len : max_name_len + 1);


        // check that all required extensions are exists and in the correct order
        HashSet<StringView> existing_ext;

        for (auto& feat : instanceFeatures) {
            if ( not feat.extension.empty() )
                existing_ext.insert( feat.extension );
        }
        for (auto& feat : deviceFeatures) {
            if ( not feat.extension.empty() )
                existing_ext.insert( feat.extension );
        }

        for (auto& feat : instanceFeatures) {
            for (auto& ext : feat.requireExts)
                CHECK_ERR( existing_ext.find( ext ) != existing_ext.end() );
        }
        for (auto& feat : deviceFeatures) {
            for (auto& ext : feat.requireExts)
                CHECK_ERR( existing_ext.find( ext ) != existing_ext.end() );
        }


        FeatureSet  set;
        set.instance.assign( std::begin(instanceFeatures), std::end(instanceFeatures) );
        set.device.assign( std::begin(deviceFeatures), std::end(deviceFeatures) );
        set.enabledExt  = RVRef(existing_ext);
        set.minVer      = minVer;
        set.maxNameLen  = max_name_len;

        for (auto& feat : set.instance)
        {
            feat.propsType = EPropsType::Instance;

            if ( not feat.extension.empty() and not _extensions.contains( feat.extension ))
                continue; // feature is not exists in headers

            feat.enabled = true;
        }

        for (auto& feat : set.device)
        {
            if ( not feat.extension.empty() and not _extensions.contains( feat.extension ))
                continue; // feature is not exists in headers

            feat.enabled = true;
        }
        return set;
    }

/*
=================================================
    ExtToName
=================================================
*/
    ND_ inline String  ExtToName (StringView name)
    {
        const HashMap< String, String > FixNames = {
            {"VK_EXT_SWAPCHAIN_COLORSPACE_EXTENSION_NAME",  "VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME"},
            {"VK_KHR_MAINTENANCE_1_EXTENSION_NAME",         "VK_KHR_MAINTENANCE1_EXTENSION_NAME"},
            {"VK_KHR_MAINTENANCE_2_EXTENSION_NAME",         "VK_KHR_MAINTENANCE2_EXTENSION_NAME"},
            {"VK_KHR_MAINTENANCE_3_EXTENSION_NAME",         "VK_KHR_MAINTENANCE3_EXTENSION_NAME"},
        };

        String  res;
        res.reserve( name.size() + 20 );

        for (usize i = 0; i < name.size(); ++i)
        {
            const char  c = name[i];
            if ( i+1 == name.size() )
            {
                const char  p = name[i-1];
                if ( not ((p >= '0' and p <= '9') or p == '_') and  (c >= '0' and c <= '4') )
                    res << '_';
            }
            res << ToUpperCase( c );
        }
        res << "_EXTENSION_NAME";

        auto iter = FixNames.find( res );
        if (iter != FixNames.end())
            return iter->second;

        return res;
    }

/*
=================================================
    _GetFeaturesBoolStruct
=================================================
*/
    String  Generator::_GetFeaturesBoolStruct (const FeatureSet &feats) const
    {
        String  str;
        str << "\tstruct VExtensions\n\t{\n"
            << "\t// ---- instance ----\n";

        for (auto& feat : feats.instance)
        {
            str << "\t\tbool  " << feat.shortName;
            AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() );
            str << " : 1;   // " << feat.extension << " \n";
        }

        str << "\n\t// ---- device ----\n";

        Version2    curr_ver = {1,0};
        for (auto& feat : feats.device)
        {
            if ( curr_ver != feat.coreVersion )
            {
                if ( feat.coreVersion > Version2{1,0} and feat.coreVersion < Version2::Max() )
                    str << "\n\t\t// ---- " << ToString( feat.coreVersion.major ) << "." << ToString( feat.coreVersion.minor ) << " ----\n";
                else
                    str << "\n\t\t// ---- ext ----\n";

                curr_ver = feat.coreVersion;
            }
            str << "\t\tbool  " << feat.shortName;
            AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() );
            str << " : 1;   // " << feat.extension << " \n";
        }

        str << "\n\t\tVExtensions () { ZeroMem( OUT this, Sizeof(*this) ); }\n"
            << "\t};\n";
        return str;
    }

/*
=================================================
    _GetFeaturesPropsStruct
=================================================
*/
    String  Generator::_GetFeaturesPropsStruct (const FeatureSet &feats) const
    {
        String  str;
        str << "\tstruct VProperties\n\t{\n"
            << "\t\tVkPhysicalDeviceProperties         properties;\n"
            << "\t\tVkPhysicalDeviceFeatures           features;\n"
            << "\t\tVkPhysicalDeviceMemoryProperties   memoryProperties;\n"
            << "\t\tVkPhysicalDeviceSubgroupProperties subgroupProperties;\n";

        {
            auto    it = _extInfo.find( "VK_KHR_portability_subset" );
            CHECK( it != _extInfo.end() );
        }

        for (auto& feat : feats.device)
        {
            if (auto info = _extInfo.find( feat.extension ); info != _extInfo.end())
            {
                const usize     old_size = str.size();

                if ( info->second.feats )
                {
                    const auto  type_name = info->second.feats.value()->data.name;
                    const auto  feat_name = String{feat.shortName} << "Feats";
                    str << "\t\t" << type_name << "  " << feat_name << ";\n";
                }
                if ( info->second.props )
                {
                    const auto  type_name = info->second.props.value()->data.name;
                    const auto  feat_name = String{feat.shortName} << "Props";
                    str << "\t\t" << type_name << "  " << feat_name << ";\n";
                }

                if ( old_size != str.size() )
                    str.insert( old_size, "\n\t\t// "s << feat.extension << "\n" );
            }
        }

        str << "\n\t\tVProperties () { ZeroMem( OUT this, Sizeof(*this) ); }\n"
            << "\t};\n";
        return str;
    }

/*
=================================================
    _GetExtensionsListFunc
=================================================
*/
    String  Generator::_GetExtensionsListFunc (const FeatureSet &feats) const
    {
        constexpr Version2  NoVer = Version2::Max();

        String  str;
        str << "\tArray<const char*>  VDeviceInitializer::_GetInstanceExtensions (InstanceVersion ver)\n"
            << "\t{\n";
        {
            std::map< Version2, std::map< Version2, String >>   core_ext;
            std::map< Version2, String >                        other_ext;

            for (auto& feat : feats.instance)
            {
                if ( not feat.enabled or feat.extension.empty() )
                    continue;

                if ( feat.coreVersion != NoVer )
                    core_ext[ feat.coreVersion ][ feat.requireVersion ] << "\n\t\t\t" << ExtToName( feat.extension ) << ",";
                else
                    other_ext[ feat.requireVersion ] << "\n\t\t\t" << ExtToName( feat.extension ) << ",";
            }

            for (auto& [core_ver, ext] : core_ext)
            {
                for (auto& [req_ver, src] : ext)
                {
                    str << "\t\tstatic const char* core_" << ToString(core_ver.major) << "_" << ToString(core_ver.minor)
                        << "_ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor)
                        << "[] =\n\t\t{" << src << "\n\t\t};\n";
                }
            }
            for (auto& [req_ver, src] : other_ext)
            {
                str << "\t\tstatic const char* ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor) << "[] =\n\t\t{" << src << "\n\t\t};\n";
            }

            str << "\t\tCHECK_ERR(( ver >= InstanceVersion{" << ToString(feats.minVer.major) << "," << ToString(feats.minVer.minor) << "} ));\n\n"
                << "\t\tArray<const char*> result;\n";

            for (auto& [core_ver, ext] : core_ext)
            {
                for (auto& [req_ver, src] : ext)
                {
                    const String    name = "core_"s << ToString(core_ver.major) << "_" << ToString(core_ver.minor) << "_ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor);

                    str << "\t\tif ( ver >= InstanceVersion{" << ToString(req_ver.major) << ',' << ToString(req_ver.minor)
                        << "} and ver < InstanceVersion{" << ToString(core_ver.major) << ',' << ToString(core_ver.minor) << "} )\n"
                        << "\t\t\tresult.insert( result.end(), std::begin(" << name << "), std::end(" << name << ") );\n";
                }
            }
            for (auto& [req_ver, src] : other_ext)
            {
                const String    name = "ext_"s + ToString(req_ver.major) << "_" << ToString(req_ver.minor);
                str << "\t\tif ( ver >= InstanceVersion{" << ToString(req_ver.major) << ',' << ToString(req_ver.minor) << "} )\n"
                    << "\t\t\tresult.insert( result.end(), std::begin(" << name << "), std::end(" << name << ") );\n";
            }
        }
        str << "\n\t\treturn result;\n\t}\n\n"
            << "\tArray<const char*>  VDeviceInitializer::_GetDeviceExtensions (DeviceVersion ver)\n"
            << "\t{\n";
        {
            std::map< Version2, std::map< Version2, String >>   core_ext;
            std::map< Version2, String >                        other_ext;

            for (auto& feat : feats.device)
            {
                if ( not feat.enabled or feat.extension.empty() )
                    continue;

                if ( feat.coreVersion != NoVer )
                    core_ext[ feat.coreVersion ][ feat.requireVersion ] << "\n\t\t\t" << ExtToName( feat.extension ) << ",";
                else
                    other_ext[ feat.requireVersion ] << "\n\t\t\t" << ExtToName( feat.extension ) << ",";
            }

            for (auto& [core_ver, ext] : core_ext)
            {
                for (auto& [req_ver, src] : ext)
                {
                    str << "\t\tstatic const char* core_" << ToString(core_ver.major) << "_" << ToString(core_ver.minor)
                        << "_ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor)
                        << "[] =\n\t\t{" << src << "\n\t\t};\n";
                }
            }
            for (auto& [req_ver, src] : other_ext)
            {
                str << "\t\tstatic const char* ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor) << "[] =\n\t\t{" << src << "\n\t\t};\n";
            }

            str << "\t\tCHECK_ERR(( ver >= DeviceVersion{" << ToString(feats.minVer.major) << "," << ToString(feats.minVer.minor) << "} ));\n\n"
                << "\t\tArray<const char*> result;\n";

            for (auto& [core_ver, ext] : core_ext)
            {
                for (auto& [req_ver, src] : ext)
                {
                    const String    name = "core_"s << ToString(core_ver.major) << "_" << ToString(core_ver.minor) << "_ext_" << ToString(req_ver.major) << "_" << ToString(req_ver.minor);

                    str << "\t\tif ( ver >= DeviceVersion{" << ToString(req_ver.major) << ',' << ToString(req_ver.minor)
                        << "} and ver < DeviceVersion{" << ToString(core_ver.major) << ',' << ToString(core_ver.minor) << "} )\n"
                        << "\t\t\tresult.insert( result.end(), std::begin(" << name << "), std::end(" << name << ") );\n";
                }
            }
            for (auto& [req_ver, src] : other_ext)
            {
                const String    name = "ext_"s + ToString(req_ver.major) << "_" << ToString(req_ver.minor);
                str << "\t\tif ( ver >= DeviceVersion{" << ToString(req_ver.major) << ',' << ToString(req_ver.minor) << "} )\n"
                    << "\t\t\tresult.insert( result.end(), std::begin(" << name << "), std::end(" << name << ") );\n";
            }
        }
        str << "\n\t\treturn result;\n\t}\n";
        return str;
    }

/*
=================================================
    _GetFeaturesAndPropertiesFunc
=================================================
*/
    String  Generator::_GetFeaturesAndPropertiesFunc (const FeatureSet &feats) const
    {
        String  str;
        str << "\tvoid  VDeviceInitializer::_InitFeaturesAndProperties (void** nextFeat)\n\t{\n"
            << "\t\tvkGetPhysicalDeviceFeatures( GetVkPhysicalDevice(), OUT &_properties.features );\n"
            << "\t\tvkGetPhysicalDeviceProperties( GetVkPhysicalDevice(), OUT &_properties.properties );\n"
            << "\t\tvkGetPhysicalDeviceMemoryProperties( GetVkPhysicalDevice(), OUT &_properties.memoryProperties );\n\n"
            << "\t\tif ( GetInstanceVersion() >= InstanceVersion{1,1} or HasInstanceExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ))\n"
            << "\t\t{\n"
            << "\t\t\tVkPhysicalDeviceFeatures2         feat2      = {};\n"
            << "\t\t\tVkPhysicalDeviceProperties2       props2     = {};\n"
            << "\t\t\tVkPhysicalDeviceMemoryProperties2 mem_props2 = {};\n\n"
            << "\t\t\tvoid **  next_feat  = &feat2.pNext;\n"
            << "\t\t\tvoid **  next_props = &props2.pNext;\n"
            << "\t\t\tvoid **  next_mem   = &mem_props2.pNext;\n\n"
            << "\t\t\tfeat2.sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;\n"
            << "\t\t\tprops2.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;\n"
            << "\t\t\tmem_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;\n\n";

        str << "\t\t\tif ( _extensions.subgroup )\n"
            << "\t\t\t{\n"
            << "\t\t\t\t*next_props = &_properties.subgroupProperties;\n"
            << "\t\t\t\tnext_props  = &_properties.subgroupProperties.pNext;\n"
            << "\t\t\t\t_properties.subgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;\n"
            << "\t\t\t}\n";

        for (auto& feat : feats.device)
        {
            if ( not feat.enabled )
                continue;

            if (auto info = _extInfo.find( feat.extension ); info != _extInfo.end())
            {
                String  tmp;
                if ( info->second.feats )
                {
                    CHECK( feat.propsType == EPropsType::Device );
                    const auto  feat_name = String{feat.shortName} << "Feats";
                    tmp << "\t\t\t\t*next_feat = &_properties." << feat_name << ";\n"
                        << "\t\t\t\tnext_feat  = &_properties." << feat_name << ".pNext;\n"
                        << "\t\t\t\t_properties." << feat_name << ".sType = " << info->second.featsSType << ";\n";
                }
                if ( info->second.props )
                {
                    const auto  feat_name = String{feat.shortName} << "Props";

                    if ( feat.propsType == EPropsType::Device )
                    {
                        tmp << "\t\t\t\t*next_props = &_properties." << feat_name << ";\n"
                            << "\t\t\t\tnext_props  = &_properties." << feat_name << ".pNext;\n"
                            << "\t\t\t\t_properties." << feat_name << ".sType = " << info->second.propsSType << ";\n";
                    }
                    else
                    if ( feat.propsType == EPropsType::Memory )
                    {
                        tmp << "\t\t\t\t*next_mem = &_properties." << feat_name << ";\n"
                            << "\t\t\t\tnext_mem  = &_properties." << feat_name << ".pNext;\n"
                            << "\t\t\t\t_properties." << feat_name << ".sType = " << info->second.propsSType << ";\n";
                    }
                    else
                        RETURN_ERR( "unsupported property type" );
                }
                if ( tmp.size() )
                    str << "\t\t\tif ( _extensions." << feat.shortName << " )\n\t\t\t{\n" << tmp << "\t\t\t}\n";
            }
        }

        str << "\n\t\t\t*next_feat  = null;"
            << "\n\t\t\t*next_props = null;"
            << "\n\t\t\t*next_mem   = null;\n\n"
            << "\t\t\tvkGetPhysicalDeviceFeatures2KHR( GetVkPhysicalDevice(), OUT &feat2 );\n"
            << "\t\t\tvkGetPhysicalDeviceProperties2KHR( GetVkPhysicalDevice(), OUT &props2 );\n"
            << "\t\t\tvkGetPhysicalDeviceMemoryProperties2KHR( GetVkPhysicalDevice(), OUT &mem_props2 );\n"
            << "\t\t\t*nextFeat = feat2.pNext;\n"
            << "\t\t}else{\n"
            << "\t\t\t*nextFeat = null;\n"
            << "\t\t}\n"
            << "\t}\n";
        return str;
    }

/*
=================================================
    _GetLogFeaturesFunc
=================================================
*/
    String  Generator::_GetLogFeaturesFunc (const FeatureSet &feats) const
    {
        String  str;
        str << "\tString  VDeviceInitializer::_GetVulkanExtensionsString () const\n"
            << "\t{\n"
            << "\t\tString src;\n"
            << "\t\tsrc << \"\\n  ---- instance ----\"";

        usize   j = 0;
        for (auto& feat : feats.instance)
        {
            ++j;
            str << "\n\t\t\t<< \"\\n  " << feat.shortName << ':';
            AppendToString( INOUT str, feat.shortName.size(), feats.maxNameLen, j&1, '.', ' ' );
            str << "\" << ToString( _extensions." << feat.shortName << " )";
        }

        Version2    curr_ver = {1,0};
        for (auto& feat : feats.device)
        {
            if ( curr_ver != feat.coreVersion )
            {
                if ( feat.coreVersion > Version2{1,0} and feat.coreVersion < Version2::Max() )
                    str << "\n\t\t\t<< \"\\n  ---- " << ToString( feat.coreVersion.major ) << "." << ToString( feat.coreVersion.minor ) << " ----\"";
                else
                    str << "\n\t\t\t<< \"\\n  ---- ext ----\"";

                curr_ver = feat.coreVersion;
            }

            ++j;
            str << "\n\t\t\t<< \"\\n  " << feat.shortName << ':';
            AppendToString( INOUT str, feat.shortName.size(), feats.maxNameLen, j&1, '.', ' ' );
            str << "\" << ToString( _extensions." << feat.shortName << " )";
        }

        str << ";\n\t\treturn src;\n"
            << "\t}\n";
        return str;
    }

/*
=================================================
    _GetCheckFeaturesFunc
=================================================
*/
    String  Generator::_GetCheckFeaturesFunc (const FeatureSet &feats) const
    {
        constexpr Version2  NoVer = Version2::Max();

        String  str;
        str << "\tvoid  VDeviceInitializer::_CheckInstanceExtensions ()\n\t{\n"
            << "\t\tCHECK_ERRV(( GetInstanceVersion() >= InstanceVersion{" << ToString(feats.minVer.major) << "," << ToString(feats.minVer.minor) << "} ));\n\n";

        for (auto& feat : feats.instance)
        {
            if ( not feat.enabled )
                continue;

            str << "\t\t_extensions." << feat.shortName;
            AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() ); 
            str << " = ";

            bool    has_ver_check = false;
            if ( feat.coreVersion > feats.minVer and feat.coreVersion != NoVer )
            {
                str << "(GetInstanceVersion() >= InstanceVersion{" << ToString( feat.coreVersion.major ) << ',' << ToString( feat.coreVersion.minor ) << "})";
                has_ver_check = true;
            }

            bool    has_ext_check = false;
            if ( not feat.extension.empty() )
            {
                if ( has_ver_check )
                    str << " or ";

                str << "(";
                if ( feat.requireVersion > feats.minVer )
                    str << "GetInstanceVersion() >= InstanceVersion{" << ToString( feat.requireVersion.major ) << ',' << ToString( feat.requireVersion.minor ) << "} and ";

                str << "HasInstanceExtension( " << ExtToName( feat.extension ) << " ))";
                has_ext_check = true;
            }

            if ( not (has_ver_check or has_ext_check) )
                str << "true";

            str << ";\n";
        }

        str << "\t}\n\n"
            << "\tvoid  VDeviceInitializer::_CheckDeviceExtensions ()\n\t{\n"
            << "\t\tCHECK_ERRV(( GetDeviceVersion() >= DeviceVersion{" << ToString(feats.minVer.major) << "," << ToString(feats.minVer.minor) << "} ));\n\n";

        for (auto& feat : feats.device)
        {
            if ( not feat.enabled )
                continue;

            str << "\t\t_extensions." << feat.shortName;
            AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() );
            str << " = ";

            bool    has_ver_check = false;
            if ( feat.coreVersion > feats.minVer and feat.coreVersion != NoVer )
            {
                str << "(GetDeviceVersion() >= DeviceVersion{" << ToString( feat.coreVersion.major ) << ',' << ToString( feat.coreVersion.minor ) << "})";
                has_ver_check = true;
            }

            bool    has_ext_check = false;
            if ( not feat.extension.empty() )
            {
                if ( has_ver_check )
                    str << " or ";

                str << "(";
                if ( feat.requireVersion > feats.minVer )
                    str << "GetDeviceVersion() >= DeviceVersion{" << ToString( feat.requireVersion.major ) << ',' << ToString( feat.requireVersion.minor ) << "} and ";

                str << "HasDeviceExtension( " << ExtToName( feat.extension ) << " ))";
                has_ext_check = true;
            }

            if ( not (has_ver_check or has_ext_check) )
                str << "true";

            str << ";\n";
        }

        str << "\t}\n";
        return str;
    }

/*
=================================================
    GenVulkanFeatures
=================================================
*/
    bool Generator::GenVulkanFeatures (const Path &outputFolder, Version2 minVer) const
    {
        const auto  feats = _GetFeatures( minVer );

        String  str;
        str << "#ifdef VKFEATS_STRUCT\n"
            << _GetFeaturesBoolStruct( feats ) << "\n\n"
            << _GetFeaturesPropsStruct( feats )
            << "#endif // VKFEATS_STRUCT\n\n\n";

        str << "#ifdef VKFEATS_FN_DECL\n"
            << "\tND_ static Array<const char*>  _GetInstanceExtensions (InstanceVersion ver);\n"
            << "\tND_ static Array<const char*>  _GetDeviceExtensions (DeviceVersion ver);\n"
            << "\tND_ String  _GetVulkanExtensionsString () const;\n"
            << "\tvoid  _InitFeaturesAndProperties (void** nextFeat);\n"
            << "\tvoid  _CheckInstanceExtensions ();\n"
            << "\tvoid  _CheckDeviceExtensions ();\n"
            << "#endif // VKFEATS_FN_DECL\n\n\n";

        str << "#ifdef VKFEATS_FN_IMPL\n"
            << _GetExtensionsListFunc( feats ) << "\n"
            << _GetCheckFeaturesFunc( feats ) << "\n"
            << _GetFeaturesAndPropertiesFunc( feats ) << "\n"
            << _GetLogFeaturesFunc( feats )
            << "#endif // VKFEATS_FN_IMPL\n\n";

      #if not AE_PRIVATE_USE_TABS
        str = Parser::TabsToSpaces( str );
      #endif

        const Path      file_name = outputFolder / "vk_features.h";
        FileWStream     file{ file_name };
        CHECK_ERR( file.IsOpen() );
        CHECK_ERR( file.Write( str ));
        return true;
    }

} // AE::Vulkan
