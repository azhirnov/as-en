// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VDescriptorSet.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
/*
=================================================
	destructor
=================================================
*/
	VDescriptorSet::~VDescriptorSet () __NE___
	{
		CHECK( _descrSet.handle == Default );
		CHECK( _allocator == null );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VDescriptorSet::Create (ResourceManager &resMngr, DescriptorSetLayoutID layoutId, const DescSetParams* params, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( allocator != null );
		CHECK_ERR( layoutId != Default );

		CHECK_ERR( _descrSet.handle == Default );
		CHECK_ERR( _allocator == null );

		_layoutId = resMngr.AcquireResource( layoutId );
		CHECK_ERR( _layoutId );

		GFX_DBG_ONLY( _Validate( resMngr, params ));

		if_unlikely( not allocator->Allocate( _layoutId, params, OUT _descrSet ))
		{
			resMngr.ImmediatelyRelease( INOUT _layoutId );
			RETURN_ERR( "descriptor set allocation failed" );
		}

		resMngr.GetDevice().SetObjectName( _descrSet.handle, dbgName, VK_OBJECT_TYPE_DESCRIPTOR_SET );

		_allocator = RVRef(allocator);

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VDescriptorSet::Destroy (ResourceManager &resMngr) __NE___
	{
		if ( (_allocator != null) and (_descrSet.handle != Default) )
		{
			_allocator->Deallocate( _layoutId, INOUT _descrSet );
		}

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_allocator = null;

		CHECK( _descrSet.handle == Default );

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	_Validate
=================================================
*/
#if AE_DBG_GRAPHICS
	bool  VDescriptorSet::_Validate (ResourceManager &resMngr, const DescSetParams* params)
	{
		auto*	layout = resMngr.GetResource( _layoutId.Get() );
		CHECK_ERR( layout != null );  // should not happen

		if ( params != null and params->variableArraySize != 0 )
		{
			const auto [count, name_arr, un_arr, offsets] = layout->GetUniforms();
			GRES_CHECK_MSG( count > 0, "At least one uniform must exist to apply variable count" );

			// find last descriptor
			uint	max_binding		= 0;
			uint	last_desc_idx	= 0;

			for (uint i = 0; i < count; ++i)
			{
				if ( un_arr[i].binding.vkIndex > max_binding )
				{
					max_binding		= un_arr[i].binding.vkIndex;
					last_desc_idx	= i;
				}
			}

			if ( count > 0 )
			{
				auto&	un = un_arr[ last_desc_idx ];
				GRES_CHECK_MSG( AllBits( un.flags, EDescriptorFlags::VariableSize ),
					"Last uniform in descriptor set must have 'VariableSize' flag." );

				GRES_CHECK_MSG( params->variableArraySize <= un.arraySize,
					"'variableArraySize' ("s << ToString(params->variableArraySize) <<
					") must be <= than defined 'arraySize' (" << ToString(un.arraySize) << ")" );
			}
		}
		return true;
	}
#endif

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
