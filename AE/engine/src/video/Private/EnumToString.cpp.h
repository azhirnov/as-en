// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "video/Public/Common.h"
#include "graphics/Private/EnumToString.h"

namespace AE::Base
{

/*
=================================================
	ToString (EMediaType)
=================================================
*/
	ND_ inline StringView  ToString (Video::EMediaType value) __NE___
	{
		switch_enum( value )
		{
			case Video::EMediaType::Video :		return "Video";
			case Video::EMediaType::Audio :		return "Audio";
			case Video::EMediaType::Unknown :
			case Video::EMediaType::_Count :	break;
		}
		switch_end
		RETURN_ERR( "unknown media type" );
	}

/*
=================================================
	ToString (EColorPreset)
=================================================
*/
	ND_ inline StringView  ToString (Video::EColorPreset value) __NE___
	{
		switch_enum( value )
		{
			case Video::EColorPreset::Unspecified :		return "Unspecified";
			case Video::EColorPreset::MPEG_BT709 :		return "MPEG_BT709";
			case Video::EColorPreset::MPEG_BT709_2 :	return "MPEG_BT709_2";
			case Video::EColorPreset::JPEG_BT709 :		return "JPEG_BT709";
			case Video::EColorPreset::MPEG_BT2020_1 :	return "MPEG_BT2020_1";
			case Video::EColorPreset::MPEG_BT2020_2 :	return "MPEG_BT2020_2";
			case Video::EColorPreset::MPEG_SMPTE170M :	return "MPEG_SMPTE170M";
			case Video::EColorPreset::Unknown :
			case Video::EColorPreset::_Count :			break;
		}
		switch_end
		RETURN_ERR( "unknown color preset" );
	}


} // AE::Base
