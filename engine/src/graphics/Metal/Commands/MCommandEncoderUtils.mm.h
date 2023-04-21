// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"
#include "graphics/Metal/MEnumCast.mm.h"
#include "graphics/Metal/Commands/MCommandBuffer.mm.h"

namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	CreateComputeEncoder
=================================================
*/
	inline void  CreateComputeEncoder (INOUT MCommandBuffer					&cmdbuf,
									   INOUT MetalSampleBufferAttachments 	&sampleBuffers,
									   NtStringView							dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
		  #if not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 11.0, ios 14.0, *))
			{
				auto*	desc = [[[MTLComputePassDescriptor alloc] init] autorelease];
				desc.dispatchType = MTLDispatchTypeConcurrent;

				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));

				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() computeCommandEncoderWithDescriptor : desc ]);							// autorelease
			}
			else
		  #endif
				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() computeCommandEncoderWithDispatchType : MTLDispatchTypeConcurrent ]);	// autorelease
			
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];								// autorelease
		}
	}
	
/*
=================================================
	CreateRenderEncoder
=================================================
*/
	inline void  CreateRenderEncoder (INOUT MCommandBuffer					&cmdbuf,
									  MetalRenderPassDesc					mtlDesc,
									  INOUT MetalSampleBufferAttachments 	&sampleBuffers,
									  NtStringView							dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
			MTLRenderPassDescriptor*	desc = mtlDesc.Cast();

		  #if not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 11.0, ios 14.0, *))
			{
				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));
			}
		  #endif
			
			cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() renderCommandEncoderWithDescriptor : desc ]);	// autorelease
			
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];	// autorelease
		}
	}

	inline void  CreateMtRenderEncoder (INOUT MCommandBuffer				&cmdbuf,
										MetalRenderPassDesc					mtlDesc,
									    INOUT MetalSampleBufferAttachments 	&sampleBuffers,
										NtStringView						dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
			MTLRenderPassDescriptor*	desc = mtlDesc.Cast();

		  #if not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 11.0, ios 14.0, *))
			{
				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));
			}
		  #endif
			
			cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() parallelRenderCommandEncoderWithDescriptor : desc ]);	// autorelease
			
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];			// autorelease
		}
	}
	
/*
=================================================
	CreateBlitEncoder
=================================================
*/
	inline void  CreateBlitEncoder (INOUT MCommandBuffer 				&cmdbuf,
									INOUT MetalSampleBufferAttachments 	&sampleBuffers,
									NtStringView						dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
		  #if not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 11.0, ios 14.0, *))
			{
				auto*	desc = [[[MTLBlitPassDescriptor alloc] init] autorelease];
				
				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));

				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() blitCommandEncoderWithDescriptor : desc ]);	// autorelease
			}
			else
		  #endif
			{
				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() blitCommandEncoder ]);							// autorelease
			}
				
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];		// autorelease
		}
	}
	
/*
=================================================
	CreateResourceStateEncoder
=================================================
*/
	inline void  CreateResourceStateEncoder (INOUT MCommandBuffer 					&cmdbuf,
											 INOUT MetalSampleBufferAttachments 	&sampleBuffers,
											 NtStringView							dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
		  #if not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 11.0, ios 14.0, *))
			{
				auto*	desc = [[[MTLResourceStatePassDescriptor alloc] init] autorelease];
				
				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));

				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() resourceStateCommandEncoderWithDescriptor : desc ]);	// autorelease
			}
			else
		  #endif
			if ( @available( macos 11.0, ios 13.0, *))
			{
				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() resourceStateCommandEncoder ]);						// autorelease
			}
			
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];				// autorelease
		}
	}
	
