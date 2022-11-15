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
#include "platform/IO/VRSurface.h"

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
			explicit VRRenderSurface (VRDeviceEmulator &vr) : _vrDev{vr} {}

			// IOutputSurface //
			AsyncTask	Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps) override;
			bool		GetTargets (OUT RenderTargets_t &targets) const override;
			AsyncTask	End (ArrayView<AsyncTask> deps) override;
		};
		

		class InputActions final : public InputActionsBase
		{
		private:
			VRDeviceEmulator &	_vrDev;
			DubleBufferedQueue	_dbQueue;
			
		public:
			InputActions (VRDeviceEmulator &vr, DubleBufferedQueue *q) :
				InputActionsBase{q ? *q : _dbQueue}, _vrDev{vr}
			{}
			
			// IInputActions //
			ActionQueueReader  ReadInput (FrameUID frameId) const override;
			void  NextFrame (FrameUID frameId) override;
			bool  SetMode (const InputModeName &value) override;
			bool  LoadSerialized (MemRefRStream &) override;
			bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &) const override;

			bool  BeginBindAction (const InputModeName &mode, const InputActionName &action, EValueType type, EGestureType gesture) override;
			bool  EndBindAction () override;
			bool  IsBindActionActive () const override;

			// ISerializable //
			bool  Serialize (Serializing::Serializer &)		C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &) __NE_OV;
		};

		
		class WindowEventListener final : public IWindow::IWndListener
		{
		private:
			VRDeviceEmulator &	_vrDev;

		public:
			explicit WindowEventListener (VRDeviceEmulator &vr) : _vrDev{vr} {}
			~WindowEventListener () override {}

			void  OnUpdate (IWindow &) override {}
			void  OnResize (IWindow &, const uint2 &) override {}
			
			void  OnSurfaceCreated (IWindow &) override {}
			void  OnSurfaceDestroyed (IWindow &) override {}

			void  OnStateChanged (IWindow &, EState) override;
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
		VRDeviceEmulator (IApplication &app, VRDeviceListener, IInputActions* dst);
		~VRDeviceEmulator ();

		ND_ bool  Create ();

	// VRDeviceBase //
		bool  Update (Duration_t timeSinceStart) override;

	// IVRDevice //
		bool  Setup (const Settings &) override;
		
		StringView			GetApiName ()	const	override	{ return "vremulator"; }
		IInputActions&		InputActions ()			override	{ return _input; }
		IOutputSurface&		GetSurface ()			override	{ return _surface; }
		
		bool  CreateRenderSurface (const VRImageDesc &desc) override;

	private:
		void  _Destroy ();
		void  _ProcessInput ();
	};


} // AE::App
