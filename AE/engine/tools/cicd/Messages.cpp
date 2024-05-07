// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "cicd/Messages.h"

namespace AE::CICD
{
	using namespace AE::Serializing;

	DECL_SERIALIZER( GPUInfo,				gapiVersion, device, name )
}

namespace AE::CICD::Msg
{

	DECL_SERIALIZER( ClientInfo,			type, name,
											info.cpuArch, info.cpuVendor, info.cpuFS,
											info.osType, info.osVersion, info.osName,
											info.gapi, info.gpuDevices )
	DECL_SERIALIZER( BeginSession,			id )
	DECL_SERIALIZER( EndSession,			id )

	DECL_SERIALIZER( ServerInfo,			scripts )
	DECL_SERIALIZER( ServerRunScript,		script )
	DECL_SERIALIZER( ServerScriptSubmitted,	ok )
	DECL_EMPTY_SERIALIZER( ServerCancelAll	)

	DECL_SERIALIZER( Log,					sessionId, length, part )
	DECL_SERIALIZER( LogGroup,				sessionId, groupName )

	DECL_SERIALIZER( UploadFile,			sessionId, filename, size, folderId, mode )
	DECL_SERIALIZER( RequestUploadFile,		sessionId, srcFile, dstFile, mode )

	DECL_SERIALIZER( UploadFolder,			sessionId, folderName, folderId, mode )
	DECL_SERIALIZER( RequestUploadFolder,	sessionId, srcFolder, dstFolder, filter, mode )

	DECL_SERIALIZER( CMakeInit,				sessionId,
											params.compilerVersion, params.compiler, params.arch,
											params.config, params.options, params.source, params.build )

	DECL_SERIALIZER( CMakeBuild,			sessionId, build, config, target, threadCount )
	DECL_SERIALIZER( AndroidBuild,			sessionId, projectFolder, isDebug, target )
	DECL_SERIALIZER( AndroidPatchGradle,	sessionId, buildGradlePath, cmakeOpt )

	DECL_SERIALIZER( RunTest,				sessionId, exe, workDir )
	DECL_SERIALIZER( RunScript,				sessionId, exe )
	DECL_SERIALIZER( AndroidRunTest,		sessionId, libName, fnName )

	DECL_SERIALIZER( FileSystemCommand,		sessionId, type, arg0, arg1 )

	DECL_SERIALIZER( GitClone,				sessionId, tag, repo, folder, recurseSubmodules )
	DECL_SERIALIZER( GitCommitAndPush,		sessionId, path, branch )
	DECL_SERIALIZER( GitRebase,				sessionId, path, srcBranch, dstBranch )

	DECL_SERIALIZER( Unzip,					sessionId, archive )


} // AE::CICD::Msg
