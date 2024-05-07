// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"

namespace AE::Profiler
{
	using AE::Graphics::EQueueType;

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
		using PipelineStatistic	= Graphics::IQueryManager::GraphicsPipelineStatistic;

	  #if defined(AE_ENABLE_VULKAN)
		using Query = Graphics::VQueryManager::Query;

		struct BatchCmdbufKey
		{
			const void*		batch;
			VkCommandBuffer	cmdbuf;
			EContextType	type;

			BatchCmdbufKey ()									__NE___	{}
			BatchCmdbufKey (const void*		batch,
							VkCommandBuffer	cmdbuf,
							EContextType	type)				__NE___ :
				batch{batch}, cmdbuf{cmdbuf}, type{type} {}

			ND_ bool	operator == (const BatchCmdbufKey &rhs) C_NE___;
			ND_ HashVal	CalcHash ()								C_NE___;
		};

	  #elif defined(AE_ENABLE_METAL)
		using Query = Graphics::MQueryManager::Query;

		struct BatchCmdbufKey
		{
			const void*			batch;
		//	MetalCommandBuffer	cmdbuf;
			EContextType		type;

			BatchCmdbufKey ()									__NE___	{}
			BatchCmdbufKey (const void*			batch,
						//	MetalCommandBuffer	cmdbuf,
							EContextType		type)			__NE___	:
				batch{batch}, type{type} {}

			ND_ bool	operator == (const BatchCmdbufKey &rhs) C_NE___;
			ND_ HashVal	CalcHash ()								C_NE___;
		};

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using Query		= Graphics::RQueryManager::Query;
		using CmdBuf	= Graphics::_hidden_::RSoftwareCmdBuf;

		struct BatchCmdbufKey
		{
			const void*			batch;
			const CmdBuf*		cmdbuf;
			EContextType		type;

			BatchCmdbufKey ()									__NE___	{}
			BatchCmdbufKey (const void*		batch,
							const CmdBuf*	cmdbuf,
							EContextType	type)				__NE___	:
				batch{batch}, cmdbuf{cmdbuf}, type{type} {}

			ND_ bool	operator == (const BatchCmdbufKey &rhs) C_NE___;
			ND_ HashVal	CalcHash ()								C_NE___;
		};

	  #else
	  #	error not implemented
	  #endif

		struct Pass
		{
		//	Query			pplnStat;
			Query			timestamp;
			String			name;
			RGBA8u			color;
			bool			recorded	= false;	// set 'true' in 'EndContext()'
		};

		struct ContextInfo
		{
			EQueueType		queue		= Default;
			EContextType	ctxType		= Default;
			Array<Pass>		passes;
		};
		using ActiveCmdbufs_t = FlatHashMap< BatchCmdbufKey, ContextInfo, DefaultHasher_CalcHash<BatchCmdbufKey> >;


		struct PerFrameData
		{
			SharedMutex			guard;
			ActiveCmdbufs_t		activeCmdbufs;

			PerFrameData ();

			void  Clear ();
		};
		using PerFrame_t = StaticArray< PerFrameData, Graphics::GraphicsConfig::MaxFrames+1 >;

		using MemoryUsage_t = Optional< Graphics::DeviceMemoryUsage >;


	// variables
	private:
		struct {
			Atomic<uint>		frameCount		{0};
			FAtomic<double>		accumframeTime	{0.0};	// nanoseconds
			float				result			= 0.f;	// fps
			nanosecondsf		dt				{0.f};
			nanosecondsf		ext				{0.f};	// external time per frame, may be vsync, video dec/enc or other apps
		}					_fps;

		struct {
			nanosecondsd		min		{0.0};
			nanosecondsd		max		{0.0};
		}					_gpuTime;

		struct {
			AtomicByte<Bytes>	accumWrite;
			AtomicByte<Bytes>	accumRead;
			Bytes				avgWrite;
			Bytes				avgRead;
		}					_memTraffic;

		MemoryUsage_t		_memUsage;

		PerFrame_t			_perFrame;

		uint				_writeIndex	: 8;
		uint				_readIndex	: 8;

	  #ifdef AE_ENABLE_IMGUI
		ImColumnHistoryDiagram	_imHistory;
	  #endif


	// methods
	public:
		explicit GraphicsProfiler (TimePoint_t startTime)																		__NE___;

		void  DrawImGUI ();
		void  Draw (Canvas &canvas);
		void  Update (secondsf dt);


	  // IGraphicsProfiler //

		// queue
		void  SetQueue (EQueueType type, StringView name)																		__NE_OV;

		// batch
		void  BeginBatch (FrameUID frameId, const void* batch, StringView name)													__NE_OV;
		void  SubmitBatch (const void* batch, EQueueType queue)																	__NE_OV;
		void  BatchComplete (const void* batch)																					__NE_OV;

		// draw batch
		void  BeginDrawBatch (const void* batch, StringView name)																__NE_OV;

	  #if defined(AE_ENABLE_VULKAN)
		// context
		void  BeginContext (const void* batch, VkCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type)	__NE_OV;
		void  EndContext (const void* batch, VkCommandBuffer cmdbuf, EContextType type)											__NE_OV;

	  #elif defined(AE_ENABLE_METAL)
		// context
		void  BeginContext (OUT MetalSampleBufferAttachments &sampleBuffers, const void* batch, MetalCommandBuffer cmdbuf,
							StringView taskName, RGBA8u color, EContextType type)												__NE_OV;
		void  EndContext (const void* batch, MetalCommandBuffer cmdbuf, EContextType type)										__NE_OV;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		// context
		void  BeginContext (const void* batch, void* cmdbuf, StringView taskName, RGBA8u color, EContextType type)				__Th_OV;
		void  EndContext (const void* batch, void* cmdbuf, EContextType type)													__Th_OV;

	  #else
	  #	error not implemented
	  #endif

		// frames
		void  NextFrame (FrameUID frameId)																						__NE_OV;


	private:
		void  _ReadResults ();
	};


} // AE::Profiler
