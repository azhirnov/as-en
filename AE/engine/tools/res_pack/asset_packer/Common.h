// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Serializing.h"
#include "pch/Scripting.h"
#include "pch/VFS.h"

#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Private/EnumUtils.h"
#include "graphics_rhi/Scripting/GraphicsBindings.h"

#include "geometry_tools/SphericalCube/SphericalCubeMath.h"

#include "res_pack/asset_packer/AssetPacker.h"

namespace AE::AssetPacker
{
	using AE::Scripting::ScriptEnginePtr;

	using ECubeFace = AE::GeometryTools::ECubeFace;

	using EArchivePackerFileType = VFS::ArchivePacker::EFileType;

	template <typename T>
	using ScriptRC			= Scripting::AngelScriptHelper::SharedPtr<T>;
	using EnableScriptRC	= Scripting::AngelScriptHelper::SimpleRefCounter;


	class ScriptTexture;
	class ScriptSharedImage;
	class ScriptImageAtlas;
	class ScriptRasterFont;
	class ScriptMesh;
	class ScriptModel;
	class ScriptMaterial;
	class ScriptResourceMeta;


	using ScriptTexturePtr		= ScriptRC< ScriptTexture >;
	using ScriptSharedImagePtr	= ScriptRC< ScriptSharedImage >;
	using ScriptImageAtlasPtr	= ScriptRC< ScriptImageAtlas >;
	using ScriptRasterFontPtr	= ScriptRC< ScriptRasterFont >;
	using ScriptMeshPtr			= ScriptRC< ScriptMesh >;
	using ScriptModelPtr		= ScriptRC< ScriptModel >;
	using ScriptMaterialPtr		= ScriptRC< ScriptMaterial >;
	using ScriptResourceMetaPtr	= ScriptRC< ScriptResourceMeta >;


} // AE::AssetPacker


AE_DECL_SCRIPT_TYPE( AE::AssetPacker::ECubeFace,	"ECubeFace"	);
