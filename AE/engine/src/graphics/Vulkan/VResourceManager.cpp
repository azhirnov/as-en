// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/Vulkan/VEnumCast.h"

# include "graphics/Vulkan/Allocators/VUniMemAllocator.h"
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/Allocators/VDedicatedMemAllocator.h"

# include "graphics/Vulkan/Descriptors/VDefaultDescriptorAllocator.h"

namespace AE::Graphics
{
#	include "graphics/Private/ResourceManager.cpp.h"

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
	void  VResourceManager::_DestroyResource (VFramebufferID id) __NE___
	{
		Unused( _ImmediatelyReleaseResource( id ));
	}

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
		return _CreateResource<MemoryID>( "failed when creating memory object", buffer, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
	}

	Strong<MemoryID>  VResourceManager::CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateResource<MemoryID>( "failed when creating memory object", image, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
	}

	Strong<RenderPassID>  VResourceManager::CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, RenderPassID compatId, StringView dbgName) __NE___
	{
		return _CreateResource<RenderPassID>( "failed when creating render pass", *this, info, vkInfo, compatId, dbgName );
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
		CHECK_ERR( pack );

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
			dst[i]  = _CreateResource<DescriptorSetID>( "failed when creating descriptor set", *this, layoutId, allocator, dbgName );
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
		return _CreateResource<SamplerID>( "failed when creating sampler", *this, info, ycbcrInfo, dbgName );
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
								 uint3{ desc.area.Width(), desc.area.Height(), desc.layerCount.Get() }};

		// find in cache
		{
			std::shared_lock	rlock{ _resPool.fbCacheGuard };

			auto	iter = _resPool.fbCache.find( key );
			if_likely( iter != _resPool.fbCache.end() )
			{
				// increase ref counter to safely use fb in current frame
				auto	id = AcquireResource( iter->second );
				CHECK_ERR( id );
				rlock.unlock();

				// decrease ref counter when current frame is complete execution on the GPU side
				_DelayedReleaseResource2( iter->second );
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
			auto [iter, inserted] = _resPool.fbCache.emplace( key, raw_id );

			if_likely( inserted )
			{
				CHECK_ERR( data.Data().SetCachePtr( iter ));
				actual_id.Attach( raw_id );
			}
			else
			{
				actual_id = AcquireResource( iter->second );
				CHECK( actual_id );
			}
		}

		// framebuffer already added to cache
		if_unlikely( *actual_id != raw_id )
		{
			CHECK( _ImmediatelyReleaseResource( raw_id ) == 0 );
		}

		// decrease ref counter when current frame is complete execution on the GPU side
		_DelayedReleaseResource2( VFramebufferID{actual_id} );
		return actual_id.Release();
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
