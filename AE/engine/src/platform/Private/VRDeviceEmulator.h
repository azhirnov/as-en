// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Input actions redirections:
		VR window input -> VR emulator InputActions -> (optional) Window InputActions
*/

#pragma once

#include "platform/Public/Window.h"
#include "platform/Private/WindowBase.h"
#include "platform/Private/InputActionsBase.h"
#include "platform/Private/ProjectionImpl.h"
#include "platform/Private/VRSurface.h"

#include "platform/GLFW/WindowGLFW.h"

namespace AE::App
{
	using Threading::RWSpinLock;
	using Graphics::RenderCoro;


	//
	// VR Session Emulator
	//

	class VRDeviceEmulator final : public VRSessionBase
	{
	// types
	private:

		//
		// VR Render Surface
		//
		class VRRenderSurface final : public VRSurface
		{
		// variables
		private:
			VRDeviceEmulator &		_vrSession;
			CommandBatchPtr			_presentBatch;
			AsyncTask				_acquireImg;
			Atomic<uint>			_projIdx		{0};

		// methods
		public:
			explicit VRRenderSurface (VRDeviceEmulator &vr)										__NE___	: _vrSession{vr} {}

			// IOutputSurface //
			AsyncTask			Begin (CommandBatchPtr, CommandBatchPtr, ArrayView<AsyncTask>)	__NE_OV;
			bool				GetTargets (OUT RenderTargets_t &targets)						C_NE_OV;
			AsyncTask			End (ArrayView<AsyncTask> deps)									__NE_OV;

			bool				SetSurfaceMode (const SurfaceInfo &)							__NE_OV;

			SurfaceFormats_t	GetSurfaceFormats ()											C_NE_OV;
			PresentModes_t		GetPresentModes ()												C_NE_OV;
			SurfaceInfo			GetSurfaceInfo ()												C_NE_OV;

		private:
			ND_ bool			_GetDstTargets (OUT RenderTargets_t &targets)					C_NE___;

			static RenderCoro	_BlitImageTask (VRRenderSurface &)								__NE___;
		};


		//
		// Input Actions
		//
		class InputActions final : public InputActionsBase
		{
		// variables
		private:
			VRDeviceEmulator &			_vrSession;

		// methods
		public:
			InputActions (VRDeviceEmulator &vr, TsDoubleBufferedQueue* q)								__NE___	:
				InputActionsBase{ q }, _vrSession{vr}
			{}

			// IInputActions //
			ActionQueueReader  ReadInput (FrameUID frameId)												C_NE_OV;
			void  NextFrame (FrameUID frameId)															__NE_OV;
			bool  SetMode (InputModeName::Ref value)													__NE_OV;
			bool  LoadSerialized (MemRefRStream &)														__NE_OV;
			bool  GetReflection (InputModeName::Ref, InputActionName::Ref, OUT Reflection &)			C_NE_OV;

			bool  BeginBindAction (InputModeName::Ref, InputActionName::Ref, EValueType, EGestureType)	__NE_OV;
			bool  EndBindAction ()																		__NE_OV;
			bool  IsBindActionActive ()																	C_NE_OV;

			// ISerializable //
			bool  Serialize (Serializing::Serializer &)													C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)												__NE_OV;
		};


		//
		// Window Event Listener
		//
		class WindowEventListener final : public IWindow::IWndListener
		{
		// variables
		private:
			VRDeviceEmulator &	_vrSession;

		// methods
		public:
			explicit WindowEventListener (VRDeviceEmulator &vr)		__NE___	: _vrSession{vr} {}
			~WindowEventListener ()									__NE_OV {}

			void  OnSurfaceCreated (IWindow &)						__NE_OV;
			void  OnSurfaceDestroyed (IWindow &)					__NE_OV;

			void  OnStateChanged (IWindow &, EState)				__NE_OV;
		};

		using Projections_t = StaticArray< ProjectionImpl, 4 >;	// 2 eyes with double buffering


	// variables
	private:
		WindowPtr					_window;

		VRRenderSurface				_surface;
		InputActions				_input;
		Projections_t				_projections;

		StaticArray< float4x4, 2 >	_view;			// camera view matrix (left, right)

		RWSpinLock					_hmdRotationGuard;
		float2						_hmdRotation;
		FrameUID					_lastFrameId;


	// methods
	public:
		VRDeviceEmulator (ApplicationBase&, Unique<IWndListener>, IInputActions*)__NE___;
		~VRDeviceEmulator ()													__NE___;

		ND_ bool  Create ()														__NE___;


	// IWindow //
		void  Close ()															__NE_OV;
		bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc)			__NE_OV;
		bool  SetBrightness (Percent level)										__NE_OV	{ return _window->SetBrightness( level ); }
		bool  SetColorSpace (EColorSpace value)									C_NE_OV	{ return _window->SetColorSpace( value ); }

		uint2				GetSurfaceSize ()									C_NE_OV	{ return _window->GetSurfaceSize(); }
		Monitor				GetMonitor ()										C_NE_OV	{ return _window->GetMonitor(); }
		NativeWindow		GetNative ()										C_NE_OV	{ return _window->GetNative(); }

		IInputActions&		InputActions ()										__NE_OV	{ return _input; }
		IOutputSurface&		GetSurface ()										__NE_OV	{ return _surface; }


	// IVRSession //
		EDeviceType			GetDeviceType ()									C_NE_OV	{ return EDeviceType::Emulator; }

		bool  Setup (const Settings &)											__NE_OV;


	private:
		void  _Destroy ()														__NE___;
		void  _ProcessInput ()													__NE___;

	// WindowBase //
		bool  ProcessMessages ()												__NE_OV;
		void  _CreateSwapchain ()												__NE_OV;
		void  _DestroySwapchain ()												__NE_OV;
	};


} // AE::App
