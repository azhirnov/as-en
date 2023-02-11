// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/DefaultImpl/AppConfig.h"

namespace AE::App
{

	//
	// Base Application interface
	//

	class IBaseApp : public EnableRC< IBaseApp >
	{
	// interface
	public:
		ND_ virtual bool  OnSurfaceCreated (IOutputSurface &)									__NE___ = 0;
			virtual void  InitInputActions (IInputActions &)									__NE___	= 0;

		// Returns 'true' if new task started, 'false' if task with these params already started.
			virtual bool  StartMainLoop (Ptr<IInputActions>, Ptr<IOutputSurface>, AnyTypeRef)	__NE___	= 0;
			virtual void  StopMainLoop ()														__NE___	= 0;
	};


} // AE::App
