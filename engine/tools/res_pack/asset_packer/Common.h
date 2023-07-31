// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "AssetPacker.pch.h"
#include "geometry_tools/SphericalCube/SphericalCubeMath.h"

namespace AE::AssetPacker
{
    using AE::Scripting::ScriptEnginePtr;

    using ECubeFace = AE::GeometryTools::ECubeFace;

    using EArchivePackerFileType = VFS::ArchivePacker::EFileType;

    template <typename T>
    using ScriptRC          = Scripting::AngelScriptHelper::SharedPtr<T>;
    using EnableScriptRC    = Scripting::AngelScriptHelper::SimpleRefCounter;

} // AE::AssetPacker


AE_DECL_SCRIPT_TYPE( AE::AssetPacker::ECubeFace,    "ECubeFace" );
