// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VRTCluster.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTASBuildSizes	VRTCluster::GetBuildSizes (const VResourceManager &resMngr, const RTClusterInfo &desc) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		VkClusterAccelerationStructureInputInfoNV	info		= {};
		VkAccelerationStructureBuildSizesInfoKHR	size_info	= {};
		InputStorage_t								input_data;

		CHECK_ERR( _Convert( resMngr, desc, OUT info, OUT input_data ));

		auto&	dev = resMngr.GetDevice();
		dev.vkGetClusterAccelerationStructureBuildSizesNV( dev.GetVkDevice(), &info, OUT &size_info );

		RTASBuildSizes	res;
		res.rtasSize			= Bytes{size_info.accelerationStructureSize};
		res.buildScratchSize	= Bytes{size_info.buildScratchSize};
		res.updateScratchSize	= Bytes{size_info.updateScratchSize};
		return res;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VRTCluster::IsSupported (const VResourceManager &resMngr, const RTClusterInfo &) __NE___
	{
		if ( resMngr.GetFeatureSet().accelerationStructure() != FeatureSet::EFeature::RequireTrue )
			return false;

		if ( not resMngr.GetDevice().GetVExtensions().clusterAccelStructNV )
			return false;

		// TODO
		return true;
	}

/*
=================================================
	_Convert
=================================================
*/
	bool  VRTCluster::_Convert (const VResourceManager &resMngr, const RTClusterInfo &desc,
								OUT VkClusterAccelerationStructureInputInfoNV &inputInfo, OUT InputStorage_t &storage) __NE___
	{
		inputInfo.sType	= VK_STRUCTURE_TYPE_CLUSTER_ACCELERATION_STRUCTURE_INPUT_INFO_NV;
		inputInfo.pNext	= null;

		inputInfo.maxAccelerationStructureCount = desc.maxRTASCount;
		inputInfo.flags							= VEnumCast( desc.options );
		inputInfo.opType						= VEnumCast( desc.opType );
		inputInfo.opMode						= VEnumCast( desc.opMode );

		CHECK_ERR( inputInfo.maxAccelerationStructureCount > 0 );
		CHECK_ERR( inputInfo.opType != VK_CLUSTER_ACCELERATION_STRUCTURE_OP_TYPE_MAX_ENUM_NV );
		CHECK_ERR( inputInfo.opMode != VK_CLUSTER_ACCELERATION_STRUCTURE_OP_MODE_MAX_ENUM_NV );

		auto&	props	= resMngr.GetDevice().GetVProperties().clusterAccelStructNVProps;
		auto&	fs		= resMngr.GetFeatureSet();

		switch_enum( desc.opType )
		{
			case ERTClusterOpType::MoveObjects :
			{
				CHECK_ERR( HoldsAlternative< RTClusterInfo::MoveObjectsInput >( desc.opInput ));

				auto&	src				= *UnionGet< RTClusterInfo::MoveObjectsInput >( desc.opInput );
				auto&	dst_input		= storage.Ref<VkClusterAccelerationStructureMoveObjectsInputNV>();
				dst_input.sType			= VK_STRUCTURE_TYPE_CLUSTER_ACCELERATION_STRUCTURE_MOVE_OBJECTS_INPUT_NV;
				dst_input.pNext			= null;
				dst_input.type			= VEnumCast( src.type );
				dst_input.noMoveOverlap	= src.noMoveOverlap;
				dst_input.maxMovedBytes	= VkDeviceSize{src.maxMovedBytes};

				inputInfo.opInput.pMoveObjects = &dst_input;
				break;
			}

			case ERTClusterOpType::BuildClustersGeometry :
			{
				CHECK_ERR( HoldsAlternative< RTClusterInfo::RTGeometryInput >( desc.opInput ));

				auto&	src			= *UnionGet< RTClusterInfo::RTGeometryInput >( desc.opInput );
				auto&	dst_input	= storage.Ref<VkClusterAccelerationStructureClustersBottomLevelInputNV>();
				dst_input.sType		= VK_STRUCTURE_TYPE_CLUSTER_ACCELERATION_STRUCTURE_CLUSTERS_BOTTOM_LEVEL_INPUT_NV;
				dst_input.pNext		= null;
				dst_input.maxTotalClusterCount = src.maxTotalClusterCount;
				dst_input.maxClusterCountPerAccelerationStructure = src.maxClusterPerAS;

				inputInfo.opInput.pClustersBottomLevel = &dst_input;
				break;
			}

			case ERTClusterOpType::BuildTriangleCluster :
			case ERTClusterOpType::BuildTriangleClusterTemplate :
			case ERTClusterOpType::InstantiateTriangleCluster :
			{
				CHECK_ERR( HoldsAlternative< RTClusterInfo::TriangleClusterInput >( desc.opInput ));

				auto&	src			= *UnionGet< RTClusterInfo::TriangleClusterInput >( desc.opInput );
				auto&	dst_input	= storage.Ref<VkClusterAccelerationStructureTriangleClusterInputNV>();
				dst_input.sType	= VK_STRUCTURE_TYPE_CLUSTER_ACCELERATION_STRUCTURE_TRIANGLE_CLUSTER_INPUT_NV;
				dst_input.pNext	= null;
				dst_input.vertexFormat					= VEnumCast( src.vertexFormat );
				dst_input.maxGeometryIndexValue			= src.maxGeometryIndexValue;
				dst_input.maxClusterUniqueGeometryCount	= src.maxClusterUniqueGeometryCount;
				dst_input.maxClusterTriangleCount		= src.maxClusterTriangleCount;
				dst_input.maxClusterVertexCount			= src.maxClusterVertexCount;
				dst_input.maxTotalTriangleCount			= src.maxTotalTriangleCount;
				dst_input.maxTotalVertexCount			= src.maxTotalVertexCount;
				dst_input.minPositionTruncateBitCount	= src.minPositionTruncateBitCount;

				CHECK_ERR( dst_input.vertexFormat != VK_FORMAT_MAX_ENUM );
				CHECK_ERR( fs.accelStructVertexFormats.contains( src.vertexFormat ));

				CHECK_ERR( dst_input.maxClusterTriangleCount	<= props.maxTrianglesPerCluster );
				CHECK_ERR( dst_input.maxClusterVertexCount		<= props.maxVerticesPerCluster );
				CHECK_ERR( dst_input.maxGeometryIndexValue		<= props.maxClusterGeometryIndex );

				inputInfo.opInput.pTriangleClusters = &dst_input;
				break;
			}

			case ERTClusterOpType::Unknown :
			default :
				RETURN_ERR( "unsupported cluster op type" );
		}
		switch_end

		return true;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  VRTCluster::ConvertBuildInfo (const VResourceManager &resMngr, const RTClusterBuild &cmd,
										OUT VkClusterAccelerationStructureCommandsInfoNV &buildInfo, OUT InputStorage_t &storage) __NE___
	{
		buildInfo.sType	= VK_STRUCTURE_TYPE_CLUSTER_ACCELERATION_STRUCTURE_COMMANDS_INFO_NV;
		buildInfo.pNext	= null;

		CHECK_ERR( _Convert( resMngr, cmd, OUT buildInfo.input, OUT storage ));

	  #if AE_VALIDATE_GCTX
		const RTASBuildSizes	build_sizes = GetBuildSizes( resMngr, cmd );
	  #endif

		// scratch buffer
		{
			auto*	buf = resMngr.GetResource( cmd.scratch.id );
			CHECK_ERR( buf != null );

			CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::ASBuild_Scratch ));

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.scratchData = BitCast<VkDeviceAddress>( addr + cmd.scratch.offset );
		}

		// dstImplicitData
		if ( cmd.opMode == ERTClusterOpMode::ImplicitDst )
		{
			auto*	buf = resMngr.GetResource( cmd.dstImplicitData.id );
			CHECK_ERR( buf != null );

			const Bytes		buf_size = buf->Size();
			CHECK_ERR( cmd.dstImplicitData.offset < buf_size );

			#if AE_VALIDATE_GCTX
			{
				Bytes	size;
				if ( cmd.opType == ERTClusterOpType::MoveObjects )
					size = build_sizes.rtasSize;	// TODO
				else
					size = build_sizes.rtasSize;

				CHECK_ERR( cmd.dstImplicitData.offset + size <= buf_size );
				CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::RTAS_Storage ));
			}
			#endif

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.dstImplicitData = BitCast<VkDeviceAddress>( addr + cmd.dstImplicitData.offset );
		}
		else
		{
			buildInfo.dstImplicitData = {};
		}

		// dstAddressesArray
		if ( cmd.opMode == ERTClusterOpMode::ExplicitDst )
		{
			auto*	buf = resMngr.GetResource( cmd.dstAddressesArray.id );
			CHECK_ERR( buf != null );

			const Bytes		buf_size = buf->Size();
			CHECK_ERR( cmd.dstAddressesArray.offset < buf_size );
			CHECK_ERR( cmd.dstAddressesArray.offset + cmd.dstAddressesArray.size <= buf_size );

			CHECK_ERR( cmd.dstAddressesArray.stride >= 8 );		// from specs
			CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::RTAS_Storage ));

			// TODO: validate size

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.dstAddressesArray.deviceAddress	= BitCast<VkDeviceAddress>( addr + cmd.dstAddressesArray.offset );
			buildInfo.dstAddressesArray.size			= VkDeviceSize{cmd.dstAddressesArray.size};
			buildInfo.dstAddressesArray.stride			= VkDeviceSize{cmd.dstAddressesArray.stride};
		}
		else
		{
			buildInfo.dstAddressesArray = {};
		}

		// dstSizesArray (optional)
		if ( cmd.dstSizesArray.id )
		{
			auto*	buf = resMngr.GetResource( cmd.dstSizesArray.id );
			CHECK_ERR( buf != null );

			const Bytes		buf_size = buf->Size();
			CHECK_ERR( cmd.dstSizesArray.offset < buf_size );
			CHECK_ERR( cmd.dstSizesArray.offset + cmd.dstSizesArray.size <= buf_size );

			CHECK_ERR( cmd.dstSizesArray.stride >= 4 );		// from specs
			CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::RTAS_Storage ));

			// TODO: validate size

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.dstSizesArray.deviceAddress	= BitCast<VkDeviceAddress>( addr + cmd.dstSizesArray.offset );
			buildInfo.dstSizesArray.size			= VkDeviceSize{cmd.dstSizesArray.size};
			buildInfo.dstSizesArray.stride			= VkDeviceSize{cmd.dstSizesArray.stride};
		}
		else
		{
			buildInfo.dstSizesArray = {};
		}

		// srcInfosArray
		{
			auto*	buf = resMngr.GetResource( cmd.srcInfosArray.id );
			CHECK_ERR( buf != null );

			const Bytes		buf_size = buf->Size();
			CHECK_ERR( cmd.srcInfosArray.offset < buf_size );
			CHECK_ERR( cmd.srcInfosArray.offset + cmd.srcInfosArray.size <= buf_size );

			CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::ASBuild_ReadOnly ));

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.srcInfosArray.deviceAddress	= BitCast<VkDeviceAddress>( addr + cmd.srcInfosArray.offset );
			buildInfo.srcInfosArray.size			= VkDeviceSize{cmd.srcInfosArray.size};
			buildInfo.srcInfosArray.stride			= VkDeviceSize{cmd.srcInfosArray.stride};
		}

		// srcInfosCount
		{
			auto*	buf = resMngr.GetResource( cmd.srcInfosCount.id );
			CHECK_ERR( buf != null );

			CHECK_ERR( AllBits( buf->Description().usage, EBufferUsage::ASBuild_ReadOnly ));

			DeviceAddress	addr = buf->GetDeviceAddress();
			CHECK_ERR( addr != Default );

			buildInfo.srcInfosCount = BitCast<VkDeviceAddress>( addr + cmd.srcInfosCount.offset );
		}

		buildInfo.addressResolutionFlags = VEnumCast( cmd.addressResolution );

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
