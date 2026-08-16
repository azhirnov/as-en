// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics/UI/Common.h"

namespace AE::UI
{

	enum class EStyleState : ubyte
	{
		Unknown		= 0,
		Invisible	= 1 << 0,	//
		Disabled	= 1 << 1,	//
		MouseOver	= 1 << 2,	//
		TouchDown	= 1 << 3,	//
		Selected	= 1 << 4,	// for checkbox and other
		Moving		= 1 << 5,	// layout moving, align to pixels must be disabled
		Focused		= 1 << 6,	//
		_Last,
		All			= CT_AllBitMask2<EStyleState>,
		_Inherited	= Invisible | Disabled | Moving,
	};


	enum class EStyleIndex : ubyte
	{
		Disabled,
		Enabled,
		MouseOver,
		TouchDown,
		Selected,
		_Count
	};


	enum class EStackOrigin : ubyte
	{
		Left,		// horizontal stack, from left to right
		Right,		// horizontal stack, from right to left
		Bottom,		// vertical stack, from bottom to top
		Top,		// vertical stack, from top to bottom
		_Count
	};


	enum class EDirection : ubyte
	{
		Horizontal,
		Vertical,
		_Count
	};


	enum class ELayoutAlign : ubyte
	{
		Unknown		= 0,
		Left		= 1 << 0,
		Right		= 1 << 1,
		CenterX		= 1 << 2,
		Bottom		= 1 << 3,
		Top			= 1 << 4,
		CenterY		= 1 << 5,
		_Last,
		All			= CT_AllBitMask2<ELayoutAlign>,

		FillX		= Left | Right,
		FillY		= Bottom | Top,
		Center		= CenterX | CenterY,
		Fill		= FillX | FillY,
	};


	enum class EEdge : ubyte
	{
		Left,
		Right,
		Bottom,
		Top,
		_Count
	};
//-----------------------------------------------------------------------------



/*
=================================================
	StyleStateToIndex
=================================================
*/
	NdCx__ EStyleIndex  StyleStateToIndex (const EStyleState state) __NE___
	{
		switch ( state )
		{
			case EStyleState::Selected :							return EStyleIndex::Selected;
			case EStyleState::Selected | EStyleState::MouseOver :	return EStyleIndex::Selected;
			case EStyleState::Selected | EStyleState::TouchDown :	return EStyleIndex::Selected;
			case EStyleState::MouseOver :							return EStyleIndex::MouseOver;
			case EStyleState::TouchDown :							return EStyleIndex::TouchDown;
			case EStyleState::Disabled :							return EStyleIndex::Disabled;
		}
		return EStyleIndex::Enabled;
	}


} // AE::UI
