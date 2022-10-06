// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/InputActions.h"
#include "platform/Public/IApplication.h"

namespace AE::App
{

	//
	// Gesture Recognizer
	//

	class GestureRecognizer
	{
	// types
	private:
		using Duration_t = IApplication::Duration_t;

		struct Touch
		{
			float2		delta;		// mm
			float2		pos;		// mm

			Duration_t	startTime	{};
			float2		startPos;	// mm
			
			Touch () {}
		};

		static constexpr uint	MaxTouches	= 8;


	// variables
	private:
		uint										_activeTouches	= 0;
		StaticArray< ushort, MaxTouches >			_touchIDs;			// map touch id to touch data
		StaticArray< Touch, MaxTouches >			_touchData;
		StaticArray< EGestureState, MaxTouches >	_touchStates;

		struct {
			bool			isActive		= false;
			ubyte			touchIdx		= UMax;
			Duration_t		lastTapTime		{};
		}				_tapRecognizer;

		struct {
			float2			prevPos;
			ubyte			touchIdx		= UMax;
			bool			isActive		= false;
		}				_dragRecognizer;

	public:
		struct {
			EGestureType	type			= Default;
			float2			pos;
		}				tapGesture;

		struct {
			EGestureState	state			= Default;
			float2			pos;
		}				dragGesture;


	// methods
	public:
		GestureRecognizer () {}
		
		void  Update (Duration_t timestamp);

		void  SetTouch (uint id, float2 posInMm, EGestureState state, Duration_t timestamp);


	private:
		void  _RecognizeTaps (uint activeCount, int touchIdx, Duration_t timestamp);
		void  _RecognizeDragging (uint activeCount, int touchIdx, Duration_t timestamp);
	};


} // AE::App
