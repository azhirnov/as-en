// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "cicd/CICD.pch.h"

namespace AE::CICD
{
	using namespace AE::Base;
	using namespace AE::Networking;

	using AE::Serializing::SerializedID;
	using AE::Graphics::EGPUVendor;
	using AE::Graphics::EGraphicsDeviceID;
	using EOSType = EOperationSystem;


	enum class EClientType : ubyte
	{
		Unknown		= 0,
		Build,
		Test,
		User,
	};

	enum class ECPUFeatureSet : ubyte
	{
		Unknown		= 0,

		// x64
		AVX256,
		AVX512,

		// ARM
		Neon,
		SVE,
		SVE2,

		_Count
	};

	enum class EGraphicsAPI : ubyte
	{
		Unknown		= 0,
		Vulkan,
		Metal,
		_Count,
	};

	struct GPUInfo : public Serializing::ISerializable
	{
		Version2			gapiVersion;
		EGraphicsDeviceID	device			= Default;
		String				name;

		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)	__NE_OV;
	};

	struct MachineInfo
	{
		ECPUArch					cpuArch		= Default;
		ECPUVendor					cpuVendor	= Default;
		EnumSet<ECPUFeatureSet>		cpuFS		= Default;

		EOSType						osType		= Default;
		Version2					osVersion;
		String						osName;

		EGraphicsAPI				gapi		= Default;
		FixedArray< GPUInfo, 4 >	gpuDevices;
	};


	enum class ECompiler : ubyte
	{
		Unknown		= 0,
		MSVC,
		MSVC_Clang,
		Linux_GCC,
		Linux_Clang,
		Linux_Clang_Ninja,
		MacOS_Clang,
		iOS_Clang,
	};

	struct CMakeParams
	{
		uint		compilerVersion		= 0;			// auto
		ECompiler	compiler			= Default;
		ECPUArch	arch				= Default;
		String		config;
		String		options;
		Path		source;
		Path		build;
	};

	enum class ECopyMode : ubyte
	{
		Unknown		= 0,
		FileReplace,
		FileMerge,
		FolderReplace,				// delete previous folder and copy new
		FolderMerge_FileReplace,	// keep previous, replace same files
		FolderMerge_FileMerge,		// keep previous, rename same files
		FolderMerge_FileKeep,		// keep previous, keep origin files
	};

} // AE::CICD
//-----------------------------------------------------------------------------


namespace AE::CICD::Msg
{
	using namespace AE::Serializing;


	struct BaseMsg : public ISerializable, public EnableRC<BaseMsg>
	{
		BaseMsg ()					__NE___	{}
		BaseMsg (BaseMsg &&)		__NE___ {}
		BaseMsg (const BaseMsg &)	__NE___	{}
	};

	struct SessionMsg : BaseMsg
	{
		uint	sessionId	= 0;
	};


