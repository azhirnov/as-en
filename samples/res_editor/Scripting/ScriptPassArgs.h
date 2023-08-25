// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptBuffer.h"
#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"
#include "res_editor/Scripting/ScriptRTScene.h"
#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

    //
    // Pass Arguments
    //

    class ScriptPassArgs final
    {
    // types
    public:
        using ResourceUnion_t = Union< NullUnion, ScriptBufferPtr, ScriptImagePtr, ScriptVideoImagePtr, ScriptRTScenePtr >;

        struct Argument
        {
            String              name;
            ResourceUnion_t     res;
            EResourceState      state           = Default;
            String              samplerName;
        };
        using Arguments_t   = Array< Argument >;


    // variables
    private:
        Arguments_t                 _args;
        FlatHashSet< String >       _uniqueNames;
        Function<void(Argument &)>  _onAddArg;


    // methods
    public:
        explicit ScriptPassArgs (Function<void(Argument &)> fn)                                         : _onAddArg{RVRef(fn)} {}

        ND_ bool                Empty ()                                                                C_NE___ { return _args.empty(); }
        ND_ Arguments_t const&  Args ()                                                                 C_NE___ { return _args; }

        void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)                            __Th___;

        void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)                              __Th___;
        void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)                             __Th___;
        void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)                           __Th___;

        void  ArgImageIn (const String &name, const ScriptImagePtr &img)                                __Th___;
        void  ArgImageOut (const String &name, const ScriptImagePtr &img)                               __Th___;
        void  ArgImageInOut (const String &name, const ScriptImagePtr &img)                             __Th___;

        void  ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName)   __Th___;
        void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___;


        template <typename DSL, typename AS, typename AT>
        void  ArgsToDescSet (EShaderStages stages, DSL &dsLayout, AS arraySize, AT accessType)          C_Th___;
        void  InitResources (OUT ResourceArray &resources)                                              C_Th___;
        void  ValidateArgs ()                                                                           C_Th___;
        void  AddLayoutReflection ()                                                                    C_Th___;


    private:
        void  _AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage)            __Th___;
        void  _AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage)             __Th___;
    };



/*
=================================================
    ArgsToDescSet
=================================================
*/
    template <typename DSL, typename AS, typename AT>
    void  ScriptPassArgs::ArgsToDescSet (const EShaderStages stages, DSL &dsLayout, AS arraySize, AT accessType) C_Th___
    {
        for (auto& arg : _args)
        {
            Visit( arg.res,
                [&] (ScriptBufferPtr buf) {
                    if ( buf->HasLayout() ){
                        dsLayout->AddStorageBuffer( uint(stages), arg.name, arraySize, buf->GetTypeName(), accessType, arg.state );
                    }else{
                    // TODO
                    //  dsLayout->AddStorageTexelBuffer( uint(stages), arg.name, arraySize, PipelineCompiler::EImageType(buf->TexelBufferType()),
                    //                                   buf->GetViewFormat(), accessType, arg.state );
                    }
                },
                [&] (ScriptImagePtr tex) {
                    const auto  type = PipelineCompiler::EImageType(tex->ImageType());
                    if ( arg.samplerName.empty() )
                        dsLayout->AddStorageImage( uint(stages), arg.name, arraySize, type, tex->Description().format, accessType, arg.state );
                    else
                        dsLayout->AddCombinedImage_ImmutableSampler( uint(stages), arg.name, type, arg.state, arg.samplerName );
                },
                [&] (ScriptVideoImagePtr video) {
                    dsLayout->AddCombinedImage_ImmutableSampler( uint(stages), arg.name, PipelineCompiler::EImageType(video->ImageType()), arg.state, arg.samplerName );
                },
                [&] (ScriptRTScenePtr) {
                    dsLayout->AddRayTracingScene( uint(stages), arg.name, arraySize );
                },
                [] (NullUnion) {
                    CHECK_THROW_MSG( false, "unsupported argument type" );
                }
            );
        }
    }


} // AE::ResEditor
