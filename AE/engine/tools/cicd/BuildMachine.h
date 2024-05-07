// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef CICD_BUILD_MACHINE
# include "cicd/BaseMachine.h"

namespace AE::CICD
{

	//
	// Build Machine
	//

	class BuildMachine : public BaseMachine
	{
	// methods
	public:
		BuildMachine (const Path &baseDir, StringView name);
		~BuildMachine ();

		ND_ bool  Run (RC<IServerProvider> serverProvider);


	public:
		// cmake //
		ND_ bool  _CMakeInit (const CMakeParams &params);
		ND_ bool  _CMakeBuild (const Path &buildPath, StringView config, StringView target, uint threadCount);

		// android //
		ND_ bool  _AndroidBuild (const Path &projectFolder, bool isDebug, StringView target);
		ND_ bool  _AndroidGetApk (bool isDebug, StringView target, const Path &basePath, OUT Path &path);


		// TODO: binary search for test fail commit


	private:
		ND_ bool  _RegisterCommands ();

			bool  _Cb_CMakeInit (Msg::CMakeInit &);
			bool  _Cb_CMakeBuild (const Msg::CMakeBuild &);
			bool  _Cb_AndroidBuild (const Msg::AndroidBuild &);
			bool  _Cb_GitClone (const Msg::GitClone &);
			bool  _Cb_BeginSession (const Msg::BeginSession &);
			bool  _Cb_GitCommitAndPush (const Msg::GitCommitAndPush &);
			bool  _Cb_GitRebase (const Msg::GitRebase &);
			bool  _Cb_RunScript (const Msg::RunScript &);
			bool  _Cb_AndroidPatchGradle (const Msg::AndroidPatchGradle &);
			bool  _Cb_Unzip (const Msg::Unzip &);

			void  _OnConnected ()		override;
			void  _OnDisconnected ()	override;
	};


} // AE::CICD

#endif // CICD_BUILD_MACHINE
