// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBaseRenderPass.h"

namespace AE::ResEditor
{
	//
	// Postprocess
	//

	class ScriptPostprocess final : public ScriptBaseRenderPass
	{
	// types
	public:
		enum class EPostprocess : uint
		{
			Unknown			= 0,	// entry: Main
			Shadertoy,				// entry: mainImage

			ShadertoyVR,			// entry: mainVR
			ShadertoyVR_180,
			ShadertoyVR_360,
			Shadertoy_360,

			_Count,
		};


	// variables
	private:
		const Path				_pplnPath;
		const EPostprocess		_ppFlags;


	// methods
	public:
		ScriptPostprocess () = delete;
		ScriptPostprocess (const String &pipelineName, EPostprocess ppFlags,
						   const String &defines, EFlags baseFlags)										__Th___;

		static void  Bind (const ScriptEnginePtr &se)													__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)										__Th___;

	// ScriptBasePass //
		ND_ RC<IPass>  ToPass ()																		C_Th_OV;


	private:
		ND_ auto  _CompilePipeline (OUT Bytes &ubSize)													C_Th___;
			void  _CompilePipeline2 (OUT Bytes &ubSize)													C_Th___;
			void  _CompilePipeline3 (const String &subpass, const String &vs, const String &fs,
									 uint fsLine, const String &pplnName, uint shaderOpts,
									 EPipelineOpt pplnOpt)												C_Th___;

		ND_ static auto  _CreateUBType ()																__Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)											C_Th_OV;
	};

	AE_BIT_OPERATORS( ScriptPostprocess::EPostprocess );


} // AE::ResEditor