/*
=================================================
	CreateAccelStructEncoder
=================================================
*/
	inline void  CreateAccelStructEncoder (INOUT MCommandBuffer 				&cmdbuf,
										   INOUT MetalSampleBufferAttachments 	&sampleBuffers,
										   NtStringView							dbgName) __NE___
	{
		CHECK_ERRV( cmdbuf.GetCmdBuf() );

		@autoreleasepool
		{
		  #if 0 //not AE_METAL_NATIVE_DEBUGGER
			if ( @available( macos 13.0, ios 16.0, *))
			{
				auto*	desc = [[[(MTLAccelerationStructurePassDescriptor alloc] init] autorelease];
				
				CHECK( sampleBuffers.MoveTo( desc.sampleBufferAttachments ));

				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() accelerationStructureCommandEncoderWithDescriptor : desc ]);	// autorelease
			}
			else
		  #endif
			if ( @available( macos 11.0, ios 14.0, *))
			{
				cmdbuf.SetEncoder([ cmdbuf.AsCmdBuffer() accelerationStructureCommandEncoder ]);						// autorelease
			}
			
			if ( not dbgName.empty() )
				[ cmdbuf.AsEncoder() setLabel: [NSString stringWithUTF8String : dbgName.c_str()] ];						// autorelease
		}
	}
//-----------------------------------------------------------------------------
	

		
/*
=================================================
	CopyImage***
=================================================
*/
	inline void  CopyImageWithoutOffsets (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges)
	{
		for (auto& range : ranges)
		{
			ASSERT( range.srcSubres.layerCount == range.dstSubres.layerCount );
			ASSERT( All( IsZero( range.srcOffset )));
			ASSERT( All( IsZero( range.dstOffset )));

			[ encoder copyFromTexture : srcImage.Cast()
						  sourceSlice : range.srcSubres.baseArrayLayer.Get()
						  sourceLevel : range.srcSubres.mipLevel.Get()
							toTexture : dstImage.Cast()
					 destinationSlice : range.dstSubres.baseArrayLayer.Get()
					 destinationLevel : range.dstSubres.mipLevel.Get()
						   sliceCount : Min( range.srcSubres.layerCount, range.dstSubres.layerCount )
						   levelCount : 1];
		}
	}

	inline void  CopyImageImpl (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges)
	{
		for (auto& range : ranges)
		{
			ASSERT( range.srcSubres.layerCount == range.dstSubres.layerCount );

			for (uint layer = 0, layer_cnt = Min( range.srcSubres.layerCount, range.dstSubres.layerCount ); layer < layer_cnt; ++layer)
			{
				[ encoder copyFromTexture : srcImage.Cast()
							  sourceSlice : range.srcSubres.baseArrayLayer.Get() + layer
							  sourceLevel : range.srcSubres.mipLevel.Get()
							 sourceOrigin : MTLOriginMake( range.srcOffset.x, range.srcOffset.y, range.srcOffset.z )
							   sourceSize : MTLSizeMake( range.extent.x, range.extent.y, range.extent.z )
								toTexture : dstImage.Cast()
						 destinationSlice : range.dstSubres.baseArrayLayer.Get() + layer
						 destinationLevel : range.dstSubres.mipLevel.Get()
						destinationOrigin : MTLOriginMake( range.dstOffset.x, range.dstOffset.y, range.dstOffset.z )];
			}
		}
	}

	inline void  CopyImage3Dto2D (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges)
	{
		for (auto& range : ranges)
		{
			ASSERT( range.srcSubres.layerCount == range.dstSubres.layerCount );

			for (uint layer = 0, layer_cnt = Min( range.srcSubres.layerCount, range.dstSubres.layerCount ); layer < layer_cnt; ++layer)
			{
				[ encoder copyFromTexture : srcImage.Cast()
							  sourceSlice : range.srcSubres.baseArrayLayer.Get()
							  sourceLevel : range.srcSubres.mipLevel.Get()
							 sourceOrigin : MTLOriginMake( range.srcOffset.x, range.srcOffset.y, range.srcOffset.z + layer )
							   sourceSize : MTLSizeMake( range.extent.x, range.extent.y, range.extent.z )
								toTexture : dstImage.Cast()
						 destinationSlice : range.dstSubres.baseArrayLayer.Get() + layer
						 destinationLevel : range.dstSubres.mipLevel.Get()
						destinationOrigin : MTLOriginMake( range.dstOffset.x, range.dstOffset.y, range.dstOffset.z )];
			}
		}
	}

	inline void  CopyImage2Dto3D (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges)
	{
		for (auto& range : ranges)
		{
			ASSERT( range.srcSubres.layerCount == range.dstSubres.layerCount );

			for (uint layer = 0, layer_cnt = Min( range.srcSubres.layerCount, range.dstSubres.layerCount ); layer < layer_cnt; ++layer)
			{
				[ encoder copyFromTexture : srcImage.Cast()
							  sourceSlice : range.srcSubres.baseArrayLayer.Get() + layer
							  sourceLevel : range.srcSubres.mipLevel.Get()
							 sourceOrigin : MTLOriginMake( range.srcOffset.x, range.srcOffset.y, range.srcOffset.z )
							   sourceSize : MTLSizeMake( range.extent.x, range.extent.y, range.extent.z )
								toTexture : dstImage.Cast()
						 destinationSlice : range.dstSubres.baseArrayLayer.Get()
						 destinationLevel : range.dstSubres.mipLevel.Get()
						destinationOrigin : MTLOriginMake( range.dstOffset.x, range.dstOffset.y, range.dstOffset.z + layer )];
			}
		}
	}
		
	inline void  CopyImage (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges)
	{
		MTLTextureType	src_type	= [ srcImage.Cast() textureType ];
		MTLTextureType	dst_type	= [ dstImage.Cast() textureType ];
		const uint3		src_dim		{ [srcImage.Cast() width], [srcImage.Cast() height], [srcImage.Cast() depth] };
		const uint3		dst_dim		{ [dstImage.Cast() width], [dstImage.Cast() height], [dstImage.Cast() depth] };

		if ( (src_dim.x == dst_dim.x) & (src_dim.y == dst_dim.y) )
		{
			bool	suitable = true;
			for (auto& range : ranges)
			{
				suitable &= All( IsZero( range.srcOffset ));
				suitable &= All( IsZero( range.dstOffset ));
				suitable &= All( range.extent == src_dim );
				suitable &= All( range.extent == dst_dim );
			}

			if ( suitable )
				return CopyImageWithoutOffsets( encoder, srcImage, dstImage, ranges );
		}
		
		if ( (src_type == MTLTextureType3D) & (dst_type != MTLTextureType3D) )
			return CopyImage3Dto2D( encoder, srcImage, dstImage, ranges );
		
		if ( (src_type == MTLTextureType3D) & (dst_type != MTLTextureType3D) )
			return CopyImage2Dto3D( encoder, srcImage, dstImage, ranges );

		return CopyImageImpl( encoder, srcImage, dstImage, ranges );
	}
	
