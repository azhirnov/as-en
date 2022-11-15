// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "AssetPacker.h"
#include "scripting/Impl/ScriptTypes.h"
#include "geometry_tools/SphericalCube/SphericalCubeMath.h"

namespace AE::AssetPacker
{
	using AE::Scripting::AngelScriptHelper;
	using AE::Scripting::ScriptEnginePtr;
	
	using ECubeFace = AE::GeometryTools::ECubeFace;

	using EArchivePackerFileType = VFS::ArchivePacker::EFileType;

} // AE::AssetPacker


namespace AE::Scripting
{
	AE_DECL_SCRIPT_TYPE( AssetPacker::ECubeFace,	"ECubeFace"	);
}
