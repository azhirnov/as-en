// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "platform/Public/AppEnums.h"
#include "platform/Public/InputActions.h"

namespace AE::Base
{
	using App::EGestureType;
	using App::EGestureState;
	using EInputActionValueType = App::IInputActions::EValueType;

/*
=================================================
	ToString (EGestureType)
=================================================
*/
	ND_ inline StringView  ToString (EGestureType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			// continious
			case EGestureType::Hold :			return "Hold";
			case EGestureType::Move :			return "Move";
			case EGestureType::LongPress_Move :	return "LongPress_Move";
			case EGestureType::Scale2D :		return "Scale2D";
			case EGestureType::Rotate2D :		return "Rotate2D";

			// single
			case EGestureType::Down :			return "Down";
			case EGestureType::Click :			return "Click";
			case EGestureType::DoubleClick :	return "DoubleClick";
			case EGestureType::LongPress :		return "LongPress";

			case EGestureType::_Count :
			case EGestureType::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown gesture type" );
	}

/*
=================================================
	ToString (EGestureState)
=================================================
*/
	ND_ inline StringView  ToString (EGestureState value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EGestureState::Begin :		return "Begin";
			case EGestureState::Update :	return "Update";
			case EGestureState::End :		return "End";
			case EGestureState::Cancel :	return "Cancel";
			case EGestureState::Outside :	return "Outside";
			case EGestureState::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown gesture state" );
	}
	
/*
=================================================
	ToString (EInputActionValueType)
=================================================
*/
	ND_ inline StringView  ToString (EInputActionValueType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EInputActionValueType::Bool :		return "Bool";
			case EInputActionValueType::Int :		return "Int";
			case EInputActionValueType::Float :		return "Float";
			case EInputActionValueType::Float2 :	return "Float2";
			case EInputActionValueType::Float3 :	return "Float3";
			case EInputActionValueType::Float4 :	return "Float4";
			case EInputActionValueType::Quat :		return "Quat";
			case EInputActionValueType::Float4x4 :	return "Float4x4";
			case EInputActionValueType::String :	return "String";
			case EInputActionValueType::Unknown :	return "none";
			case EInputActionValueType::_Count :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown input action value type" );
	}


} // AE::Base
