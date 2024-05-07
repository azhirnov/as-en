// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "demo/Common.h"
#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Sample Application
	//

	class SampleApplication final : public AppV1::AppCoreV1
	{
	// methods
	public:
		SampleApplication ()				__NE___;

		void  OnStart (IApplication &)		__NE_OV;

	private:
		ND_ bool  _InitVFS (IApplication &);
	};



	//
	// Sample Core
	//

	class SampleCore final : public AppV1::IBaseApp
	{
	// types
	private:
		struct MainLoopData
		{
			Ptr<IInputActions>		input;		// lifetime is same as Window/VRDevice lifetime
			Ptr<IOutputSurface>		output;		// lifetime is same as Window/VRDevice lifetime
		};
		using MainLoopSync_t	= Threading::DRCSynchronized< MainLoopData >;


	// variables
	private:
		Strong<PipelinePackID>		_pplnPack;
		MainLoopSync_t				_mainLoop;
		RC<ISample>					_sample;

		RC<ArrayRStream>			_inputActionsData;

		Atomic<bool>				_initialized {false};

		IApplicationTS				_app;


	// methods
	public:
		SampleCore ()																__NE___;
		~SampleCore ();

		ND_	bool  LoadInputActions ();
			void  SetApplication (IApplication &app)										{ _app = IApplicationTS{app}; }

	private:
		ND_ bool  _CompileBaseResources (IOutputSurface &);
		ND_ bool  _CompileResources (IOutputSurface &);


	// IBaseApp (main thread) //
	private:
		bool  OnSurfaceCreated (IWindow &)											__NE_OV;
		void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, EWndState)	__NE_OV;
		void  StopRendering (Ptr<IOutputSurface>)									__NE_OV;
		void  RenderFrame ()														__NE_OV;
		void  WaitFrame (const Threading::EThreadArray	&threadMask,
						 Ptr<IWindow>					window,
						 Ptr<IVRDevice>					vrDevice)					__NE_OV;

		void  _InitInputActions (IInputActions &);
	};


} // AE::Samples::Demo
