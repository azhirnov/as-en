// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptBasePass.h"
#include "Passes/RasterMip.h"

namespace AE::ResEditor
{

	//
	// Rasterize to Mipmaps
	//

	class ScriptRasterMip final : public ScriptBasePass
	{
	// types
	private:
		struct Variable
		{
			String				inName;
			String				outName;
			ScriptImagePtr		image;
			MipmapLevel			baseMipmap;
			String				sampler;		// for input
		};


	// variables
	private:
		const Path			_pplnPath;

		Array<Variable>		_variables;


	// methods
	public:
		ScriptRasterMip ()																	= delete;
		ScriptRasterMip (const String &name, const String &defines)							__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																__Th_OV;


	private:
		static void  _Variable (Scripting::ScriptArgList args)								__Th___;

		ND_ auto  _CompilePipeline ()														C_Th___;
			void  _CompilePipeline2 ()														C_Th___;
			void  _CompilePipeline3 (const String &vs, const String &fs,
									 uint line, const String &pplnName,
									 uint shaderOpts, EPipelineOpt pplnOpt)					C_Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								__Th_OV;
	};


} // AE::ResEditor
