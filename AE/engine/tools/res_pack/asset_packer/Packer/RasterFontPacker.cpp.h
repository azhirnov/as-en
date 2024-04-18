// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
	SaveImage
=================================================
*/
#ifdef AE_BUILD_ASSET_PACKER
	ND_ inline bool  RasterFontPacker_SaveImage (const RasterFontPacker &self, WStream &stream, const ResLoader::IntermImage &src) __NE___
	{
		return ImagePacker_SaveImage( stream, self.Header(), src );
	}
#endif
/*
=================================================
	IsValid
=================================================
*/
	ND_ inline bool  RasterFontPacker_IsValid (const RasterFontPacker &self) __NE___
	{
		CHECK_ERR( ImagePacker_IsValid( self.Header() ));

		CHECK_ERR( not self.glyphMap.empty() );
		CHECK_ERR( not self.fontHeight.empty() );

		FlatHashSet< CharUtf32 >	unique_chars;
		for (auto& [key, glyph] : self.glyphMap)
		{
			unique_chars.insert( key.Symbol() );
		}

		for (auto h : self.fontHeight)
		{
			for (auto c : unique_chars)
			{
				CHECK_ERR( self.glyphMap.contains( RasterFontPacker::GlyphKey{ c, h }));
			}
		}
		return true;
	}

/*
=================================================
	Serialize
=================================================
*/
#ifdef AE_BUILD_ASSET_PACKER
	ND_ inline bool  RasterFontPacker_Serialize (const RasterFontPacker &self, Serializing::Serializer &ser) __NE___
	{
		ASSERT( RasterFontPacker_IsValid( self ));
		return ser( self._header, self.sdfConfig, self.glyphMap, self.fontHeight );
	}
#endif
/*
=================================================
	Deserialize
=================================================
*/
	ND_ inline bool  RasterFontPacker_Deserialize (OUT RasterFontPacker &self, Serializing::Deserializer &des) __NE___
	{
		bool	res = des( OUT self._header );
		res &= (self._header.magic == RasterFontPacker::Magic);
		res &= (self._header.version == RasterFontPacker::Version);

		if_unlikely( not res )
			return false;

		res = des( OUT self.sdfConfig, OUT self.glyphMap, OUT self.fontHeight );
		ASSERT( RasterFontPacker_IsValid( self ));

		return res;
	}
