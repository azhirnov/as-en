// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/BufferDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Geometry Cluster
	//

	struct VRTCluster : Noninstanceable
	{
	// types
	public:
		static constexpr auto	_InputDataSize	= Max(	sizeof(VkClusterAccelerationStructureClustersBottomLevelInputNV),
														sizeof(VkClusterAccelerationStructureTriangleClusterInputNV),
														sizeof(VkClusterAccelerationStructureMoveObjectsInputNV) );
		static constexpr auto	_InputDataAlign	= Max(	alignof(VkClusterAccelerationStructureClustersBottomLevelInputNV),
														alignof(VkClusterAccelerationStructureTriangleClusterInputNV),
														alignof(VkClusterAccelerationStructureMoveObjectsInputNV) );
		using InputStorage_t	= UntypedStorage< _InputDataSize, _InputDataAlign >;


	// methods
	public:
		ND_ static RTASBuildSizes	GetBuildSizes (const ResourceManager &, const RTClusterInfo &desc)						__NE___;

		ND_ static bool				ConvertBuildInfo (const ResourceManager &, const RTClusterBuild &,
													  OUT VkClusterAccelerationStructureCommandsInfoNV &buildInfo,
													  OUT InputStorage_t &storage)											__NE___;

		ND_ static bool				IsSupported (const ResourceManager &, const RTClusterInfo &build)						__NE___;

	private:
		ND_ static bool  _Convert (const ResourceManager &, const RTClusterInfo &desc,
								   OUT VkClusterAccelerationStructureInputInfoNV &inputInfo, OUT InputStorage_t &storage)	__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
