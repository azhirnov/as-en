// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/UI/StyleCollection.h"

namespace AE
{
namespace
{

/*
=================================================
	Serialize / Deserialize (_BaseStyle)
=================================================
*/
	ND_ inline bool  Serialize_BaseStyle (const UI::StyleCollection::_BaseStyle &c, Serializing::Serializer &ser) __NE___
	{
		return ser( c._pplnName );
	}

	ND_ inline bool  Deserialize_BaseStyle (OUT UI::StyleCollection::_BaseStyle &c, Serializing::Deserializer &des) __NE___
	{
		return des( OUT c._pplnName );
	}

/*
=================================================
	Serialize / Deserialize (ColorStyle)
=================================================
*/
	ND_ inline bool  Serialize_ColorStyle (const UI::StyleCollection::ColorStyle &c, Serializing::Serializer &ser) __NE___
	{
		return	Serialize_BaseStyle( c, ser ) and
				ser( c.color.disabled, c.color.enabled, c.color.mouseOver, c.color.touchDown, c.color.selected );
	}

	ND_ inline bool  Deserialize_ColorStyle (OUT UI::StyleCollection::ColorStyle &c, Serializing::Deserializer &des) __NE___
	{
		return	Deserialize_BaseStyle( c, des ) and
				des( OUT c.color.disabled, OUT c.color.enabled, OUT c.color.mouseOver, OUT c.color.touchDown, OUT c.color.selected );
	}

/*
=================================================
	Serialize / Deserialize (ImageStyle)
=================================================
*/
	ND_ inline bool  Serialize_ImageStyle (const UI::StyleCollection::ImageStyle &c, Serializing::Serializer &ser) __NE___
	{
		return	Serialize_ColorStyle( c, ser ) and
				ser( c.image.disabled, c.image.enabled, c.image.mouseOver, c.image.touchDown, c.image.selected );
	}

	ND_ inline bool  Deserialize_ImageStyle (OUT UI::StyleCollection::ImageStyle &c, Serializing::Deserializer &des) __NE___
	{
		return	Deserialize_ColorStyle( c, des ) and
				des( OUT c.image.disabled, OUT c.image.enabled, OUT c.image.mouseOver, OUT c.image.touchDown, OUT c.image.selected );
	}

/*
=================================================
	Serialize / Deserialize (FontStyle)
=================================================
*/
	ND_ inline bool  Serialize_FontStyle (const UI::StyleCollection::FontStyle &c, Serializing::Serializer &ser) __NE___
	{
		// TODO
		return Serialize_ColorStyle( c, ser );
	}

	ND_ inline bool  Deserialize_FontStyle (OUT UI::StyleCollection::FontStyle &c, Serializing::Deserializer &des) __NE___
	{
		// TODO
		return Deserialize_ColorStyle( c, des );
	}


} // namespace
} // AE
