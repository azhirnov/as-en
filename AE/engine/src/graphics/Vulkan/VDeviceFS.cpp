// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
namespace
{
	// https://vulkan.gpuinfo.org/displayextensionproperty.php?extensionname=VK_KHR_maintenance3&extensionproperty=maxPerSetDescriptors&platform=all
	static constexpr uint	c_MaxPerSetDescriptors = 512;
}

/*
=================================================
	InitFeatureSet
=================================================
*/
	void  VDevice::InitFeatureSet (OUT FeatureSet &outFeatureSet) C_NE___
	{
		StaticAssert( sizeof(FeatureSet) == 696 );

		using EFeature = FeatureSet::EFeature;
		const EFeature	True	= EFeature::RequireTrue;
		const EFeature	False	= EFeature::RequireFalse;

		outFeatureSet.SetAll( False );

		const auto&	feats10	= _properties.features;		// Vulkan 1.0 core features
		const auto&	limits	= _properties.properties.limits;

		EShaderStages	all_stages = EShaderStages::Vertex | EShaderStages::Fragment | EShaderStages::Compute;
		if ( feats10.geometryShader )			all_stages |= EShaderStages::Geometry;
		if ( feats10.tessellationShader )		all_stages |= EShaderStages::TessControl | EShaderStages::TessEvaluation;
		if ( _extensions.meshShader )			all_stages |= EShaderStages::MeshTask | EShaderStages::Mesh;
		if ( _extensions.rayTracingPipeline )	all_stages |= EShaderStages::AllRayTracing;

		#define SET_FEAT2( _name_, _feat_ )	outFeatureSet._name_ = _feat_._name_ ? True : False
		#define SET_FEAT( _name_ )			outFeatureSet._name_ = feats10._name_  ? True : False

		SET_FEAT( alphaToOne );
		SET_FEAT( depthBiasClamp );
		SET_FEAT( depthBounds );
		SET_FEAT( depthClamp );
		SET_FEAT( dualSrcBlend );
		SET_FEAT( fillModeNonSolid );
		SET_FEAT( independentBlend );
		SET_FEAT( logicOp );
		SET_FEAT( sampleRateShading );
		SET_FEAT( variableMultisampleRate );

		if ( _extensions.subgroup )
		{
			CHECK( _properties.subgroupProperties.subgroupSize > 0 );
			CHECK( IsPowerOfTwo( _properties.subgroupProperties.subgroupSize ));

			outFeatureSet.subgroup			= True;
			outFeatureSet.subgroupStages	= AEEnumCast( VkShaderStageFlagBits(_properties.subgroupProperties.supportedStages) ) & all_stages;
			outFeatureSet.subgroupQuadStages= _properties.subgroupProperties.quadOperationsInAllStages ? all_stages : EShaderStages::Fragment | EShaderStages::Compute;
			outFeatureSet.minSubgroupSize	= CheckCast<ushort>(_properties.subgroupProperties.subgroupSize);
			outFeatureSet.maxSubgroupSize	= CheckCast<ushort>(_properties.subgroupProperties.subgroupSize);
			outFeatureSet.subgroupTypes		= ESubgroupTypes::Float32 | ESubgroupTypes::Int32;

			for (auto f : BitfieldIterate( VkSubgroupFeatureFlagBits( _properties.subgroupProperties.supportedOperations )))
			{
				switch_enum( f )
				{
					case VK_SUBGROUP_FEATURE_BASIC_BIT :			outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Basic_Begin,				ESubgroupOperation::_Basic_End );			break;
					case VK_SUBGROUP_FEATURE_VOTE_BIT :				outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Vote_Begin,				ESubgroupOperation::_Vote_End );			break;
					case VK_SUBGROUP_FEATURE_ARITHMETIC_BIT :		outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Arithmetic_Begin,		ESubgroupOperation::_Arithmetic_End );		break;
					case VK_SUBGROUP_FEATURE_BALLOT_BIT :			outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Ballot_Begin,			ESubgroupOperation::_Ballot_End );			break;
					case VK_SUBGROUP_FEATURE_SHUFFLE_BIT :			outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Shuffle_Begin,			ESubgroupOperation::_Shuffle_End );			break;
					case VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT :	outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_ShuffleRelative_Begin,	ESubgroupOperation::_ShuffleRelative_End );	break;
					case VK_SUBGROUP_FEATURE_CLUSTERED_BIT :		outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Clustered_Begin,			ESubgroupOperation::_Clustered_End );		break;
					case VK_SUBGROUP_FEATURE_QUAD_BIT :				outFeatureSet.subgroupOperations.InsertRange( ESubgroupOperation::_Quad_Begin,				ESubgroupOperation::_Quad_End );			break;
					case VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV :
					case VK_SUBGROUP_FEATURE_ROTATE_BIT_KHR :
					case VK_SUBGROUP_FEATURE_ROTATE_CLUSTERED_BIT_KHR :	break;	// TODO
					case VK_SUBGROUP_FEATURE_FLAG_BITS_MAX_ENUM :
					default_unlikely :								DBG_WARNING( "unknown subgroup feature" );	break;
				}
				switch_end
			}

			outFeatureSet.subgroupBroadcastDynamicId = _vkDeviceVersion >= DeviceVersion{1,2} ? True : False;
		}

		if ( _extensions.subgroupSizeControl )
		{
			CHECK( _properties.subgroupSizeControlFeats.subgroupSizeControl );
			CHECK( IsPowerOfTwo( _properties.subgroupSizeControlProps.minSubgroupSize ));
			CHECK( IsPowerOfTwo( _properties.subgroupSizeControlProps.maxSubgroupSize ));
			CHECK( outFeatureSet.minSubgroupSize <= _properties.subgroupProperties.subgroupSize );
			CHECK( outFeatureSet.maxSubgroupSize >= _properties.subgroupProperties.subgroupSize );

			outFeatureSet.subgroupSizeControl		= True;
			outFeatureSet.requiredSubgroupSizeStages= AEEnumCast( VkShaderStageFlagBits(_properties.subgroupSizeControlProps.requiredSubgroupSizeStages) ) & all_stages;
			outFeatureSet.minSubgroupSize			= CheckCast<ushort>(_properties.subgroupSizeControlProps.minSubgroupSize);
			outFeatureSet.maxSubgroupSize			= CheckCast<ushort>(_properties.subgroupSizeControlProps.maxSubgroupSize);
		}

		if ( _extensions.subgroupExtendedTypes )
		{
			CHECK( _properties.subgroupExtendedTypesFeats.shaderSubgroupExtendedTypes );

			if ( feats10.shaderInt16 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int16;
			if ( feats10.shaderInt64 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int64;

			if ( _extensions.shaderFloat16Int8 ) {
				if ( _properties.shaderFloat16Int8Feats.shaderInt8 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int8;
				if ( _properties.shaderFloat16Int8Feats.shaderFloat16 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Float16;
			}
		}

		/*if ( _extensions.shaderSubgroupUniformControlFlow )
		{
			CHECK( _properties.shaderSubgroupUniformControlFlowFeats.shaderSubgroupUniformControlFlow );
			outFeatureSet.shaderSubgroupUniformControlFlow = True;
		}*/

		if ( _extensions.shaderFloat16Int8 )
		{
			SET_FEAT2( shaderInt8,		_properties.shaderFloat16Int8Feats );
			SET_FEAT2( shaderFloat16,	_properties.shaderFloat16Int8Feats );
		}
		SET_FEAT( shaderInt16 );
		SET_FEAT( shaderInt64 );
		SET_FEAT( shaderFloat64 );

		if ( _extensions.storage16bit )
		{
			SET_FEAT2( storageBuffer16BitAccess,			_properties.storage16bitFeats );
			SET_FEAT2( uniformAndStorageBuffer16BitAccess,	_properties.storage16bitFeats );
			SET_FEAT2( storagePushConstant16,				_properties.storage16bitFeats );
			SET_FEAT2( storageInputOutput16,				_properties.storage16bitFeats );
		}

		if ( _extensions.storage8bits )
		{
			SET_FEAT2( storageBuffer8BitAccess,				_properties.storage8bitsFeats );
			SET_FEAT2( uniformAndStorageBuffer8BitAccess,	_properties.storage8bitsFeats );
			SET_FEAT2( storagePushConstant8,				_properties.storage8bitsFeats );
		}

		if ( _extensions.uniformBufferStandardLayout )
			SET_FEAT2( uniformBufferStandardLayout, _properties.uniformBufferStandardLayoutFeats );

		if ( _extensions.scalarBlockLayout )
			SET_FEAT2( scalarBlockLayout, _properties.scalarBlockLayoutFeats );

		SET_FEAT( fragmentStoresAndAtomics );
		SET_FEAT( vertexPipelineStoresAndAtomics );

		if ( _extensions.shaderImageAtomicInt64 )
			SET_FEAT2( shaderImageInt64Atomics, _properties.shaderImageAtomicInt64Feats );

		if ( _extensions.shaderAtomicInt64 )
		{
			SET_FEAT2( shaderBufferInt64Atomics,	_properties.shaderAtomicInt64Feats );
			SET_FEAT2( shaderSharedInt64Atomics,	_properties.shaderAtomicInt64Feats );
		}

		if ( _extensions.shaderAtomicFloat )
		{
			SET_FEAT2( shaderBufferFloat32Atomics,		_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderBufferFloat32AtomicAdd,	_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderBufferFloat64Atomics,		_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderBufferFloat64AtomicAdd,	_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderSharedFloat32Atomics,		_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderSharedFloat32AtomicAdd,	_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderSharedFloat64Atomics,		_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderSharedFloat64AtomicAdd,	_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderImageFloat32Atomics,		_properties.shaderAtomicFloatFeats );
			SET_FEAT2( shaderImageFloat32AtomicAdd,		_properties.shaderAtomicFloatFeats );
		}

		if ( _extensions.shaderAtomicFloat2 )
		{
			SET_FEAT2( shaderBufferFloat16Atomics,		_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderBufferFloat16AtomicAdd,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderBufferFloat16AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderBufferFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderBufferFloat64AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderSharedFloat16Atomics,		_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderSharedFloat16AtomicAdd,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderSharedFloat16AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderSharedFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderSharedFloat64AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
			SET_FEAT2( shaderImageFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		}

		if ( _extensions.shaderViewportIndexLayer )
		{
			outFeatureSet.shaderOutputViewportIndex	= True; // same as in VkPhysicalDeviceVulkan12Features
			outFeatureSet.shaderOutputLayer			= True;
		}

		if ( _extensions.shaderClock )
		{
			SET_FEAT2( shaderSubgroupClock,	_properties.shaderClockFeats );
			SET_FEAT2( shaderDeviceClock,	_properties.shaderClockFeats );
		}

		if ( _extensions.cooperativeMatrix )
		{
			outFeatureSet.cooperativeMatrix			= _properties.cooperativeMatrixFeats.cooperativeMatrix ? True : False;
			outFeatureSet.cooperativeMatrixStages	= AEEnumCast( VkShaderStageFlagBits( _properties.cooperativeMatrixProps.cooperativeMatrixSupportedStages )) & all_stages;

			// use vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR to get supported props
		}

		if ( _extensions.bufferDeviceAddress )
			SET_FEAT2( bufferDeviceAddress, _properties.bufferDeviceAddressFeats );

		SET_FEAT( shaderClipDistance );
		SET_FEAT( shaderCullDistance );
		SET_FEAT( shaderResourceMinLod );

		outFeatureSet.shaderDrawParameters	= _extensions.shaderDrawParams		? True : False;
		outFeatureSet.shaderSMBuiltinsNV	= _extensions.shaderSMBuiltinsNV	? True : False;
		outFeatureSet.shaderCoreBuiltinsARM	= _extensions.shaderCoreBuiltinsARM	? True : False;
		outFeatureSet.shaderStencilExport	= _extensions.shaderStencilExport	? True : False;

		SET_FEAT( shaderUniformBufferArrayDynamicIndexing );
		SET_FEAT( shaderSampledImageArrayDynamicIndexing );
		SET_FEAT( shaderStorageBufferArrayDynamicIndexing );
		SET_FEAT( shaderStorageImageArrayDynamicIndexing );

		if ( _extensions.descriptorIndexing )
		{
			SET_FEAT2( shaderInputAttachmentArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformTexelBufferArrayDynamicIndexing,	_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageTexelBufferArrayDynamicIndexing,	_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderSampledImageArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageImageArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderInputAttachmentArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformTexelBufferArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageTexelBufferArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
			SET_FEAT2( runtimeDescriptorArray,							_properties.descriptorIndexingFeats );

			if ( not _properties.descriptorIndexingProps.shaderUniformBufferArrayNonUniformIndexingNative )
				outFeatureSet.shaderUniformBufferArrayNonUniformIndexing = False;

			if ( not _properties.descriptorIndexingProps.shaderSampledImageArrayNonUniformIndexingNative )
				outFeatureSet.shaderSampledImageArrayNonUniformIndexing = False;

			if ( not _properties.descriptorIndexingProps.shaderStorageBufferArrayNonUniformIndexingNative )
				outFeatureSet.shaderStorageBufferArrayNonUniformIndexing = False;

			if ( not _properties.descriptorIndexingProps.shaderStorageImageArrayNonUniformIndexingNative )
				outFeatureSet.shaderStorageImageArrayNonUniformIndexing = False;

			if ( not _properties.descriptorIndexingProps.shaderInputAttachmentArrayNonUniformIndexingNative )
				outFeatureSet.shaderInputAttachmentArrayNonUniformIndexing = False;
		}

		SET_FEAT( shaderStorageImageMultisample );
		SET_FEAT( shaderStorageImageReadWithoutFormat );
		SET_FEAT( shaderStorageImageWriteWithoutFormat );

		if ( _extensions.memoryModel )
		{
			SET_FEAT2( vulkanMemoryModel,								_properties.memoryModelFeats );
			SET_FEAT2( vulkanMemoryModelDeviceScope,					_properties.memoryModelFeats );
			SET_FEAT2( vulkanMemoryModelAvailabilityVisibilityChains,	_properties.memoryModelFeats );
		}

		if ( _extensions.shaderDemoteToHelperInvocation )
			SET_FEAT2( shaderDemoteToHelperInvocation, _properties.shaderDemoteToHelperInvocationFeats );

		if ( _extensions.shaderTerminateInvocation )
			SET_FEAT2( shaderTerminateInvocation, _properties.shaderTerminateInvocationFeats );

		if ( _extensions.zeroInitializeWorkgroupMem )
			SET_FEAT2( shaderZeroInitializeWorkgroupMemory, _properties.zeroInitializeWorkgroupMemFeats );

		// TODO: shaderIntegerDotProduct

		if ( _extensions.fragShaderInterlock )
		{
			SET_FEAT2( fragmentShaderSampleInterlock,		_properties.fragShaderInterlockFeats );
			SET_FEAT2( fragmentShaderPixelInterlock,		_properties.fragShaderInterlockFeats );
			SET_FEAT2( fragmentShaderShadingRateInterlock,	_properties.fragShaderInterlockFeats );
		}

		if ( _extensions.fragmentBarycentric )
			SET_FEAT2( fragmentShaderBarycentric,		_properties.fragmentBarycentricFeats );

		if ( _extensions.fragShadingRate )
		{
			SET_FEAT2( pipelineFragmentShadingRate,		_properties.fragShadingRateFeats );
			SET_FEAT2( primitiveFragmentShadingRate,	_properties.fragShadingRateFeats );
			SET_FEAT2( attachmentFragmentShadingRate,	_properties.fragShadingRateFeats );

			SET_FEAT2( primitiveFragmentShadingRateWithMultipleViewports,	_properties.fragShadingRateProps );
			SET_FEAT2( layeredShadingRateAttachments,						_properties.fragShadingRateProps );
			SET_FEAT2( fragmentShadingRateWithShaderDepthStencilWrites,		_properties.fragShadingRateProps );
			SET_FEAT2( fragmentShadingRateWithSampleMask,					_properties.fragShadingRateProps );
			SET_FEAT2( fragmentShadingRateWithShaderSampleMask,				_properties.fragShadingRateProps );
			SET_FEAT2( fragmentShadingRateWithFragmentShaderInterlock,		_properties.fragShadingRateProps );
			SET_FEAT2( fragmentShadingRateWithCustomSampleLocations,		_properties.fragShadingRateProps );

			StaticArray< VkPhysicalDeviceFragmentShadingRateKHR, 12 >	shading_rates	= {};
			uint														count			= uint(shading_rates.size());
			for (auto& sr : shading_rates) {
				sr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_KHR;
			}

			VK_CHECK( vkGetPhysicalDeviceFragmentShadingRatesKHR( _vkPhysicalDevice, INOUT &count, OUT shading_rates.data() ));

			for (uint i = 0; i < count; ++i)
			{
				EShadingRate	size	= EShadingRate_FromSize({ shading_rates[i].fragmentSize.width, shading_rates[i].fragmentSize.height });
				EShadingRate	samples = EShadingRate_FromSampleBits( shading_rates[i].sampleCounts );
				uint2			size2	= EShadingRate_Size( size | samples );

				Unused( size2 );
				ASSERT( size2.x == shading_rates[i].fragmentSize.width );
				ASSERT( size2.y == shading_rates[i].fragmentSize.height );

				outFeatureSet.fragmentShadingRates.push_back( size | samples );
			}
			std::sort( outFeatureSet.fragmentShadingRates.begin(), outFeatureSet.fragmentShadingRates.end() );

			outFeatureSet.fragmentShadingRateTexelSize.minX		= POTValue{_properties.fragShadingRateProps.minFragmentShadingRateAttachmentTexelSize.width }.GetPOT();
			outFeatureSet.fragmentShadingRateTexelSize.minY		= POTValue{_properties.fragShadingRateProps.minFragmentShadingRateAttachmentTexelSize.height}.GetPOT();
			outFeatureSet.fragmentShadingRateTexelSize.maxX		= POTValue{_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSize.width }.GetPOT();
			outFeatureSet.fragmentShadingRateTexelSize.maxY		= POTValue{_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSize.height}.GetPOT();
			outFeatureSet.fragmentShadingRateTexelSize.aspect	= POTValue{_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSizeAspectRatio}.GetPOT();
		}

		if ( _extensions.rayQuery and _extensions.accelerationStructure )
		{
			SET_FEAT2( rayQuery,			_properties.rayQueryFeats );
			outFeatureSet.rayQueryStages =	all_stages;
		}

		if ( _extensions.rayTracingPipeline and _extensions.accelerationStructure )
		{
			SET_FEAT2( rayTracingPipeline,				_properties.rayTracingPipelineFeats );
			SET_FEAT2( rayTraversalPrimitiveCulling,	_properties.rayTracingPipelineFeats );
			outFeatureSet.maxRayRecursionDepth		=	_properties.rayTracingPipelineProps.maxRayRecursionDepth;
		}

		outFeatureSet.maxShaderVersion.spirv = (_spirvVersion.major * 100) + (_spirvVersion.minor * 10);

		SET_FEAT( drawIndirectFirstInstance );
		if ( _extensions.drawIndirectCount )
			outFeatureSet.drawIndirectCount = True;

		if ( _extensions.multiview )
		{
			SET_FEAT2( multiview,					_properties.multiviewFeats );
			SET_FEAT2( multiviewGeometryShader,		_properties.multiviewFeats );
			SET_FEAT2( multiviewTessellationShader,	_properties.multiviewFeats );
			outFeatureSet.maxMultiviewViewCount		= _properties.multiviewProps.maxMultiviewViewCount;
		}

		SET_FEAT( multiViewport );
		outFeatureSet.maxViewports = limits.maxViewports;

		if ( _extensions.sampleLocations )
		{
			outFeatureSet.sampleLocations = True;
			SET_FEAT2( variableSampleLocations, _properties.sampleLocationsProps );
		}

		outFeatureSet.perDescrSet_maxUniformBuffersDynamic	= limits.maxDescriptorSetUniformBuffersDynamic;
		outFeatureSet.perDescrSet_maxStorageBuffersDynamic	= limits.maxDescriptorSetStorageBuffersDynamic;
		outFeatureSet.perDescrSet.maxInputAttachments		= limits.maxDescriptorSetInputAttachments;
		outFeatureSet.perDescrSet.maxSampledImages			= limits.maxDescriptorSetSampledImages;
		outFeatureSet.perDescrSet.maxSamplers				= limits.maxDescriptorSetSamplers;
		outFeatureSet.perDescrSet.maxStorageBuffers			= limits.maxDescriptorSetStorageBuffers;
		outFeatureSet.perDescrSet.maxStorageImages			= limits.maxDescriptorSetStorageImages;
		outFeatureSet.perDescrSet.maxUniformBuffers			= limits.maxDescriptorSetUniformBuffers;
		outFeatureSet.perDescrSet.maxTotalResources			= _extensions.maintenance3 ? _properties.maintenance3Props.maxPerSetDescriptors : c_MaxPerSetDescriptors;

		outFeatureSet.perStage.maxInputAttachments			= limits.maxPerStageDescriptorInputAttachments;
		outFeatureSet.perStage.maxSampledImages				= limits.maxPerStageDescriptorSampledImages;
		outFeatureSet.perStage.maxSamplers					= limits.maxPerStageDescriptorSamplers;
		outFeatureSet.perStage.maxStorageBuffers			= limits.maxPerStageDescriptorStorageBuffers;
		outFeatureSet.perStage.maxStorageImages				= limits.maxPerStageDescriptorStorageImages;
		outFeatureSet.perStage.maxUniformBuffers			= limits.maxPerStageDescriptorUniformBuffers;
		outFeatureSet.perStage.maxTotalResources			= limits.maxPerStageResources;

		if ( _extensions.accelerationStructure )
		{
			SET_FEAT2( accelerationStructureIndirectBuild, _properties.accelerationStructureFeats );

			outFeatureSet.perDescrSet.maxAccelStructures	= _properties.accelerationStructureProps.maxDescriptorSetAccelerationStructures;
			outFeatureSet.perStage.maxAccelStructures		= _properties.accelerationStructureProps.maxPerStageDescriptorAccelerationStructures;
		}

		outFeatureSet.maxTexelBufferElements= limits.maxTexelBufferElements;
		outFeatureSet.maxUniformBufferSize	= limits.maxUniformBufferRange;
		outFeatureSet.maxStorageBufferSize	= limits.maxStorageBufferRange;
		outFeatureSet.maxDescriptorSets		= CheckCast<ushort>(limits.maxBoundDescriptorSets);
		outFeatureSet.maxTexelOffset		= CheckCast<ushort>(Min( limits.maxTexelOffset, Max( Abs(limits.minTexelOffset)-1, 0 )));
		outFeatureSet.maxTexelGatherOffset	= CheckCast<ushort>(Min( limits.maxTexelGatherOffset, Max( Abs(limits.minTexelGatherOffset)-1, 0 )));

		outFeatureSet.maxFragmentOutputAttachments			= CheckCast<ushort>(limits.maxFragmentOutputAttachments);
		outFeatureSet.maxFragmentDualSrcAttachments			= CheckCast<ushort>(limits.maxFragmentDualSrcAttachments);
		outFeatureSet.maxFragmentCombinedOutputResources	= limits.maxFragmentCombinedOutputResources;
		outFeatureSet.maxPushConstantsSize					= limits.maxPushConstantsSize;

		if ( _extensions.portabilitySubset )
		{
			SET_FEAT2( constantAlphaColorBlendFactors,			_properties.portabilitySubsetFeats );
			SET_FEAT2( shaderSampleRateInterpolationFunctions,	_properties.portabilitySubsetFeats );
			SET_FEAT2( pointPolygons,							_properties.portabilitySubsetFeats );
			SET_FEAT2( tessellationIsolines,					_properties.portabilitySubsetFeats );
			SET_FEAT2( tessellationPointMode,					_properties.portabilitySubsetFeats );
			SET_FEAT2( multisampleArrayImage,					_properties.portabilitySubsetFeats );
			SET_FEAT2( samplerMipLodBias,						_properties.portabilitySubsetFeats );
			SET_FEAT2( triangleFans,							_properties.portabilitySubsetFeats );
		}
		else
		{
			outFeatureSet.constantAlphaColorBlendFactors		= True;
			outFeatureSet.shaderSampleRateInterpolationFunctions= outFeatureSet.sampleRateShading;
			outFeatureSet.pointPolygons							= True;
			outFeatureSet.tessellationIsolines					= True;
			outFeatureSet.tessellationPointMode					= True;
			outFeatureSet.multisampleArrayImage					= True;
			outFeatureSet.samplerMipLodBias						= True;
			outFeatureSet.triangleFans							= True;
		}

		outFeatureSet.computeShader						= True;
		outFeatureSet.maxComputeSharedMemorySize		= limits.maxComputeSharedMemorySize;
		outFeatureSet.maxComputeWorkGroupInvocations	= limits.maxComputeWorkGroupInvocations;
		outFeatureSet.maxComputeWorkGroupSizeX			= Min( limits.maxComputeWorkGroupSize[0], limits.maxComputeWorkGroupInvocations );
		outFeatureSet.maxComputeWorkGroupSizeY			= Min( limits.maxComputeWorkGroupSize[1], limits.maxComputeWorkGroupInvocations );
		outFeatureSet.maxComputeWorkGroupSizeZ			= Min( limits.maxComputeWorkGroupSize[2], limits.maxComputeWorkGroupInvocations );

		if ( _extensions.meshShader )
		{
			SET_FEAT2( taskShader,	_properties.meshShaderFeats );
			SET_FEAT2( meshShader,	_properties.meshShaderFeats );

			outFeatureSet.maxTaskWorkGroupSize					= Min(	_properties.meshShaderProps.maxTaskWorkGroupSize[0],
																		_properties.meshShaderProps.maxTaskWorkGroupSize[1],
																		_properties.meshShaderProps.maxTaskWorkGroupSize[2],
																		_properties.meshShaderProps.maxTaskWorkGroupInvocations );
			outFeatureSet.maxMeshWorkGroupSize					= Min(	_properties.meshShaderProps.maxMeshWorkGroupSize[0],
																		_properties.meshShaderProps.maxMeshWorkGroupSize[1],
																		_properties.meshShaderProps.maxMeshWorkGroupSize[2],
																		_properties.meshShaderProps.maxMeshWorkGroupInvocations );
			outFeatureSet.maxMeshOutputVertices					= _properties.meshShaderProps.maxMeshOutputVertices;
			outFeatureSet.maxMeshOutputPrimitives				= _properties.meshShaderProps.maxMeshOutputPrimitives;
			outFeatureSet.maxMeshOutputPerVertexGranularity		= _properties.meshShaderProps.meshOutputPerVertexGranularity;
			outFeatureSet.maxMeshOutputPerPrimitiveGranularity	= _properties.meshShaderProps.meshOutputPerPrimitiveGranularity;
			outFeatureSet.maxTaskPayloadSize					= _properties.meshShaderProps.maxTaskPayloadSize;
			outFeatureSet.maxTaskSharedMemorySize				= _properties.meshShaderProps.maxTaskSharedMemorySize;
			outFeatureSet.maxTaskPayloadAndSharedMemorySize		= _properties.meshShaderProps.maxTaskPayloadAndSharedMemorySize;
			outFeatureSet.maxMeshSharedMemorySize				= _properties.meshShaderProps.maxMeshSharedMemorySize;
			outFeatureSet.maxMeshPayloadAndSharedMemorySize		= _properties.meshShaderProps.maxMeshPayloadAndSharedMemorySize;
			outFeatureSet.maxMeshOutputMemorySize				= _properties.meshShaderProps.maxMeshOutputMemorySize;
			outFeatureSet.maxMeshPayloadAndOutputMemorySize		= _properties.meshShaderProps.maxMeshPayloadAndOutputMemorySize;
			outFeatureSet.maxMeshMultiviewViewCount				= _properties.meshShaderProps.maxMeshMultiviewViewCount;
			outFeatureSet.maxPreferredTaskWorkGroupInvocations	= _properties.meshShaderProps.maxPreferredTaskWorkGroupInvocations;
			outFeatureSet.maxPreferredMeshWorkGroupInvocations	= _properties.meshShaderProps.maxPreferredMeshWorkGroupInvocations;
		}

		if ( _extensions.vertexDivisor and _properties.vertexDivisorFeats.vertexAttributeInstanceRateDivisor )
		{
			outFeatureSet.vertexDivisor				= True;
			outFeatureSet.maxVertexAttribDivisor	= _properties.vertexDivisorProps.maxVertexAttribDivisor;
		}

		outFeatureSet.maxVertexAttributes	= limits.maxVertexInputAttributes;
		outFeatureSet.maxVertexBuffers		= limits.maxVertexInputBindings;

		SET_FEAT( geometryShader );
		SET_FEAT( tessellationShader );
		// skip tileShader

		SET_FEAT( imageCubeArray );
		SET_FEAT( textureCompressionASTC_LDR );
		SET_FEAT( textureCompressionETC2 );
		SET_FEAT( textureCompressionBC );
		// skip imageViewMinLod		// VkPhysicalDeviceImageViewMinLodFeaturesEXT

		if ( _extensions.imageFormatList )
			outFeatureSet.imageViewFormatList = True;

		if ( _extensions.maintenance2 )
			outFeatureSet.imageViewExtendedUsage = True;

		if ( _extensions.astcHdr )
		{
			SET_FEAT2( textureCompressionASTC_HDR, _properties.astcHdrFeats );
		}
		outFeatureSet.maxImageArrayLayers = limits.maxImageArrayLayers;
		// TODO: storageImageFormats

		SET_FEAT( samplerAnisotropy );
		outFeatureSet.maxSamplerAnisotropy	= limits.maxSamplerAnisotropy;
		outFeatureSet.maxSamplerLodBias		= limits.maxSamplerLodBias;

		if ( _extensions.samplerMirrorClamp )
			outFeatureSet.samplerMirrorClampToEdge = True;

		if ( _extensions.samplerYcbcrConversion )
			outFeatureSet.samplerYcbcrConversion = True;

		if ( _extensions.ycbcr2Plane444 )
			outFeatureSet.ycbcr2Plane444 = True;

		if ( _extensions.samplerFilterMinmax )
		{
			outFeatureSet.samplerFilterMinmax = True;
			SET_FEAT2( filterMinmaxImageComponentMapping,	_properties.samplerFilterMinmaxProps );
			//SET_FEAT2( filterMinmaxSingleComponentFormats,	_properties.samplerFilterMinmaxProps );
		}

		constexpr usize	max_samples = CT_SizeOfInBits< FeatureSet::SampleCountBits >;
		for (usize i = 0; i < max_samples; ++i)
		{
			const uint	mask = 1u << i;

			if ( uint(limits.framebufferColorSampleCounts) & mask )
				outFeatureSet.framebufferColorSampleCounts = FeatureSet::SampleCountBits( uint(outFeatureSet.framebufferColorSampleCounts) | mask );

			if ( uint(limits.framebufferDepthSampleCounts) & mask )
				outFeatureSet.framebufferDepthSampleCounts = FeatureSet::SampleCountBits( uint(outFeatureSet.framebufferDepthSampleCounts) | mask );
		}
		ASSERT( outFeatureSet.framebufferColorSampleCounts != Default );
		ASSERT( outFeatureSet.framebufferDepthSampleCounts != Default );

		outFeatureSet.maxFramebufferLayers = limits.maxFramebufferLayers;

		for (uint i = 0; i < uint(EPixelFormat::_Count); ++i)
		{
			const EPixelFormat	fmt		= EPixelFormat(i);
			VkFormatProperties	props	= {};

			if ( not _extensions.astcHdr and EPixelFormat_IsASTC_HDR( fmt ))
				continue;

			if ( not _extensions.samplerYcbcrConversion and EPixelFormat_IsYcbcr( fmt ))
				continue;

			if ( not _extensions.ycbcr2Plane444 and EPixelFormat_IsYcbcr2Plane444( fmt ))
				continue;

			vkGetPhysicalDeviceFormatProperties( GetVkPhysicalDevice(), VEnumCast( fmt ), OUT &props );

			if ( props.optimalTilingFeatures & (VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) )
				outFeatureSet.attachmentFormats.insert( fmt );

			if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT )
				outFeatureSet.attachmentBlendFormats.insert( fmt );


			if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT )
				outFeatureSet.storageImageAtomicFormats.insert( fmt );

			if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT )
				outFeatureSet.storageImageFormats.insert( fmt );


			if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT )
				outFeatureSet.linearSampledFormats.insert( fmt );


			if ( props.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT )
				outFeatureSet.uniformTexBufferFormats.insert( fmt );

			if ( props.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT )
				outFeatureSet.storageTexBufferFormats.insert( fmt );

			if ( props.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT )
				outFeatureSet.storageTexBufferAtomicFormats.insert( fmt );

			// TODO: hwCompressedAttachmentFormats, lossyCompressedAttachmentFormats
		}

		static const EVertexType	vertex_formats[] = {
			#define AE_VERTEXTYPE_VISIT( _name_, _value_ )	EVertexType::_name_,
			AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
			#undef AE_VERTEXTYPE_VISIT
		};
		for (auto fmt : vertex_formats)
		{
			VkFormatProperties	props = {};
			vkGetPhysicalDeviceFormatProperties( GetVkPhysicalDevice(), VEnumCast( fmt ), OUT &props );

			if ( props.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT )
				outFeatureSet.vertexFormats.insert( fmt );

			if ( _extensions.accelerationStructure and (props.bufferFeatures & VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR) != 0 )
				outFeatureSet.accelStructVertexFormats.insert( fmt );
		}

		outFeatureSet.queues.required	= GetAvailableQueues();
		outFeatureSet.queues.supported	= outFeatureSet.queues.required;
		{
			uint	count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, null );
			if ( count > 0 )
			{
				QueueFamilyProperties_t  queue_family_props;
				queue_family_props.resize( Min( count, queue_family_props.capacity() ));

				vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, OUT queue_family_props.data() );
				queue_family_props.resize( Min( count, queue_family_props.size() ));

				StaticAssert( uint(EQueueMask::All) == 0x1F );
				for (auto& props : queue_family_props)
				{
					if ( AllBits( props.queueFlags, VK_QUEUE_GRAPHICS_BIT ) and
						 not AnyBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::Graphics;

					if ( AllBits( props.queueFlags, VK_QUEUE_COMPUTE_BIT ) and
						 not AnyBits( props.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::AsyncCompute;

					if ( AllBits( props.queueFlags, VK_QUEUE_TRANSFER_BIT ) and
						 not AnyBits( props.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::AsyncTransfer;

					if ( AllBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR ) and
						 not AnyBits( props.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::VideoDecode;

					if ( AllBits( props.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR ) and
						 not AnyBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::VideoEncode;
				}
			}
		}

		outFeatureSet.externalFormatAndroid = (_extensions.androidExternalMemoryHwBuf ? True : False);

		outFeatureSet.AddDevice( _properties.properties.vendorID,
								 _properties.properties.deviceID,
								 _properties.properties.deviceName );

		CHECK( outFeatureSet.IsValid() );
		outFeatureSet.Validate();

		#undef SET_FEAT
		#undef SET_FEAT2
	}

/*
=================================================
	_InitFeaturesAndPropertiesByFeatureSet
=================================================
*/
	bool  VDeviceInitializer::_InitFeaturesAndPropertiesByFeatureSet (const FeatureSet &inFS) __NE___
	{
		#define SET_FEAT( _name_ )			feats10._name_ = (inFS._name_ == True ? VK_TRUE : VK_FALSE)
		#define SET_FEAT2( _name_, _feat_ )	_feat_._name_  = (inFS._name_ == True ? VK_TRUE : VK_FALSE)

		StaticAssert( sizeof(FeatureSet) == 696 );
		using EFeature = FeatureSet::EFeature;

		auto&			feats10		= _properties.features;
		auto&			f16i8_feats = _properties.shaderFloat16Int8Feats;

		const EFeature	True		= EFeature::RequireTrue;

		SET_FEAT( alphaToOne );
		SET_FEAT( depthBiasClamp );
		SET_FEAT( depthBounds );
		SET_FEAT( depthClamp );
		SET_FEAT( dualSrcBlend );
		SET_FEAT( fillModeNonSolid );
		SET_FEAT( independentBlend );
		SET_FEAT( logicOp );
		SET_FEAT( sampleRateShading );
		SET_FEAT( variableMultisampleRate );

		if ( inFS.subgroup == True )
		{
			CHECK_ERR( _extensions.subgroup );
		}
		else
			_extensions.subgroup = false;

		if ( inFS.subgroupSizeControl == True )
		{
			CHECK_ERR( _extensions.subgroupSizeControl );
		}
		else
		{
			_extensions.subgroupSizeControl								= false;
			_properties.subgroupSizeControlFeats.computeFullSubgroups	= false;
			_properties.subgroupSizeControlFeats.subgroupSizeControl	= false;
		}

		if ( AnyBits( inFS.subgroupTypes, ~(ESubgroupTypes::Float32 | ESubgroupTypes::Int32) ))
		{
			CHECK_ERR( _extensions.subgroupExtendedTypes );
			_properties.subgroupExtendedTypesFeats.shaderSubgroupExtendedTypes	= true;

			if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int16 ))	CHECK_ERR( feats10.shaderInt16 )	else feats10.shaderInt16 = false;
			if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int64 ))	CHECK_ERR( feats10.shaderInt64 )	else feats10.shaderInt64 = false;

			if ( AnyBits( inFS.subgroupTypes, ESubgroupTypes::Int8 | ESubgroupTypes::Float16 ))
			{
				CHECK_ERR( _extensions.shaderFloat16Int8 );
				if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int8 ))		CHECK_ERR( f16i8_feats.shaderInt8 )		else f16i8_feats.shaderInt8		= false;
				if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Float16 ))	CHECK_ERR( f16i8_feats.shaderFloat16 )	else f16i8_feats.shaderFloat16	= false;
			}
			else
			{
				_extensions.shaderFloat16Int8	= false;
				f16i8_feats.shaderInt8			= false;
				f16i8_feats.shaderFloat16		= false;
			}
		}
		else
		{
			_extensions.subgroupExtendedTypes									= false;
			_properties.subgroupExtendedTypesFeats.shaderSubgroupExtendedTypes	= false;
		}

		if ( inFS.shaderInt8	== True  or
			 inFS.shaderFloat16	== True )
		{
			CHECK_ERR( _extensions.shaderFloat16Int8 );

			SET_FEAT2( shaderInt8,		f16i8_feats );
			SET_FEAT2( shaderFloat16,	f16i8_feats );
		}
		else
			_extensions.shaderFloat16Int8 = false;

		SET_FEAT( shaderInt16 );
		SET_FEAT( shaderInt64 );
		SET_FEAT( shaderFloat64 );

		_extensions.storage16bit =	(inFS.storageBuffer16BitAccess			 == True)	or
									(inFS.uniformAndStorageBuffer16BitAccess == True)	or
									(inFS.storagePushConstant16				 == True)	or
									(inFS.storageInputOutput16				 == True);
		SET_FEAT2( storageBuffer16BitAccess,			_properties.storage16bitFeats );
		SET_FEAT2( uniformAndStorageBuffer16BitAccess,	_properties.storage16bitFeats );
		SET_FEAT2( storagePushConstant16,				_properties.storage16bitFeats );
		SET_FEAT2( storageInputOutput16,				_properties.storage16bitFeats );

		_extensions.storage8bits =	(inFS.storageBuffer8BitAccess			== True)	or
									(inFS.uniformAndStorageBuffer8BitAccess	== True)	or
									(inFS.storagePushConstant8				== True);
		SET_FEAT2( storageBuffer8BitAccess,				_properties.storage8bitsFeats );
		SET_FEAT2( uniformAndStorageBuffer8BitAccess,	_properties.storage8bitsFeats );
		SET_FEAT2( storagePushConstant8,				_properties.storage8bitsFeats );

		_extensions.scalarBlockLayout = (inFS.scalarBlockLayout == True);
		SET_FEAT2( scalarBlockLayout, _properties.scalarBlockLayoutFeats );

		SET_FEAT( fragmentStoresAndAtomics );
		SET_FEAT( vertexPipelineStoresAndAtomics );

		_extensions.shaderImageAtomicInt64 = (inFS.shaderImageInt64Atomics == True);
		SET_FEAT2( shaderImageInt64Atomics, _properties.shaderImageAtomicInt64Feats );

		_extensions.shaderAtomicInt64 =	(inFS.shaderBufferInt64Atomics	== True) or
										(inFS.shaderSharedInt64Atomics	== True);
		SET_FEAT2( shaderBufferInt64Atomics,	_properties.shaderAtomicInt64Feats );
		SET_FEAT2( shaderSharedInt64Atomics,	_properties.shaderAtomicInt64Feats );

		_extensions.shaderAtomicFloat =	(inFS.shaderBufferFloat32Atomics	== True)	or
										(inFS.shaderBufferFloat32AtomicAdd	== True)	or
										(inFS.shaderBufferFloat64Atomics	== True)	or
										(inFS.shaderBufferFloat64AtomicAdd	== True)	or
										(inFS.shaderSharedFloat32Atomics	== True)	or
										(inFS.shaderSharedFloat32AtomicAdd	== True)	or
										(inFS.shaderSharedFloat64Atomics	== True)	or
										(inFS.shaderSharedFloat64AtomicAdd	== True)	or
										(inFS.shaderImageFloat32Atomics		== True)	or
										(inFS.shaderImageFloat32AtomicAdd	== True);
		SET_FEAT2( shaderBufferFloat32Atomics,		_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderBufferFloat32AtomicAdd,	_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderBufferFloat64Atomics,		_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderBufferFloat64AtomicAdd,	_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderSharedFloat32Atomics,		_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderSharedFloat32AtomicAdd,	_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderSharedFloat64Atomics,		_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderSharedFloat64AtomicAdd,	_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderImageFloat32Atomics,		_properties.shaderAtomicFloatFeats );
		SET_FEAT2( shaderImageFloat32AtomicAdd,		_properties.shaderAtomicFloatFeats );

		_extensions.shaderAtomicFloat2  =	(inFS.shaderBufferFloat16Atomics		== True)	or
											(inFS.shaderBufferFloat16AtomicAdd		== True)	or
											(inFS.shaderBufferFloat16AtomicMinMax	== True)	or
											(inFS.shaderBufferFloat32AtomicMinMax	== True)	or
											(inFS.shaderBufferFloat64AtomicMinMax	== True)	or
											(inFS.shaderSharedFloat16Atomics		== True)	or
											(inFS.shaderSharedFloat16AtomicAdd		== True)	or
											(inFS.shaderSharedFloat16AtomicMinMax	== True)	or
											(inFS.shaderSharedFloat32AtomicMinMax	== True)	or
											(inFS.shaderSharedFloat64AtomicMinMax	== True)	or
											(inFS.shaderImageFloat32AtomicMinMax	== True);
		SET_FEAT2( shaderBufferFloat16Atomics,		_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderBufferFloat16AtomicAdd,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderBufferFloat16AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderBufferFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderBufferFloat64AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderSharedFloat16Atomics,		_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderSharedFloat16AtomicAdd,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderSharedFloat16AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderSharedFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderSharedFloat64AtomicMinMax,	_properties.shaderAtomicFloat2Feats );
		SET_FEAT2( shaderImageFloat32AtomicMinMax,	_properties.shaderAtomicFloat2Feats );

		_extensions.shaderViewportIndexLayer =	(inFS.shaderOutputViewportIndex	== True)	or
												(inFS.shaderOutputLayer			== True);

		_extensions.shaderClock =	(inFS.shaderSubgroupClock	== True)	or
									(inFS.shaderDeviceClock		== True);
		SET_FEAT2( shaderSubgroupClock,	_properties.shaderClockFeats );
		SET_FEAT2( shaderDeviceClock,	_properties.shaderClockFeats );

		if ( inFS.cooperativeMatrix == True )
		{
			_extensions.cooperativeMatrix										= true;
			_properties.cooperativeMatrixFeats.cooperativeMatrix				= VK_TRUE;
			_properties.cooperativeMatrixProps.cooperativeMatrixSupportedStages	= VEnumCast( inFS.cooperativeMatrixStages );
		}

		_extensions.bufferDeviceAddress = (inFS.bufferDeviceAddress == True);
		SET_FEAT2( bufferDeviceAddress, _properties.bufferDeviceAddressFeats );

		SET_FEAT( shaderClipDistance );
		SET_FEAT( shaderCullDistance );
		SET_FEAT( shaderResourceMinLod );

		_extensions.shaderDrawParams		= (inFS.shaderDrawParameters == True);
		_extensions.shaderSMBuiltinsNV		= (inFS.shaderSMBuiltinsNV == True);
		_extensions.shaderCoreBuiltinsARM	= (inFS.shaderCoreBuiltinsARM == True);
		_extensions.shaderStencilExport		= (inFS.shaderStencilExport == True);

		SET_FEAT( shaderUniformBufferArrayDynamicIndexing );
		SET_FEAT( shaderSampledImageArrayDynamicIndexing );
		SET_FEAT( shaderStorageBufferArrayDynamicIndexing );
		SET_FEAT( shaderStorageImageArrayDynamicIndexing );

		SET_FEAT2( shaderInputAttachmentArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformTexelBufferArrayDynamicIndexing,	_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageTexelBufferArrayDynamicIndexing,	_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderSampledImageArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageImageArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderInputAttachmentArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformTexelBufferArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageTexelBufferArrayNonUniformIndexing,	_properties.descriptorIndexingFeats );
		SET_FEAT2( runtimeDescriptorArray,							_properties.descriptorIndexingFeats );

		SET_FEAT( shaderStorageImageMultisample );
		SET_FEAT( shaderStorageImageReadWithoutFormat );
		SET_FEAT( shaderStorageImageWriteWithoutFormat );

		_extensions.memoryModel =	(inFS.vulkanMemoryModel								== True)	or
									(inFS.vulkanMemoryModelDeviceScope					== True)	or
									(inFS.vulkanMemoryModelAvailabilityVisibilityChains	== True);
		SET_FEAT2( vulkanMemoryModel,								_properties.memoryModelFeats );
		SET_FEAT2( vulkanMemoryModelDeviceScope,					_properties.memoryModelFeats );
		SET_FEAT2( vulkanMemoryModelAvailabilityVisibilityChains,	_properties.memoryModelFeats );

		_extensions.shaderTerminateInvocation = (inFS.shaderTerminateInvocation == True);
		SET_FEAT2( shaderTerminateInvocation, _properties.shaderTerminateInvocationFeats );

		_extensions.zeroInitializeWorkgroupMem = (inFS.shaderZeroInitializeWorkgroupMemory == True);
		SET_FEAT2( shaderZeroInitializeWorkgroupMemory, _properties.zeroInitializeWorkgroupMemFeats );

		_extensions.fragShaderInterlock =	(inFS.fragmentShaderSampleInterlock		 == True)	or
											(inFS.fragmentShaderPixelInterlock		 == True)	or
											(inFS.fragmentShaderShadingRateInterlock == True);
		SET_FEAT2( fragmentShaderSampleInterlock,		_properties.fragShaderInterlockFeats );
		SET_FEAT2( fragmentShaderPixelInterlock,		_properties.fragShaderInterlockFeats );
		SET_FEAT2( fragmentShaderShadingRateInterlock,	_properties.fragShaderInterlockFeats );

		_extensions.fragmentBarycentric = (inFS.fragmentShaderBarycentric == True);
		SET_FEAT2( fragmentShaderBarycentric,	_properties.fragmentBarycentricFeats );

		_extensions.fragShadingRate =	(inFS.pipelineFragmentShadingRate	== True)	or
										(inFS.primitiveFragmentShadingRate	== True)	or
										(inFS.attachmentFragmentShadingRate == True);
		SET_FEAT2( pipelineFragmentShadingRate,		_properties.fragShadingRateFeats );
		SET_FEAT2( primitiveFragmentShadingRate,	_properties.fragShadingRateFeats );
		SET_FEAT2( attachmentFragmentShadingRate,	_properties.fragShadingRateFeats );
		_properties.fragShadingRateProps.minFragmentShadingRateAttachmentTexelSize				= BitCast<VkExtent2D>(inFS.fragmentShadingRateTexelSize.Min());
		_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSize				= BitCast<VkExtent2D>(inFS.fragmentShadingRateTexelSize.Max());
		_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSizeAspectRatio	= inFS.fragmentShadingRateTexelSize.MaxAspect();

		_extensions.rayQuery = (inFS.rayQuery == True);
		SET_FEAT2( rayQuery,	_properties.rayQueryFeats );

		_extensions.rayTracingPipeline  =	(inFS.rayTracingPipeline			== True)	or
											(inFS.rayTraversalPrimitiveCulling	== True);
		SET_FEAT2( rayTracingPipeline,				_properties.rayTracingPipelineFeats );
		SET_FEAT2( rayTraversalPrimitiveCulling,	_properties.rayTracingPipelineFeats );

		_extensions.accelerationStructure = _extensions.rayTracingPipeline or _extensions.rayQuery;

		SET_FEAT( drawIndirectFirstInstance );
		_extensions.drawIndirectCount = (inFS.drawIndirectCount == True);

		_extensions.multiview =	(inFS.multiview						== True)	or
								(inFS.multiviewGeometryShader		== True)	or
								(inFS.multiviewTessellationShader	== True);
		SET_FEAT2( multiview,					_properties.multiviewFeats );
		SET_FEAT2( multiviewGeometryShader,		_properties.multiviewFeats );
		SET_FEAT2( multiviewTessellationShader,	_properties.multiviewFeats );

		SET_FEAT( multiViewport );

		_extensions.sampleLocations = (inFS.sampleLocations == True);

		SET_FEAT2( constantAlphaColorBlendFactors,			_properties.portabilitySubsetFeats );
		SET_FEAT2( shaderSampleRateInterpolationFunctions,	_properties.portabilitySubsetFeats );
		SET_FEAT2( pointPolygons,							_properties.portabilitySubsetFeats );
		SET_FEAT2( tessellationIsolines,					_properties.portabilitySubsetFeats );
		SET_FEAT2( tessellationPointMode,					_properties.portabilitySubsetFeats );
		SET_FEAT2( multisampleArrayImage,					_properties.portabilitySubsetFeats );
		SET_FEAT2( samplerMipLodBias,						_properties.portabilitySubsetFeats );
		SET_FEAT2( triangleFans,							_properties.portabilitySubsetFeats );

		_extensions.meshShader  =	(inFS.taskShader	== True)	or
									(inFS.meshShader	== True);
		SET_FEAT2( taskShader,	_properties.meshShaderFeats );
		SET_FEAT2( meshShader,	_properties.meshShaderFeats );

		SET_FEAT( geometryShader );
		SET_FEAT( tessellationShader );

		SET_FEAT( imageCubeArray );
		SET_FEAT( textureCompressionASTC_LDR );
		SET_FEAT( textureCompressionETC2 );
		SET_FEAT( textureCompressionBC );

		_extensions.imageFormatList = (inFS.imageViewFormatList == True);
		_extensions.maintenance2 |= (inFS.imageViewExtendedUsage == True);

		_extensions.astcHdr = (inFS.textureCompressionASTC_HDR == True);
		SET_FEAT2( textureCompressionASTC_HDR, _properties.astcHdrFeats );

		SET_FEAT( samplerAnisotropy );

		_extensions.samplerMirrorClamp	= (inFS.samplerMirrorClampToEdge == True);

		_extensions.samplerFilterMinmax	= (inFS.samplerFilterMinmax == True);
		SET_FEAT2( filterMinmaxImageComponentMapping,	_properties.samplerFilterMinmaxProps );

		_extensions.samplerYcbcrConversion	= (inFS.samplerYcbcrConversion == True);
		_extensions.ycbcr2Plane444			= (inFS.ycbcr2Plane444 == True);

		if ( inFS.externalFormatAndroid == True ) {
			_extensions.androidExternalMemoryHwBuf	= true;
			_extensions.samplerYcbcrConversion		= true;
			_extensions.externalMemory				= true;
			_extensions.queueFamilyForeign			= true;
			_extensions.dedicatedAllocation			= true;
		}

		return true;

		#undef SET_FEAT
		#undef SET_FEAT2
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
