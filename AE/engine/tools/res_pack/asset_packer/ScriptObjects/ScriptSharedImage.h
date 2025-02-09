// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ScriptTexture.h"

namespace AE::AssetPacker
{

	//
	// Script Shared Image
	//

	class ScriptSharedImage final : public EnableScriptRC
	{
	// types
	public:
		struct Result
		{
			RectU	region;				// src: size with padding, dst: region in '_image'
			uint	id		= UMax;		// index in 'res'
		};

	private:
		struct ImageRegion : Result
		{
			uint	res		= UMax;		// index in '_atlases', '_fonts'

			ND_ bool  operator == (const ImageRegion &rhs) const {
				return All( region == rhs.region ) and id == rhs.id and res == rhs.res;
			}
		};

		using ImageRegionArr_t	= Array< ImageRegion >;

		using AtlasMap_t		= FlatHashMap< ScriptImageAtlas*, uint >;		// index in '_atlases'
		using RasterFontMap_t	= FlatHashMap< ScriptRasterFont*, uint >;		// index in '_fonts'

		static constexpr uint	c_AtlasBit	= 1u << 28;
		static constexpr uint	c_FontBit	= 1u << 29;

		enum class EState : uint
		{
			Recording,
			Arranged,
			Stored,
		};


	// variables
	private:
		ScriptTexturePtr				_image;

		Array< ScriptImageAtlas* >		_atlases;
		AtlasMap_t						_atlasMap;

		Array< ScriptRasterFont* >		_fonts;
		RasterFontMap_t					_fontMap;

		ImageRegionArr_t				_imageRegions;

		String							_metaDataName;

		EState							_state			= EState::Recording;
		bool							_hasFormat		= false;


	// methods
	public:
		ScriptSharedImage ();
		~ScriptSharedImage ();

		void  AddSubImages (ScriptImageAtlas &atlas, ArrayView<Result>)		__Th___;
		void  AddSubImages (ScriptRasterFont &font, ArrayView<Result>)		__Th___;

		void  Store (const String &nameInArchive)							__Th___;
		void  PutMeta (const ScriptResourceMetaPtr &, const String &name)	__Th___;

		void  SetFormat (EPixelFormat fmt)									__Th___;

		ND_ EPixelFormat	DstFormat ()									C_Th___	{ CHECK_THROW( _image );  return _image->DstFormat(); }
		ND_ EPixelFormat	IntermFormat ()									C_Th___	{ CHECK_THROW( _image );  return _image->IntermFormat(); }
		ND_ StringView		MetaName ()										C_NE___	{ return _metaDataName; }

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

	private:
		void  _Arrange (const String &nameInArchive)						__Th___;

		ND_ Array<Result>	_GetResult (uint res)							C_Th___;
	};

} // AE::AssetPacker

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptSharedImage,	"SharedImage" );
