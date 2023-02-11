// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Metal/Commands/MBarrierManager.h"
# include "graphics/Metal/Commands/MAccumDeferredBarriers.h"
# include "graphics/Metal/MRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Metal software Command Buffer
	//

	class MSoftwareCmdBuf final : public SoftwareCmdBufBase
	{
	// types
	public:
		using Viewport_t = RenderPassDesc::Viewport;

		struct DebugMarkerCmd : BaseCmd
		{
			//char		text [];
		};
		
		struct PushDebugGroupCmd : BaseCmd
		{
			//char		text [];
		};
		
		struct PopDebugGroupCmd : BaseCmd
		{};

		struct MemoryBarrierCmd : BaseCmd
		{
			// TODO
		};
		
		struct SetBufferCmd : BaseCmd
		{
			EShader			shaderType;
			MBufferIndex	index;
			MetalBuffer		buffer;
			Bytes			offset;
		};

		struct SetBufferOffsetCmd : BaseCmd
		{
			EShader			shaderType;
			MBufferIndex	index;
			Bytes			offset;
		};

		struct SetBytesCmd : BaseCmd
		{
			EShader			shaderType;
			Bytes16u		dataSize;
			MBufferIndex	index;
		};

		struct SetSamplerCmd : BaseCmd
		{
			EShader			shaderType;
			MSamplerIndex	index;
			MetalSampler	sampler;
		};

		struct SetSampler2Cmd : BaseCmd
		{
			EShader			shaderType;
			MSamplerIndex	index;
			MetalSampler	sampler;
			float			lodMinClamp;
			float			lodMaxClamp;
		};

		struct SetTextureCmd : BaseCmd
		{
			EShader			shaderType;
			MTextureIndex	index;
			MetalImage		texture;
		};

		struct SetVisibleFunctionTableCmd : BaseCmd
		{
			EShader					shaderType;
			MBufferIndex			index;
			MetalVisibleFnTable		table;
		};

		struct SetIntersectionFunctionTableCmd : BaseCmd
		{
			EShader						shaderType;
			MBufferIndex				index;
			MetalIntersectionFnTable	table;
		};

		struct SetAccelerationStructureCmd : BaseCmd
		{
			EShader				shaderType;
			MBufferIndex		index;
			MetalAccelStruct	as;
		};

		struct SetThreadgroupMemoryLengthCmd : BaseCmd
		{
			EShader				shaderType;
			MThreadgroupIndex	index;
			Bytes16u			length;
			Bytes16u			offset;
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
		
		struct BeginTransferCommandsCmd : BaseCmd
		{};

		struct FillBufferCmd : BaseCmd
		{
			MetalBuffer		buffer;
			Bytes			offset;
			Bytes			size;
			uint			data;
		};

		struct CopyBufferCmd : BaseCmd
		{
			MetalBuffer		srcBuffer;
			MetalBuffer		dstBuffer;
			uint			rangesCount;
			//BufferCopy	ranges []
		};

		struct CopyImageCmd : BaseCmd
		{
			MetalImage		srcImage;
			MetalImage		dstImage;
			uint			rangesCount;
			//ImageCopy		ranges []
		};

		struct CopyBufferToImageCmd : BaseCmd
		{
			MetalBuffer			srcBuffer;
			MetalImage			dstImage;
			uint				rangesCount;
			//BufferImageCopy2	ranges []
		};

		struct CopyImageToBufferCmd : BaseCmd
		{
			MetalImage			srcImage;
			MetalBuffer			dstBuffer;
			uint				rangesCount;
			//BufferImageCopy2	ranges []
		};

		struct GenerateMipmapsCmd : BaseCmd
		{
			MetalImage		image;
		};

		struct SynchronizeResourceCmd : BaseCmd
		{
			MetalResource	resource;
		};

		//-------------------------------------------------
		// compute commands
		
		struct BeginComputeCommandsCmd : BaseCmd
		{};

		struct DispatchThreadgroupsCmd : BaseCmd
		{
			packed_uint3	threadgroupsPerGrid;
			packed_uint3	threadsPerThreadgroup;
		};

		struct DispatchThreadsCmd : BaseCmd
		{
			packed_uint3	threadsPerGrid;
			packed_uint3	threadsPerThreadgroup;
		};

		struct DispatchThreadgroupsIndirectCmd : BaseCmd
		{
			MetalBuffer		indirectBuffer;
			Bytes			offset;
			packed_uint3	threadsPerThreadgroup;
		};

		struct SetImageblockCmd : BaseCmd
		{
			packed_uint2	dimInPix;
		};

		struct BindComputePipelineCmd : BaseCmd
		{
			MetalComputePipeline	ppln;
		};

		//-------------------------------------------------
		// graphics commands
		
		struct BeginRenderPassCmd : BaseCmd
		{
			MetalRenderPassDescRC	desc;		// TODO: RC or per frame autorelease
		};
		
		struct EndRenderPassCmd : BaseCmd
		{};

		//-------------------------------------------------
		// draw commands
		
		struct BindGraphicsPipelineCmd : BaseCmd
		{
			MetalRenderPipeline		ppln;
		};
		
		struct SetDynamicRenderStateCmd : BaseCmd
		{
			MDynamicRenderState		value;
		};

		struct SetDepthStencilStateCmd : BaseCmd
		{
			MetalDepthStencilState	value;
		};

		struct SetPolygonModeCmd : BaseCmd
		{
			EPolygonMode	value;
		};

		struct SetCullModeCmd : BaseCmd
		{
			ECullMode		value;
		};

		struct SetFrontFacingCmd : BaseCmd
		{
			bool			ccw;
		};

		struct SetDepthClampCmd : BaseCmd
		{
			bool			value;
		};

		struct SetTessellationFactorCmd : BaseCmd
		{
			MetalBuffer		buffer;
			Bytes			offset;
			Bytes			instanceStride;
		};

		struct SetTessellationFactorScaleCmd : BaseCmd
		{
			float			value;
		};

		struct SetVertexAmplificationCountCmd : BaseCmd
		{
			uint			count;
		};
		
		//struct SetVertexAmplificationMappingCmd : BaseCmd
		//{
		//	VertexAmplificationViewMapping	mapping;
		//};

		struct SetDepthBiasCmd : BaseCmd
		{
			float			depthBiasConstantFactor;
			float			depthBiasClamp;
			float			depthBiasSlopeFactor;
		};

		struct SetStencilReferenceCmd : BaseCmd
		{
			uint			frontReference;
			uint			backReference;
		};

		struct SetViewportCmd : BaseCmd
		{
			uint			count;
			//Viewport_t	viewports []
		};

		struct SetScissorCmd : BaseCmd
		{
			uint			count;
			//RectI			scissors []
		};

		struct SetBlendConstantsCmd : BaseCmd
		{
			RGBA32f			color;
		};

		struct DrawPrimitivesCmd : BaseCmd
		{
			uint			primitiveType;
			uint			firstVertex;
			uint			vertexCount;
			uint			instanceCount;
			uint			firstInstance;
		};
		
		struct DrawIndexedPrimitivesCmd : BaseCmd
		{
			ushort			primitiveType;
			ushort			indexType;
			uint			indexCount;
			MetalBuffer		indexBuffer;
			Bytes			indexBufferOffset;
			uint			instanceCount;
			uint			vertexOffset;
			uint			firstInstance;
		};
		
		struct DrawPrimitivesIndirectCmd : BaseCmd
		{
			MetalBuffer		indirectBuffer;
			Bytes			indirectBufferOffset;
			uint			drawCount;
			uint			primitiveType;
			Bytes			stride;
		};
		
		struct DrawIndexedPrimitivesIndirectCmd : BaseCmd
		{
			ushort			primitiveType;
			ushort			indexType;
			uint			drawCount;
			MetalBuffer		indexBuffer;
			Bytes			indexBufferOffset;
			MetalBuffer		indirectBuffer;
			Bytes			indirectBufferOffset;
			Bytes			stride;
		};

		struct DrawMeshThreadgroupsCmd : BaseCmd
		{
			packed_uint3	threadgroupsPerGrid;
			packed_uint3	threadsPerObjectThreadgroup;
			packed_uint3	threadsPerMeshThreadgroup;
		};
		
		struct DrawMeshThreadsCmd : BaseCmd
		{
			packed_uint3	threadsPerGrid;
			packed_uint3	threadsPerObjectThreadgroup;
			packed_uint3	threadsPerMeshThreadgroup;
		};
		
		struct DrawMeshThreadgroupsIndirectCmd : BaseCmd
		{
			MetalBuffer		indirectBuffer;
			Bytes			indirectBufferOffset;
			packed_uint3	threadsPerObjectThreadgroup;
			packed_uint3	threadsPerMeshThreadgroup;
			uint			drawCount;
			Bytes32u		stride;
		};

		struct DispatchThreadsPerTileCmd : BaseCmd
		{
			packed_uint2	threadsPerTile;
		};

		//-------------------------------------------------
		// acceleration structure build commands
		
		struct BeginAccelStructBuildCommandsCmd : BaseCmd
		{};
		
		struct WriteCompactedSizeCmd : BaseCmd
		{
			MetalAccelStruct 	as;
			MetalBuffer 		dstBuffer;
			Bytes 				offset;
		};

		//-------------------------------------------------
		// ray tracing commands


	private:
		#define AE_BASE_IND_CTX_COMMANDS( _visitor_ )\
			/* shared commands */\
			_visitor_( DebugMarkerCmd )\
			_visitor_( PushDebugGroupCmd )\
			_visitor_( PopDebugGroupCmd )\
			_visitor_( MemoryBarrierCmd )\
			_visitor_( SetBufferCmd )\
			_visitor_( SetBufferOffsetCmd )\
			_visitor_( SetBytesCmd )\
			_visitor_( SetSamplerCmd )\
			_visitor_( SetSampler2Cmd )\
			_visitor_( SetTextureCmd )\
			_visitor_( SetVisibleFunctionTableCmd )\
			_visitor_( SetIntersectionFunctionTableCmd )\
			_visitor_( SetAccelerationStructureCmd )\
			_visitor_( SetThreadgroupMemoryLengthCmd )\
			_visitor_( ProfilerBeginContextCmd )\
			_visitor_( ProfilerEndContextCmd )\
			/* transfer commands */\
			_visitor_( BeginTransferCommandsCmd )\
			_visitor_( FillBufferCmd )\
			_visitor_( CopyBufferCmd )\
			_visitor_( CopyImageCmd )\
			_visitor_( CopyBufferToImageCmd )\
			_visitor_( CopyImageToBufferCmd )\
			_visitor_( GenerateMipmapsCmd )\
			_visitor_( SynchronizeResourceCmd )\
			/* compute commands */\
			_visitor_( BeginComputeCommandsCmd )\
			_visitor_( DispatchThreadgroupsCmd )\
			_visitor_( DispatchThreadsCmd )\
			_visitor_( DispatchThreadgroupsIndirectCmd )\
			_visitor_( SetImageblockCmd )\
			_visitor_( BindComputePipelineCmd )\
			/* graphics commands */\
			_visitor_( BeginRenderPassCmd )\
			_visitor_( EndRenderPassCmd )\
			/* draw commands */\
			_visitor_( BindGraphicsPipelineCmd )\
			_visitor_( SetDynamicRenderStateCmd )\
			_visitor_( SetDepthStencilStateCmd )\
			_visitor_( SetPolygonModeCmd )\
			_visitor_( SetCullModeCmd )\
			_visitor_( SetFrontFacingCmd )\
			_visitor_( SetDepthClampCmd )\
			_visitor_( SetTessellationFactorCmd )\
			_visitor_( SetTessellationFactorScaleCmd )\
			_visitor_( SetVertexAmplificationCountCmd )\
			/*_visitor_( SetVertexAmplificationMappingCmd )*/\
			_visitor_( SetDepthBiasCmd )\
			_visitor_( SetStencilReferenceCmd )\
			_visitor_( SetViewportCmd )\
			_visitor_( SetScissorCmd )\
			_visitor_( SetBlendConstantsCmd )\
			_visitor_( DrawPrimitivesCmd )\
			_visitor_( DrawIndexedPrimitivesCmd )\
			_visitor_( DrawPrimitivesIndirectCmd )\
			_visitor_( DrawIndexedPrimitivesIndirectCmd )\
			_visitor_( DrawMeshThreadgroupsCmd )\
			_visitor_( DrawMeshThreadsCmd )\
			_visitor_( DrawMeshThreadgroupsIndirectCmd )\
			_visitor_( DispatchThreadsPerTileCmd )\
			/* acceleration structure build commands */\
			_visitor_( BeginAccelStructBuildCommandsCmd )\
			_visitor_( WriteCompactedSizeCmd )\
		
		using Commands_t	= TypeList<
				#define AE_BASE_IND_CTX_VISIT( _name_ )		_name_,
				AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
				#undef AE_BASE_IND_CTX_VISIT
				void
			>::PopBack::type;
			
		using CmdContent_t	= TypeList<
				// shared commands
				char,
				// transfer commands
				BufferCopy, ImageCopy, BufferImageCopy, BufferImageCopy2
				// compute commands
				// graphics commands
				// draw commands
				// acceleration structure build commands
				// ray tracing commands
			>;
		
		
	private:
		struct _CmdProcessor;
		struct _Encoders;
		
		
	// methods
	public:
		MSoftwareCmdBuf ()											__NE___	{}
		
		ND_ MBakedCommands	Bake ()									__NE___;
		
		template <typename CmdType, typename ...DynamicTypes>
		ND_ CmdType&  CreateCmd (usize dynamicArraySize = 0)		__Th___	{ return SoftwareCmdBufBase::_CreateCmd< Commands_t, CmdType, DynamicTypes... >( dynamicArraySize ); }
		
		void  DebugMarker (DebugLabel dbg)							__Th___;
		void  PushDebugGroup (DebugLabel dbg)						__Th___;
		void  PopDebugGroup ()										__Th___;
		void  PipelineBarrier (const MDependencyInfo &)				__Th___;

		void  ProfilerBeginContext (IGraphicsProfiler*, const void*, DebugLabel, IGraphicsProfiler::EContextType)	__Th___;
		void  ProfilerEndContext (IGraphicsProfiler*, const void*, IGraphicsProfiler::EContextType)					__Th___;
		
		void  DbgFillBuffer (MetalBuffer buffer, Bytes offset, Bytes size, uint data)								__Th___;
		
		ND_ static bool  Execute (INOUT MCommandBuffer &cmdbuf, void* root)											__NE___;
		
	private:
		ND_ bool  _Validate (const void* root)						C_NE___	{ return SoftwareCmdBufBase::_Validate( root, Commands_t::Count ); }
	};
	
	using MSoftwareCmdBufPtr = Unique< MSoftwareCmdBuf >;
	
	
	
	//
	// Metal Indirect Context base class
	//

	class _MBaseIndirectContext
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = true;
		
		using CmdBuf_t = MSoftwareCmdBufPtr;

	protected:
		#define AE_BASE_IND_CTX_VISIT( _name_ )		using _name_ = MSoftwareCmdBuf::_name_;
		AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
		#undef AE_BASE_IND_CTX_VISIT


	// variables
	protected:
		MSoftwareCmdBufPtr	_cmdbuf;


	// methods
	public:
		virtual ~_MBaseIndirectContext ()										__NE___	{ DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }
		
		void  PipelineBarrier (const MDependencyInfo &info)						__Th___	{ _cmdbuf->PipelineBarrier( info ); }

	protected:
		explicit _MBaseIndirectContext (DebugLabel dbg)							__Th___;
		explicit _MBaseIndirectContext (MSoftwareCmdBufPtr cmdbuf)				__Th___: _cmdbuf{RVRef(cmdbuf)}  { CHECK_THROW( _IsValid() ); }
		_MBaseIndirectContext (DebugLabel dbg, MSoftwareCmdBufPtr cmdbuf)		__Th___;

		ND_ bool	_IsValid ()													C_NE___	{ return _cmdbuf and _cmdbuf->IsValid(); }

		void  _DebugMarker (DebugLabel dbg)										__Th___	{ _cmdbuf->DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)									__Th___	{ _cmdbuf->PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()													__Th___	{ _cmdbuf->PopDebugGroup(); }

		ND_ MBakedCommands		_EndCommandBuffer ()							__Th___;
		ND_ MSoftwareCmdBufPtr  _ReleaseCommandBuffer ()						__Th___;

		ND_ static MSoftwareCmdBufPtr  _ReuseOrCreateCommandBuffer (MSoftwareCmdBufPtr cmdbuf, DebugLabel dbg) __Th___;
	};



	//
	// Metal Indirect Context base class with barrier manager
	//

	class MBaseIndirectContext : public _MBaseIndirectContext
	{
	// variables
	protected:
		MBarrierManager		_mngr;
		

	// methods
	public:
		explicit MBaseIndirectContext (const RenderTask &task)					__Th___ : MBaseIndirectContext{ task, Default } {}
		MBaseIndirectContext (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf)__Th___;
		~MBaseIndirectContext ()												__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
		ND_ bool	_NoPendingBarriers ()										C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()												C_NE___	{ return _mngr.GetDevice().GetFeatures(); }

		ND_ MBakedCommands		_EndCommandBuffer ()							__Th___;
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _MBaseIndirectContext::_MBaseIndirectContext (DebugLabel dbg, MSoftwareCmdBufPtr cmdbuf) __Th___ :
		_cmdbuf{ _ReuseOrCreateCommandBuffer( RVRef(cmdbuf), dbg )}
	{
		CHECK_THROW( _IsValid() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseIndirectContext::MBaseIndirectContext (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf) __Th___ :
		_MBaseIndirectContext{ DebugLabel{ task.DbgFullName(), task.DbgColor() }, RVRef(cmdbuf) },
		_mngr{ task }
	{
		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
	}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline MBakedCommands  MBaseIndirectContext::_EndCommandBuffer () __Th___
	{
		if ( auto* bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
			PipelineBarrier( *bar );

		return _MBaseIndirectContext::_EndCommandBuffer();
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
	forceinline bool  MBakedCommands::Execute (INOUT MCommandBuffer &cmdbuf) C_NE___
	{
		return Graphics::_hidden_::MSoftwareCmdBuf::Execute( INOUT cmdbuf, _root );
	}

} // AE::Graphics

#endif // AE_ENABLE_METAL
