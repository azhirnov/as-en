// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Input actions redirections:
		VR window input -> VR emulator InputActions -> (optional) Window InputActions
*/

#pragma once

#include "platform/Public/IWindow.h"
#include "platform/Private/InputActionsBase.h"
#include "platform/Private/VRDeviceBase.h"
#include "platform/Private/ProjectionImpl.h"
#include "platform/Private/VRSurface.h"

#include "platform/GLFW/WindowGLFW.h"

namespace AE::App
{
	using AE::Threading::RWSpinLock;


	//
	// VR Device Emulator
	//

	class VRDeviceEmulator final : public VRDeviceBase
	{
	// types
	private:

		//
		// VR Render Surface
		//
		class VRRenderSurface final : public VRSurface
		{
		// types
		private:
			class BlitImageTask;

		// variables
		private:
			VRDeviceEmulator &		_vrDev;
			CommandBatchPtr			_presentBatch;
			AsyncTask				_acquireImg;
			Atomic<uint>			_projIdx		{0};

		// methods
		public:
			explicit VRRenderSurface (VRDeviceEmulator &vr)																		__NE___	: _vrDev{vr} {}

			// IOutputSurface //
			AsyncTask			Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)	__NE_OV;
			bool				GetTargets (OUT RenderTargets_t &targets)														C_NE_OV;
			AsyncTask			End (ArrayView<AsyncTask> deps)																	__NE_OV;

			bool				SetSurfaceMode (const SurfaceInfo &)															__NE_OV;

			SurfaceFormats_t	GetSurfaceFormats ()																			C_NE_OV;
			PresentModes_t		GetPresentModes ()																				C_NE_OV;
			SurfaceInfo			GetSurfaceInfo ()																				C_NE_OV;

		private:
			ND_ bool			_GetDstTargets (OUT RenderTargets_t &targets)													C_NE___;
		};


		//
		// Input Actions
		//
		class InputActions final : public InputActionsBase
		{
		// variables
		private:
			VRDeviceEmulator &			_vrDev;

		// methods
		public:
			InputActions (VRDeviceEmulator &vr, TsDoubleBufferedQueue* q)															__NE___	:
				InputActionsBase{ q }, _vrDev{vr}
			{}

			// IInputActions //
			ActionQueueReader  ReadInput (FrameUID frameId)																			C_NE_OV;
			void  NextFrame (FrameUID frameId)																						__NE_OV;
			bool  SetMode (InputModeName::Ref value)																				__NE_OV;
			bool  LoadSerialized (MemRefRStream &)																					__NE_OV;
			bool  GetReflection (InputModeName::Ref mode, InputActionName::Ref action, OUT Reflection &)							C_NE_OV;

			bool  BeginBindAction (InputModeName::Ref mode, InputActionName::Ref action, EValueType type, EGestureType gesture)		__NE_OV;
			bool  EndBindAction ()																									__NE_OV;
			bool  IsBindActionActive ()																								C_NE_OV;

			// ISerializable //
			bool  Serialize (Serializing::Serializer &)																				C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)																			__NE_OV;
		};


		//
		// Window Event Listener
		//
		class WindowEventListener final : public IWindow::IWndListener
		{
		// variables
		private:
			VRDeviceEmulator &	_vrDev;

		// methods
		public:
			explicit WindowEventListener (VRDeviceEmulator &vr)		__NE___	: _vrDev{vr} {}
			~WindowEventListener ()									__NE_OV {}

			void  OnSurfaceCreated (IWindow &)						__NE_OV {}
			void  OnSurfaceDestroyed (IWindow &)					__NE_OV {}

			void  OnStateChanged (IWindow &, EState)				__NE_OV;
		};

		using Projections_t = StaticArray< ProjectionImpl, 4 >;	// 2 eyes with double buffering


	// variables
	private:
		IApplication &				_app;

		WindowEventListener*		_wndListener	= null;
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
		VRDeviceEmulator (IApplication &app, VRDeviceListener, IInputActions*)		__NE___;
		~VRDeviceEmulator ()														__NE___;

		ND_ bool  Create ()															__NE___;

	// VRDeviceBase //
		bool  Update (Duration_t timeSinceStart)									__NE_OV;

	// IVRDevice //
		bool  Setup (const Settings &)												__NE_OV;

		StringView			GetApiName ()											C_NE_OV	{ return "vremulator"; }
		IInputActions&		InputActions ()											__NE_OV	{ return _input; }
		IOutputSurface&		GetSurface ()											__NE_OV	{ return _surface; }

		bool  CreateRenderSurface (const VRImageDesc &desc)							__NE_OV;

	private:
		void  _Destroy ();
		void  _ProcessInput ();
	};


} // AE::App
