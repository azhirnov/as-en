// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "graphics_rhi/Public/RayTracingDesc.h"
#include "graphics_rhi/GraphicsImpl.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VEnumCast.h"
#endif
#include "graphics_rhi/Metal/MEnumCast.h"

namespace AE::Graphics
{
namespace
{
#ifndef AE_ENABLE_VULKAN
/*
=================================================
	VkGeometryInstanceFlagBitsKHR
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
		return lhs = VkGeometryInstanceFlagBitsKHR(Base::ToNearUInt(lhs) | Base::ToNearUInt(rhs));
	}

/*
=================================================
	VkPartitionedAccelerationStructureInstanceFlagBitsNV
=================================================
*/
	enum VkPartitionedAccelerationStructureInstanceFlagBitsNV {
		VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE_BIT_NV = 0x00000001,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FLIP_FACING_BIT_NV = 0x00000002,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE_BIT_NV = 0x00000004,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE_BIT_NV = 0x00000008,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_ENABLE_EXPLICIT_BOUNDING_BOX_NV = 0x00000010,
	};

	VkPartitionedAccelerationStructureInstanceFlagBitsNV  operator |= (VkPartitionedAccelerationStructureInstanceFlagBitsNV &lhs, VkPartitionedAccelerationStructureInstanceFlagBitsNV rhs) __NE___
	{
		return lhs = VkPartitionedAccelerationStructureInstanceFlagBitsNV(Base::ToNearUInt(lhs) | Base::ToNearUInt(rhs));
	}

/*
=================================================
	VkPartitionedAccelerationStructureOpTypeNV
=================================================
*/
	enum VkPartitionedAccelerationStructureOpTypeNV {
		VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_INSTANCE_NV = 0,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_UPDATE_INSTANCE_NV = 1,
		VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_PARTITION_TRANSLATION_NV = 2,
	};

#endif // not AE_ENABLE_VULKAN

#ifdef AE_ENABLE_VULKAN
	StaticAssert( sizeof(RTSceneBuild::InstanceVk) == sizeof(VkAccelerationStructureInstanceKHR) );

	StaticAssert( sizeof(RTPartitionedSceneBuild::WriteInstanceVk) == sizeof(VkPartitionedAccelerationStructureWriteInstanceDataNV) );
	StaticAssert( sizeof(RTPartitionedSceneBuild::UpdateInstanceVk) == sizeof(VkPartitionedAccelerationStructureUpdateInstanceDataNV) );
	StaticAssert( sizeof(RTPartitionedSceneBuild::WritePartitionTranslationVk) == sizeof(VkPartitionedAccelerationStructureWritePartitionTranslationDataNV) );
	StaticAssert( sizeof(RTPartitionedSceneBuild::BuildIndirectCommandVk) == sizeof(VkBuildPartitionedAccelerationStructureIndirectCommandNV) );

	StaticAssert( sizeof(VkPartitionedAccelerationStructureOpTypeNV) == sizeof(EPartitionedCmd) );
	StaticAssert( uint(VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_INSTANCE_NV) == uint(EPartitionedCmd::WriteInstance) );
	StaticAssert( uint(VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_UPDATE_INSTANCE_NV) == uint(EPartitionedCmd::UpdateInstance) );
	StaticAssert( uint(VK_PARTITIONED_ACCELERATION_STRUCTURE_OP_TYPE_WRITE_PARTITION_TRANSLATION_NV) == uint(EPartitionedCmd::WritePartitionTranslation) );
#endif

/*
=================================================
	VEnumCast (ERTInstanceOpt)
=================================================
*/
	Nd__In VkGeometryInstanceFlagBitsKHR  VEnumCast (ERTInstanceOpt values) __NE___
	{
		VkGeometryInstanceFlagBitsKHR	result = Zero;

		for (auto t : BitfieldIterate( values ))
		{
			switch_enum( t )
			{
				case ERTInstanceOpt::TriangleCullDisable :			result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;	break;
				case ERTInstanceOpt::TriangleFrontCCW :				result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FLIP_FACING_BIT_KHR;			break;
				case ERTInstanceOpt::ForceOpaque :					result |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;					break;
				case ERTInstanceOpt::ForceNonOpaque :				result |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;					break;
				case ERTInstanceOpt::DisableOpacityMicromaps :		result |= VK_GEOMETRY_INSTANCE_DISABLE_OPACITY_MICROMAPS_EXT;			break;
				case ERTInstanceOpt::ForceOpacityMicromap2State	:	result |= VK_GEOMETRY_INSTANCE_FORCE_OPACITY_MICROMAP_2_STATE_EXT;		break;

				case ERTInstanceOpt::_Last :
				case ERTInstanceOpt::All :
				case ERTInstanceOpt::Unknown :
				default_unlikely :									RETURN_ERR( "unknown RT instance options", Zero );
			}
			switch_end
		}
		return result;
	}

/*
=================================================
	VEnumCast (EPartitionedInstanceOpt)
=================================================
*/
	Nd__In VkPartitionedAccelerationStructureInstanceFlagBitsNV  VEnumCast (EPartitionedInstanceOpt values) __NE___
	{
		VkPartitionedAccelerationStructureInstanceFlagBitsNV	result = Zero;

		for (auto t : BitfieldIterate( values ))
		{
			switch_enum( t )
			{
				case EPartitionedInstanceOpt::TriangleCullDisable :	result |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE_BIT_NV;	break;
				case EPartitionedInstanceOpt::TriangleFrontCCW :	result |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FLIP_FACING_BIT_NV;			break;
				case EPartitionedInstanceOpt::ForceOpaque :			result |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE_BIT_NV;					break;
				case EPartitionedInstanceOpt::ForceNonOpaque :		result |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE_BIT_NV;				break;
				case EPartitionedInstanceOpt::EnableExplicitAABB :	result |= VK_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCE_FLAG_ENABLE_EXPLICIT_BOUNDING_BOX_NV;		break;

				case EPartitionedInstanceOpt::_Last :
				case EPartitionedInstanceOpt::All :
				case EPartitionedInstanceOpt::Unknown :
				default_unlikely :							RETURN_ERR( "unknown RT instance options", Zero );
			}
			switch_end
		}
		return result;
	}
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
	static constexpr auto	ERTInstanceOpt_OpacityMicromapMask	= ERTInstanceOpt::DisableOpacityMicromaps | ERTInstanceOpt::ForceOpacityMicromap2State;

