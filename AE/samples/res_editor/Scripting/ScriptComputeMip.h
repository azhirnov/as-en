// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBasePass.h"
#include "res_editor/Passes/ComputeMip.h"

namespace AE::ResEditor
{

	//
	// Compute Mipmaps
	//

	class ScriptComputeMip final : public ScriptBasePass
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
			EResourceState		inState		= Default;
			EResourceState		outState	= Default;
		};


	// variables
	private:
		const Path			_pplnPath;
		
		Array<Variable>		_variables;


	// methods
	public:
		ScriptComputeMip ()																	= delete;
		ScriptComputeMip (const String &name, const String &defines)						__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)							__Th___;

	// ScriptBasePass //

		// Returns non-null pass or throw exception.
		RC<IPass>  ToPass ()																__Th_OV;


	private:
		static void  _Variable (Scripting::ScriptArgList args)								__Th___;

		ND_ auto  _CompilePipeline (OUT Bytes &ubSize)										C_Th___;
			void  _CompilePipeline2 (OUT Bytes &ubSize)										C_Th___;
			void  _CompilePipeline3 (const String &cs, uint line, const String &pplnName,
									 uint shaderOpts, EPipelineOpt pplnOpt)					C_Th___;

		ND_ static auto  _CreateUBType ()													__Th___;
		ND_ static auto  _CreatePCType ()													__Th___;

	// ScriptBasePass //
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)								C_Th_OV;
	};


} // AE::ResEditor
