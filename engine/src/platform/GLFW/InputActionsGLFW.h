// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_GLFW
# include "platform/Private/InputActionsBase.h"
# include "platform/GLFW/SerializableInputActionsGLFW.h"

typedef struct GLFWwindow GLFWwindow;

namespace AE::App
{

	//
	// Input Actions for GLFW
	//

	class InputActionsGLFW final : public InputActionsBase
	{
	// types
	private:
		using EInputType = SerializableInputActionsGLFW::EInputType;
		
		static constexpr uint	_Version = SerializableInputActionsGLFW::_Version;


	// variables
	private:
		DubleBufferedQueue	_dbQueue;

		float2				_cursorPos;

		float2				_toSNorm;
		float2				_pixToMm;

		bool				_mouseLBPressed		= false;
		

	// methods
	public:
		explicit InputActionsGLFW (DubleBufferedQueue *q) : InputActionsBase{q ? *q : _dbQueue} {}
		
		// api for window
		void  SetKey (int key, EGestureState state, Duration_t timestamp);
		void  SetMouseButton (int button, EGestureState state, Duration_t timestamp);
		void  SetCursorPos (float2 pos);
		void  SetMouseWheel (float2 delta);
		void  SetMonitor (const uint2 &surfaceSize, const Monitor &);
		void  CursorPosChanged (float2 pos)				{ _cursorPos = pos; }


	// IInputActions //
		bool  LoadSerialized (MemRefRStream &stream) override;


	// ISerializable //
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
		

	private:
		bool  _ValidateForVREmulation ();

		ND_ static constexpr bool  _IsKey (EInputType type)			{ return SerializableInputActionsGLFW::_IsKey( type ); }
		ND_ static constexpr bool  _IsCursor1D (EInputType type)	{ return SerializableInputActionsGLFW::_IsCursor1D( type ); }
		ND_ static constexpr bool  _IsCursor2D (EInputType type)	{ return SerializableInputActionsGLFW::_IsCursor2D( type ); }
	};


} // AE::App

#endif // AE_ENABLE_GLFW
