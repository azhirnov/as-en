// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/ScriptObjects/ScriptRenderState.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static ScriptRenderState*  ScriptRenderState_Ctor (const String &name) {
		return ScriptRenderState::Create( name ).Detach();
	}

} // namespace

/*
=================================================
	constructor
=================================================
*/
	ScriptRenderState::ScriptRenderState (const String &name) __NE___ :
		_name{ name }
	{}

	RenderStatePtr  ScriptRenderState::Create (const String &name) __Th___
	{
		RenderStatePtr	result {new ScriptRenderState{ name }};

		auto&	map = ObjectStorage::Instance()->renderStatesMap;
		CHECK_THROW_MSG( map.emplace( name, result ).second,
			"RenderState with name '"s << name << "' is already defined" );

		return result;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRenderState::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptRenderState>	binder{ se };
		binder.CreateRef( 0, False{} );

		binder.Comment( "Create render state.\n"
						"Name is used only in script." );
		binder.AddFactoryCtor( &ScriptRenderState_Ctor, {"name"} );

		binder.AddProperty( &ScriptRenderState::_state, &RenderState::color,			"color"			);
		binder.AddProperty( &ScriptRenderState::_state, &RenderState::depth,			"depth"			);
		binder.AddProperty( &ScriptRenderState::_state, &RenderState::stencil,			"stencil"		);
		binder.AddProperty( &ScriptRenderState::_state, &RenderState::inputAssembly,	"inputAssembly"	);
		binder.AddProperty( &ScriptRenderState::_state, &RenderState::rasterization,	"rasterization"	);
		binder.AddProperty( &ScriptRenderState::_state, &RenderState::multisample,		"multisample"	);
	}


} // AE::PipelineCompiler
