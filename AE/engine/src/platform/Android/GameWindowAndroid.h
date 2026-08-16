// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Private/WindowBase.h"
# include "platform/Android/InputActionsAndroid.h"
# include "platform/Private/WindowSurface.h"

namespace AE::App
{

	//
	// Android Game Window
	//

	class GameWindowAndroid final : public WindowBaseWithSurface
	{
		friend class GameAppAndroid;

	// types
	private:


	// variables
	private:
		uint2					_surfSize;
		InputActionsAndroid		_input;


	// methods
	public:

	// IWindow //
		void  Close ()										__NE_OV;
		bool  SetBrightness (Percent)						__NE_OV;
		bool  SetColorSpace (EColorSpace)					C_NE_OV;

		uint2			GetSurfaceSize ()					C_NE_OV;
		Monitor			GetMonitor ()						C_NE_OV;

		IInputActions&	InputActions ()						__NE_OV	{ return _input; }
		NativeWindow	GetNative ()						C_NE_OV;


	private:
		GameWindowAndroid (GameAppAndroid &,
						   Unique<IWndListener> listener)	__NE___;

		void  _Init (Unique<IWndListener>	listener,
					 IInputActions*			dstActions)		__NE___;

		bool  ProcessMessages ()							__NE_OV;

		using WindowBase::_SetStateV2;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
