// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe: only main thread
*/

#pragma once

#include "platform/DefaultV1/AppConfig.h"

namespace AE::AppV1
{

	//
	// Base Application interface
	//

	class IBaseApp : public EnableRC< IBaseApp >
	{
	// interface
	public:
		ND_ virtual bool  OnSurfaceCreated (IOutputSurface &)						__NE___ = 0;
			virtual void  InitInputActions (IInputActions &)						__NE___	= 0;

			virtual void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>)	__NE___	= 0;
			virtual void  StopRendering ()											__NE___	= 0;
			virtual void  SurfaceDestroyed ()										__NE___ = 0;

			virtual void  WaitFrame (const Threading::EThreadArray &)				__NE___	= 0;
			virtual void  RenderFrame ()											__NE___ = 0;
	};


} // AE::AppV1
