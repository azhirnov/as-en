// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/Private/InputActionsBase.h"

namespace AE::App
{

	//
	// Gesture Recognizer
	//

	class GestureRecognizer
	{
	// types
	private:
		using Duration_t	= IApplication::Duration_t;
		using InputKey		= InputActionsBase::InputKey;
		using GestureBits_t	= EnumSet< EGestureType >;

		struct Touch
		{
			float2		delta;		// pix
			float2		pos;		// pix

			Duration_t	startTime	{};
			float2		startPos;	// pix

			Touch () {}
		};

		static constexpr uint	MaxTouches	= 8;

		enum class TouchID : ushort { Unknown = 0xFFFF };

		using TouchIDs_t	= StaticArray< TouchID,			MaxTouches >;
		using TouchData_t	= StaticArray< Touch,			MaxTouches >;
		using TouchStates_t	= StaticArray< EGestureState,	MaxTouches >;

		static constexpr auto	_LongPressDuration		= InputActionsBase::_LongPressDuration;
		static constexpr auto	_DoubleTapMaxDuration	= InputActionsBase::_DoubleTapMaxDuration;
		static constexpr auto	_SingleTapMaxDuration	= InputActionsBase::_ClickMaxDuration;


	// variables
	private:
		uint				_activeTouches	= 0;
		TouchIDs_t			_touchIDs;			// map touch id to touch data
		TouchData_t			_touchData;
		TouchStates_t		_touchStates;

		struct {
			bool				isActive		= false;
			bool				doubleTap		= false;
			ubyte				touchIdx		= UMax;
			Duration_t			lastTapTime		{};
		}					_tapRecognizer;

		struct {
			EGestureType		type			= Default;
			EGestureState		state			= Default;
			ubyte				touchIdx		= UMax;

			ND_ bool			IsActive ()		{ return state < EGestureState::End; }
		}					_dragRecognizer;

		struct {
			bool				isActive		= false;
			TouchID				touchID0		= Default;
			TouchID				touchID1		= Default;
			float				scale			= 0.f;
			float				rotate			= 0.f;
		}					_twoTouchRecognizer;

		const ushort		_touchPosPxCode;
		const ushort		_touchPosMmCode;
		const ushort		_touchDeltaPxCode;
		const ushort		_touchDeltaNormCode;
		const ushort		_multiTouchCode;

		static const auto	_id					= ControllerID::Touchscreen;


	// methods
	public:
		GestureRecognizer (ushort touchPosPxCode, ushort touchPosMmCode,
						   ushort touchDeltaPxCode, ushort touchDeltaNormCode,
						   ushort multiTouchCode)													__NE___;

		void  Update (Duration_t timestamp, InputActionsBase &ia)									__NE___;
		void  SetTouch (uint id, const float2 posInPx, EGestureState state, Duration_t timestamp)	__NE___;


	private:
		void  _RecognizeTaps (uint activeCount, Duration_t timestamp, InputActionsBase &ia);
		void  _RecognizeDragging (uint activeCount, Duration_t timestamp, InputActionsBase &ia);
		void  _Recognize2Touch (uint activeCount, Duration_t timestamp, InputActionsBase &ia);

		template <typename T>
		ND_ static constexpr InputKey	_Pack (T key,
											   EGestureType gesture,
											   EGestureState state = EGestureState::Update)	__NE___ { return InputActionsBase::_Pack( key, gesture, state ); }
	};


} // AE::App
