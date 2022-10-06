// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Vulkan/Commands/VRenderTaskScheduler.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"

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
		
		struct GenerateMipmapsCmd : BaseCmd
		{
			VkImage				image;
			uint3				dimension;
			uint				levelCount;
			uint				layerCount;
			EImageAspect		aspect;
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

		struct DrawMeshTasksNVCmd : BaseCmd
		{
			uint			taskCount;
			uint			firstTask;
		};

		struct DrawMeshTasksIndirectNVCmd : BaseCmd
		{
			VkBuffer		indirectBuffer;
			VkDeviceSize	indirectBufferOffset;
			uint			drawCount;
			uint			stride;
		};
		
		struct DrawMeshTasksIndirectCountNVCmd : BaseCmd
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
		
		//-------------------------------------------------
		// ray tracing commands

		struct TraceRaysCmd : BaseCmd
		{
		};

		struct TraceRaysIndirectCmd : BaseCmd
		{
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
			_visitor_( GenerateMipmapsCmd )\
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
			_visitor_( DrawMeshTasksNVCmd )\
			_visitor_( DrawMeshTasksIndirectNVCmd )\
			_visitor_( DrawMeshTasksIndirectCountNVCmd )\
			_visitor_( DispatchTileCmd )\
			/* acceleration structure build commands */\
			_visitor_( BuildASCmd )\
			_visitor_( CopyASCmd )\
			/* ray tracing commands */\
			_visitor_( TraceRaysCmd )\
			_visitor_( TraceRaysIndirectCmd )\

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
		VSoftwareCmdBuf ()	{}
		
		ND_ VBakedCommands	Bake ();

		template <typename CmdType, typename ...DynamicTypes>
		ND_ CmdType&  CreateCmd (usize dynamicArraySize = 0)	{ return SoftwareCmdBufBase::_CreateCmd< Commands_t, CmdType, DynamicTypes... >( dynamicArraySize ); }

		void  DebugMarker (NtStringView text, RGBA8u color);
		void  PushDebugGroup (NtStringView text, RGBA8u color);
		void  PopDebugGroup ();
		void  CommitBarriers (const VkDependencyInfoKHR &);
		
		void  BindDescriptorSet (VkPipelineBindPoint bindPoint, VkPipelineLayout layout, uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);
		void  BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout);
		void  PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void *values, EShaderStages stages);
		
		void  DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
		
		ND_ static bool  Execute (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, void* root);

	private:
		ND_ bool  _Validate (const void* root) const	{ return SoftwareCmdBufBase::_Validate( root, Commands_t::Count ); }
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
		virtual ~_VBaseIndirectContext ();

		ND_ bool				IsValid ()			const	{ return _cmdbuf and _cmdbuf->IsValid(); }
		ND_ VBakedCommands		EndCommandBuffer ();
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		explicit _VBaseIndirectContext (VSoftwareCmdBufPtr cmdbuf) : _cmdbuf{RVRef(cmdbuf)} {}

		explicit _VBaseIndirectContext (NtStringView dbgName);
		_VBaseIndirectContext (NtStringView dbgName, VSoftwareCmdBufPtr cmdbuf);

		void  _DebugMarker (NtStringView text, RGBA8u color)		{ _cmdbuf->DebugMarker( text, color ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u color)		{ _cmdbuf->PushDebugGroup( text, color ); }
		void  _PopDebugGroup ()										{ _cmdbuf->PopDebugGroup(); }

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)	{ _cmdbuf->DbgFillBuffer( buffer, offset, size, data ); }
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
		explicit VBaseIndirectContext (Ptr<VCommandBatch> batch);
		VBaseIndirectContext (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf);
		~VBaseIndirectContext () override;
		
		ND_ VBakedCommands		EndCommandBuffer ()			{ ASSERT( _NoPendingBarriers() );  return _VBaseIndirectContext::EndCommandBuffer(); }
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _VBaseIndirectContext::ReleaseCommandBuffer(); }

	protected:
		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		const	{ return _mngr.GetDevice().GetExtensions(); }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