/*
=================================================
	CopyBufferToImage
=================================================
*/
	inline void  CopyBufferToImage (id<MTLBlitCommandEncoder> encoder, MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy2> ranges)
	{
		ASSERT( srcBuffer and dstImage );
		ASSERT( not ranges.empty() );

		for (auto& range : ranges)
		{
			ASSERT( range.rowPitch > 0_b );
			ASSERT( range.slicePitch > 0_b and (range.slicePitch % range.rowPitch == 0) );

			Bytes	buf_offset = range.bufferOffset;

			for (uint layer = 0; layer < range.imageSubres.layerCount; ++layer)
			{
				[ encoder copyFromBuffer : srcBuffer.Cast()
							sourceOffset : NS::UInteger(buf_offset)
					   sourceBytesPerRow : NS::UInteger(range.rowPitch)
					 sourceBytesPerImage : NS::UInteger(range.slicePitch)
							  sourceSize : MTLSizeMake( range.imageExtent.x, range.imageExtent.y, range.imageExtent.z )
							   toTexture : dstImage.Cast()
						destinationSlice : range.imageSubres.baseArrayLayer.Get() + layer
						destinationLevel : range.imageSubres.mipLevel.Get()
					   destinationOrigin : MTLOriginMake( range.imageOffset.x, range.imageOffset.y, range.imageOffset.z )];

				buf_offset += range.slicePitch;
			}
		}
	}
	
