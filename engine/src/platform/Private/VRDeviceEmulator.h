// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Input actions redirections:
		VR window input -> VR emulator InputActions -> (optional) Window InputActions 
*/

#pragma once

#include "base/Platforms/Platform.h"

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

		class VRRenderSurface final : public VRSurface
		{
		private:
			class BlitImageTask;

		private:
			VRDeviceEmulator &	_vrDev;

		public:
			explicit VRRenderSurface (VRDeviceEmulator &vr)																__NE___	: _vrDev{vr} {}

			// IOutputSurface //
			AsyncTask		Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)	__NE_OV;
			bool			GetTargets (OUT RenderTargets_t &targets)														C_NE_OV;
			AsyncTask		End (ArrayView<AsyncTask> deps)																	__NE_OV;

			AllImages_t		GetAllImages ()																					C_NE_OV;
			TargetSizes_t	GetTargetSizes ()																				C_NE_OV	{ return Default; }
			ColorFormats_t	GetColorFormats ()																				C_NE_OV	{ return Default; }
			PresentModes_t	GetPresentModes ()																				C_NE_OV	{ return Default; }
			SurfaceInfo		GetSurfaceInfo ()																				C_NE_OV	{ return Default; }
		};
		

		class InputActions final : public InputActionsBase
		{
		private:
			VRDeviceEmulator &	_vrDev;
			DubleBufferedQueue	_dbQueue;
			
		public:
			InputActions (VRDeviceEmulator &vr, DubleBufferedQueue *q)																__NE___	:
				InputActionsBase{q ? *q : _dbQueue}, _vrDev{vr}
			{}
			
			// IInputActions //
			ActionQueueReader  ReadInput (FrameUID frameId)																			C_NE_OV;
			void  NextFrame (FrameUID frameId)																						__NE_OV;
			bool  SetMode (const InputModeName &value)																				__NE_OV;
			bool  LoadSerialized (MemRefRStream &)																					__NE_OV;
			bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &)						C_NE_OV;

			bool  BeginBindAction (const InputModeName &mode, const InputActionName &action, EValueType type, EGestureType gesture)	__NE_OV;
			bool  EndBindAction ()																									__NE_OV;
			bool  IsBindActionActive ()																								C_NE_OV;

			// ISerializable //
			bool  Serialize (Serializing::Serializer &)																				C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)																			__NE_OV;
		};

		
		class WindowEventListener final : public IWindow::IWndListener
		{
		private:
			VRDeviceEmulator &	_vrDev;

		public:
			explicit WindowEventListener (VRDeviceEmulator &vr)		__NE___	: _vrDev{vr} {}
			~WindowEventListener ()									__NE_OV {}

			void  OnUpdate (IWindow &)								__NE_OV {}
			void  OnResize (IWindow &, const uint2 &)				__NE_OV {}
			
			void  OnSurfaceCreated (IWindow &)						__NE_OV {}
			void  OnSurfaceDestroyed (IWindow &)					__NE_OV {}

			void  OnStateChanged (IWindow &, EState)				__NE_OV;
		};


	// variables
	private:
		IApplication &			_app;

		WindowEventListener*	_wndListener	= null;
		WindowPtr				_window;

		VRRenderSurface			_surface;
		InputActions			_input;
		ProjectionImpl<float>	_projections [2];
		
		float4x4				_hmdPose;
		float3					_hmdPosition;
		float4x4				_view[2];		// camera view matrix (left, right)

		RWSpinLock				_hmdRotationGuard;
		float2					_hmdRotation;
		FrameUID				_lastFrameId;


	// methods
	public:
		VRDeviceEmulator (IApplication &app, VRDeviceListener, IInputActions* dst)	__NE___;
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
