// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Device interface only used to be sure that all device implementations has the same methods.

	Thread-safe:
	 - const methods are safe after device initialization and before destruction
*/

#pragma once

#include "graphics/Public/DeviceProperties.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/FeatureSet.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/Queue.h"

namespace AE::Graphics
{
	enum class EGraphicsAPI : ubyte
	{
		Vulkan	= 1,
		Metal	= 2,
	};

	enum class EGraphicsAdapterType : ubyte
	{
		Unknown,
		Discrete,
		Integrated,
		Software,
	};



	//
	// Graphics Device interface
	//

	class NO_VTABLE IDevice : public Noncopyable
	{
	// types
	public:
		using ResourceFlags		= DeviceResourceFlags;
		using DevMemoryUsageOpt	= Optional< DeviceMemoryUsage >;


	// interface
	public:
		ND_ virtual EGraphicsAPI				GetGraphicsAPI ()					C_NE___ = 0;

		ND_ virtual ResourceFlags const&		GetResourceFlags ()					C_NE___	= 0;
		ND_ virtual DeviceProperties const&		GetDeviceProperties ()				C_NE___ = 0;

		ND_ virtual EQueueMask					GetAvailableQueues ()				C_NE___	= 0;
		ND_ virtual StringView					GetDeviceName ()					C_NE___	= 0;

		ND_ virtual DeviceMemoryInfo			GetMemoryInfo ()					C_NE___ = 0;
		ND_ virtual DevMemoryUsageOpt			GetMemoryUsage ()					C_NE___ = 0;

		ND_ virtual EGraphicsAdapterType		AdapterType ()						C_NE___ = 0;

		ND_ virtual bool						IsInitialized ()					C_NE___	= 0;
		ND_ virtual bool						IsUnderDebugger ()					C_NE___ = 0;

		ND_ virtual bool						CheckConstantLimits ()				C_NE___ = 0;
		ND_ virtual bool						CheckExtensions ()					C_NE___ = 0;

	// TODO:
		//ND_ virtual VQueuePtr					GetQueue (EQueueType type)			C_NE___	= 0;
		//ND_ virtual ArrayView<VQueue>			GetQueues ()						C_NE___	= 0;
	};


} // AE::Graphics
