// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RPipelineLayout.h"
# include "graphics/Remote/RResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RPipelineLayout::~RPipelineLayout () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _plId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RPipelineLayout::Create (RResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _plId );

		_descriptorSets = ci.ds;

		const auto	api = resMngr.GetDevice().GetGraphicsAPI();

		for (auto [name, src] : ci.pc)
		{
			PushConst	dst;
			dst.typeName	= src.typeName;
			dst.size		= src.size;

			switch_enum( api )
			{
				case EGraphicsAPI::Vulkan :	dst.idx = PushConstantIndex{ src.vulkanOffset,  src.stage, src.typeName, src.size };	break;
				case EGraphicsAPI::Metal :	dst.idx = PushConstantIndex{ src.metalBufferId, src.stage, src.typeName, src.size };	break;
			}
			switch_end
			_pushConstants.emplace( name, dst );
		}

		GFX_DBG_ONLY( _debugName = ci.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RPipelineLayout::Destroy (RResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_descriptorSets.clear();
		_pushConstants.clear();

		_plId = Default;
		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetDescriptorSetLayout
=================================================
*/
	bool  RPipelineLayout::GetDescriptorSetLayout (DescriptorSetName::Ref dsName, OUT DescriptorSetLayoutID &layoutId, OUT DescSetBinding &binding) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _descriptorSets.find( dsName );

		if_likely( it != _descriptorSets.end() )
		{
			binding		= it->second.first;
			layoutId	= it->second.second;

			ASSERT( binding );
			ASSERT( layoutId );
			return true;
		}
		return false;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
