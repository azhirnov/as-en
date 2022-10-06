// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ASBuildCtx --> DirectASBuildCtx    --> BarrierMngr --> Vulkan device 
				\-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
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
	// Vulkan Direct AS Build Context implementation
	//
	
	class _VDirectASBuildCtx : public VBaseDirectContext
	{
	// methods
	public:
		void  Copy  (const VkCopyAccelerationStructureInfoKHR &info);
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectASBuildCtx (Ptr<VCommandBatch> batch) : VBaseDirectContext{ batch } {}
		_VDirectASBuildCtx (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf) : VBaseDirectContext{ batch, RVRef(cmdbuf) } {}
		
		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
	};



	//
	// Vulkan Indirect AS Build Context implementation
	//
	
	class _VIndirectASBuildCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  Copy  (const VkCopyAccelerationStructureInfoKHR &info);

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectASBuildCtx (Ptr<VCommandBatch> batch) : VBaseIndirectContext{ batch } {}
		_VIndirectASBuildCtx (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ batch, RVRef(cmdbuf) } {}

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
	};


	
	//
	// Vulkan AS Build Context implementation
	//

	template <typename CtxImpl>
	class _VASBuildContextImpl : public CtxImpl, public IASBuildContext
	{
	// types
	public:
		static constexpr bool	IsASBuildContext		= true;
		static constexpr bool	IsVulkanASBuildContext	= true;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VASBuildContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VASBuildContextImpl (Ptr<VCommandBatch> batch) : RawCtx{ batch } {}
		
		template <typename RawCmdBufType>
		_VASBuildContextImpl (Ptr<VCommandBatch> batch, RawCmdBufType&& cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_VASBuildContextImpl () = delete;
		_VASBuildContextImpl (const _VASBuildContextImpl &) = delete;

		using RawCtx::Copy;
		
		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst) override								{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) override				{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override;
		
		void  Build  (const RTSceneBuild &cmd, RTSceneID dst) override;
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) override;
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }

		ND_ AccumBar  AccumBarriers ()							{ return AccumBar{ *this }; }

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectASBuildContext		= _hidden_::_VASBuildContextImpl< _hidden_::_VDirectASBuildCtx >;
	using VIndirectASBuildContext	= _hidden_::_VASBuildContextImpl< _hidden_::_VIndirectASBuildCtx >;

} // AE::Graphics

#endif	// AE_ENABLE_VULKAN
