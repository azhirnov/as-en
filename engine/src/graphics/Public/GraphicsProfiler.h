// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/FrameUID.h"

#include "graphics/Public/MetalTypes.h"
#include "graphics/Public/VulkanTypes.h"

namespace AE::Graphics
{

	//
	// Graphics Profiler interface
	//

	class IGraphicsProfiler : public EnableRC<IGraphicsProfiler>
	{
	// types
	public:
		enum class EContextType : ubyte
		{
			Transfer,
			Compute,
			Graphics,
			RenderPass,
			ASBuild,
			RayTracing,
			Unknown		= 0xFF
		};


	// interface
	public:
		// queue
		virtual void  SetQueue (EQueueType type, StringView name) = 0;

		// batch
		virtual void  BeginBatch (FrameUID frameId, const void* batch, StringView name) = 0;
		virtual void  SubmitBatch (const void* batch, EQueueType queue) = 0;
		virtual void  BatchComplete (const void* batch) = 0;

		// draw batch
		virtual void  BeginDrawBatch (const void* batch, StringView name) = 0;
		
	  #if defined(AE_ENABLE_VULKAN)
		// context
		virtual void  BeginContext (const void* batch, VkCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type) = 0;
		virtual void  EndContext (const void* batch, VkCommandBuffer cmdbuf, EContextType type) = 0;
	
	  #elif defined(AE_ENABLE_METAL)
		// context
		virtual void  BeginContext (const void* batch, MetalCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type) = 0;
		virtual void  EndContext (const void* batch, MetalCommandBuffer cmdbuf, EContextType type) = 0;
	  #endif

		// frames
		virtual void  RequestNextFrame (FrameUID frameId) = 0;
		virtual void  NextFrame (FrameUID frameId) = 0;

		// memory
	};


} // AE::Graphics