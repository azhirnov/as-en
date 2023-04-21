// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	VideoDecodeCtx	--> DirectVideoDecodeCtx   --> BarrierMngr --> Vulkan device 
					\-> IndirectVideoDecodeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Video Decode Context implementation
	//
	
	class _VDirectVideoDecodeCtx : public VBaseDirectContext
	{
	// methods
	public:
		void  Decode (const VkVideoDecodeInfoKHR &info)		__Th___;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()				__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()			__Th___;
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectVideoDecodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task)					__Th___;
		_VDirectVideoDecodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task, VCommandBuffer cmdbuf)	__Th___;
	};



	//
	// Vulkan Indirect Video Decode Context implementation
	//
	
	class _VIndirectVideoDecodeCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  Decode (const VkVideoDecodeInfoKHR &info)		__Th___;
		
		ND_ VBakedCommands		EndCommandBuffer ()			__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()		__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectVideoDecodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task)					__Th___;
		_VIndirectVideoDecodeCtx (const VkVideoBeginCodingInfoKHR &, const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)	__Th___;
	};


	
	//
	// Vulkan Video Decode Context implementation
	//

	template <typename CtxImpl>
	class _VVideoDecodeContextImpl : public CtxImpl, public IASBuildContext
	{
	// types
	public:
		static constexpr bool	IsVideoDecodeContext		= true;
		static constexpr bool	IsVulkanVideoDecodeContext	= true;

		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= VAccumBarriers< _VVideoDecodeContextImpl< CtxImpl >>;
		using DeferredBar	= VAccumDeferredBarriersForCtx< _VVideoDecodeContextImpl< CtxImpl >>;
		using Validator_t	= VideoDecodeContextValidation;


	// methods
	public:
		explicit _VVideoDecodeContextImpl (VideoSessionID, const RenderTask &task)			__Th___;
		_VVideoDecodeContextImpl (VideoSessionID, const RenderTask &task, CmdBuf_t cmdbuf)	__Th___;

		_VVideoDecodeContextImpl ()															= delete;
		_VVideoDecodeContextImpl (const _VVideoDecodeContextImpl &)							= delete;
		
		void  Decode (const VideoDecodeCmd &)												__Th___;

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectVideoDecodeContext		= Graphics::_hidden_::_VVideoDecodeContextImpl< Graphics::_hidden_::_VDirectVideoDecodeCtx >;
	using VIndirectVideoDecodeContext	= Graphics::_hidden_::_VVideoDecodeContextImpl< Graphics::_hidden_::_VIndirectVideoDecodeCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VVideoDecodeContextImpl<C>::_VVideoDecodeContextImpl (VideoSessionID sessionId, const RenderTask &task) __Th___ :
		_VVideoDecodeContextImpl{ task, Default }
	{}
	
	template <typename C>
	_VVideoDecodeContextImpl<C>::_VVideoDecodeContextImpl (VideoSessionID sessionId, const RenderTask &task, CmdBuf_t cmdbuf) __Th___ :
		RawCtx{ task, RVRef(cmdbuf) }
	{
		CHECK_THROW( AnyBits( EQueueMask::VideoDecode, task.GetQueueMask() ));

		auto&	session = _GetResourcesOrThrow( sessionId );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
