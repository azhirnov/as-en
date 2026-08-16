// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe:  yes
*/

#pragma once

#include "platform/Public/InputActions.h"

namespace AE::AppV2
{
	using namespace AE::Base;


	//
	// View Mode interface
	//

	class IViewMode : public EnableRC<IViewMode>
	{
	// types
	public:
		using InputModeName		= App::InputModeName;
		using ActionQueueReader = App::IInputActions::ActionQueueReader;
		using IOutputSurface	= App::IOutputSurface;
		using AsyncTask			= Threading::AsyncTask;


	// interface
	public:

		// Called when view will start to show on screen.
		//  Thread-safe:  main thread only
		//
		ND_ virtual bool			Open (Ptr<IOutputSurface>,
										  AnyTypeCRef params)			__NE___	= 0;


		// Called when view will hidden by another view.
		//  Thread-safe:  main thread only
		//
			virtual void			Close ()							__NE___	= 0;


		// Process input and draw frame.
		// Returns last task in chain: ProcessInput -> Draw -> Submit.
		//  Thread-safe:  main thread only
		//
		ND_ virtual AsyncTask		Update (ActionQueueReader,
											Ptr<IOutputSurface>,
											ArrayView<AsyncTask> deps)	__NE___ = 0;


		// Returns name of input mode.
		// Used when view will start to show (after 'Open()')
		// and when input method is changed, for example from mouse+keyboard to VR controller.
		//  Thread-safe:  main thread only
		//
		ND_ virtual InputModeName	GetInputMode ()						C_NE___ = 0;


		// Notify view that app will be paused or will be resumed.
		// View may free resources and stop background work when app is paused,
		// then allocate resources again and start background work when app resumed.
		// Some UI implementations may use it to switch between views.
		// You don't need to save paused state and check it in 'Update()' because rendering is always paused.
		//
			virtual void			SetPaused (bool pause)				__NE___ { Unused( pause ); }
	};


} // AE::AppV2
