// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

/*
=================================================
	IsValid
=================================================
*/
	ND_ inline bool  ImageAtlasPacker_IsValid (const ImageAtlasPacker &self) __NE___
	{
		CHECK_ERR( ImagePacker_IsValid( self._header.hdr ));

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
	Serialize / Deserialize
=================================================
*/
	ND_ inline bool  ImageAtlasPacker_Serialize (const ImageAtlasPacker &self, Serializing::Serializer &ser) __NE___
	{
		ASSERT( ImageAtlasPacker_IsValid( self ));
		return ser( self._header, self.map, self.rects );
	}

	ND_ inline bool  ImageAtlasPacker_Deserialize (OUT ImageAtlasPacker &self, Serializing::Deserializer &des) __NE___
	{
		bool	res = des( OUT self._header );
		res &= (self._header.magic == ImageAtlasPacker::Magic);
		res &= (self._header.version == ImageAtlasPacker::Version);

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
*/
#ifdef AE_BUILD_ASSET_PACKER
	ND_ inline bool  ImageAtlasPacker_SaveImage (const ImageAtlasPacker &self, WStream &stream, const ResLoader::IntermImage &src) __NE___
	{
		return ImagePacker_SaveImage( stream, self._header.hdr, src );
	}
#endif

