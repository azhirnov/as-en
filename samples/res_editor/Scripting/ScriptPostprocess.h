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
			Unknown				= 0,		// entry: Main
			Shadertoy			= 1 << 0,	// entry: mainImage
			ShadertoyVR			= 1 << 1,	// entry: mainVR
			ShadertoyVR_180		= 1 << 2,
			ShadertoyVR_360		= 1 << 3,
			Shadertoy_360		= 1 << 4,
			_Last,
			_ShadertoyBits		= Shadertoy | ShadertoyVR | ShadertoyVR_180 | ShadertoyVR_360 | Shadertoy_360,
		};

	private:
		using TextureUnion_t = Union< NullUnion, ScriptImagePtr, ScriptVideoImagePtr, ScriptBufferPtr >;

		struct Input
		{
			TextureUnion_t	tex;
			String			samplerName;
			String			uniformName;
		};


	// variables
	private:
		const Path			_pplnPath;
		const EPostprocess	_ppFlags;
		String				_defines;

		Array<Input>		_input;


	// methods
	public:
		ScriptPostprocess () : ScriptBaseRenderPass{Default}, _ppFlags{Default} {}
		ScriptPostprocess (const String &pipelineName, EPostprocess ppFlags,
						   const String &defines, EFlags baseFlags)										__Th___;
		
		void  InputBuf (const String &name, const ScriptBufferPtr &buf)									__Th___;
		void  InputImg (const String &name, const ScriptImagePtr &tex, const String &samplerName)		__Th___;
		void  InputVideo (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___;
		void  InputController (const ScriptBaseControllerPtr &)											__Th___;

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
	};

	AE_BIT_OPERATORS( ScriptPostprocess::EPostprocess );


} // AE::ResEditor
