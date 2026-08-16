// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

/*
=================================================
	IsValid
=================================================
*/
	Nd__In bool  ImageAtlasPacker_IsValid (const ImageAtlasPacker &self) __NE___
	{
		using EFileFlags = ImageAtlasPacker::EFileFlags;

		if ( AnyBits( self._header.flags, EFileFlags::HasImage | EFileFlags::SeparateData ))
			CHECK_ERR( ImagePacker_IsValid( self._imageHeader ));

		CHECK_ERR( not self.map.empty() );
		CHECK_ERR( not self.rects.empty() );

		for (auto& [name, idx] : self.map)
		{
			CHECK_ERR( name.IsDefined() );
			CHECK_ERR( idx < self.rects.size() );
		}
		return true;
	}

/*
=================================================
	Serialize
=================================================
*/
#ifdef AE_BUILD_ASSET_PACKER
	Nd__In bool  ImageAtlasPacker_Serialize (const ImageAtlasPacker &self, Serializing::Serializer &ser) __NE___
	{
		using EFileFlags = ImageAtlasPacker::EFileFlags;

		ASSERT( ImageAtlasPacker_IsValid( self ));

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

		return res and ser( self.map, self.rects );
	}
#endif
/*
=================================================
	Deserialize
=================================================
*/
	Nd__In bool  ImageAtlasPacker_Deserialize (OUT ImageAtlasPacker &self, Serializing::Deserializer &des) __NE___
	{
		using EFileFlags = ImageAtlasPacker::EFileFlags;

		bool	res = des( OUT self._header );
		res &= (self._header.magic == ImageAtlasPacker::Magic);
		res &= (self._header.version == ImageAtlasPacker::Version);

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

		res = des( OUT self.map, OUT self.rects );
		ASSERT( ImageAtlasPacker_IsValid( self ));

		return res;
	}

/*
=================================================
	SaveImage
=================================================
*
#ifdef AE_BUILD_ASSET_PACKER
	Nd__In bool  ImageAtlasPacker_SaveImage (const ImageAtlasPacker &self, WStream &stream, const ResLoader::IntermImage &src) __NE___
	{
		return ImagePacker_SaveImage( stream, self._header.hdr, src );
	}
#endif
*/
