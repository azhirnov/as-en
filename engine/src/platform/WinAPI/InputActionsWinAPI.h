// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_WINDOWS) && !defined(AE_ENABLE_GLFW)

# include "platform/Private/InputActionsBase.h"
# include "platform/Private/GestureRecognizer.h"
# include "platform/WinAPI/SerializableInputActionsWinAPI.h"

namespace AE::App
{
	class WindowWinAPI;

	
	//
	// Input Actions for WinAPI
	//

	class InputActionsWinAPI final : public InputActionsBase
	{
	// types
	private:
		using EInputType = SerializableInputActionsWinAPI::EInputType;
		
		static constexpr uint	_Version = SerializableInputActionsWinAPI::_Version;


	// variables
	private:
		DubleBufferedQueue	_dbQueue;

		float2				_cursorPos;
		
		float2				_toSNorm;
		float2				_pixToMm;
		uint2				_surfaceSize;

		GestureRecognizer	_gestureRecognizer;


	// methods
	public:
		InputActionsWinAPI () : InputActionsBase{_dbQueue} {}
		
		ND_ bool  ProcessMessage (uint uMsg, usize wParam, ssize lParam, Duration_t timestamp);
		ND_ bool  Register (void* wnd);
			void  Unregister ();
			void  SetMonitor (const uint2 &size, const Monitor &);

			void  Update (Duration_t timeSinceStart);


	// IInputActions //
		void  NextFrame (FrameUID frameId) override		{ _dbQueue.NextFrame( frameId ); }

		bool  LoadSerialized (RStream &stream) override;
		

	// ISerializable //
		bool  Serialize (Serializing::Serializer &) const override;
		bool  Deserialize (Serializing::Deserializer &) override;


	private:
		void  _UpdateKey (EInputType type, EGestureState state, ControllerID id, Duration_t timestamp);
		void  _SetCursorPos (float2 pos);
		void  _SetCursorDelta (float2 pos);
		void  _SetMouseWheel (EInputType type, float delta);

		ND_ static constexpr bool  _IsKey (EInputType type)			{ return SerializableInputActionsWinAPI::_IsKey( type ); }
		ND_ static constexpr bool  _IsCursor1D (EInputType type)	{ return SerializableInputActionsWinAPI::_IsCursor1D( type ); }
		ND_ static constexpr bool  _IsCursor2D (EInputType type)	{ return SerializableInputActionsWinAPI::_IsCursor2D( type ); }
	};


} // AE::App

#endif // AE_PLATFORM_WINDOWS and not AE_ENABLE_GLFW