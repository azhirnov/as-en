// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"
# include "graphics/Vulkan/Commands/VAccumDeferredBarriers.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Vulkan software Command Buffer
	//

	class VSoftwareCmdBuf final : public SoftwareCmdBufBase
	{
	// types
	public:
		struct DebugMarkerCmd : BaseCmd
		{
			RGBA8u		color;
			//char		text[];
		};
		
		struct PushDebugGroupCmd : BaseCmd
		{
			RGBA8u		color;
			//char		text[];
		};
		
		struct PopDebugGroupCmd : BaseCmd
		{
		};
		
		struct PipelineBarrierCmd : BaseCmd
		{
			ushort						dependencyFlags;		// VkDependencyFlags
			ushort						memoryBarrierCount;
			ushort						bufferBarrierCount;
			ushort						imageBarrierCount;
			//VkMemoryBarrier2			memoryBarriers[];
			//VkBufferMemoryBarrier2	bufferBarriers[];
			//VkImageMemoryBarrier2		imageBarriers[];
		};
		
		struct BindDescriptorSetCmd : BaseCmd
		{
			VkPipelineLayout	layout;
			VkDescriptorSet		destSet;
			VkPipelineBindPoint	bindPoint;
			ushort				index;
			ushort				dynamicOffsetCount;
			//uint				dynamicOffsets[];
		};
		
		struct BindPipelineCmd : BaseCmd
		{
			VkPipeline			pipeline;
			VkPipelineLayout	layout;
			VkPipelineBindPoint	bindPoint;
		};

		struct PushConstantCmd : BaseCmd
		{
			VkPipelineLayout	layout;
			uint				offset;
			uint				size;
			VkShaderStageFlags	stages;
			//ubyte				data[];
		};

		struct ProfilerBeginContextCmd : BaseCmd
		{
			const void*							batch;
			IGraphicsProfiler*					prof;
			IGraphicsProfiler::EContextType		type;
			RGBA8u								color;
			//char								taskName[];
		};

		struct ProfilerEndContextCmd : BaseCmd
		{
			const void*							batch;
			IGraphicsProfiler*					prof;
			IGraphicsProfiler::EContextType		type;
			bool								end;	// to make unique type
		};

		//-------------------------------------------------
		// transfer commands

		struct ClearColorImageCmd : BaseCmd
		{
			VkImage						image;
			VkImageLayout				layout;
			VkClearColorValue			color;
			uint						rangeCount;
			//VkImageSubresourceRange	ranges[];
		};
		
		struct ClearDepthStencilImageCmd : BaseCmd
		{
			VkImage						image;
			VkImageLayout				layout;
			VkClearDepthStencilValue	depthStencil;
			uint						rangeCount;
			//VkImageSubresourceRange	ranges[];
		};
		
		struct FillBufferCmd : BaseCmd
		{
			VkBuffer			buffer;
			Bytes				offset;
			Bytes				size;
			uint				data;
		};
		
		struct UpdateBufferCmd : BaseCmd
		{
			VkBuffer			buffer;
			Bytes				offset;
			Bytes32u			size;
			//ubyte				data[];
		};
		
		struct CopyBufferCmd : BaseCmd
		{
			VkBuffer			srcBuffer;
			VkBuffer			dstBuffer;
			uint				regionCount;
			//VkBufferCopy		regions[];
		};
		
		struct CopyImageCmd : BaseCmd
		{
			VkImage				srcImage;
			VkImageLayout		srcLayout;
			VkImage				dstImage;
			VkImageLayout		dstLayout;
			uint				regionCount;
			//VkImageCopy		regions[];
		};
		
		struct CopyBufferToImageCmd : BaseCmd
		{
			VkBuffer			srcBuffer;
			VkImage				dstImage;
			VkImageLayout		dstLayout;
			uint				regionCount;
			//VkBufferImageCopy	regions[];
		};
		
		struct CopyImageToBufferCmd : BaseCmd
		{
			VkImage				srcImage;
			VkImageLayout		srcLayout;
			VkBuffer			dstBuffer;
			uint				regionCount;
			//VkBufferImageCopy	regions[];
		};

		struct BlitImageCmd : BaseCmd
		{
			VkImage				srcImage;
			VkImageLayout		srcLayout;
			VkImage				dstImage;
			VkImageLayout		dstLayout;
			VkFilter			filter;
			uint				regionCount;
			//VkImageBlit		regions[];
		};
		
		struct ResolveImageCmd : BaseCmd
		{
			VkImage				srcImage;
			VkImageLayout		srcLayout;
			VkImage				dstImage;
			VkImageLayout		dstLayout;
			uint				regionCount;
			//VkImageResolve	regions[];
		};

		struct GenerateMipmapsCmd : BaseCmd
		{
			VkImage				image;
			packed_uint3		dimension;
			uint				levelCount;
			uint				layerCount;
			EImageAspect		aspect;
		};
		
		struct CopyQueryPoolResultsCmd : BaseCmd
		{
			VkQueryPool			srcPool;
			uint				srcIndex;
			uint				srcCount;
			VkBuffer			dstBuffer;
			Bytes				dstOffset;
			Bytes32u			stride;
			VkQueryResultFlags	flags;
		};

		//-------------------------------------------------
		// compute commands

		struct DispatchCmd : BaseCmd
		{
			uint		groupCount[3];
		};

		struct DispatchBaseCmd : BaseCmd
		{
			uint		baseGroup[3];
			uint		groupCount[3];
		};

		struct DispatchIndirectCmd : BaseCmd
		{
			VkBuffer	buffer;
			Bytes		offset;
		};
		
		//-------------------------------------------------
		// graphics commands

		struct BeginRenderPassCmd : BaseCmd
		{
			// VkSubpassBeginInfo
			VkSubpassContents	contents;
			// VkRenderPassBeginInfo
			VkRenderPass		renderPass;
			VkFramebuffer		framebuffer;
			VkRect2D			renderArea;
			uint				clearValueCount;
			//VkClearValue		clearValues[];
		};

		struct NextSubpassCmd : BaseCmd
		{
			// VkSubpassBeginInfo
			VkSubpassContents	contents;
			// VkSubpassEndInfo
		};

		struct EndRenderPassCmd : BaseCmd
		{
			// VkSubpassEndInfo
		};

		struct ExecuteCommandsCmd : BaseCmd
		{
			uint				count;
			// VkCommandBuffer	commands[];
		};
		
		//-------------------------------------------------
		// draw commands
		
		struct SetViewportCmd : BaseCmd
		{
			ushort			first;
			ushort			count;
			//VkViewport	viewports[];
		};

		struct SetScissorCmd : BaseCmd
		{
			ushort		first;
			ushort		count;
			//VkRect2D	scissors[];
		};

		struct SetDepthBiasCmd : BaseCmd
		{
			float		depthBiasConstantFactor;
			float		depthBiasClamp;
			float		depthBiasSlopeFactor;
		};

		struct SetDepthBoundsCmd : BaseCmd
		{
			float		minDepthBounds;
			float		maxDepthBounds;
		};

		struct SetStencilCompareMaskCmd : BaseCmd
		{
			VkStencilFaceFlagBits	faceMask;
			uint					compareMask;
		};

		struct SetStencilWriteMaskCmd : BaseCmd
		{
			VkStencilFaceFlagBits	faceMask;
			uint					writeMask;
		};

		struct SetStencilReferenceCmd : BaseCmd
		{
			VkStencilFaceFlagBits	faceMask;
			uint					reference;
		};

		struct SetBlendConstantsCmd : BaseCmd
		{
			RGBA32f			color;
		};

		struct BindIndexBufferCmd : BaseCmd
		{
			VkBuffer		buffer;
			VkDeviceSize	offset;
			VkIndexType		indexType;
		};

		struct BindVertexBuffersCmd : BaseCmd
		{
			ushort			firstBinding;
			ushort			count;
			//VkBuffer		buffers[];
			//VkDeviceSize	offsets[];
		};

		struct DrawCmd : BaseCmd
		{
			uint			vertexCount;
			uint			instanceCount;
			uint			firstVertex;
			uint			firstInstance;
		};

		struct DrawIndexedCmd : BaseCmd
		{
			uint			indexCount;
			uint			instanceCount;
			uint			firstIndex;
			int				vertexOffset;
			uint			firstInstance;
		};

		struct DrawIndirectCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			uint			drawCount;
			uint			stride;
		};
		
		struct DrawIndexedIndirectCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			uint			drawCount;
			uint			stride;
		};
		
		struct DrawIndirectCountCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			VkBuffer		countBuffer;
			VkDeviceSize	countBufferOffset;
			uint			maxDrawCount;
			uint			stride;
		};

		struct DrawIndexedIndirectCountCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			VkBuffer		countBuffer;
			VkDeviceSize	countBufferOffset;
			uint			maxDrawCount;
			uint			stride;
		};

		struct DrawMeshTasksCmd : BaseCmd
		{
			packed_uint3	taskCount;
		};

		struct DrawMeshTasksIndirectCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			uint			drawCount;
			uint			stride;
		};
		
		struct DrawMeshTasksIndirectCountCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			VkBuffer		countBuffer;
			VkDeviceSize	countBufferOffset;
			uint			maxDrawCount;
			uint			stride;
		};

		struct DispatchTileCmd : BaseCmd
		{};
		
		//-------------------------------------------------
		// acceleration structure build commands
		
		struct BuildASCmd : BaseCmd
		{
			VkAccelerationStructureBuildGeometryInfoKHR			info;			// ppGeometries - used frame allocator
			VkAccelerationStructureBuildRangeInfoKHR const*		pRangeInfos;	// used frame allocator
		};

		struct CopyASCmd : BaseCmd
		{
			VkAccelerationStructureKHR			src;
			VkAccelerationStructureKHR			dst;
			VkCopyAccelerationStructureModeKHR	mode;
		};

		struct CopyASToMemoryCmd : BaseCmd
		{
			VkAccelerationStructureKHR			src;
			VkDeviceAddress						dst;
			VkCopyAccelerationStructureModeKHR	mode;
		};

		struct CopyMemoryToASCmd : BaseCmd
		{
			VkDeviceAddress						src;
			VkAccelerationStructureKHR			dst;
			VkCopyAccelerationStructureModeKHR	mode;
		};

		struct WriteASPropertiesCmd : BaseCmd
		{
			VkAccelerationStructureKHR			as;
			VkQueryPool							pool;
			uint								index;
			VkQueryType							type;
		};
		
		//-------------------------------------------------
		// ray tracing commands

		struct TraceRaysCmd : BaseCmd
		{
			VkStridedDeviceAddressRegionKHR		raygen;
			VkStridedDeviceAddressRegionKHR		miss;
			VkStridedDeviceAddressRegionKHR		hit;
			VkStridedDeviceAddressRegionKHR		callable;
			packed_uint3						dim;
		};

		struct TraceRaysIndirectCmd : BaseCmd
		{
			VkStridedDeviceAddressRegionKHR		raygen;
			VkStridedDeviceAddressRegionKHR		miss;
			VkStridedDeviceAddressRegionKHR		hit;
			VkStridedDeviceAddressRegionKHR		callable;
			VkDeviceAddress						indirectDeviceAddress;
		};
		
		struct TraceRaysIndirect2Cmd : BaseCmd
		{
			VkDeviceAddress						indirectDeviceAddress;
		};

		struct RayTracingSetStackSizeCmd : BaseCmd
		{
			uint		size;
		};


	private:
		#define AE_BASE_IND_CTX_COMMANDS( _visitor_ ) \
			/* shared commands */\
			_visitor_( DebugMarkerCmd )\
			_visitor_( PushDebugGroupCmd )\
			_visitor_( PopDebugGroupCmd )\
			_visitor_( PipelineBarrierCmd )\
			_visitor_( BindDescriptorSetCmd )\
			_visitor_( BindPipelineCmd )\
			_visitor_( PushConstantCmd )\
			_visitor_( ProfilerBeginContextCmd )\
			_visitor_( ProfilerEndContextCmd )\
			/* transfer commands */\
			_visitor_( ClearColorImageCmd )\
			_visitor_( ClearDepthStencilImageCmd )\
			_visitor_( FillBufferCmd )\
			_visitor_( UpdateBufferCmd )\
			_visitor_( CopyBufferCmd )\
			_visitor_( CopyImageCmd )\
			_visitor_( CopyBufferToImageCmd )\
			_visitor_( CopyImageToBufferCmd )\
			_visitor_( BlitImageCmd )\
			_visitor_( ResolveImageCmd )\
			_visitor_( GenerateMipmapsCmd )\
			_visitor_( CopyQueryPoolResultsCmd )\
			/* compute commands */\
			_visitor_( DispatchCmd )\
			_visitor_( DispatchBaseCmd )\
			_visitor_( DispatchIndirectCmd )\
			/* graphics commands */\
			_visitor_( BeginRenderPassCmd )\
			_visitor_( NextSubpassCmd )\
			_visitor_( EndRenderPassCmd )\
			_visitor_( ExecuteCommandsCmd )\
			/* draw commands */\
			_visitor_( SetViewportCmd )\
			_visitor_( SetScissorCmd )\
			_visitor_( SetDepthBiasCmd )\
			_visitor_( SetDepthBoundsCmd )\
			_visitor_( SetStencilCompareMaskCmd )\
			_visitor_( SetStencilWriteMaskCmd )\
			_visitor_( SetStencilReferenceCmd )\
			_visitor_( SetBlendConstantsCmd )\
			_visitor_( BindIndexBufferCmd )\
			_visitor_( BindVertexBuffersCmd )\
			_visitor_( DrawCmd )\
			_visitor_( DrawIndexedCmd )\
			_visitor_( DrawIndirectCmd )\
			_visitor_( DrawIndexedIndirectCmd )\
			_visitor_( DrawIndirectCountCmd )\
			_visitor_( DrawIndexedIndirectCountCmd )\
			_visitor_( DrawMeshTasksCmd )\
			_visitor_( DrawMeshTasksIndirectCmd )\
			_visitor_( DrawMeshTasksIndirectCountCmd )\
			_visitor_( DispatchTileCmd )\
			/* acceleration structure build commands */\
			_visitor_( BuildASCmd )\
			_visitor_( CopyASCmd )\
			_visitor_( CopyASToMemoryCmd )\
			_visitor_( CopyMemoryToASCmd )\
			_visitor_( WriteASPropertiesCmd )\
			/* ray tracing commands */\
			_visitor_( TraceRaysCmd )\
			_visitor_( TraceRaysIndirectCmd )\
			_visitor_( TraceRaysIndirect2Cmd )\
			_visitor_( RayTracingSetStackSizeCmd )\

		using Commands_t	= TypeList<
				#define AE_BASE_IND_CTX_VISIT( _name_ )		_name_,
				AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
				#undef AE_BASE_IND_CTX_VISIT
				void
			>::PopBack::type;

		using CmdContent_t	= TypeList<
				// shared commands
				uint, VkMemoryBarrier2, VkBufferMemoryBarrier2, VkImageMemoryBarrier2,
				// transfer commands 
				VkImageSubresourceRange, VkBufferCopy, VkImageCopy, VkBufferImageCopy, VkImageBlit,
				// compute commands
				// graphics commands
				VkCommandBuffer,
				// draw commands
				VkRect2D, VkViewport, VkBuffer, VkDeviceSize,
				// acceleration structure build commands
				VkAccelerationStructureBuildRangeInfoKHR, VkAccelerationStructureGeometryKHR
				// ray tracing commands
			>;

	private:
		struct CmdProcessor;


	// methods
	public:
		VSoftwareCmdBuf ()										__NE___	{}
		
		ND_ VBakedCommands	Bake ()								__NE___;

		template <typename CmdType, typename ...DynamicTypes>
		ND_ CmdType&  CreateCmd (usize dynamicArraySize = 0)	__Th___	{ return SoftwareCmdBufBase::_CreateCmd< Commands_t, CmdType, DynamicTypes... >( dynamicArraySize ); }

		void  DebugMarker (DebugLabel dbg)						__Th___;
		void  PushDebugGroup (DebugLabel dbg)					__Th___;
		void  PopDebugGroup ()									__Th___;
		void  CommitBarriers (const VkDependencyInfo &)			__Th___;
		
		void  BindDescriptorSet (VkPipelineBindPoint bindPoint, VkPipelineLayout layout, uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)	__Th___;
		void  BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout)																__Th___;
		void  PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void *values, EShaderStages stages)											__Th___;
		
		void  ProfilerBeginContext (IGraphicsProfiler* prof, const void* batch, StringView taskName, RGBA8u color, IGraphicsProfiler::EContextType type)			__Th___;
		void  ProfilerEndContext (IGraphicsProfiler* prof, const void* batch, IGraphicsProfiler::EContextType type)													__Th___;

		void  DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)																					__Th___;
		
		ND_ static bool  Execute (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, void* root)																			__NE___;

	private:
		ND_ bool  _Validate (const void* root)					C_NE___	{ return SoftwareCmdBufBase::_Validate( root, Commands_t::Count ); }
	};

	using VSoftwareCmdBufPtr = Unique< VSoftwareCmdBuf >;
	

	
	//
	// Vulkan Indirect Context base class
	//

	class _VBaseIndirectContext
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = true;
		
	protected:
		#define AE_BASE_IND_CTX_VISIT( _name_ )		using _name_ = VSoftwareCmdBuf::_name_;
		AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
		#undef AE_BASE_IND_CTX_VISIT


	// variables
	protected:
		VSoftwareCmdBufPtr	_cmdbuf;


	// methods
	public:
		virtual ~_VBaseIndirectContext ()												__NE___	{ DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }
		
		void  PipelineBarrier (const VkDependencyInfo &info)							__Th___	{ _cmdbuf->CommitBarriers( info ); }

	protected:
		explicit _VBaseIndirectContext (VSoftwareCmdBufPtr cmdbuf)						__NE___	: _cmdbuf{RVRef(cmdbuf)} {}

		explicit _VBaseIndirectContext (DebugLabel dbg)									__Th___;
		_VBaseIndirectContext (DebugLabel dbg, VSoftwareCmdBufPtr cmdbuf)				__Th___;

		ND_ bool	_IsValid ()															C_NE___	{ return _cmdbuf and _cmdbuf->IsValid(); }

		void  _DebugMarker (DebugLabel dbg)												__Th___	{ _cmdbuf->DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)											__Th___	{ _cmdbuf->PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()															__Th___	{ _cmdbuf->PopDebugGroup(); }

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)		__Th___	{ _cmdbuf->DbgFillBuffer( buffer, offset, size, data ); }

		ND_ VBakedCommands		_EndCommandBuffer ()									__Th___;
		ND_ VSoftwareCmdBufPtr  _ReleaseCommandBuffer ()								__Th___;

		ND_ static VSoftwareCmdBufPtr  _ReuseOrCreateCommandBuffer (VSoftwareCmdBufPtr cmdbuf) __Th___;
	};



	//
	// Vulkan Indirect Context base class with barrier manager
	//

	class VBaseIndirectContext : public _VBaseIndirectContext
	{
	// variables
	protected:
		VBarrierManager		_mngr;
		

	// methods
	public:
		explicit VBaseIndirectContext (const RenderTask &task)						__Th___;
		VBaseIndirectContext (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)	__Th___;
		~VBaseIndirectContext ()													__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
			void	_CommitBarriers ()												__Th___;

		ND_ bool	_NoPendingBarriers ()											C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()												C_NE___	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()													C_NE___	{ return _mngr.GetDevice().GetProperties().features; }
			
		ND_ VBakedCommands		_EndCommandBuffer ()								__Th___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline VBaseIndirectContext::VBaseIndirectContext (const RenderTask &task) __Th___ :
		_VBaseIndirectContext{ DebugLabel{ task.DbgFullName(), task.DbgColor() }},	// throw
		_mngr{ task }
	{
		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
	}
		
	inline VBaseIndirectContext::VBaseIndirectContext (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf) __Th___ :
		_VBaseIndirectContext{ _ReuseOrCreateCommandBuffer( RVRef(cmdbuf) )},
		_mngr{ task }
	{
		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
	}
		
/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  VBaseIndirectContext::_CommitBarriers () __Th___
	{
		auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			_cmdbuf->CommitBarriers( *bar );	// throw
			_mngr.ClearBarriers();
		}
	}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline VBakedCommands  VBaseIndirectContext::_EndCommandBuffer () __Th___
	{
		if ( auto* bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
			PipelineBarrier( *bar );

		return _VBaseIndirectContext::_EndCommandBuffer();
	}

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	Execute
----
	'cmdbuf' must be in the recording state
=================================================
*/
	forceinline bool  VBakedCommands::Execute (VulkanDeviceFn fn, VkCommandBuffer cmdbuf) C_NE___
	{
		return Graphics::_hidden_::VSoftwareCmdBuf::Execute( fn, cmdbuf, _root );
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
