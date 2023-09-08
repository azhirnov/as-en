// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Generator.h"
#include "base/Algorithms/Parser.h"

namespace AE::Vulkan
{
    using namespace std::string_view_literals;

/*
=================================================
    _IsWord
=================================================
*/
    bool Generator::_IsWord (StringView value)
    {
        for (usize i = 0; i < value.length(); ++i)
        {
            const char  c = value[i];
            bool        valid = false;

            valid |= (c >= 'a' and c <= 'z');
            valid |= (c >= 'A' and c <= 'Z');
            valid |= (c == '_');

            if ( i > 0 )
                valid |= (c >= '0' and c <= '9');

            if ( not valid )
                return false;
        }
        return true;
    }

/*
=================================================
    _IsNumber
=================================================
*/
    bool Generator::_IsNumber (StringView value)
    {
        if ( EndsWithIC( value, "ull" ))
            value = value.substr( 0, value.length()-3 );
        else
        if ( EndsWithIC( value, "ul" ))
            value = value.substr( 0, value.length()-2 );
        else
        if ( EndsWithIC( value, "u" ))
            value = value.substr( 0, value.length()-1 );


        if ( StartsWithIC( value, "0x" ))
        {
            value = value.substr( 2 );
            for (auto& c : value)
            {
                if ( not (c >= '0' and c <= '9') and
                     not (c >= 'A' and c <= 'F') and
                     not (c >= 'a' and c <= 'f') )
                    return false;
            }
        }
        else
        {
            if ( value[0] == '-' or value[0] == '+' )
                value = value.substr( 1 );

            for (auto& c : value)
            {
                if ( not (c >= '0' and c <= '9') )
                    return false;
            }
        }
        return true;
    }

/*
=================================================
    _IsTypeOrQual
=================================================
*/
    bool Generator::_IsTypeOrQual (StringView value)
    {
        return  value == "*"        or
                value == "["        or
                value == "]"        or
                _IsWord( value )    or
                _IsNumber( value );
    }

/*
=================================================
    _GenerateVK1
=================================================
*/
    bool Generator::_GenerateVK1 (StringView fileData,
                                  const uint fileIndex,
                                  ArrayView< StringView > enableIfdef,
                                  ArrayView< StringView > disableIfdef,
                                  bool defaultSkip)
    {
        enum class EMode
        {
            None,
            Struct,
            Enum,
            Func,
            Define,
        };

        Array< StringView > lines;
        Parser::DivideLines( fileData, lines );

        VkEnumInfo          curr_enum;
        VkStructInfo        curr_struct;
        VkFunctionInfo      curr_func;

        Array< StringView > tokens;
        EMode               mode            = EMode::None;
        bool                is_voidfunc     = false;

        StringView          curr_ext;
        Array<bool>         skip_stack;     skip_stack.push_back( defaultSkip );

        const Array< StringView >   skip_funcs = {
            // VkAllocationCallbacks
            "vkAllocationFunction",
            "vkReallocationFunction",
            "vkFreeFunction",
            "vkInternalAllocationNotification",
            "vkInternalFreeNotification",

            "vkVoidFunction",

            "VkDebugReportCallbackEXT",
        };

        const auto  ParseArgs   = [&mode, &is_voidfunc, &curr_func, &curr_ext, this] (ArrayView<StringView> tokens, usize j)
        {{
            FuncArg     curr_arg;

            for (; j < tokens.size(); ++j)
            {
                if ( not is_voidfunc )
                {
                    if ( tokens[j] == "," or tokens[j] == ")" )
                    {
                        // skip array type
                        if ( curr_arg.type.back() == "]" )
                        {
                            CHECK( curr_arg.type.size() > 3 );
                            curr_arg.name = *(curr_arg.type.end()-4);
                            curr_arg.type.erase( curr_arg.type.end()-4 );
                        }
                        else
                        {
                            curr_arg.name = curr_arg.type.back();
                            curr_arg.type.pop_back();
                        }

                        CHECK( _IsWord( curr_arg.name ) and not curr_arg.name.empty() );
                        CHECK( not curr_arg.type.empty() );

                        curr_func.args.push_back( RVRef(curr_arg) );
                        curr_arg = Default;
                    }
                    else
                    {
                        CHECK( _IsTypeOrQual( tokens[j] ));
                        curr_arg.type.push_back( tokens[j] );
                    }
                }

                if ( tokens[j] == ")" )
                {
                    mode = EMode::None;
                    break;
                }
            }

            if ( mode == EMode::None and not is_voidfunc )
            {
                CHECK( _funcs.find( SearchableFunc{curr_func.name} ) == _funcs.end() );

                curr_func.extension = curr_ext;
                CHECK( _funcs.insert( RVRef(curr_func) ).second );
                curr_func = Default;
            }
        }};

        const auto  ParseEnumField  = [] (ArrayView<StringView> tokens, usize i, OUT EnumField &outValue)
        {{
            // find name
            if ( i < tokens.size() )
            {
                CHECK( StartsWith( tokens[i], "VK_" ));
                CHECK( _IsWord( tokens[i] ));

                outValue.name = tokens[i];
                ++i;
            }

            // find value
            for (; i < tokens.size(); ++i)
            {
                if ( tokens[i] == "=" )
                    continue;

                //CHECK( _IsNumber( tokens[i] ));

                outValue.value = tokens[i];
                break;
            }

            CHECK( not outValue.name.empty()  and
                    not outValue.value.empty() );
        }};

        const auto  ParseStructField = [] (ArrayView<StringView> tokens, usize i, OUT FuncArg &outField)
        {{
            for (; i < tokens.size(); ++i)
            {
                if ( tokens[i] == "[" )
                {
                    // skip array
                    CHECK( tokens.size() >= i+2 );
                    CHECK( tokens[i+2] == "]" );
                    i += 2;
                }
                else
                if ( tokens[i] == ";" or tokens[i] == ":" )
                {
                    outField.name = outField.type.back();
                    outField.type.pop_back();

                    CHECK( _IsWord( outField.name ) and not outField.name.empty() );
                    CHECK( not outField.type.empty() );
                    return;
                }
                else
                {
                    CHECK( _IsTypeOrQual( tokens[i] ));
                    outField.type.push_back( tokens[i] );
                }
            }
        }};


        for (auto& curr_line : lines)
        {
            Parser::DivideString_CPP( curr_line, OUT tokens );

            switch ( mode )
            {
                case EMode::Struct :
                {
                    if ( tokens.size() > 1 and
                         tokens[0] == "}" )
                    {
                        CHECK( _structs.find( SearchableStruct{curr_struct.name} ) == _structs.end() );

                        curr_struct.extension = curr_ext;
                        CHECK( _structs.insert( RVRef(curr_struct) ).second );

                        curr_struct = Default;
                        mode        = EMode::None;
                    }
                    else
                    {
                        FuncArg     val;
                        ParseStructField( tokens, 0, OUT val );
                        curr_struct.fields.push_back( RVRef(val) );
                    }
                    break;
                }

                case EMode::Enum :
                {
                    if ( tokens.size() > 1 and
                         tokens[0] == "}" )
                    {
                        CHECK( _enums.find( SearchableEnum{curr_enum.name} ) == _enums.end() );

                        curr_enum.extension = curr_ext;
                        CHECK( _enums.insert( RVRef(curr_enum) ).second );

                        curr_enum   = Default;
                        mode        = EMode::None;
                    }
                    else
                    if ( tokens.size() >= 2 and
                         (tokens[0] == "#" and (tokens[1] == "ifdef" or tokens[1] == "endif")) )
                    {
                        // skip #ifdef beta_extension #endif
                    }
                    else
                    {
                        EnumField   val;
                        ParseEnumField( tokens, 0, OUT val );
                        curr_enum.fields.push_back( RVRef(val) );
                    }
                    break;
                }

                case EMode::Func :
                {
                    ParseArgs( tokens, 0 );
                    break;
                }

                case EMode::Define :
                {
                    if ( tokens.empty() or tokens.back() != "\\" )
                    {
                        mode = EMode::None;
                    }
                    break;
                }
            }


            if ( tokens.size() > 2      and
                 tokens[0] == "#"       and
                 tokens[1] == "define" )
            {
                if ( _extensions.count( tokens[2] ))
                {
                    curr_ext = tokens[2];
                    AE_LOGI( "Ext: "s << curr_ext );
                }
            }

            if ( tokens.size() == 3     and
                 tokens[0] == "#"       and
                 tokens[1] == "ifndef"  and
                 tokens[2] == "VK_NO_PROTOTYPES" )
            {
                curr_ext = {};
            }

            if ( tokens.size() > 1 and
                 tokens[0] == "#" and
                 tokens[1] == "endif" )
            {
                skip_stack.pop_back();
                continue;
            }

            // is need to skip
            if ( tokens.size() > 2 and
                 tokens[0] == "#" and
                (tokens[1] == "ifdef" or tokens[1] == "ifndef") )
            {
                bool    is_ifdef    = ( tokens[1] == "ifdef" );
                bool    enabled     = false;
                bool    disabled    = false;

                for (auto& item : enableIfdef)
                {
                    if ( item == tokens[2] )
                    {
                        enabled = true;
                        break;
                    }
                }

                for (auto& item : disableIfdef)
                {
                    if ( item == tokens[2] )
                    {
                        disabled = true;
                        break;
                    }
                }

                // enabled == false and disable == false is OK
                CHECK( (not enabled and not disabled) or enabled != disabled );

                skip_stack.push_back( is_ifdef ? not enabled and disabled : not( not enabled and disabled ));
                continue;
            }

            if ( tokens.size() > 2      and
                 tokens[0] == "#"       and
                 tokens[1] == "if" )
            {
                auto    last = skip_stack.back();
                skip_stack.push_back( last );
                continue;
            }

            if ( tokens.size() > 2      and
                 tokens[0] == "#"       and
                 (tokens[1] == "ifdef" or tokens[1] == "ifndef" or tokens[1] == "if") )
            {
                auto    last = skip_stack.back();
                skip_stack.push_back( last );
                continue;
            }

            if ( skip_stack.back() )
                continue;

            // struct / enum / union
            if ( tokens.size() > 3              and
                 tokens[0] == "typedef"         and
                 (tokens[1] == "struct" or tokens[1] == "enum" or tokens[1] == "union") and
                 StartsWith( tokens[2], "Vk" ))
            {
                if ( tokens[1] == "enum" )
                {
                    curr_enum.name          = tokens[2];
                    curr_enum.fileIndex     = fileIndex;
                    mode                    = EMode::Enum;
                }
                else
                {
                    curr_struct.name        = tokens[2];
                    curr_struct.fileIndex   = fileIndex;
                    mode                    = EMode::Struct;
                }
                continue;
            }


            // function
            if ( tokens.size() > 1      and
                 tokens[0] == "typedef" )
            {
                StringView  prefix      = "PFN_";
                usize       ret_pos     = 0;
                usize       name_pos    = 0;

                for (usize j = 1; j < tokens.size(); ++j) {
                    if ( tokens[j-1] == "(" and tokens[j] == "VKAPI_PTR" ) {
                        ret_pos = j-1;
                        break;
                    }
                }

                for (usize j = ret_pos; j < tokens.size(); ++j) {
                    if ( StartsWithIC( tokens[j], prefix )) {
                        name_pos = j;
                        break;
                    }
                }

                if ( name_pos > 0 )
                {
                    mode = EMode::Func;

                    StringView  func_name = tokens[name_pos].substr( prefix.length() );

                    is_voidfunc = false;

                    for (auto& item : skip_funcs)
                    {
                        if ( func_name == item )
                        {
                            is_voidfunc = true;
                            break;
                        }
                    }

                    if ( not is_voidfunc )
                    {
                        curr_func.fileIndex = fileIndex;
                        curr_func.name      = func_name;

                        CHECK( _IsWord( curr_func.name ));

                        for (usize k = 1; k < ret_pos; ++k) {
                            curr_func.result.type.push_back( tokens[k] );
                        }
                    }

                    usize   j = name_pos;

                    // move to args
                    for (; j < tokens.size(); ++j) {
                        if ( tokens[j] == "(" )
                            break;
                    }

                    ParseArgs( tokens, ++j );
                    continue;
                }
            }


            // bitfield
            if ( tokens.size() == 4     and
                 tokens[0] == "typedef" and
                 tokens[1] == "VkFlags" and
                 tokens[3] == ";" )
            {
                String  enum_name { tokens[2] };

                if ( enum_name.back() == 's' )
                {
                    enum_name.pop_back();
                    enum_name << "Bits";
                }

                auto    iter = _enums.find( SearchableEnum{enum_name} );

                VkBitfieldInfo  bitfield;
                bitfield.name       = tokens[2];
                bitfield.enumName   = iter != _enums.end() ? iter->data.name : "";
                bitfield.fileIndex  = fileIndex;
                bitfield.extension  = curr_ext;

                CHECK( _bitfields.insert( bitfield ).second );
                continue;
            }


            // alias
            if ( tokens.size() == 4                     and
                 tokens[0] == "typedef"                 and
                 tokens[3] == ";"                       and
                 not _basicTypes.contains( tokens[1] )  and
                 not _resourceTypes.contains( tokens[1] ))
            {
                if ( StartsWith( tokens[2], tokens[1] ))
                {
                    auto    st_iter = _structs.find( SearchableStruct{tokens[1]} );
                    if ( st_iter != _structs.end() )
                    {
                        auto    alias = st_iter->data;

                        CHECK( alias.extension.empty() );
                        CHECK( not curr_ext.empty() );

                        alias.name      = tokens[2];
                        alias.extension = curr_ext;

                        CHECK( _structs.insert( RVRef(alias) ).second );
                        continue;
                    }

                    auto    en_iter = _enums.find( SearchableEnum{tokens[1]} );
                    if ( en_iter != _enums.end() )
                    {
                        continue;
                    }

                    auto    bf_iter = _bitfields.find( SearchableBitfield{tokens[1]} );
                    if ( bf_iter != _bitfields.end() )
                    {
                        continue;
                    }

                    AE_LOGI( "skip alias: "s << tokens[2] << " = " << tokens[1] );
                }
                else
                    AE_LOGI( "skip alias: "s << tokens[2] << " = " << tokens[1] );
            }


            // define
            if ( tokens.size() > 1      and
                 tokens[0] == "#"       and
                 tokens[1] != "include" )
            {
                mode = ( tokens.back() == "\\" ? EMode::Define : EMode::None );
                continue;
            }

            // const
            if ( tokens.size() > 5      and
                 tokens[0] == "static"  and
                 tokens[1] == "const"   and
                 tokens[4] == "=" )
            {
                VkConstInfo ci;
                ci.type     = tokens[2];
                ci.name     = tokens[3];
                ci.value    = tokens[5];

                const char* end = ci.value.data();
                for (; *end != ';' and *end != '\0'; ++end) {}
                ci.value = StringView{ ci.value.data(), end };

                auto    iter = _constants.emplace( ci.type, ConstSet_t{} ).first;

                iter->second.push_back( RVRef(ci) );
                continue;
            }

            if ( tokens.size() > 1 and tokens[0] == "#" )
                CHECK( tokens[1] == "include" );
        }

        CHECK( skip_stack.size() == 1 );
        return true;
    }

/*
=================================================
    ParseVkHeaders
=================================================
*/
    bool Generator::ParseVkHeaders (const Path &folder)
    {
        CHECK_ERR( FileSystem::IsDirectory( folder ));

        struct VkHeaderFile
        {
            StringView              filename;
            ArrayView<StringView>   enableIfdef;
            ArrayView<StringView>   disableIfdef;
            bool                    defaultSkip;
        };

        const VkHeaderFile  file_names[] = {
            { "vulkan_core.h",          { "VK_NO_PROTOTYPES" },     { "VULKAN_CORE_H_",         "__cplusplus" },    false },
            { "vulkan_beta.h",          { "VK_NO_PROTOTYPES" },     { "VULKAN_BETA_H_",         "__cplusplus" },    false }
            /*,
            { "vulkan_win32.h",         { "VK_NO_PROTOTYPES" },     { "VULKAN_WIN32_H_",        "__cplusplus" },    true },
            { "vulkan_android.h",       { "VK_NO_PROTOTYPES" },     { "VULKAN_ANDROID_H_",      "__cplusplus" },    true },
            { "vulkan_xlib.h",          { "VK_NO_PROTOTYPES" },     { "VULKAN_XLIB_H_",         "__cplusplus" },    true },
            { "vulkan_xlib_xrandr.h",   { "VK_NO_PROTOTYPES" },     { "VULKAN_XLIB_XRANDR_H_",  "__cplusplus" },    true },
            { "vulkan_xcb.h",           { "VK_NO_PROTOTYPES" },     { "VULKAN_XCB_H_",          "__cplusplus" },    true },
            //{ "vulkan_mir.h",         { "VK_NO_PROTOTYPES" },     { "VULKAN_MIR_H_",          "__cplusplus" },    true },
            { "vulkan_wayland.h",       { "VK_NO_PROTOTYPES" },     { "VULKAN_WAYLAND_H_",      "__cplusplus" },    true }*/
        };

        _funcs.clear();
        _enums.clear();
        _bitfields.clear();
        _structs.clear();

        for (auto& info : file_names)
        {
            const Path      path = folder / info.filename;
            FileRStream     file{ path };

            if ( not file.IsOpen() )
                continue;

            _fileData.push_back({ "", info.disableIfdef.front() });

            String&     buf = _fileData.back().data;
            CHECK( file.Read( usize(file.Size()), OUT buf ));

            CHECK_ERR( _BuildExtensionList( buf ));
            CHECK_ERR( _GenerateVK1( buf, uint(_fileData.size()-1), info.enableIfdef, info.disableIfdef, info.defaultSkip ));
        }

        _BuildExtensionInfo();
        return true;
    }

/*
=================================================
    SetFunctionsScope
=================================================
*/
    bool Generator::SetFunctionsScope ()
    {
        CHECK_ERR( not _resourceTypes.empty() );

        const HashSet<StringView>   lib_fns = {
            "vkGetInstanceProcAddr"
        };
        const HashSet<StringView>   inst_fns = {
            "vkDeviceMemoryReportCallbackEXT",
            "vkDebugReportCallbackEXT",
            "vkDebugUtilsMessengerCallbackEXT",
            "vkGetDeviceProcAddr"
        };

        for (auto& fn : _funcs)
        {
            CHECK_ERR( not fn.data.args.empty() );

            ResourceTypes_t::const_iterator     res_info;

            for (auto& type : fn.data.args.front().type)
            {
                res_info = _resourceTypes.find( type );
                if ( res_info != _resourceTypes.end() )
                    break;
            }

            bool    is_inst = inst_fns.find( fn.data.name ) != inst_fns.end();
            bool    is_lib  = lib_fns.find( fn.data.name ) != lib_fns.end();

            if ( not is_inst and
                (res_info == _resourceTypes.end() or is_lib) )
            {
                fn.data.scope = EFuncScope::Library;
            }
            else
            if ( res_info->second.type == VK_OBJECT_TYPE_INSTANCE or
                 res_info->second.type == VK_OBJECT_TYPE_PHYSICAL_DEVICE or
                 is_inst )
            {
                fn.data.scope = EFuncScope::Instance;
            }
            else
            {
                fn.data.scope = EFuncScope::Device;
            }
        }
        return true;
    }

/*
=================================================
    BuildBasicTypeMap
=================================================
*/
    bool Generator::BuildBasicTypeMap ()
    {
        _basicTypes.clear();
        _basicTypes.insert({ "void",            EBasicType::Void });
        _basicTypes.insert({ "VkBool32",        EBasicType::Bool });
        _basicTypes.insert({ "char",            EBasicType::Char });
        _basicTypes.insert({ "int",             EBasicType::Int });
        _basicTypes.insert({ "int32_t",         EBasicType::Int });
        _basicTypes.insert({ "uint32_t",        EBasicType::UInt });
        _basicTypes.insert({ "uint8_t",         EBasicType::UInt });
        _basicTypes.insert({ "uint16_t",        EBasicType::UInt });
        _basicTypes.insert({ "uint64_t",        EBasicType::ULong });
        _basicTypes.insert({ "int64_t",         EBasicType::Long });
        _basicTypes.insert({ "size_t",          EBasicType::USize });
        _basicTypes.insert({ "VkDeviceSize",    EBasicType::ULong });
        //_basicTypes.insert({ "VkFlags",       EBasicType::UInt });
        _basicTypes.insert({ "VkSampleMask",    EBasicType::UInt });
        _basicTypes.insert({ "float",           EBasicType::Float });
        _basicTypes.insert({ "double",          EBasicType::Double });
        _basicTypes.insert({ "VkDeviceAddress", EBasicType::Handle });
        //_basicTypes.insert({ "PFN_vkDebugUtilsMessengerCallbackEXT",  EBasicType::Handle });
        _basicTypes.insert({ "VkDebugReportCallbackEXT",                EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkDebugReportCallbackEXT",            EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkAllocationFunction",                EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkReallocationFunction",              EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkFreeFunction",                      EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkInternalAllocationNotification",    EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkInternalFreeNotification",          EBasicType::Handle });
        _basicTypes.insert({ "PFN_vkDebugUtilsMessengerCallbackEXT",    EBasicType::Handle });

        // vulkan_android
        _basicTypes.insert({ "ANativeWindow",       EBasicType::Struct });
        _basicTypes.insert({ "AHardwareBuffer",     EBasicType::Struct });

        // vulkan_mir
        _basicTypes.insert({ "MirConnection",       EBasicType::Struct });
        _basicTypes.insert({ "MirSurface",          EBasicType::Struct });

        // vulkan_wayland
        _basicTypes.insert({ "wl_display",          EBasicType::Struct });
        _basicTypes.insert({ "wl_surface",          EBasicType::Struct });

        // vulkan_win32
        _basicTypes.insert({ "HINSTANCE",           EBasicType::Handle });
        _basicTypes.insert({ "HWND",                EBasicType::Handle });
        _basicTypes.insert({ "HANDLE",              EBasicType::Handle });
        _basicTypes.insert({ "LPCWSTR",             EBasicType::WCharString });
        _basicTypes.insert({ "SECURITY_ATTRIBUTES", EBasicType::Struct });
        _basicTypes.insert({ "DWORD",               EBasicType::UInt });
        _basicTypes.insert({ "HMONITOR",            EBasicType::Handle });

        // vulkan_xcb
        _basicTypes.insert({ "xcb_connection_t",    EBasicType::Struct });
        _basicTypes.insert({ "xcb_window_t",        EBasicType::Handle });
        _basicTypes.insert({ "xcb_visualid_t",      EBasicType::Handle });  // TODO: check

        // vulkan_xlib
        _basicTypes.insert({ "Display",             EBasicType::Struct });
        _basicTypes.insert({ "Window",              EBasicType::Handle });
        _basicTypes.insert({ "VisualID",            EBasicType::Handle });  // TODO: check

        // vulkan_xlib_xrandr
        _basicTypes.insert({ "RROutput",            EBasicType::Handle });  // TODO: check

        return true;
    }

/*
=================================================
    BuildResourceTypeMap
=================================================
*/
    bool Generator::BuildResourceTypeMap ()
    {
        _resourceTypes.clear();
        _resourceTypes.insert({ "VkInstance",                   { VK_OBJECT_TYPE_INSTANCE,                      "VK_OBJECT_TYPE_INSTANCE" }});
        _resourceTypes.insert({ "VkPhysicalDevice",             { VK_OBJECT_TYPE_PHYSICAL_DEVICE,               "VK_OBJECT_TYPE_PHYSICAL_DEVICE" }});
        _resourceTypes.insert({ "VkDevice",                     { VK_OBJECT_TYPE_DEVICE,                        "VK_OBJECT_TYPE_DEVICE" }});
        _resourceTypes.insert({ "VkQueue",                      { VK_OBJECT_TYPE_QUEUE,                         "VK_OBJECT_TYPE_QUEUE" }});
        _resourceTypes.insert({ "VkSemaphore",                  { VK_OBJECT_TYPE_SEMAPHORE,                     "VK_OBJECT_TYPE_SEMAPHORE" }});
        _resourceTypes.insert({ "VkCommandBuffer",              { VK_OBJECT_TYPE_COMMAND_BUFFER,                "VK_OBJECT_TYPE_COMMAND_BUFFER" }});
        _resourceTypes.insert({ "VkFence",                      { VK_OBJECT_TYPE_FENCE,                         "VK_OBJECT_TYPE_FENCE" }});
        _resourceTypes.insert({ "VkDeviceMemory",               { VK_OBJECT_TYPE_DEVICE_MEMORY,                 "VK_OBJECT_TYPE_DEVICE_MEMORY" }});
        _resourceTypes.insert({ "VkBuffer",                     { VK_OBJECT_TYPE_BUFFER,                        "VK_OBJECT_TYPE_BUFFER" }});
        _resourceTypes.insert({ "VkImage",                      { VK_OBJECT_TYPE_IMAGE,                         "VK_OBJECT_TYPE_IMAGE" }});
        _resourceTypes.insert({ "VkEvent",                      { VK_OBJECT_TYPE_EVENT,                         "VK_OBJECT_TYPE_EVENT" }});
        _resourceTypes.insert({ "VkQueryPool",                  { VK_OBJECT_TYPE_QUERY_POOL,                    "VK_OBJECT_TYPE_QUERY_POOL" }});
        _resourceTypes.insert({ "VkBufferView",                 { VK_OBJECT_TYPE_BUFFER_VIEW,                   "VK_OBJECT_TYPE_BUFFER_VIEW" }});
        _resourceTypes.insert({ "VkImageView",                  { VK_OBJECT_TYPE_IMAGE_VIEW,                    "VK_OBJECT_TYPE_IMAGE_VIEW" }});
        _resourceTypes.insert({ "VkShaderModule",               { VK_OBJECT_TYPE_SHADER_MODULE,                 "VK_OBJECT_TYPE_SHADER_MODULE" }});
        _resourceTypes.insert({ "VkPipelineCache",              { VK_OBJECT_TYPE_PIPELINE_CACHE,                "VK_OBJECT_TYPE_PIPELINE_CACHE" }});
        _resourceTypes.insert({ "VkPipelineLayout",             { VK_OBJECT_TYPE_PIPELINE_LAYOUT,               "VK_OBJECT_TYPE_PIPELINE_LAYOUT" }});
        _resourceTypes.insert({ "VkRenderPass",                 { VK_OBJECT_TYPE_RENDER_PASS,                   "VK_OBJECT_TYPE_RENDER_PASS" }});
        _resourceTypes.insert({ "VkPipeline",                   { VK_OBJECT_TYPE_PIPELINE,                      "VK_OBJECT_TYPE_PIPELINE" }});
        _resourceTypes.insert({ "VkDescriptorSetLayout",        { VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,         "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT" }});
        _resourceTypes.insert({ "VkSampler",                    { VK_OBJECT_TYPE_SAMPLER,                       "VK_OBJECT_TYPE_SAMPLER" }});
        _resourceTypes.insert({ "VkDescriptorPool",             { VK_OBJECT_TYPE_DESCRIPTOR_POOL,               "VK_OBJECT_TYPE_DESCRIPTOR_POOL" }});
        _resourceTypes.insert({ "VkDescriptorSet",              { VK_OBJECT_TYPE_DESCRIPTOR_SET,                "VK_OBJECT_TYPE_DESCRIPTOR_SET" }});
        _resourceTypes.insert({ "VkFramebuffer",                { VK_OBJECT_TYPE_FRAMEBUFFER,                   "VK_OBJECT_TYPE_FRAMEBUFFER" }});
        _resourceTypes.insert({ "VkCommandPool",                { VK_OBJECT_TYPE_COMMAND_POOL,                  "VK_OBJECT_TYPE_COMMAND_POOL" }});
        _resourceTypes.insert({ "VkSurfaceKHR",                 { VK_OBJECT_TYPE_SURFACE_KHR,                   "VK_OBJECT_TYPE_SURFACE_KHR" }});
        _resourceTypes.insert({ "VkSwapchainKHR",               { VK_OBJECT_TYPE_SWAPCHAIN_KHR,                 "VK_OBJECT_TYPE_SWAPCHAIN_KHR" }});
        _resourceTypes.insert({ "VkDisplayKHR",                 { VK_OBJECT_TYPE_DISPLAY_KHR,                   "VK_OBJECT_TYPE_DISPLAY_KHR" }});
        _resourceTypes.insert({ "VkDisplayModeKHR",             { VK_OBJECT_TYPE_DISPLAY_MODE_KHR,              "VK_OBJECT_TYPE_DISPLAY_MODE_KHR" }});
        _resourceTypes.insert({ "VkValidationCacheEXT",         { VK_OBJECT_TYPE_VALIDATION_CACHE_EXT,          "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT" }});
        _resourceTypes.insert({ "VkSamplerYcbcrConversion",     { VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION,      "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION" }});
        _resourceTypes.insert({ "VkDescriptorUpdateTemplate",   { VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE,    "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE" }});
        _resourceTypes.insert({ "VkAccelerationStructureNV",    { VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV,     "VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV" }});


        // TODO: validation
        return true;
    }

/*
=================================================
    _BuildExtensionList
=================================================
*/
    bool Generator::_BuildExtensionList (StringView fileData)
    {
        Array< StringView > lines;
        Parser::DivideLines( fileData, lines );

        Array< StringView > tokens;

        for (auto& curr_line : lines)
        {
            Parser::DivideString_CPP( curr_line, OUT tokens );

            if ( tokens.size() > 5                          and
                 tokens[0] == "#"                           and
                 tokens[1] == "define"                      and
                 EndsWith( tokens[2], "_EXTENSION_NAME" )   and
                 tokens[3] == "\""                          and
                 tokens[5] == "\""                          )
            {
                CHECK( _extensions.insert( tokens[4] ).second );
            }
        }
        return true;
    }

/*
=================================================
    ExtToStruct
=================================================
*/
    ND_ inline String  ExtToStruct (StringView extName, StringView suffix)
    {
        String  res;
        usize   num_divs = 0;
        for (usize i = 1; i < extName.size(); ++i)
        {
            const char  c = extName[i];
            const char  p = extName[i-1];
            if ( c == '_' )
                ++num_divs;
            else
            if ( num_divs >= 2 )
                res << (p == '_' ? ToUpperCase(c) : ToLowerCase(c));
        }
        return res << suffix;
    }

/*
=================================================
    _BuildExtensionInfo
=================================================
*/
    void Generator::_BuildExtensionInfo ()
    {
        const auto  FindSType = [&] (StringView structName) -> StringView
        {{
            auto    en_iter = _enums.find( SearchableEnum{"VkStructureType"} );
            CHECK_ERR( en_iter != _enums.end() );

            const StringView    field_prefix    = "VK_STRUCTURE_TYPE_";
            const StringView    rhs             = structName.substr( "Vk"sv.size() );

            StringView  best_match;
            usize       num_chars   = 0;

            for (const auto& field : en_iter->data.fields)
            {
                const auto  lhs = field.name.substr( field_prefix.size() );

                for (usize lpos = 0, rpos = 0, match = 0;;)
                {
                    if ( lpos >= lhs.size() or rpos >= rhs.size() )
                    {
                        if ( match > num_chars )
                        {
                            num_chars   = match;
                            best_match  = field.name;
                        }
                        break;
                    }

                    if ( ToLowerCase(lhs[lpos]) == ToLowerCase(rhs[rpos]) )
                    {
                        ++lpos;
                        ++rpos;
                        ++match;
                        continue;
                    }

                    ++lpos;
                }

                if ( num_chars == rhs.size() )
                    break;
            }

            if ( num_chars + 2 < rhs.size() )
                RETURN_ERR( "Failed to find sType for "s << structName );

            return best_match;
        }};

        const HashSet<StringView>   skip_ext = {
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
        };

        for (auto iter = _structs.begin(); iter != _structs.end(); ++iter)
        {
            auto&   ext = iter->data.extension;

            if ( ext.empty() )
                continue;

            if ( skip_ext.contains( ext ))
                continue;

            auto&   info = _extInfo[ ext ];
            info.structs.push_back( iter );

            if ( StartsWith( iter->data.name, "VkPhysicalDevice" ))
            {
                if ( HasSubStringIC( iter->data.name, ExtToStruct( ext, "Features" )))
                {
                    CHECK( not info.feats.has_value() );
                    info.feats      = iter;
                    info.featsSType = FindSType( iter->data.name );
                }
                else
                if ( HasSubStringIC( iter->data.name, ExtToStruct( ext, "Properties" )))
                {
                    CHECK( not info.props.has_value() );
                    info.props      = iter;
                    info.propsSType = FindSType( iter->data.name );
                }
                else
                if ( EndsWithIC( ext, "Properties" ) and HasSubStringIC( iter->data.name, ExtToStruct( ext, "" )))
                {
                    CHECK( not info.props.has_value() );
                    info.props      = iter;
                    info.propsSType = FindSType( iter->data.name );
                }
            }
        }

        for (auto iter = _enums.begin(); iter != _enums.end(); ++iter)
        {
            if ( not iter->data.extension.empty() )
                _extInfo[ iter->data.extension ].enums.push_back( iter );
        }

        for (auto iter = _bitfields.begin(); iter != _bitfields.end(); ++iter)
        {
            if ( not iter->data.extension.empty() )
                _extInfo[ iter->data.extension ].bitfields.push_back( iter );
        }
    }

} // AE::Vulkan
