// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Core/UIScreen.h"

namespace AE::VFS
{
	class ReloadableArchiveStorage;
}

namespace AE::UIEditor
{

	//
	// Script Executor
	//

	class ScriptExe final
	{
	// variables
	private:
		RC<VFS::ReloadableArchiveStorage>	_vfStorage;

		uint		_counter			= 0;

		void*		_compilePipelines	= null;
		void*		_packAssets			= null;

		Library		_pipelineCompilerLib;
		Library		_assetPackerLib;


	// methods
	public:
		ScriptExe ()														__NE___;
		~ScriptExe ()														__NE___;

		ND_ auto  Run (const Path &filePath, IOutputSurface &)				__NE___ -> SharedPtr<UIScreen::Ctor>;

		ND_ bool  InitVFS ()												__NE___;


	private:
		ND_ bool  _CompilePipelines (const Path &)							C_NE___;
		ND_ bool  _BuildResources (const Path &, const Path &)				C_NE___;

		ND_ RTechInfo  _CompileResources (const Path &, IOutputSurface &)	__NE___;

	};


} // AE::UIEditor
