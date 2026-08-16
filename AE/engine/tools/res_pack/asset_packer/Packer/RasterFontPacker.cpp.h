// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

/*
=================================================
	IsValid
=================================================
*/
	Nd__In bool  RasterFontPacker_IsValid (const RasterFontPacker &self) __NE___
	{
		using EFileFlags = RasterFontPacker::EFileFlags;

		if ( AnyBits( self._header.flags, EFileFlags::HasImage | EFileFlags::SeparateData ))
			ASSERT( ImagePacker_IsValid( self._imageHeader ));

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
	Nd__In bool  RasterFontPacker_Serialize (const RasterFontPacker &self, Serializing::Serializer &ser) __NE___
	{
		using EFileFlags = RasterFontPacker::EFileFlags;

		ASSERT( RasterFontPacker_IsValid( self ));

		bool	res = ser( self._header );

		if ( AllBits( self._header.flags, EFileFlags::SeparateData )){
			res &= ser( self._imageHeader, self._imageFileName );
		}else
		if ( AllBits( self._header.flags, EFileFlags::HasResName )){
			res &= ser( self._imageResName );
		}else
		if ( AllBits( self._header.flags, EFileFlags::HasImage )){
			res &= ser( self._imageHeader );
		}

		return res and ser( self.sdfConfig, self.glyphMap, self.fontHeight );
	}
#endif
/*
=================================================
	Deserialize
=================================================
*/
	Nd__In bool  RasterFontPacker_Deserialize (OUT RasterFontPacker &self, Serializing::Deserializer &des) __NE___
	{
		using EFileFlags = RasterFontPacker::EFileFlags;

		bool	res = des( OUT self._header );
		res &= (self._header.magic == RasterFontPacker::Magic);
		res &= (self._header.version == RasterFontPacker::Version);

		if_unlikely( not res )
			return false;

		if ( AllBits( self._header.flags, EFileFlags::SeparateData ))
			res = des( OUT self._imageHeader, OUT self._imageFileName );
		else
		if ( AllBits( self._header.flags, EFileFlags::HasResName ))
			res = des( OUT self._imageResName );
		else
		if ( AllBits( self._header.flags, EFileFlags::HasImage ))
			res = des( OUT self._imageHeader );

		if_unlikely( not res )
			return false;

		res = des( OUT self.sdfConfig, OUT self.glyphMap, OUT self.fontHeight );
		ASSERT( RasterFontPacker_IsValid( self ));

		return res;
	}
