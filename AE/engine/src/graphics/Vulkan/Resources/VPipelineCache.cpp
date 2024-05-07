// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelineCache.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VPipelineCache::~VPipelineCache () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _cache == Default );
	}

/*
=================================================
	_Create
=================================================
*/
	bool  VPipelineCache::_Create (const VResourceManager& resMngr, StringView dbgName, ArrayView<char> initialData) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _cache == Default );

		auto&	dev = resMngr.GetDevice();

		VkPipelineCacheCreateInfo	info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		if ( initialData.size() > sizeof(VkPipelineCacheHeaderVersionOne) )
		{
			VkPipelineCacheHeaderVersionOne		header;
			std::memcpy( &header, initialData.data(), sizeof(header) );

			auto&	props = dev.GetVProperties().properties;

			if ( header.headerVersion	== VK_PIPELINE_CACHE_HEADER_VERSION_ONE and
				 header.headerSize		== 32									and
				 header.deviceID		== props.deviceID						and
				 header.vendorID		== props.vendorID						and
				 MemEqual( header.pipelineCacheUUID, props.pipelineCacheUUID ))
			{
				info.initialDataSize	= usize(ArraySizeOf( initialData ));
				info.pInitialData		= initialData.data();
			}
			else
			{
				RETURN_ERR( "invalid or incompatible pipeline cache header" );
			}
		}

		VK_CHECK_ERR( dev.vkCreatePipelineCache( dev.GetVkDevice(), &info, null, OUT &_cache ));

		dev.SetObjectName( _cache, dbgName, VK_OBJECT_TYPE_PIPELINE_CACHE );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VPipelineCache::Create (const VResourceManager& resMngr, StringView dbgName, RC<RStream> stream) __NE___
	{
		CHECK_ERR( stream != null and stream->IsOpen() );

		Array<char>	data;
		CHECK_ERR( stream->Read( stream->RemainingSize(), data ));

		return _Create( resMngr, dbgName, data );
	}

	bool  VPipelineCache::Create (const VResourceManager& resMngr, StringView dbgName) __NE___
	{
		return _Create( resMngr, dbgName, Default );
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VPipelineCache::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _cache != Default )
		{
			auto&	dev = resMngr.GetDevice();
			dev.vkDestroyPipelineCache( dev.GetVkDevice(), _cache, null );
			_cache = Default;
		}

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetData
=================================================
*/
	bool  VPipelineCache::GetData (const VDevice &dev, OUT Array<char> &data) C_NE___
	{
		DRC_EXLOCK( _drCheck );

		data.clear();

		usize	size = 0;
		VK_CHECK_ERR( dev.vkGetPipelineCacheData( dev.GetVkDevice(), _cache, OUT &size, null ));

		if ( size == 0 )
			return true;

		data.resize( size );
		VK_CHECK_ERR( dev.vkGetPipelineCacheData( dev.GetVkDevice(), _cache, OUT &size, data.data() ));

		return true;
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
