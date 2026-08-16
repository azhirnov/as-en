// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VDevice.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
namespace
{
	// https://vulkan.gpuinfo.org/displayextensionproperty.php?extensionname=VK_KHR_maintenance3&extensionproperty=maxPerSetDescriptors&platform=all
	static constexpr uint	c_MaxPerSetDescriptors = 512;

	Nd__In POTValue  CastPOT (uint src) __NE___
	{
		POTValue	dst {PowerOfTwo( Max( IntLog2( src ), 0 ))};
		DEBUG_ONLY( if ( uint{dst} != src ) AE_LOG_DBG( "Not a POT value ("s << ToString(src) << ")" );)
		return dst;
	}

	Nd__In POTBytes  CastPOTBytes (uint src) __NE___
	{
		POTBytes	dst {PowerOfTwo( Max( IntLog2( src ), 0 ))};
		DEBUG_ONLY( if ( uint{dst} != src ) AE_LOG_DBG( "Not a POT value ("s << ToString(src) << ")" );)
		return dst;
	}

# ifdef AE_ENABLE_LOGS
	#include "vulkan_loader/vkenum_to_str.h"
# endif
} // namespace

/*
=================================================
	InitFeatureSet
=================================================
*/
	void  VDevice::InitFeatureSet (OUT FeatureSet &outFeatureSet) C_NE___
	{
		StaticAssert( FeatureSet::GetFeatureCount() == 281 );

		using EFeature	= FeatureSet::EFeature;
		using KiBytes	= FeatureSet::KiBytes;

		const EFeature	True	= EFeature::RequireTrue;
		const EFeature	False	= EFeature::RequireFalse;

		outFeatureSet.Init( False );

		const auto&	feats10	= _properties.features;		// Vulkan 1.0 core features
		const auto&	limits	= _properties.properties.limits;

		EShaderStages	all_stages = EShaderStages::Vertex | EShaderStages::Fragment | EShaderStages::Compute;
		if ( feats10.geometryShader )			all_stages |= EShaderStages::Geometry;
		if ( feats10.tessellationShader )		all_stages |= EShaderStages::TessControl | EShaderStages::TessEvaluation;
		if ( _extensions.meshShader )			all_stages |= EShaderStages::MeshTask | EShaderStages::Mesh;
		if ( _extensions.rayTracingPipeline )	all_stages |= EShaderStages::AllRayTracing;

		#define SET_FEAT2( _name_, _feat_ )	outFeatureSet._name_ = _feat_._name_ ? True : False
		#define SET_FEAT( _name_ )			outFeatureSet._name_ = feats10._name_  ? True : False
		#define SET_EXT2( _name_, _ext_ )	outFeatureSet._name_ = _extensions._ext_  ? True : False
		#define SET_EXT( _name_ )			outFeatureSet._name_ = _extensions._name_  ? True : False

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
		SET_FEAT( largePoints );
		SET_FEAT( wideLines );

		if ( _extensions.subgroup )
		{
			CHECK( _properties.subgroupProperties.subgroupSize > 0 );
			CHECK( IsPowerOfTwo( _properties.subgroupProperties.subgroupSize ));

			outFeatureSet.subgroup			= True;
			outFeatureSet.subgroupStages	= AEEnumCast( VkShaderStageFlagBits(_properties.subgroupProperties.supportedStages) ) & all_stages;
			outFeatureSet.subgroupQuadStages= _properties.subgroupProperties.quadOperationsInAllStages ? all_stages : EShaderStages::Fragment | EShaderStages::Compute;
			outFeatureSet.minSubgroupSize	= CastPOT( _properties.subgroupProperties.subgroupSize );
			outFeatureSet.maxSubgroupSize	= CastPOT( _properties.subgroupProperties.subgroupSize );
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
						CHECK( _extensions.subgroupPartitioned );
						outFeatureSet.subgroupOperations.insert( ESubgroupOperation::PartitionedNV );
						break;

					case VK_SUBGROUP_FEATURE_ROTATE_BIT :
						CHECK( _properties.shaderSubgroupRotateFeats.shaderSubgroupRotate == VK_TRUE );
						outFeatureSet.subgroupOperations.insert( ESubgroupOperation::Rotate );
						break;

					case VK_SUBGROUP_FEATURE_ROTATE_CLUSTERED_BIT :
						CHECK( _properties.shaderSubgroupRotateFeats.shaderSubgroupRotateClustered == VK_TRUE );
						outFeatureSet.subgroupOperations.insert( ESubgroupOperation::RotateClustered );
						break;

					case VK_SUBGROUP_FEATURE_FLAG_BITS_MAX_ENUM :
					default_unlikely :								DBG_WARNING( "unknown subgroup feature" );	break;
				}
				switch_end
			}

			outFeatureSet.subgroupBroadcastDynamicId = _vkDeviceVersion >= DeviceVersion{1,2} ? True : False;
		}

		if ( _extensions.subgroupSizeControl and _properties.subgroupSizeControlFeats.subgroupSizeControl )
		{
			CHECK( IsPowerOfTwo( _properties.subgroupSizeControlProps.minSubgroupSize ));
			CHECK( IsPowerOfTwo( _properties.subgroupSizeControlProps.maxSubgroupSize ));
			CHECK( outFeatureSet.minSubgroupSize <= _properties.subgroupProperties.subgroupSize );
			CHECK( outFeatureSet.maxSubgroupSize >= _properties.subgroupProperties.subgroupSize );

			outFeatureSet.subgroupSizeControl		= True;
			outFeatureSet.requiredSubgroupSizeStages= AEEnumCast( VkShaderStageFlagBits(_properties.subgroupSizeControlProps.requiredSubgroupSizeStages) ) & all_stages;
			outFeatureSet.minSubgroupSize			= CastPOT( _properties.subgroupSizeControlProps.minSubgroupSize );
			outFeatureSet.maxSubgroupSize			= CastPOT( _properties.subgroupSizeControlProps.maxSubgroupSize );
		}

		if ( _extensions.subgroupExtendedTypes )
		{
			if ( feats10.shaderInt16 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int16;
			if ( feats10.shaderInt64 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int64;

			if ( _extensions.shaderFloat16Int8 ) {
				if ( _properties.shaderFloat16Int8Feats.shaderInt8 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Int8;
				if ( _properties.shaderFloat16Int8Feats.shaderFloat16 )	outFeatureSet.subgroupTypes |= ESubgroupTypes::Float16;
			}
		}

		SET_EXT( shaderSubgroupUniformControlFlow );
		SET_EXT( shaderMaximalReconvergence );
		SET_EXT( shaderQuadControl );
		SET_EXT( shaderExpectAssume );

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

		SET_EXT( uniformBufferStandardLayout );
		SET_EXT( scalarBlockLayout );

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


		if ( _extensions.shaderAtomicPackedFp16 )
			outFeatureSet.shaderAtomicPackedFp16 = True;

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

		if ( _extensions.cooperativeMatrix and _properties.cooperativeMatrixFeats.cooperativeMatrix )
		{
			outFeatureSet.cooperativeMatrix			= True;
			outFeatureSet.cooperativeMatrixStages	= AEEnumCast( VkShaderStageFlagBits( _properties.cooperativeMatrixProps.cooperativeMatrixSupportedStages )) & all_stages;

			FixedArray<VkCooperativeMatrixPropertiesKHR, 32>	mat_props;
			uint	count = 0;

			VK_CHECK( vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR( GetVkPhysicalDevice(), OUT &count, null ));

			mat_props.resize( count );
			for (auto& mp : mat_props) {
				mp.sType = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_KHR;
				mp.pNext = null;
			}

			VK_CHECK( vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR( GetVkPhysicalDevice(), INOUT &count, OUT mat_props.data() ));

			for (auto& mp : mat_props)
			{
				if ( mp.scope != VK_SCOPE_SUBGROUP_KHR )
					continue;

				CoopMatrixConfig	cfg;
				cfg.a	= AEEnumCast( mp.AType );
				cfg.b	= AEEnumCast( mp.BType );
				cfg.c	= AEEnumCast( mp.CType );
				cfg.res	= AEEnumCast( mp.ResultType );
				cfg.m	= ubyte(mp.MSize);
				cfg.n	= ubyte(mp.NSize);
				cfg.k	= ubyte(mp.KSize);

				if ( auto ecfg = cfg.ToECoopMatrixCfg();  ecfg != Default )
					outFeatureSet.cooperativeMatrixConfig.insert( ecfg );
			}

		  #if 0
			String	str = "CooperativeMatrixProperties:";
			for (auto& mp : mat_props)
			{
				str << "\nMSize: " << ToString( mp.MSize ) << ", NSize: " << ToString( mp.NSize )
					<< ", KSize: " << ToString( mp.KSize ) << ", AType: " << VkComponentTypeKHRToString( mp.AType )
					<< ", BType: " << VkComponentTypeKHRToString( mp.BType )
					<< ", CType: " << VkComponentTypeKHRToString( mp.CType )
					<< ", ResultType: " << VkComponentTypeKHRToString( mp.ResultType )
					<< ", saturatingAccumulation: " << ToString( bool(mp.saturatingAccumulation) )
					<< ", scope: " << VkScopeKHRToString( mp.scope );
			}
			AE_LOGI( str );
		  #endif
			ASSERT( outFeatureSet.cooperativeMatrixConfig.Any() );
		}

		if ( _extensions.cooperativeVectorNV and _properties.cooperativeVectorNVFeats.cooperativeVector )
		{
			outFeatureSet.cooperativeVector			= True;
			outFeatureSet.cooperativeVectorTraining = _properties.cooperativeVectorNVFeats.cooperativeVectorTraining ? True : False;

			FixedArray<VkCooperativeVectorPropertiesNV, 32>	vec_props;
			uint	count = 0;

			VK_CHECK( vkGetPhysicalDeviceCooperativeVectorPropertiesNV( GetVkPhysicalDevice(), OUT &count, null ));

			vec_props.resize( count );
			for (auto& vp : vec_props) {
				vp.sType = VK_STRUCTURE_TYPE_COOPERATIVE_VECTOR_PROPERTIES_NV;
				vp.pNext = null;
			}

			VK_CHECK( vkGetPhysicalDeviceCooperativeVectorPropertiesNV( GetVkPhysicalDevice(), INOUT &count, OUT vec_props.data() ));

			for (auto& vp : vec_props)
			{
				CoopVectorConfig	cfg;
				cfg.inputType				= AEEnumCast( vp.inputType );
				cfg.inputInterpretation		= AEEnumCast( vp.inputInterpretation );
				cfg.matrixInterpretation	= AEEnumCast( vp.matrixInterpretation );
				cfg.biasInterpretation		= AEEnumCast( vp.biasInterpretation );
				cfg.resultType				= AEEnumCast( vp.resultType );
				cfg.transpose				= vp.transpose == VK_TRUE;

				if ( auto ecfg = cfg.ToECoopVecCfg();  ecfg != Default )
					outFeatureSet.cooperativeVectorConfig.insert( ecfg );
			}
			ASSERT( outFeatureSet.cooperativeVectorConfig.Any() );

		  #if 0
			String	str = "CooperativeVectorProperties:";
			for (auto& vp : vec_props)
			{
				str << "\ninputType: " << VkComponentTypeKHRToString( vp.inputType )
					<< ", inputInterpretation: " << VkComponentTypeKHRToString( vp.inputInterpretation )
					<< ", matrixInterpretation: " << VkComponentTypeKHRToString( vp.matrixInterpretation )
					<< ", biasInterpretation: " << VkComponentTypeKHRToString( vp.biasInterpretation )
					<< ", resultType: " << VkComponentTypeKHRToString( vp.resultType )
					<< ", transpose: " << ToString( bool(vp.transpose) );
			}
			AE_LOGI( str );
		  #endif
		}

		if ( _extensions.bufferDeviceAddress )
			SET_FEAT2( bufferDeviceAddress, _properties.bufferDeviceAddressFeats );

		SET_FEAT( shaderClipDistance );
		SET_FEAT( shaderCullDistance );
		SET_FEAT( shaderResourceMinLod );

		SET_EXT2( shaderDrawParameters, shaderDrawParams );
		SET_EXT( shaderSMBuiltinsNV	);
		SET_EXT( shaderCoreBuiltinsARM );
		SET_EXT( shaderStencilExport );

		SET_FEAT( shaderUniformBufferArrayDynamicIndexing );
		SET_FEAT( shaderSampledImageArrayDynamicIndexing );
		SET_FEAT( shaderStorageBufferArrayDynamicIndexing );
		SET_FEAT( shaderStorageImageArrayDynamicIndexing );

		if ( _extensions.descriptorIndexing )
		{
			SET_FEAT2( shaderInputAttachmentArrayDynamicIndexing,			_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformTexelBufferArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageTexelBufferArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformBufferArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderSampledImageArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageBufferArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageImageArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderInputAttachmentArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderUniformTexelBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( shaderStorageTexelBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
			SET_FEAT2( runtimeDescriptorArray,								_properties.descriptorIndexingFeats );
			SET_FEAT2( descriptorBindingVariableDescriptorCount,			_properties.descriptorIndexingFeats );
			SET_FEAT2( quadDivergentImplicitLod,							_properties.descriptorIndexingProps );
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

		SET_EXT( shaderDemoteToHelperInvocation );
		SET_EXT( shaderTerminateInvocation );
		SET_EXT2( shaderZeroInitializeWorkgroupMemory, zeroInitializeWorkgroupMem );

		// TODO: shaderIntegerDotProduct

		if ( _extensions.fragShaderInterlock )
		{
			SET_FEAT2( fragmentShaderSampleInterlock,		_properties.fragShaderInterlockFeats );
			SET_FEAT2( fragmentShaderPixelInterlock,		_properties.fragShaderInterlockFeats );
			SET_FEAT2( fragmentShaderShadingRateInterlock,	_properties.fragShaderInterlockFeats );
		}

		SET_EXT2( fragmentShaderBarycentric, fragmentBarycentric );
		SET_EXT( nonSeamlessCubeMap );

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

			if ( _properties.fragShadingRateFeats.attachmentFragmentShadingRate == VK_TRUE )
			{
				outFeatureSet.fragmentShadingRateTexelSize.minX = POTValue{ _properties.fragShadingRateProps.minFragmentShadingRateAttachmentTexelSize.width }.GetPOT();
				outFeatureSet.fragmentShadingRateTexelSize.minY = POTValue{ _properties.fragShadingRateProps.minFragmentShadingRateAttachmentTexelSize.height }.GetPOT();
				outFeatureSet.fragmentShadingRateTexelSize.maxX = POTValue{ _properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSize.width }.GetPOT();
				outFeatureSet.fragmentShadingRateTexelSize.maxY = POTValue{ _properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSize.height }.GetPOT();
				outFeatureSet.fragmentShadingRateTexelSize.aspectRatio = POTValue{ _properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSizeAspectRatio }.GetPOT();
			}
		}

		if ( _extensions.fragDensityMap )
		{
			SET_FEAT2( fragmentDensityMap,						_properties.fragDensityMapFeats );
			SET_FEAT2( fragmentDensityMapDynamic,				_properties.fragDensityMapFeats );
			SET_FEAT2( fragmentDensityMapNonSubsampledImages,	_properties.fragDensityMapFeats );
			SET_FEAT2( fragmentDensityInvocations,				_properties.fragDensityMapProps );
		}
		if ( _extensions.fragDensityMap2 )
		{
			SET_FEAT2( subsampledLoads,						_properties.fragDensityMap2Props );
			outFeatureSet.maxSubsampledArrayLayers			= POTValue{ _properties.fragDensityMap2Props.maxSubsampledArrayLayers };
			outFeatureSet.perPipeline_maxSubsampledSamplers	= LimitCheckCast{ _properties.fragDensityMap2Props.maxDescriptorSetSubsampledSamplers };
		}

		if ( _extensions.rayQuery and _extensions.accelerationStructure )
		{
			outFeatureSet.rayQuery		 = True;
			outFeatureSet.rayQueryStages =	all_stages;
		}

		if ( _extensions.rayTracingPipeline and _extensions.accelerationStructure )
		{
			SET_FEAT2( rayTracingPipeline,				_properties.rayTracingPipelineFeats );
			SET_FEAT2( rayTraversalPrimitiveCulling,	_properties.rayTracingPipelineFeats );
			outFeatureSet.maxRayRecursionDepth		=	LimitCheckCast{ _properties.rayTracingPipelineProps.maxRayRecursionDepth };
		}

		outFeatureSet.maxShaderVersion.spirv = (_spirvVersion.major * 100) + (_spirvVersion.minor * 10);

		SET_FEAT( drawIndirectFirstInstance );
		SET_EXT( drawIndirectCount );

		outFeatureSet.maxDrawIndirectCount = limits.maxDrawIndirectCount;

		if ( _extensions.multiview )
		{
			SET_FEAT2( multiview,					_properties.multiviewFeats );
			SET_FEAT2( multiviewGeometryShader,		_properties.multiviewFeats );
			SET_FEAT2( multiviewTessellationShader,	_properties.multiviewFeats );
			outFeatureSet.maxMultiviewViewCount		= LimitCheckCast{ _properties.multiviewProps.maxMultiviewViewCount };
		}

		SET_FEAT( multiViewport );
		outFeatureSet.maxViewports = LimitCheckCast{ limits.maxViewports };

		if ( _extensions.sampleLocations )
		{
			outFeatureSet.sampleLocations = True;
			SET_FEAT2( variableSampleLocations, _properties.sampleLocationsProps );
		}

		outFeatureSet.perPipeline_maxUniformBuffersDynamic	= ubyte(limits.maxDescriptorSetUniformBuffersDynamic);
		outFeatureSet.perPipeline_maxStorageBuffersDynamic	= ubyte(limits.maxDescriptorSetStorageBuffersDynamic);
		outFeatureSet.perPipeline_maxTotalBuffersDynamic	= ubyte(Min( outFeatureSet.perPipeline_maxUniformBuffersDynamic + outFeatureSet.perPipeline_maxStorageBuffersDynamic, 255 ));
		outFeatureSet.perPipeline.maxInputAttachments		= limits.maxDescriptorSetInputAttachments;
		outFeatureSet.perPipeline.maxSampledImages			= limits.maxDescriptorSetSampledImages;
		outFeatureSet.perPipeline.maxSamplers				= limits.maxDescriptorSetSamplers;
		outFeatureSet.perPipeline.maxStorageBuffers			= limits.maxDescriptorSetStorageBuffers;
		outFeatureSet.perPipeline.maxStorageImages			= limits.maxDescriptorSetStorageImages;
		outFeatureSet.perPipeline.maxUniformBuffers			= limits.maxDescriptorSetUniformBuffers;
		outFeatureSet.perDescSet_maxTotalResources			= _extensions.maintenance3 ? _properties.maintenance3Props.maxPerSetDescriptors : c_MaxPerSetDescriptors;

		outFeatureSet.perStage.maxInputAttachments			= limits.maxPerStageDescriptorInputAttachments;
		outFeatureSet.perStage.maxSampledImages				= limits.maxPerStageDescriptorSampledImages;
		outFeatureSet.perStage.maxSamplers					= limits.maxPerStageDescriptorSamplers;
		outFeatureSet.perStage.maxStorageBuffers			= limits.maxPerStageDescriptorStorageBuffers;
		outFeatureSet.perStage.maxStorageImages				= limits.maxPerStageDescriptorStorageImages;
		outFeatureSet.perStage.maxUniformBuffers			= limits.maxPerStageDescriptorUniformBuffers;
		outFeatureSet.perStage_maxTotalResources			= limits.maxPerStageResources;

		if ( _extensions.maintenance7 )
		{
			outFeatureSet.perPipeline_maxUniformBuffersDynamic	= ubyte(_properties.maintenance7Props.maxDescriptorSetTotalUniformBuffersDynamic);
			outFeatureSet.perPipeline_maxStorageBuffersDynamic	= ubyte(_properties.maintenance7Props.maxDescriptorSetTotalStorageBuffersDynamic);
			outFeatureSet.perPipeline_maxTotalBuffersDynamic	= ubyte(_properties.maintenance7Props.maxDescriptorSetTotalBuffersDynamic);
		}

		if ( _extensions.accelerationStructure )
		{
			SET_FEAT2( accelerationStructureIndirectBuild, _properties.accelerationStructureFeats );

			outFeatureSet.perPipeline.maxAccelStructures	= _properties.accelerationStructureProps.maxDescriptorSetAccelerationStructures;
			outFeatureSet.perStage.maxAccelStructures		= _properties.accelerationStructureProps.maxPerStageDescriptorAccelerationStructures;
		}

		if ( _extensions.clusterAccelStructNV )
			outFeatureSet.clusterAccelerationStructure = True;	// TODO: use macros

		if ( _extensions.partitionedAccelStructNV )
			outFeatureSet.partitionedAccelerationStructure = True;

		outFeatureSet.maxTexelBufferElements= LimitCheckCast{ limits.maxTexelBufferElements };
		outFeatureSet.maxUniformBufferSize	= LimitCheckCast{ limits.maxUniformBufferRange };
		outFeatureSet.maxStorageBufferSize	= LimitCheckCast{ limits.maxStorageBufferRange };
		outFeatureSet.maxDescriptorSets		= LimitCheckCast{ limits.maxBoundDescriptorSets };
		outFeatureSet.maxTexelOffset		= LimitCheckCast{ Min( limits.maxTexelOffset, Max( Abs(limits.minTexelOffset)-1, 0 ))};
		outFeatureSet.maxTexelGatherOffset	= LimitCheckCast{ Min( limits.maxTexelGatherOffset, Max( Abs(limits.minTexelGatherOffset)-1, 0 ))};

		outFeatureSet.maxFragmentOutputAttachments			= LimitCheckCast{ limits.maxFragmentOutputAttachments };
		outFeatureSet.maxFragmentDualSrcAttachments			= LimitCheckCast{ limits.maxFragmentDualSrcAttachments };
		outFeatureSet.maxFragmentCombinedOutputResources	= limits.maxFragmentCombinedOutputResources;
		outFeatureSet.maxPushConstantsSize					= CastPOTBytes( limits.maxPushConstantsSize );

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
		outFeatureSet.maxComputeSharedMemorySize		= KiBytes{ limits.maxComputeSharedMemorySize };
		outFeatureSet.maxComputeWorkGroupInvocations	= CastPOT( limits.maxComputeWorkGroupInvocations );
		outFeatureSet.maxComputeWorkGroupSizeX			= CastPOT( Min( limits.maxComputeWorkGroupSize[0], limits.maxComputeWorkGroupInvocations ));
		outFeatureSet.maxComputeWorkGroupSizeY			= CastPOT( Min( limits.maxComputeWorkGroupSize[1], limits.maxComputeWorkGroupInvocations ));
		outFeatureSet.maxComputeWorkGroupSizeZ			= CastPOT( Min( limits.maxComputeWorkGroupSize[2], limits.maxComputeWorkGroupInvocations ));

		if ( _extensions.meshShader )
		{
			SET_FEAT2( taskShader,	_properties.meshShaderFeats );
			SET_FEAT2( meshShader,	_properties.meshShaderFeats );

			outFeatureSet.maxTaskWorkGroupSize					= CastPOT( Min( _properties.meshShaderProps.maxTaskWorkGroupSize[0],
																				_properties.meshShaderProps.maxTaskWorkGroupSize[1],
																				_properties.meshShaderProps.maxTaskWorkGroupSize[2],
																				_properties.meshShaderProps.maxTaskWorkGroupInvocations ));
			outFeatureSet.maxMeshWorkGroupSize					= CastPOT( Min( _properties.meshShaderProps.maxMeshWorkGroupSize[0],
																				_properties.meshShaderProps.maxMeshWorkGroupSize[1],
																				_properties.meshShaderProps.maxMeshWorkGroupSize[2],
																				_properties.meshShaderProps.maxMeshWorkGroupInvocations ));
			outFeatureSet.maxMeshOutputVertices					= CastPOT( _properties.meshShaderProps.maxMeshOutputVertices );
			outFeatureSet.maxMeshOutputPrimitives				= CastPOT( _properties.meshShaderProps.maxMeshOutputPrimitives );
			outFeatureSet.maxMeshOutputPerVertexGranularity		= CastPOT( _properties.meshShaderProps.meshOutputPerVertexGranularity );
			outFeatureSet.maxMeshOutputPerPrimitiveGranularity	= CastPOT( _properties.meshShaderProps.meshOutputPerPrimitiveGranularity );
			outFeatureSet.maxTaskPayloadSize					= KiBytes{ _properties.meshShaderProps.maxTaskPayloadSize };
			outFeatureSet.maxTaskSharedMemorySize				= KiBytes{ _properties.meshShaderProps.maxTaskSharedMemorySize };
			outFeatureSet.maxTaskPayloadAndSharedMemorySize		= KiBytes{ _properties.meshShaderProps.maxTaskPayloadAndSharedMemorySize };
			outFeatureSet.maxMeshSharedMemorySize				= KiBytes{ _properties.meshShaderProps.maxMeshSharedMemorySize };
			outFeatureSet.maxMeshPayloadAndSharedMemorySize		= KiBytes{ _properties.meshShaderProps.maxMeshPayloadAndSharedMemorySize };
			outFeatureSet.maxMeshOutputMemorySize				= KiBytes{ _properties.meshShaderProps.maxMeshOutputMemorySize };
			outFeatureSet.maxMeshPayloadAndOutputMemorySize		= KiBytes{ _properties.meshShaderProps.maxMeshPayloadAndOutputMemorySize };
			outFeatureSet.maxMeshMultiviewViewCount				= LimitCheckCast{ _properties.meshShaderProps.maxMeshMultiviewViewCount };
			outFeatureSet.maxPreferredTaskWorkGroupInvocations	= CastPOT( _properties.meshShaderProps.maxPreferredTaskWorkGroupInvocations );
			outFeatureSet.maxPreferredMeshWorkGroupInvocations	= CastPOT( _properties.meshShaderProps.maxPreferredMeshWorkGroupInvocations );
		}

		if ( _extensions.vertexDivisor and _properties.vertexDivisorFeats.vertexAttributeInstanceRateDivisor )
		{
			outFeatureSet.vertexDivisor				= True;
			outFeatureSet.maxVertexAttribDivisor	= _properties.vertexDivisorProps.maxVertexAttribDivisor;
		}

		outFeatureSet.maxVertexAttributes	= LimitCheckCast{ limits.maxVertexInputAttributes };
		outFeatureSet.maxVertexBuffers		= LimitCheckCast{ limits.maxVertexInputBindings };

		SET_FEAT( geometryShader );
		SET_FEAT( tessellationShader );
		// skip tileShader

		SET_FEAT( imageCubeArray );
		SET_FEAT( textureCompressionASTC_LDR );
		SET_FEAT( textureCompressionETC2 );
		SET_FEAT( textureCompressionBC );
		// skip imageViewMinLod		// VkPhysicalDeviceImageViewMinLodFeaturesEXT

		SET_EXT2( imageViewFormatList,			imageFormatList );
		SET_EXT2( imageViewExtendedUsage,		maintenance2 );
		SET_EXT2( separateDepthStencilRW,		maintenance2 );
		SET_EXT2( textureCompressionASTC_HDR,	astcHdr );

		outFeatureSet.maxImageDimension1D	= CastPOT( limits.maxImageDimension1D );
		outFeatureSet.maxImageDimension2D	= CastPOT( limits.maxImageDimension2D );
		outFeatureSet.maxImageDimension3D	= CastPOT( limits.maxImageDimension3D );
		outFeatureSet.maxImageDimensionCube	= CastPOT( limits.maxImageDimensionCube );
		outFeatureSet.maxImageArrayLayers	= CastPOT( limits.maxImageArrayLayers );

		SET_FEAT( samplerAnisotropy );
		outFeatureSet.maxSamplerAnisotropy	= limits.maxSamplerAnisotropy;
		outFeatureSet.maxSamplerLodBias		= limits.maxSamplerLodBias;

		SET_EXT2( samplerMirrorClampToEdge, samplerMirrorClamp );
		SET_EXT( samplerYcbcrConversion );
		SET_EXT( ycbcr2Plane444 );

		if ( _extensions.samplerFilterMinmax )
		{
			outFeatureSet.samplerFilterMinmax = True;
			SET_FEAT2( filterMinmaxImageComponentMapping,	_properties.samplerFilterMinmaxProps );
		}

		if ( _extensions.rasterOrderAttachment )
		{
			SET_FEAT2( rasterizationOrderColorAttachmentAccess,		_properties.rasterOrderAttachmentFeats );
			SET_FEAT2( rasterizationOrderDepthAttachmentAccess,		_properties.rasterOrderAttachmentFeats );
			SET_FEAT2( rasterizationOrderStencilAttachmentAccess,	_properties.rasterOrderAttachmentFeats );
		}

		if ( _extensions.shaderIntegerDotProduct							and
			 _properties.shaderIntegerDotProductFeats.shaderIntegerDotProduct )
		{
			auto&	props = _properties.shaderIntegerDotProductProps;
			if ( props.integerDotProduct8BitUnsignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Unsigned8bit );
			if ( props.integerDotProduct8BitSignedAccelerated )											outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Signed8bit );
			if ( props.integerDotProduct8BitMixedSignednessAccelerated )								outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::MixedSignedness8bit );
			if ( props.integerDotProduct4x8BitPackedUnsignedAccelerated )								outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Unsigned4x8bit );
			if ( props.integerDotProduct4x8BitPackedSignedAccelerated )									outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Signed4x8bit );
			if ( props.integerDotProduct4x8BitPackedMixedSignednessAccelerated )						outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::MixedSignedness4x8bit );
			if ( props.integerDotProduct16BitUnsignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Unsigned16bit );
			if ( props.integerDotProduct16BitSignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Signed16bit );
			if ( props.integerDotProduct16BitMixedSignednessAccelerated )								outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::MixedSignedness16bit );
			if ( props.integerDotProduct32BitUnsignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Unsigned32bit );
			if ( props.integerDotProduct32BitSignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Signed32bit );
			if ( props.integerDotProduct32BitMixedSignednessAccelerated )								outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::MixedSignedness32bit );
			if ( props.integerDotProduct64BitUnsignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Unsigned64bit );
			if ( props.integerDotProduct64BitSignedAccelerated )										outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::Signed64bit );
			if ( props.integerDotProduct64BitMixedSignednessAccelerated )								outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::MixedSignedness64bit );
			if ( props.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated )					outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Unsigned8bit );
			if ( props.integerDotProductAccumulatingSaturating8BitSignedAccelerated )					outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Signed8bit );
			if ( props.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_MixedSignedness8bit );
			if ( props.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Unsigned4x8bit );
			if ( props.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Signed4x8bit );
			if ( props.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated )	outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_MixedSignedness4x8bit );
			if ( props.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated )				outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Unsigned16bit );
			if ( props.integerDotProductAccumulatingSaturating16BitSignedAccelerated )					outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Signed16bit );
			if ( props.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_MixedSignedness16bit );
			if ( props.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated )				outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Unsigned32bit );
			if ( props.integerDotProductAccumulatingSaturating32BitSignedAccelerated )					outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Signed32bit );
			if ( props.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_MixedSignedness32bit );
			if ( props.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated )				outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Unsigned64bit );
			if ( props.integerDotProductAccumulatingSaturating64BitSignedAccelerated )					outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_Signed64bit );
			if ( props.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated )			outFeatureSet.integerDotProductFeatures.insert( EIntegerDotProductFeat::AccSat_MixedSignedness64bit );
			outFeatureSet.shaderIntegerDotProduct = True;

			constexpr uint	vk_feats_count = (sizeof(props) - offsetof(VkPhysicalDeviceShaderIntegerDotProductProperties, integerDotProduct8BitUnsignedAccelerated)) / sizeof(VkBool32);
			StaticAssert( vk_feats_count == uint(EIntegerDotProductFeat::_Count) );
		}

		if ( _extensions.shaderFloat8 )
		{
			SET_FEAT2( shaderFloat8,					_properties.shaderFloat8Feats );
			SET_FEAT2( shaderFloat8CooperativeMatrix,	_properties.shaderFloat8Feats );
		}

		if ( _extensions.shaderBFloat16 )
		{
			SET_FEAT2( shaderBFloat16Type,				_properties.shaderBFloat16Feats );
			SET_FEAT2( shaderBFloat16DotProduct,		_properties.shaderBFloat16Feats );
			SET_FEAT2( shaderBFloat16CooperativeMatrix,	_properties.shaderBFloat16Feats );
		}

		if ( _extensions.conservativeRasterization )
			outFeatureSet.conservativeRasterization = True;

		if ( _extensions.deviceGeneratedCommands )
		{
			SET_FEAT2( deviceGeneratedCommands, _properties.deviceGeneratedCommandsFeats );

			auto&	props = _properties.deviceGeneratedCommandsProps;
			SET_FEAT2( deviceGeneratedCommandsMultiDrawIndirectCount,	props );
			outFeatureSet.supportedIndirectCommandsShaderStages					= AEEnumCast( VkShaderStageFlagBits( props.supportedIndirectCommandsShaderStages ));
			outFeatureSet.supportedIndirectCommandsShaderStagesPipelineBinding	= AEEnumCast( VkShaderStageFlagBits( props.supportedIndirectCommandsShaderStagesPipelineBinding ));
			outFeatureSet.maxIndirectPipelineCount = LimitCast{ props.maxIndirectPipelineCount };
		}

		if ( _extensions.opacityMicromap and _properties.opacityMicromapFeats.micromap == VK_TRUE )
		{
			auto&	props = _properties.opacityMicromapProps;

			outFeatureSet.opacityMicromap					= True;
			outFeatureSet.maxOpacity2StateSubdivisionLevel	= LimitCheckCast{ props.maxOpacity2StateSubdivisionLevel };
			outFeatureSet.maxOpacity4StateSubdivisionLevel	= LimitCheckCast{ props.maxOpacity4StateSubdivisionLevel };

			if ( _extensions.displacementMicromapNV and _properties.displacementMicromapNVFeats.displacementMicromap == VK_TRUE )
			{
				outFeatureSet.displacementMicromap = True;
				outFeatureSet.maxDisplacementMicromapSubdivisionLevel = LimitCheckCast{ _properties.displacementMicromapNVProps.maxDisplacementMicromapSubdivisionLevel };
			}
		}

		if ( _extensions.descriptorHeap )
		{
			outFeatureSet.descriptorHeap = True;
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

		outFeatureSet.maxFramebufferLayers = CastPOT( limits.maxFramebufferLayers );

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
						 NoBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::Graphics;

					if ( AllBits( props.queueFlags, VK_QUEUE_COMPUTE_BIT ) and
						 NoBits( props.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::AsyncCompute;

					if ( AllBits( props.queueFlags, VK_QUEUE_TRANSFER_BIT ) and
						 NoBits( props.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::AsyncTransfer;

					if ( AllBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR ) and
						 NoBits( props.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::VideoDecode;

					if ( AllBits( props.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR ) and
						 NoBits( props.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR ))
						outFeatureSet.queues.supported |= EQueueMask::VideoEncode;
				}
			}
		}

		SET_EXT2( externalFormatAndroid, androidExternalMemoryHwBuf );

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

		StaticAssert( FeatureSet::GetFeatureCount() == 281 );
		using EFeature = FeatureSet::EFeature;

		auto&			feats10		= _properties.features;
		auto&			f16i8_feats = _properties.shaderFloat16Int8Feats;
		auto&			limits		= _properties.properties.limits;

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
		SET_FEAT( largePoints );
		SET_FEAT( wideLines );

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

		if ( inFS.shaderSubgroupUniformControlFlow == True )
		{
			_properties.shaderSubgroupUniformControlFlowFeats.shaderSubgroupUniformControlFlow = VK_TRUE;
			_extensions.shaderSubgroupUniformControlFlow = true;
		}

		if ( inFS.shaderMaximalReconvergence == True )
		{
			_properties.shaderMaximalReconvergenceFeats.shaderMaximalReconvergence = VK_TRUE;
			_extensions.shaderMaximalReconvergence = true;
		}

		if ( inFS.shaderQuadControl == True )
		{
			_properties.shaderQuadControlFeats.shaderQuadControl = VK_TRUE;
			_extensions.shaderQuadControl = true;
		}

		if ( inFS.shaderExpectAssume == True )
			_extensions.shaderExpectAssume = true;

		if ( inFS.subgroupBroadcastDynamicId == True )
		{
			CHECK_ERR( _extensions.subgroup );
			CHECK_ERR( _vkDeviceVersion >= DeviceVersion(1,2) );
		}

		if ( AnyBits( inFS.subgroupTypes, ~(ESubgroupTypes::Float32 | ESubgroupTypes::Int32) ))
		{
			CHECK_ERR( _extensions.subgroupExtendedTypes );
			_properties.subgroupExtendedTypesFeats.shaderSubgroupExtendedTypes	= true;

			if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int16 ))	CHECK_ERR( feats10.shaderInt16 == VK_TRUE )	else feats10.shaderInt16 = false;
			if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int64 ))	CHECK_ERR( feats10.shaderInt64 == VK_TRUE )	else feats10.shaderInt64 = false;

			if ( AnyBits( inFS.subgroupTypes, ESubgroupTypes::Int8 | ESubgroupTypes::Float16 ))
			{
				CHECK_ERR( _extensions.shaderFloat16Int8 );
				if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Int8 ))		CHECK_ERR( f16i8_feats.shaderInt8 == VK_TRUE )		else f16i8_feats.shaderInt8		= false;
				if ( AllBits( inFS.subgroupTypes, ESubgroupTypes::Float16 ))	CHECK_ERR( f16i8_feats.shaderFloat16 == VK_TRUE )	else f16i8_feats.shaderFloat16	= false;
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

		_extensions.shaderAtomicPackedFp16	= (inFS.shaderAtomicPackedFp16 == True);

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

		SET_FEAT2( shaderInputAttachmentArrayDynamicIndexing,			_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformTexelBufferArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageTexelBufferArrayDynamicIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformBufferArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderSampledImageArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageBufferArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageImageArrayNonUniformIndexing,			_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderInputAttachmentArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderUniformTexelBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( shaderStorageTexelBufferArrayNonUniformIndexing,		_properties.descriptorIndexingFeats );
		SET_FEAT2( runtimeDescriptorArray,								_properties.descriptorIndexingFeats );
		SET_FEAT2( descriptorBindingVariableDescriptorCount,			_properties.descriptorIndexingFeats );
		SET_FEAT2( quadDivergentImplicitLod,							_properties.descriptorIndexingProps );

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
		_properties.fragShadingRateProps.maxFragmentShadingRateAttachmentTexelSizeAspectRatio	= inFS.fragmentShadingRateTexelSize.MaxAspectRatio();

		_extensions.fragDensityMap = (inFS.fragmentDensityMap == True);
		SET_FEAT2( fragmentDensityMap,						_properties.fragDensityMapFeats );
		SET_FEAT2( fragmentDensityMapDynamic,				_properties.fragDensityMapFeats );
		SET_FEAT2( fragmentDensityMapNonSubsampledImages,	_properties.fragDensityMapFeats );

		_extensions.fragDensityMap2 =	(inFS.fragmentDensityMap	== True)		and
										((inFS.subsampledLoads		== True)	or
										 (inFS.maxSubsampledArrayLayers > 0)	or
										 (inFS.perPipeline_maxSubsampledSamplers > 0));
		if ( _extensions.fragDensityMap2 )
			_properties.fragDensityMap2Feats.fragmentDensityMapDeferred = false;

		_extensions.rayQuery = (inFS.rayQuery == True);
		SET_FEAT2( rayQuery,	_properties.rayQueryFeats );

		_extensions.rayTracingPipeline  =	(inFS.rayTracingPipeline			== True)	or
											(inFS.rayTraversalPrimitiveCulling	== True);
		SET_FEAT2( rayTracingPipeline,				_properties.rayTracingPipelineFeats );
		SET_FEAT2( rayTraversalPrimitiveCulling,	_properties.rayTracingPipelineFeats );

		_extensions.accelerationStructure = _extensions.rayTracingPipeline or _extensions.rayQuery;
		_properties.accelerationStructureFeats.accelerationStructureIndirectBuild = (inFS.accelerationStructureIndirectBuild == True);

		_extensions.clusterAccelStructNV = (inFS.clusterAccelerationStructure == True);
		_properties.clusterAccelStructNVFeats.clusterAccelerationStructure = _extensions.clusterAccelStructNV;

		_extensions.partitionedAccelStructNV = (inFS.partitionedAccelerationStructure == True);
		_properties.partitionedAccelStructNVFeats.partitionedAccelerationStructure = _extensions.partitionedAccelStructNV;

		SET_FEAT( drawIndirectFirstInstance );
		_extensions.drawIndirectCount	= (inFS.drawIndirectCount == True);
		limits.maxDrawIndirectCount		= inFS.maxDrawIndirectCount;

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

		limits.maxDescriptorSetUniformBuffersDynamic	= inFS.perPipeline_maxUniformBuffersDynamic;
		limits.maxDescriptorSetStorageBuffersDynamic	= inFS.perPipeline_maxStorageBuffersDynamic;
		limits.maxDescriptorSetInputAttachments			= inFS.perPipeline.maxInputAttachments;
		limits.maxDescriptorSetSampledImages			= inFS.perPipeline.maxSampledImages;
		limits.maxDescriptorSetSamplers					= inFS.perPipeline.maxSamplers;
		limits.maxDescriptorSetStorageBuffers			= inFS.perPipeline.maxStorageBuffers;
		limits.maxDescriptorSetStorageImages			= inFS.perPipeline.maxStorageImages;
		limits.maxDescriptorSetUniformBuffers			= inFS.perPipeline.maxUniformBuffers;

		_properties.maintenance3Props.maxPerSetDescriptors	= inFS.perDescSet_maxTotalResources;

		limits.maxPerStageDescriptorInputAttachments	= inFS.perStage.maxInputAttachments;
		limits.maxPerStageDescriptorSampledImages		= inFS.perStage.maxSampledImages;
		limits.maxPerStageDescriptorSamplers			= inFS.perStage.maxSamplers;
		limits.maxPerStageDescriptorStorageBuffers		= inFS.perStage.maxStorageBuffers;
		limits.maxPerStageDescriptorStorageImages		= inFS.perStage.maxStorageImages;
		limits.maxPerStageDescriptorUniformBuffers		= inFS.perStage.maxUniformBuffers;
		limits.maxPerStageResources						= inFS.perStage_maxTotalResources;

		limits.maxVertexInputAttributes	= inFS.maxVertexAttributes;
		limits.maxVertexInputBindings	= inFS.maxVertexBuffers;

		limits.maxImageDimension1D		= uint{inFS.maxImageDimension1D};
		limits.maxImageDimension2D		= uint{inFS.maxImageDimension2D};
		limits.maxImageDimension3D		= uint{inFS.maxImageDimension3D};
		limits.maxImageDimensionCube	= uint{inFS.maxImageDimensionCube};
		limits.maxImageArrayLayers		= uint{inFS.maxImageArrayLayers};

		limits.maxSamplerAnisotropy		= inFS.maxSamplerAnisotropy;
		limits.maxSamplerLodBias		= inFS.maxSamplerLodBias;
		limits.maxFramebufferLayers		= uint{inFS.maxFramebufferLayers};

		return true;

		#undef SET_FEAT
		#undef SET_FEAT2
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
