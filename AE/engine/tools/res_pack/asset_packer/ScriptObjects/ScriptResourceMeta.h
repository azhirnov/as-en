// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Contains meta data for group of resources.
	Resource data stored in separate file, it is needed to load meta data
	with cached sequential access and load data with random access in async files.
*/

#pragma once

#include "res_pack/asset_packer/ScriptObjects/ObjectStorage.h"

namespace AE::AssetPacker
{

	//
	// Script Resource Meta Data
	//

	class ScriptResourceMeta final : public EnableScriptRC
	{
	// types
	private:
		using UniqueImages_t	= FlatHashMap< ScriptTexturePtr, String >;
		using UniqueAtlases_t	= FlatHashMap< ScriptImageAtlasPtr, String >;
		using UniqueFonts_t		= FlatHashMap< ScriptRasterFontPtr, String >;
		using UniqueNames_t		= FlatHashSet< String >;


	// variables
	private:
		UniqueImages_t		_images;
		UniqueAtlases_t		_atlases;
		UniqueFonts_t		_fonts;
		UniqueNames_t		_uniqueNames;


	// methods
	public:
		ScriptResourceMeta ();
		~ScriptResourceMeta ();

		void  Add (ScriptTexture &image, const String &name)	__Th___;
		void  Add (ScriptImageAtlas &atlas, const String &name)	__Th___;
		void  Add (ScriptRasterFont &font, const String &name)	__Th___;

		void  Store (const String &nameInArchive)				__Th___;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;
	};


} // AE::AssetPacker

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptResourceMeta,	"MetaData" );
