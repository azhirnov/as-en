// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_pack/pipeline_compiler/ScriptObjects/DescriptorSetLayout.h"

namespace AE::PipelineCompiler
{
	using RenderStatePtr = ScriptRC< struct ScriptRenderState >;


	//
	// Render State
	//

	struct ScriptRenderState final : EnableScriptRC
	{
	// variables
	private:
		const String	_name;
		RenderState		_state;


	// methods
	public:
		ND_ static RenderStatePtr  Create (const String &name)	__Th___;

		ND_ RenderState const&	Get ()							C_NE___	{ return _state; }
		ND_ StringView			Name ()							C_NE___	{ return _name; }

		static void  Bind (const ScriptEnginePtr &se)			__Th___;

	private:
		ScriptRenderState (const String &name)					__NE___;
	};


	// TODO: optional render state ?

} // AE::PipelineCompiler
