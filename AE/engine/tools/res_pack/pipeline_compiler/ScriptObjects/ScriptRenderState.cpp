// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptRenderState.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
    static ScriptRenderState*  ScriptRenderState_Ctor (const String &name) {
        return RenderStatePtr{ new ScriptRenderState{ name }}.Detach();
    }

} // namespace

/*
=================================================
    constructor
=================================================
*/
    ScriptRenderState::ScriptRenderState (const String &name) :
        _name{ name }
    {
        auto&   map = ObjectStorage::Instance()->renderStatesMap;
        CHECK_THROW_MSG( map.emplace( name, RenderStatePtr{this} ).second,
            "RenderState with name '"s << name << "' is already defined" );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptRenderState::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptRenderState>  binder{ se };
        binder.CreateRef();

        binder.Comment( "Create render state.\n"
                        "Name is used only in script." );
        binder.AddFactoryCtor( &ScriptRenderState_Ctor, {"name"} );

        binder.AddProperty( &ScriptRenderState::_state, &RenderState::color,            "color"         );
        binder.AddProperty( &ScriptRenderState::_state, &RenderState::depth,            "depth"         );
        binder.AddProperty( &ScriptRenderState::_state, &RenderState::stencil,          "stencil"       );
        binder.AddProperty( &ScriptRenderState::_state, &RenderState::inputAssembly,    "inputAssembly" );
        binder.AddProperty( &ScriptRenderState::_state, &RenderState::rasterization,    "rasterization" );
        binder.AddProperty( &ScriptRenderState::_state, &RenderState::multisample,      "multisample"   );
    }


} // AE::PipelineCompiler