	struct ClientInfo : BaseMsg
	{
		EClientType		type	= Default;
		String			name;
		MachineInfo		info;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct BeginSession : BaseMsg
	{
		uint		id;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct EndSession : BaseMsg
	{
		uint		id;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct ServerInfo : BaseMsg
	{
		Array<String>	scripts;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct ServerRunScript : BaseMsg
	{
		String		script;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct ServerCancelAll : BaseMsg
	{
		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct ServerScriptSubmitted : BaseMsg
	{
		bool		ok;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct Log : SessionMsg
	{
		uint		length		= 0;
		uint		part		= 0;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct LogGroup : SessionMsg
	{
		String		groupName;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct UploadFile : SessionMsg
	{
		String		filename;
		Bytes		size;
		uint		folderId	= 0;
		ECopyMode	mode		= Default;
		// TODO: hash

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct RequestUploadFile : SessionMsg
	{
		String		srcFile;
		String		dstFile;
		ECopyMode	mode		= Default;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct UploadFolder : SessionMsg
	{
		String		folderName;
		uint		folderId	= 0;
		ECopyMode	mode		= Default;
		// TODO: hash ?

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct RequestUploadFolder : SessionMsg
	{
		String		srcFolder;
		String		dstFolder;
		String		filter;
		ECopyMode	mode		= Default;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct CMakeInit : SessionMsg
	{
		CMakeParams		params;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct CMakeBuild : SessionMsg
	{
		String		build;
		String		config;
		String		target;
		uint		threadCount	= 0;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct AndroidBuild : SessionMsg
	{
		String		projectFolder;
		bool		isDebug;
		String		target;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct AndroidRunTest : SessionMsg
	{
		String		libName;
		String		fnName;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};

	struct AndroidPatchGradle : SessionMsg
	{
		String		buildGradlePath;
		String		cmakeOpt;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct RunTest : SessionMsg
	{
		Path		exe;
		Path		workDir;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct RunScript : SessionMsg
	{
		Path		exe;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct FileSystemCommand : SessionMsg
	{
		enum EType : ubyte
		{
			Unknown		= 0,
			RemoveDir,	// arg0
			MakeDir,	// arg0
			CopyDir,	// arg0, arg1
			CopyFile,	// arg0, arg1
			DeleteFile,	// arg0
		};

		EType		type		= Default;
		Path		arg0;
		Path		arg1;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct GitClone : SessionMsg
	{
		String		tag;
		String		repo;
		String		folder;
		bool		recurseSubmodules	= false;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct GitCommitAndPush : SessionMsg
	{
		String		path;
		String		branch;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct GitRebase : SessionMsg
	{
		String		path;
		String		srcBranch;
		String		dstBranch;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};


	struct Unzip : SessionMsg
	{
		Path		archive;

		bool  Serialize (Serializer &)		C_NE_OV;
		bool  Deserialize (Deserializer &)	__NE_OV;
	};

} // AE::CICD::Msg
//-----------------------------------------------------------------------------


namespace AE::Base
{
	using AE::CICD::EClientType;
	using AE::CICD::EOSType;
	using AE::CICD::ECompiler;
	using AE::CICD::ECPUFeatureSet;
	using AE::CICD::EGraphicsAPI;


	ND_ inline StringView  ToString (EClientType value)
	{
		switch_enum( value )
		{
			case EClientType::Build :	return "Build";
			case EClientType::Test :	return "Test";
			case EClientType::User :	return "User";
			case EClientType::Unknown :	break;
		}
		switch_end
		return Default;
	}

	ND_ inline StringView  ToString (ECompiler value)
	{
		switch_enum( value )
		{
			case ECompiler::MSVC :				return "MSVC";
			case ECompiler::MSVC_Clang :		return "MSVC_Clang";
			case ECompiler::Linux_GCC :			return "Linux_GCC";
			case ECompiler::Linux_Clang :		return "Linux_Clang";
			case ECompiler::Linux_Clang_Ninja:	return "Linux_Clang_Ninja";
			case ECompiler::MacOS_Clang :		return "MacOS_Clang";
			case ECompiler::iOS_Clang :			return "iOS_Clang";
			case ECompiler::Unknown :			break;
		}
		switch_end
		return Default;
	}

	ND_ inline StringView  ToString (ECPUFeatureSet value)
	{
		switch_enum( value )
		{
			case ECPUFeatureSet::AVX256 :	return "AVX256";
			case ECPUFeatureSet::AVX512 :	return "AVX512";
			case ECPUFeatureSet::Neon :		return "Neon";
			case ECPUFeatureSet::SVE :		return "SVE";
			case ECPUFeatureSet::SVE2 :		return "SVE2";
			case ECPUFeatureSet::Unknown :
			case ECPUFeatureSet::_Count :	break;
		}
		switch_end
		return Default;
	}

	ND_ inline StringView  ToString (EGraphicsAPI value)
	{
		switch_enum( value )
		{
			case EGraphicsAPI::Vulkan :		return "Vulkan";
			case EGraphicsAPI::Metal :		return "Metal";
			case EGraphicsAPI::Unknown :
			case EGraphicsAPI::_Count :		break;
		}
		switch_end
		return Default;
	}


} // AE::Base
