// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	specs:
		Only one CPU thread can access a command buffer at time.
		Multithreaded apps can use one thread per command buffer to create multiple command buffers in parallel.
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{

	//
	// Metal Primary Command buffer State
	//

	struct MPrimaryCmdBufState
	{
		Ptr<const MRenderPass>	renderPass;
		FrameUID				frameId;
		ulong					rasterOrderGroup	: 8;
		ulong					useSecondaryCmdbuf	: 1;	// parallel encoder
		void*					userData;

		MPrimaryCmdBufState ()	__NE___	:
			rasterOrderGroup{0xFF}, useSecondaryCmdbuf{false}, userData{null}
		{}

		ND_ bool  IsValid ()	C_NE___	{ return (renderPass != null) & frameId.IsValid(); }

		ND_ bool  operator == (const MPrimaryCmdBufState &rhs) C_NE___;
	};



	//
	// Metal Command Buffer
	//

	class MCommandBuffer final
	{
	// types
	public:
		enum class EncoderType
		{
			Unknown,
			Compute,
			Render,
			ParallelRender,
			AccelStruct,
			Blit,
			ResState,
		};


	// variables
	private:
		MetalCommandEncoderRC	_encoder;
		MetalCommandBufferRC	_cmdbuf;
		EQueueType				_queueType	= Default;
		EncoderType				_encType	= Default;

		DRC_ONLY(
			Threading::SingleThreadCheck	_stCheck;
		)


	// methods
	protected:
		MCommandBuffer (MetalCommandBufferRC cmdbuf, EQueueType queueType) __NE___;
		
		MCommandBuffer (const MCommandBuffer &)							= delete;
		MCommandBuffer&  operator = (const MCommandBuffer &)			= delete;

	public:
		MCommandBuffer ()												__NE___	{}
		MCommandBuffer (Base::_hidden_::DefaultType)					__NE___ {}
		MCommandBuffer (MCommandBuffer &&)								__NE___;
		template <typename T> MCommandBuffer (T enc)					__NE___	{ SetEncoder( enc ); }
		~MCommandBuffer ()												__NE___;
		
		MCommandBuffer&  operator = (MCommandBuffer && rhs)				__NE___;


	// queue //
		ND_ EQueueType				GetQueueType ()						C_NE___	{ return _queueType; }
		ND_ MQueuePtr				GetQueue ()							C_NE___;


	// encoder //
		ND_ bool					HasEncoder ()						C_NE___	{ return bool{_encoder}; }
		ND_ MetalCommandEncoder		GetEncoder ()						C_NE___	{ ASSERT( _IsInCurrentThread() );  return _encoder; }
		ND_ EncoderType				GetEncoderType ()					C_NE___	{ return _encType; }

		template <typename T>
		ND_ T						As ()								__NE___;
		ND_ auto					AsEncoder ()						__NE___;
		ND_ auto					AsComputeEncoder ()					__NE___;
		ND_ auto					AsRenderEncoder ()					__NE___;
		ND_ auto					AsParallelRenderEncoder ()			__NE___;
		ND_ auto					AsAccelStructEncoder ()				__NE___;
		ND_ auto					AsBlitEncoder ()					__NE___;
		ND_ auto					AsResStateEncoder ()				__NE___;
		
		ND_ bool					IsComputeEncoder ()					C_NE___	{ return _encType == EncoderType::Compute; }
		ND_ bool					IsRenderEncoder ()					C_NE___	{ return _encType == EncoderType::Render; }
		ND_ bool					IsParallelRenderEncoder ()			C_NE___	{ return _encType == EncoderType::ParallelRender; }
		ND_ bool					IsAccelStructEncoder ()				C_NE___	{ return _encType == EncoderType::AccelStruct; }
		ND_ bool					IsBlitEncoder ()					C_NE___	{ return _encType == EncoderType::Blit; }
		ND_ bool					IsResStateEncoder ()				C_NE___	{ return _encType == EncoderType::ResState; }
		
			template <typename T>
			bool  SetEncoder (T enc)									__NE___;
			void  SetEncoder (MetalCommandEncoderRC, EncoderType)		__NE___;
		ND_ bool  EndEncoding ()										__NE___;


	// command buffer //
		ND_ MetalCommandBufferRC	EndAndRelease ()					__NE___;
		ND_ bool					HasCmdBuf ()						C_NE___	{ return bool{_cmdbuf}; }
		ND_ MetalCommandBuffer		GetCmdBuf ()						C_NE___	{ ASSERT( _IsInCurrentThread() );  return _cmdbuf; }
		ND_ auto					AsCmdBuffer ()						__NE___;
		ND_ bool					IsRecording ()						C_NE___;
		ND_ ECommandBufferType		GetCommandBufferType ()				C_NE___;

		ND_ static MCommandBuffer	CreateCommandBuffer (EQueueType)	__NE___;
		ND_ static MCommandBuffer	CreateEmptyCmdBuffer (EQueueType)	__NE___;

		
		void  DebugMarker (DebugLabel dbg)								__NE___;
		void  PushDebugGroup (DebugLabel dbg)							__NE___;
		void  PopDebugGroup ()											__NE___;
		
		void  PipelineBarrier (const MDependencyInfo &info)				__NE___;

	private:
		DRC_ONLY( ND_ bool			_IsInCurrentThread ()				C_NE___	{ return _stCheck.Lock(); })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