	RTSceneBuild::InstanceVk&  RTSceneBuild::InstanceVk::SetFlags (ERTInstanceOpt value) __NE___
	{
	  #ifdef AE_DEBUG
		if ( AnyBits( value, ERTInstanceOpt_OpacityMicromapMask ))
		{
			CHECK_MSG( GraphicsScheduler().GetFeatureSet().opacityMicromap != FeatureSet::EFeature::RequireTrue,
				"'opacityMicromap' feature is not supported" );
		}
	  #endif

		this->flags = VEnumCast( value );
		return *this;
	}

	RTSceneBuild::InstanceMtl&  RTSceneBuild::InstanceMtl::SetFlags (ERTInstanceOpt value) __NE___
	{
		ASSERT( NoBits( value, ERTInstanceOpt_OpacityMicromapMask ));

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
//-----------------------------------------------------------------------------



/*
=================================================
	WriteInstanceVk::Init
=================================================
*/
	RTPartitionedSceneBuild::WriteInstanceVk&  RTPartitionedSceneBuild::WriteInstanceVk::Init () __NE___
	{
		explicitAABB.fill( 0.f );
		transform			= RTMatrixStorage::Identity();
		instanceCustomIndex	= 0;
		instanceMask		= UMax;
		instanceSBTOffset	= 0;
		instanceFlags		= 0;
		instanceIndex		= 0;
		partitionIndex		= 0;
		rtas				= Default;
		return *this;
	}

/*
=================================================
	WriteInstanceVk::SetInstanceFlags (ERTInstanceOpt)
=================================================
*/
	RTPartitionedSceneBuild::WriteInstanceVk&  RTPartitionedSceneBuild::WriteInstanceVk::SetInstanceFlags (ERTInstanceOpt value) __NE___
	{
		constexpr auto	compatible_flags	= ERTInstanceOpt::TriangleCullDisable | ERTInstanceOpt::TriangleFrontCCW |
											  ERTInstanceOpt::ForceOpaque | ERTInstanceOpt::ForceNonOpaque;
		ASSERT( NoBits( value, ~compatible_flags ));

		instanceFlags = VEnumCast( value & compatible_flags );
		return *this;
	}

/*
=================================================
	WriteInstanceVk::SetInstanceFlags (EPartitionedInstanceOpt)
=================================================
*/
	RTPartitionedSceneBuild::WriteInstanceVk&  RTPartitionedSceneBuild::WriteInstanceVk::SetInstanceFlags (EPartitionedInstanceOpt value) __NE___
	{
		instanceFlags = VEnumCast( value );
		return *this;
	}

/*
=================================================
	BuildIndirectCommandVk::Set
=================================================
*/
	bool  RTPartitionedSceneBuild::BuildIndirectCommandVk::Set (EPartitionedCmd type, uint count, BufferID id, Bytes offset, Bytes stride) __NE___
	{
		ASSERT( id );

		auto*	buf = GraphicsScheduler().GetResourceManager().GetResource( id, False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( buf != null )

		auto	addr = buf->GetDeviceAddress();
		CHECK_ERR( addr != Default );

		#if AE_VALIDATE_GCTX
		{
			const Bytes	buf_size = buf->Size();
			CHECK_ERR( offset < buf_size );

			CHECK_ERR( stride == 0 or stride >= ArgDataSize );
			stride = Max( stride, ArgDataSize );

			CHECK_ERR( stride * count + offset <= buf_size );
		}
		#endif

		opType			= type;
		argCount		= count;
		argData			= addr;
		argDataStride	= stride;

		return true;
	}

/*
=================================================
	SetGeometry (WriteInstanceVk)
=================================================
*/
	bool  RTPartitionedSceneBuild::SetGeometry (RTGeometryID id, INOUT WriteInstanceVk &inst) __NE___
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

/*
=================================================
	SetGeometry (UpdateInstanceVk)
=================================================
*/
	bool  RTPartitionedSceneBuild::SetGeometry (RTGeometryID id, INOUT UpdateInstanceVk &inst) __NE___
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


} // AE::Graphics
