// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VRenderTaskScheduler.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

# include "graphics_rhi/Vulkan/Allocators/VUniMemAllocator.h"
# include "graphics_rhi/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics_rhi/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics_rhi/Vulkan/Allocators/VDedicatedMemAllocator.h"

# include "graphics_rhi/Vulkan/Descriptors/VDefaultDescriptorAllocator.h"

namespace AE::Graphics
{
#	include "graphics_rhi/Private/ResourceManager.cpp.h"

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  VResourceManager::GetMemoryInfo (ImageID id, OUT VulkanMemoryObjInfo &info) C_NE___
	{
		auto*	image = GetResource( id );
		CHECK_ERR( image != null );
		return GetMemoryInfo( image->MemoryId(), OUT info );
	}

	bool  VResourceManager::GetMemoryInfo (BufferID id, OUT VulkanMemoryObjInfo &info) C_NE___
	{
		auto*	buffer = GetResource( id );
		CHECK_ERR( buffer != null );
		return GetMemoryInfo( buffer->MemoryId(), OUT info );
	}

	bool  VResourceManager::GetMemoryInfo (MemoryID id, OUT VulkanMemoryObjInfo &info) C_NE___
	{
		auto*	mem = GetResource( id );
		CHECK_ERR( mem != null );
		return mem->GetMemoryInfo( OUT info );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_DestroyResource
=================================================
*/
	template <>
	void  VResourceManager::_DestroyResource (VkSwapchainKHR handle) __NE___
	{
		_device.vkDestroySwapchainKHR( _device.GetVkDevice(), handle, null );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	Create*
=================================================
*/
	Strong<MemoryID>  VResourceManager::CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateResource<MemoryID>( ERR_MSG( "failed when creating memory object", dbgName ), buffer, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
	}

	Strong<MemoryID>  VResourceManager::CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateResource<MemoryID>( ERR_MSG( "failed when creating memory object", dbgName ), image, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
	}

	Strong<RenderPassID>  VResourceManager::CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, RenderPassID compatId, StringView dbgName) __NE___
	{
		return _CreateResource<RenderPassID>( ERR_MSG( "failed when creating render pass", dbgName ), *this, info, vkInfo, compatId, dbgName );
	}

/*
=================================================
	CreateDescriptorSets
=================================================
*/
	bool  VResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
												  PipelinePackID packId, DSLayoutName::Ref dslName,
												  DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		auto	layout_id = pack->GetDSLayout( dslName );
		CHECK_ERR( layout_id );

		return CreateDescriptorSets( OUT dst, count, layout_id, RVRef(allocator), dbgName );
	}

	bool  VResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
												  DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( dst != null and count > 0 );
		CHECK_ERR( layoutId );

		allocator = _ChooseDescAllocator( RVRef(allocator) );

		usize	i		= 0;
		bool	created	= true;

		for (; created and (i < count); ++i)
		{
			dst[i]  = _CreateResource<DescriptorSetID>( ERR_MSG( "failed when creating descriptor set", dbgName ), *this, layoutId, allocator, dbgName );
			created = (dst[i].IsValid());
		}

		if ( not created ) {
			for (usize j = 0; j < i; ++j) {
				ImmediatelyRelease( INOUT dst[j] );
			}
		}

