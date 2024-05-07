// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/RayTracingDesc.h"
#include "graphics/GraphicsImpl.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
#endif
#include "graphics/Metal/MEnumCast.h"

namespace AE::Graphics
{
namespace
{
#ifndef AE_ENABLE_VULKAN
/*
=================================================
	VEnumCast (ERTInstanceOpt)
=================================================
*/
	enum VkGeometryInstanceFlagBitsKHR {
		VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR = 0x00000001,
		VK_GEOMETRY_INSTANCE_TRIANGLE_FLIP_FACING_BIT_KHR = 0x00000002,
		VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR = 0x00000004,
		VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR = 0x00000008,
		VK_GEOMETRY_INSTANCE_FORCE_OPACITY_MICROMAP_2_STATE_EXT = 0x00000010,
		VK_GEOMETRY_INSTANCE_DISABLE_OPACITY_MICROMAPS_EXT = 0x00000020,
		VK_GEOMETRY_INSTANCE_FLAG_BITS_MAX_ENUM_KHR = 0x7FFFFFFF
	};

	VkGeometryInstanceFlagBitsKHR  operator |= (VkGeometryInstanceFlagBitsKHR &lhs, VkGeometryInstanceFlagBitsKHR rhs) __NE___
	{
		return lhs = VkGeometryInstanceFlagBitsKHR(Math::ToNearUInt(lhs) | Math::ToNearUInt(rhs));
	}

	ND_ inline VkGeometryInstanceFlagBitsKHR  VEnumCast (ERTInstanceOpt values) __NE___
	{
		VkGeometryInstanceFlagBitsKHR	result = Zero;

		for (auto t : BitfieldIterate( values ))
		{
			switch_enum( t )
			{
				case ERTInstanceOpt::TriangleCullDisable :	result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;	break;
				case ERTInstanceOpt::TriangleFrontCCW :		result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FLIP_FACING_BIT_KHR;			break;
				case ERTInstanceOpt::ForceOpaque :			result |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;					break;
				case ERTInstanceOpt::ForceNonOpaque :		result |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;					break;

				case ERTInstanceOpt::_Last :
				case ERTInstanceOpt::All :
				case ERTInstanceOpt::Unknown :
				default_unlikely :							RETURN_ERR( "unknown RT instance options", Zero );
			}
			switch_end
		}
		return result;
	}

#endif // AE_ENABLE_VULKAN
}
//-----------------------------------------------------------------------------


/*
=================================================
	RTSceneBuild::Instance::Init
=================================================
*/
	RTSceneBuild::InstanceVk&  RTSceneBuild::InstanceVk::Init () __NE___
	{
		transform			= RTMatrixStorage::Identity();
		instanceCustomIndex	= 0;
		mask				= 0xFF;
		instanceSBTOffset	= 0;
		flags				= 0;
		rtas				= Default;
		return *this;
	}

	RTSceneBuild::InstanceMtl&  RTSceneBuild::InstanceMtl::Init () __NE___
	{
		transform			= RTMatrixStorage::Identity();
		options				= 0;
		mask				= UMax;
		instanceSBTOffset	= 0;
		rtasIndex			= UMax;
		return *this;
	}

/*
=================================================
	RTSceneBuild::Instance::SetFlags
=================================================
*/
	RTSceneBuild::InstanceVk&  RTSceneBuild::InstanceVk::SetFlags (ERTInstanceOpt value) __NE___
	{
		this->flags = VEnumCast( value );
		return *this;
	}

	RTSceneBuild::InstanceMtl&  RTSceneBuild::InstanceMtl::SetFlags (ERTInstanceOpt value) __NE___
	{
		this->options = uint(MEnumCast( value ));
		return *this;
	}

/*
=================================================
	RTSceneBuild::SetGeometry
=================================================
*/
	bool  RTSceneBuild::SetGeometry (RTGeometryID id, INOUT InstanceVk &inst) __NE___
	{
		ASSERT( id );
		//ASSERT( inst.rtas == Default );

		auto*	geom = GraphicsScheduler().GetResourceManager().GetResource( id, False{"don't inc ref"}, True{"quiet"} );
		if_likely( geom != null )
		{
			CHECK( uniqueGeoms.insert( id ).first != null );

			inst.rtas = geom->GetDeviceAddress();
			return true;
		}
		return false;
	}

	bool  RTSceneBuild::SetGeometry (RTGeometryID id, INOUT InstanceMtl &inst) __NE___
	{
		ASSERT( id );
		//ASSERT( inst.rtasIndex == UMax );

		if_likely( auto it = uniqueGeoms.insert( id ).first;  it != null )
		{
			inst.rtasIndex = uint(uniqueGeoms.IndexOf( it ));
			return true;
		}
		return false;
	}


} // AE::Graphics