/*
=================================================
	CopyImageToBuffer
=================================================
*/
	inline void  CopyImageToBuffer (id<MTLBlitCommandEncoder> encoder, MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy2> ranges)
	{
		ASSERT( srcImage and dstBuffer );
		ASSERT( not ranges.empty() );

		for (auto& range : ranges)
		{
			ASSERT( range.rowPitch != 0_b );
			ASSERT( range.slicePitch > 0_b and (range.slicePitch % range.rowPitch == 0) );
			
			Bytes	buf_offset = range.bufferOffset;

			for (uint layer = 0; layer < range.imageSubres.layerCount; ++layer)
			{
				[ encoder copyFromTexture : srcImage.Cast()
							  sourceSlice : range.imageSubres.baseArrayLayer.Get() + layer
							  sourceLevel : range.imageSubres.mipLevel.Get()
							 sourceOrigin : MTLOriginMake( range.imageOffset.x, range.imageOffset.y, range.imageOffset.z )
							   sourceSize : MTLSizeMake( range.imageExtent.x, range.imageExtent.y, range.imageExtent.z )
								 toBuffer : dstBuffer.Cast()
						destinationOffset : NS::UInteger(buf_offset)
				   destinationBytesPerRow : NS::UInteger(range.rowPitch)
				 destinationBytesPerImage : NS::UInteger(range.slicePitch) ];

				buf_offset += range.slicePitch;
			}
		}
	}
	
/*
=================================================
	SetDynamicRenderState
=================================================
*/
	inline void  SetDynamicRenderState (id<MTLRenderCommandEncoder> encoder, const MDynamicRenderState &rs)
	{
		[ encoder setTriangleFillMode	: MEnumCast( rs.polygonMode )];
		[ encoder setFrontFacingWinding : (rs.frontFaceCCW ? MTLWindingCounterClockwise : MTLWindingClockwise) ];
		[ encoder setCullMode			: MEnumCast( rs.cullMode )];
		
		[ encoder setDepthBias : rs.depthBiasConstFactor
					slopeScale : rs.depthBiasSlopeFactor
						 clamp : rs.depthBiasClamp ];
	}
	
/*
=================================================
	ConvertRTGeometryBuild
=================================================
*/
	ND_ inline MTLPrimitiveAccelerationStructureDescriptor*
		ConvertRTGeometryBuild (const MResourceManager &resMngr, const RTGeometryBuild &cmd)  API_AVAILABLE(macos(11), ios(14))
	{
		const usize	count	 = cmd.triangles.size() + cmd.aabbs.size();
		auto*		geom_arr = [[[NSMutableArray<MTLAccelerationStructureGeometryDescriptor*> alloc] initWithCapacity : count] autorelease];
		
		CHECK_ERR( cmd.triangles.empty()	or cmd.triangles.data< RTGeometryBuild::TrianglesInfo >() != null );
		CHECK_ERR( cmd.triangles.empty()	or cmd.triangles.data< RTGeometryBuild::TrianglesData >() != null );
		CHECK_ERR( cmd.aabbs.empty()		or cmd.aabbs.data< RTGeometryBuild::AABBsInfo >() != null );
		CHECK_ERR( cmd.aabbs.empty()		or cmd.aabbs.data< RTGeometryBuild::AABBsData >() != null );

		for (usize i = 0; i < cmd.triangles.size(); ++i)
		{
			auto&	info	= cmd.triangles.at< RTGeometryBuild::TrianglesInfo >(i);
			auto&	data	= cmd.triangles.at< RTGeometryBuild::TrianglesData >(i);
			auto*	dst		= [[[MTLAccelerationStructureTriangleGeometryDescriptor descriptor] retain] autorelease];	// TODO: check

			
			auto*	vb	= resMngr.GetResource( data.vertexData,		False{"incRef"}, True{"quiet"} );
			auto*	ib	= resMngr.GetResource( data.indexData,		False{"incRef"}, True{"quiet"} );
			auto*	tb	= resMngr.GetResource( data.transformData,	False{"incRef"}, True{"quiet"} );
			CHECK_ERR( vb != null );
			CHECK_ERR( (ib != null) == data.indexData.IsValid() );
			CHECK_ERR( (tb != null) == data.transformData.IsValid() );
			CHECK_ERR( (info.indexType != Default) == data.indexData.IsValid() );
			CHECK_ERR( info.allowTransforms == data.transformData.IsValid() );

			dst.triangleCount		= info.maxPrimitives;
			dst.vertexBuffer		= vb->Handle().Cast();
			dst.vertexBufferOffset	= NS::UInteger(data.vertexDataOffset);
			dst.vertexStride		= NS::UInteger(data.vertexStride);
			
			if ( @available( macos 13.0, ios 16.0, *))
			{
				dst.vertexFormat	= MEnumCast2( info.vertexFormat );
				if ( tb != null )
				{
					dst.transformationMatrixBuffer			= tb->Handle().Cast();
					dst.transformationMatrixBufferOffset	= NS::UInteger(data.transformDataOffset);
				}
			}
			else
			{
				CHECK_ERR( tb == null );
			}

			if ( ib != null )
			{
				dst.indexType			= MEnumCast( info.indexType );
				dst.indexBuffer			= ib->Handle().Cast();
				dst.indexBufferOffset	= NS::UInteger(data.indexDataOffset);
			}

			[geom_arr addObject : dst];
			
			ASSERT( Bytes{data.vertexStride} >= EVertexType_SizeOf( info.vertexFormat ));
			ASSERT( (info.indexType == Default) or (data.indexDataOffset % EIndex_SizeOf( info.indexType ) == 0) );
			ASSERT( vb->Size() >= (data.vertexDataOffset + Bytes{data.vertexStride} * info.maxVertex) );
			ASSERT( (info.indexType == Default) or (ib->Size() >= (data.indexDataOffset + info.maxPrimitives * EIndex_SizeOf( info.indexType ))) );
		}
		
		for (usize i = 0; i < cmd.aabbs.size(); ++i)
		{
			auto&	info	= cmd.aabbs.at< RTGeometryBuild::AABBsInfo >(i);
			auto&	data	= cmd.aabbs.at< RTGeometryBuild::AABBsData >(i);
			auto*	dst		= [[[MTLAccelerationStructureBoundingBoxGeometryDescriptor descriptor] retain] autorelease];	// TODO: check

			auto*	buf		= resMngr.GetResource( data.data, False{"incRef"}, True{"quiet"} );
			CHECK_ERR( buf != null );

			dst.boundingBoxCount		= info.maxAABBs;
			dst.boundingBoxBuffer		= buf->Handle().Cast();
			dst.boundingBoxBufferOffset	= NS::UInteger(data.dataOffset);
			dst.boundingBoxStride		= NS::UInteger(data.stride);

			[geom_arr addObject : dst];
		}

		auto*	desc = [MTLPrimitiveAccelerationStructureDescriptor descriptor];	// autorelease
		desc.usage					= MEnumCast( cmd.options );
		desc.geometryDescriptors	= geom_arr;
		
		if ( @available( macos 13.0, ios 16.0, *))
		{
			//desc.motionKeyframeCount		= ;	// TODO
			//desc.motionStartTime			= ;
			//desc.motionEndTime			= ;
			//desc.motionStartBorderMode	= ;
			//desc.motionEndBorderMode		= ;
		}

		return desc;
	}