		return created;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CreateSampler
=================================================
*/
	Strong<SamplerID>  VResourceManager::CreateSampler (const SamplerDesc &info, StringView dbgName, const VkSamplerYcbcrConversionCreateInfo* ycbcrInfo) __NE___
	{
		return _CreateResource<SamplerID>( ERR_MSG( "failed when creating sampler", dbgName ), *this, info, ycbcrInfo, dbgName );
	}

/*
=================================================
	GetVkSampler
=================================================
*/
	VkSampler  VResourceManager::GetVkSampler (PipelinePackID packId, SamplerName::Ref name) C_NE___
	{
		auto*	res = GetResource( GetSampler( packId, name ));
		CHECK_ERR( res != null );
		return res->Handle();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CreateFramebuffer
=================================================
*/
	VFramebufferID  VResourceManager::CreateFramebuffer (const RenderPassDesc &desc) __NE___
	{
		RenderPassID	rp_id = GetCompatibleRenderPass( desc.packId, desc.renderPassName );
		CHECK_ERR( rp_id );

		VFramebuffer::Key	key{ StructView<ImageViewID>{ desc.attachments.GetValueArray(), &RenderPassDesc::Attachment::imageView },
								 rp_id,
								 uint3{ uint(desc.area.Width()), uint(desc.area.Height()), desc.layerCount.Get() }};

		// find in cache
		{
			std::shared_lock	rlock{ _resPool.fbCacheGuard };

			auto	it = _resPool.fbCache.find( key );
			if_likely( it != _resPool.fbCache.end() )
			{
				// increase ref counter to safely use fb in current frame
				auto	id = AcquireResource( it->second );
				CHECK_ERR( id );

				rlock.unlock();

				// decrease ref counter when current frame is complete execution on GPU side
				_DelayedReleaseFramebuffer( it->second );
				return id.Release();
			}
		}

		// create new framebuffer
		VFramebufferID	raw_id;
		CHECK_ERR( _Assign( OUT raw_id ));

		auto&	data = _GetResourcePool( raw_id )[ raw_id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( *this, desc, rp_id  GFX_DBG_ONLY(, HashToName( desc.renderPassName )) ))
		{
			data.Destroy( *this );
			_Unassign( raw_id );
			RETURN_ERR( "failed when creating framebuffer from render pass '"s << HashToName( desc.renderPassName ) << "'" );
		}

		data.AddRef();	// 'raw_id' rc == 1


		// add to cache
		Strong<VFramebufferID>	actual_id;
		{
			EXLOCK( _resPool.fbCacheGuard );
			auto [it, inserted] = _resPool.fbCache.emplace( key, raw_id );  // throw

			if_likely( inserted )
			{
				CHECK_ERR( data.Data().SetCachePtr( it ));
				actual_id.Attach( raw_id );
			}
			else
			{
				actual_id = AcquireResource( it->second );
				CHECK( actual_id );
			}
		}

		// framebuffer is already added to the cache
		if_unlikely( *actual_id != raw_id )
		{
			CHECK( _ImmediatelyReleaseResource( raw_id ) == 0 );
		}

		// decrease ref counter when current frame is complete execution on GPU side
		_DelayedReleaseFramebuffer( VFramebufferID{actual_id} );
		return actual_id.Release();
	}

/*
=================================================
	_DelayedReleaseFramebuffer
=================================================
*/
	inline void  VResourceManager::_DelayedReleaseFramebuffer (VFramebufferID id) __NE___
	{
		ASSERT( id != Default );

		auto&	dst = _expiredResources.GetCurrent();
		EXLOCK( dst.guard );
		ASSERT( dst.frameId == _expiredResources.GetFrameId() );

		uint	u = (id.Index() << 16) | id.Generation();
		dst.framebuffers.push_back( u );
	}

/*
=================================================
	_ReleaseFramebuffers
=================================================
*/
	inline void  VResourceManager::_ReleaseFramebuffers (const FrameUID frameId, INOUT RingBuffer<uint> &framebuffers) __NE___
	{
		constexpr uint	idx_mask	= 0x7FFF;
		constexpr uint	gen_mask	= 0xFFFF;

		StaticAssert( VFramebufferID::MaxIndex() == gen_mask );
		StaticAssert( VFramebufferID::MaxGeneration() == gen_mask );
		StaticAssert( FramebufferPool_t::capacity() < idx_mask );
		StaticAssert( DelayFramebufferDeletion*2 < gen_mask );

	  #ifdef AE_DEBUG
		if ( not framebuffers.empty() )
			ASSERT( HasBit<31>( framebuffers.front() ));
	  #endif

		for (; not framebuffers.empty(); )
		{
			uint	u = framebuffers.front();

			// frame id
			if_unlikely( HasBit<31>( u ))
			{
				uint	delta = uint(frameId.Unique() & gen_mask) - (u & gen_mask);
				if ( delta < DelayFramebufferDeletion )
					break;  // try again later

				// process until next fb id
			}
			else
			{
				// framebuffer
				VFramebufferID	id{ (u >> 16) & idx_mask, u & gen_mask };
				Unused( _ImmediatelyReleaseResource( id ));
			}
			framebuffers.pop_front();
		}
	}

/*
=================================================
	_AddFrameIDtoFramebufferList
=================================================
*/
	inline void  VResourceManager::_AddFrameIDtoFramebufferList (const FrameUID frameId, INOUT RingBuffer<uint> &framebuffers) __NE___
	{
		constexpr uint	gen_mask	= 0xFFFF;
		StaticAssert( VFramebufferID::MaxGeneration() == gen_mask );

		uint	u = uint(frameId.Unique() & gen_mask) | (1u << 31);
		framebuffers.push_back( u );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	LoadPipelineCache
=================================================
*/
	Strong<PipelineCacheID>  VResourceManager::LoadPipelineCache (RC<RStream> stream) __NE___
	{
		PipelineCacheID		id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( *this, Default, RVRef(stream) ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( "failed when loading pipeline cache" );
		}

		data.AddRef();
		return Strong<PipelineCacheID>{ id };
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Create***Allocator
=================================================
*/
	GfxMemAllocatorPtr  VResourceManager::CreateLinearGfxMemAllocator (Bytes pageSize) C_NE___
	{
		return MakeRC<VLinearMemAllocator>( pageSize );
	}

	GfxMemAllocatorPtr  VResourceManager::CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize) C_NE___
	{
		return MakeRC<VBlockMemAllocator>( blockSize, pageSize );
	}

	GfxMemAllocatorPtr  VResourceManager::CreateUnifiedGfxMemAllocator (Bytes pageSize) C_NE___
	{
		return MakeRC<VUniMemAllocator>( pageSize );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
