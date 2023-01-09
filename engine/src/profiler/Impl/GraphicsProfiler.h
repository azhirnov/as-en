// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Primitives/Atomic.h"
#include "graphics/Public/GraphicsImpl.h"
#include "profiler/Impl/ProfilerUtils.h"

namespace AE::Profiler
{
	using AE::Graphics::EQueueType;
	using AE::Graphics::FrameUID;

#ifdef AE_ENABLE_METAL
	using AE::Graphics::MetalSampleBufferAttachments;
	using AE::Graphics::MetalCommandBuffer;
#endif



	//
	// Graphics Profiler
	//

	class GraphicsProfiler final : public Graphics::IGraphicsProfiler, public ProfilerUtils
	{
	// types
	private:
		using BatchNameMap_t	= FlatHashMap< const void*, String >;

	  #if defined(AE_ENABLE_VULKAN)
		using Query				= Graphics::VQueryManager::Query;
		using PipelineStatistic	= Graphics::VQueryManager::PipelineStatistic;
		
		struct BatchCmdbufKey
		{
			const void*		batch;
			VkCommandBuffer	cmdbuf;
			EContextType	type;

			BatchCmdbufKey () {}

			BatchCmdbufKey (const void*		batch,
							VkCommandBuffer	cmdbuf,
							EContextType	type) :
				batch{batch}, cmdbuf{cmdbuf}, type{type} {}

			ND_ bool	operator == (const BatchCmdbufKey &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		
	  #elif defined(AE_ENABLE_METAL)
		using Query				= Graphics::MQueryManager::Query;
		using PipelineStatistic	= Graphics::MQueryManager::PipelineStatistic;
		
		struct BatchCmdbufKey
		{
			const void*			batch;
		//	MetalCommandBuffer	cmdbuf;
			EContextType		type;

			BatchCmdbufKey () {}

			BatchCmdbufKey (const void*			batch,
						//	MetalCommandBuffer	cmdbuf,
							EContextType		type) :
				batch{batch}, type{type} {}

			ND_ bool	operator == (const BatchCmdbufKey &rhs) const;
			ND_ HashVal	CalcHash () const;
		};

	  #else
	  #	error not implemented
	  #endif

		struct ContextInfo
		{
			EQueueType		queue		= Default;
			EContextType	ctxType		= Default;
			Query			pplnStat;
			Query			timestamp;
			String			name;
			RGBA8u			color;
		};
		using ActiveCmdbufs_t = FlatHashMap< BatchCmdbufKey, ContextInfo, DefaultHasher_CalcHash<BatchCmdbufKey> >;


		struct PerFrameData
		{
			ActiveCmdbufs_t		activeCmdbufs;

			PerFrameData ();

			void  Clear ();
		};
		using PerFrame_t = StaticArray< PerFrameData, Graphics::GraphicsConfig::MaxFrames >;


	// variables
	private:
		struct {
			secondsf		lastUpdate		{0};
			Atomic<uint>	frameCount		{0};
			FAtomic<double>	accumframeTime	{0.0};
			float			result			= 0.f;
			float			dt				= 0.f;
		}				_fps;

		struct {
			double			min	= 0;	// nanoseconds
			double			max	= 0;
		}				_gpuTime;

		SharedMutex		_guard;			// protects '_perFrame' and '_frameId'
		PerFrame_t		_perFrame;

		FrameUID		_frameId;

	  #ifdef AE_ENABLE_IMGUI
		ImColumnHistoryDiagram	_imHistory;
	  #endif


	// methods
	public:
		explicit GraphicsProfiler (TimePoint_t startTime);
		~GraphicsProfiler () {}

		void  DrawImGUI ();
		void  Draw (Canvas &canvas);
		void  Update ();


	  // IGraphicsProfiler //
	  
		// queue
		void  SetQueue (EQueueType type, StringView name)																			__NE_OV;

		// batch
		void  BeginBatch (FrameUID frameId, const void* batch, StringView name)														__NE_OV;
		void  SubmitBatch (const void* batch, EQueueType queue)																		__NE_OV;
		void  BatchComplete (const void* batch)																						__NE_OV;

		// draw batch
		void  BeginDrawBatch (const void* batch, StringView name)																	__NE_OV;
		
	  #if defined(AE_ENABLE_VULKAN)
		// context
		void  BeginContext (const void* batch, VkCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type)		__NE_OV;
		void  EndContext (const void* batch, VkCommandBuffer cmdbuf, EContextType type)												__NE_OV;

	  #elif defined(AE_ENABLE_METAL)
		// context
		void  BeginContext (OUT MetalSampleBufferAttachments &sampleBuffers, const void* batch, MetalCommandBuffer cmdbuf,
							StringView taskName, RGBA8u color, EContextType type)													__NE_OV;
		void  EndContext (const void* batch, MetalCommandBuffer cmdbuf, EContextType type)											__NE_OV;
		
	  #else
	  #	error not implemented
	  #endif

		// frames
		void  RequestNextFrame (FrameUID frameId)																					__NE_OV;
		void  NextFrame (FrameUID frameId)																							__NE_OV;


	private:
		void  _ReadResults (PerFrameData &f);
	};


} // AE::Profiler