/*
=================================================
	ConvertRTSceneBuild
=================================================
*/
	ND_ inline MTLInstanceAccelerationStructureDescriptor*
		ConvertRTSceneBuild (const MResourceManager &resMngr, const RTSceneBuild &cmd)  API_AVAILABLE(macos(11), ios(14))
	{
		CHECK_ERR( not cmd.geomArray.empty() );

		auto*	as_arr		= [[[NSMutableArray<id<MTLAccelerationStructure>> alloc] initWithCapacity : cmd.maxInstanceCount] autorelease];
		auto*	inst_buf	= resMngr.GetResource( cmd.instanceData.id, False{"incRef"}, True{"quiet"} );
		CHECK_ERR( inst_buf != null );

		auto*	desc = [MTLInstanceAccelerationStructureDescriptor descriptor]; // autorelease
		
		if ( @available( macos 12.0, ios 15.0, *))
		{
			desc.instanceDescriptorType = MTLAccelerationStructureInstanceDescriptorTypeDefault;
		}

		desc.usage								= MEnumCast( cmd.options );
		desc.instanceCount						= cmd.maxInstanceCount;
		desc.instanceDescriptorBuffer			= inst_buf->Handle().Cast();
		desc.instanceDescriptorBufferOffset		= NS::UInteger(cmd.instanceData.offset);
		desc.instanceDescriptorStride			= NS::UInteger(cmd.instanceData.stride);
		desc.instancedAccelerationStructures	= as_arr;
		
		if ( @available( macos 13.0, ios 16.0, *))
		{
			//dst.motionTransformCount
			//dst.motionTransformBuffer
			//dst.motionTransformBufferOffset
		}

		ASSERT( cmd.instanceData.stride == sizeof(MTLAccelerationStructureInstanceDescriptor) );

		return desc;
	}

} // AE::Graphics::_hidden_
