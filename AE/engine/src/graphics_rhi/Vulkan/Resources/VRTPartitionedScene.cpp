// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VRTPartitionedScene.h"
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
	RTASBuildSizes  VRTPartitionedScene::GetBuildSizes (const ResourceManager &resMngr, const RTPartitionedSceneInfo &desc) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		VkPartitionedAccelerationStructureInstancesInputNV	info		= {};
		VkAccelerationStructureBuildSizesInfoKHR			size_info	= {};

		CHECK_ERR( _Convert( resMngr, desc, OUT info ));

		auto&	dev = resMngr.GetDevice();
		dev.vkGetPartitionedAccelerationStructuresBuildSizesNV( dev.GetVkDevice(), &info, OUT &size_info );

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
	bool  VRTPartitionedScene::IsSupported (const ResourceManager &resMngr, const RTPartitionedSceneInfo &) __NE___
	{
		if ( resMngr.GetFeatureSet().accelerationStructure() != FeatureSet::EFeature::RequireTrue )
			return false;

		if ( not resMngr.GetDevice().GetVExtensions().partitionedAccelStructNV )
			return false;

		// TODO
		return true;
	}

/*
=================================================
	ConvertBufferOrAddress
=================================================
*/
namespace {
	ND_ static bool  ConvertBufferOrAddress (const ResourceManager &resMngr, const RTPartitionedSceneBuild::BufferOrAddress &un,
											 OUT VkDeviceAddress &result, EBufferUsage reqUsage) __NE___
	{
		return Visit( un,
			[&result, &resMngr, reqUsage] (const RTPartitionedSceneBuild::BufferWithOffset &buf_offset) -> bool
			{
				auto*	buf = resMngr.GetResource( buf_offset.id );
				CHECK_ERR( buf != null );

				const auto	addr = buf->GetDeviceAddress();
				CHECK_ERR( addr != Default );

				const auto	desc = buf->Description();
				CHECK_ERR( AllBits( desc.usage, reqUsage ));
				CHECK_ERR( buf_offset.offset < desc.size );

				result = BitCast<VkDeviceAddress>( addr + buf_offset.offset );
				return true;
			},
			[&result] (const DeviceAddress &addr) -> bool
			{
				CHECK_ERR( addr != Default );
				result = BitCast<VkDeviceAddress>( addr );
				return true;
			},
			[&result] (NullUnion) {
				result = 0;
				return true;
			}
		);
	}
}
/*
=================================================
	_Convert
=================================================
*/
	bool  VRTPartitionedScene::_Convert (const ResourceManager &resMngr, const RTPartitionedSceneInfo &desc,
										 OUT VkPartitionedAccelerationStructureInstancesInputNV &inputInfo) __NE___
	{
		const auto&	props = resMngr.GetDevice().GetDeviceProperties();

		inputInfo.sType	= VK_STRUCTURE_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_INSTANCES_INPUT_NV;
		inputInfo.pNext	= null;

		// TODO: VkPartitionedAccelerationStructureFlagsNV

		inputInfo.instanceCount						= desc.instanceCount;
		inputInfo.maxInstancePerPartitionCount		= desc.maxInstancesPerPartition;
		inputInfo.partitionCount					= desc.partitionCount;
		inputInfo.maxInstanceInGlobalPartitionCount	= desc.maxInstancesInGlobalPartition;

		CHECK_ERR( inputInfo.partitionCount <= props.rayTracing.maxPartitionCount );

		return true;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  VRTPartitionedScene::ConvertBuildInfo (const ResourceManager &resMngr, const RTPartitionedSceneBuild &build,
												 OUT VkBuildPartitionedAccelerationStructureInfoNV &buildInfo) __NE___
	{
		const auto&	props = resMngr.GetDevice().GetDeviceProperties();

		buildInfo.sType	= VK_STRUCTURE_TYPE_BUILD_PARTITIONED_ACCELERATION_STRUCTURE_INFO_NV;
		buildInfo.pNext	= null;

		CHECK_ERR( _Convert( resMngr, build, OUT buildInfo.input ));

		CHECK_ERR( ConvertBufferOrAddress( resMngr, build.srcPartitionedScene,	OUT buildInfo.srcAccelerationStructureData,	EBufferUsage::RTAS_Storage ));
		CHECK_ERR( ConvertBufferOrAddress( resMngr, build.dstPartitionedScene,	OUT buildInfo.dstAccelerationStructureData,	EBufferUsage::RTAS_Storage ));

		CHECK_ERR( ConvertBufferOrAddress( resMngr, build.scratch,				OUT buildInfo.scratchData,					EBufferUsage::ASBuild_Scratch ));
		CHECK_ERR( ConvertBufferOrAddress( resMngr, build.srcInfos,				OUT buildInfo.srcInfos,						EBufferUsage::ASBuild_ReadOnly ));
		CHECK_ERR( ConvertBufferOrAddress( resMngr, build.srcInfosCount,		OUT buildInfo.srcInfosCount,				EBufferUsage::ASBuild_ReadOnly ));

		CHECK_ERR( IsMultipleOf( buildInfo.srcAccelerationStructureData,	POTBytes_From<256> ));
		CHECK_ERR( IsMultipleOf( buildInfo.dstAccelerationStructureData,	POTBytes_From<256> ));
		CHECK_ERR( IsMultipleOf( buildInfo.scratchData,						props.rayTracing.scratchBufferAlign ));
		CHECK_ERR( IsMultipleOf( buildInfo.srcInfosCount,					POTBytes_From<4> ));

		CHECK_ERR( buildInfo.dstAccelerationStructureData != 0 );
		CHECK_ERR( buildInfo.scratchData != 0 );
		CHECK_ERR( buildInfo.srcInfos != 0 );
		CHECK_ERR( buildInfo.srcInfosCount != 0 );

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
