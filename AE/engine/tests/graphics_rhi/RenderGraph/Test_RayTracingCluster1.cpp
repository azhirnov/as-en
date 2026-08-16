// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{

	template <typename CtxTypes, typename CopyCtx>
	static bool  RayTracingCluster1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&	rts			= GraphicsScheduler();
		auto&	res_mngr	= rts.GetResourceManager();


		RTClusterInfo	info;
		info.maxRTASCount	= 1;
		info.opMode			= ERTClusterOpMode::ImplicitDst;

		auto&	input = info.SetInputType< ERTClusterOpType::BuildTriangleCluster >();
		input.vertexFormat					= EVertexType::Float3;
		input.maxGeometryIndexValue			= 1;
		input.maxClusterUniqueGeometryCount	= 1;
		input.maxClusterTriangleCount		= 128;
		input.maxClusterVertexCount			= 128;
		input.maxTotalTriangleCount			= 128;
		input.maxTotalVertexCount			= 128;
		input.minPositionTruncateBitCount	= 32;

		auto	sizes = res_mngr.GetRTClusterSizes( info );
		CHECK( sizes.rtasSize > 0 );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_RayTracingCluster1 ()
{
	if ( _rtPipelines == null or
		 GraphicsScheduler().GetFeatureSet().clusterAccelerationStructure != FeatureSet::EFeature::RequireTrue )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( RayTracingCluster1Test< DirectCtx,   DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
	RG_CHECK( RayTracingCluster1Test< DirectCtx,   IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

	RG_CHECK( RayTracingCluster1Test< IndirectCtx, DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
	RG_CHECK( RayTracingCluster1Test< IndirectCtx, IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // not AE_ENABLE_METAL
