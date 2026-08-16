// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Send global event to the OS.

	Thread safe: yes
*/

#pragma once

#include "platform/Public/Monitor.h"

namespace AE::App
{

	//
	// Send Input
	//

	class ISendInput
	{
	// types
	public:
		using KeyCode_t		= ushort;	// only key range from EInputType

		using KeyStates_t	= ArrayView<Pair< KeyCode_t, bool >>;

		static constexpr ushort		CursorPosTag	= 0xFFF1;		// cursor pos as 'ushort tag, x, y;' in pixels
		static constexpr ushort		CursorDeltaTag	= 0xFFF2;		// cursor delta as 'ushort tag; short x, y;' in pixels
		static constexpr ushort		MouseWheelTag	= 0xFFF3;


	// interface
	public:
		virtual ~ISendInput ()											__NE___ {}


		// Set cursor pos in pixels in global coordinates.
		// Use 'Monitor::region' and 'Monitor::workArea' to get available regions.
		//
		virtual bool  SetCursorPos (int2 pos)							__NE___	= 0;


		// Move cursor pos by delta in pixels in global coordinates.
		//
		virtual bool  MoveCursor (int2 delta)							__NE___	= 0;


		// Insert input event into keyboard or mouse input stream.
		//
		virtual bool  SetKeyState (KeyCode_t key, bool down)			__NE___	= 0;
		virtual bool  SetKeyStates (KeyStates_t states)					__NE___	= 0;


		//
		virtual bool  ClipboardPut (U8StringView str)					__NE___ = 0;


		// Allow to send cursor pos and key/mouse button states in single call.
		// Last bit indicates down state.
		// Supported: 'CursorPosTag', 'CursorDeltaTag', 'MouseWheelTag'.
		// If failed then key/mouse state is undefined.
		//
		virtual bool  ProcessSequence (ArrayView<ushort>)				__NE___ = 0;


		virtual StringView	GetApiName ()								C_NE___ = 0;

		// TODO: clipboard
	};

} // AE::App
