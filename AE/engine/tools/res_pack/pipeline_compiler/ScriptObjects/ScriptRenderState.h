// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/DescriptorSetLayout.h"

namespace AE::PipelineCompiler
{

    //
    // Render State
    //

    struct ScriptRenderState final : EnableScriptRC
    {
    // variables
    private:
        const String    _name;
        RenderState     _state;

    // methods
    public:
        ScriptRenderState () {}
        ScriptRenderState (const String &name);

        ND_ RenderState const&  Get ()                  const   { return _state; }
        ND_ StringView          Name ()                 const   { return _name; }

        static void  Bind (const ScriptEnginePtr &se)   __Th___;
    };
    using RenderStatePtr = ScriptRC< ScriptRenderState >;


    // TODO: optional render state ?

} // AE::PipelineCompiler
