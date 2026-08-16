// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VPipelineLayout.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VPipelineLayout::~VPipelineLayout () __NE___
	{
		CHECK( IsNullUnion( _layout ));
	}

/*
=================================================
	Create
=================================================
*/
	bool  VPipelineLayout::Create (ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		CHECK_ERR( IsNullUnion( _layout ));

		_descriptorSets	= ci.descSetLayouts;
		_pushConstants	= ci.pushConstants;
		GFX_DBG_ONLY( _debugName = ci.dbgName; )

		// Vulkan docs: "applications should generally stick to the same heap throughout the lifetime of the application",
		// so use heaps if they are enabled or disable extension using 'VDeviceInitializer::DeviceCreateInfo::disableFeatures'.

		if ( resMngr.GetDevice().GetVExtensions().descriptorHeap )
			return _CreateHeapMapping( resMngr, ci );
		else
			return _CreateLayout( resMngr, ci );
	}

/*
=================================================
	_CreateLayout
=================================================
*/
	bool  VPipelineLayout::_CreateLayout (ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		using VkDescriptorSetLayouts_t	= StaticArray< VkDescriptorSetLayout, GraphicsConfig::MaxDescriptorSets >;
		using VkPushConstantRanges_t	= FixedArray< VkPushConstantRange, GraphicsConfig::MaxPushConstants >;

		VkDescriptorSetLayouts_t	vk_layouts	= {};
		VkPushConstantRanges_t		vk_ranges	= {};

		for (auto& layout : vk_layouts) {
			layout = ci.emptyLayout;
		}

		uint	min_set = uint(vk_layouts.size());
		uint	max_set = 1;

		for (auto [name, ds] : _descriptorSets)
		{
			CHECK_ERR( ds.index.vkIndex != UMax );
			CHECK_ERR( vk_layouts[ ds.index.vkIndex ] == ci.emptyLayout );	// already set

			auto*	ds_layout = resMngr.GetResource( ds.layoutId );
			CHECK_ERR( ds_layout != null );

			vk_layouts[ ds.index.vkIndex ] = ds_layout->Handle();
			min_set = Min( min_set, ds.index.vkIndex );
			max_set = Max( max_set, ds.index.vkIndex + 1 );
		}

		for (auto pc : _pushConstants)
		{
			VkPushConstantRange	range = {};
			range.offset		= uint( pc.second.vulkanOffset );
			range.size			= uint( pc.second.size );
			range.stageFlags	= VEnumCast( pc.second.stage );

			vk_ranges.push_back( range );
		}

		VkPipelineLayoutCreateInfo			layout_info = {};
		layout_info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layout_info.setLayoutCount			= max_set;
		layout_info.pSetLayouts				= vk_layouts.data();
		layout_info.pushConstantRangeCount	= uint(vk_ranges.size());
		layout_info.pPushConstantRanges		= vk_ranges.data();

		VkPipelineLayout	pl;

		auto&	dev = resMngr.GetDevice();
		VK_CHECK_ERR( dev.vkCreatePipelineLayout( dev.GetVkDevice(), &layout_info, null, OUT &pl ));

		_firstDescSet	= min_set;
		_layout			= pl;

		dev.SetObjectName( pl, ci.dbgName, VK_OBJECT_TYPE_PIPELINE_LAYOUT );
		return true;
	}

/*
=================================================
	AddDescriptorSetLayoutMapping
=================================================
*/
namespace {
	struct HeapOffsets
	{
		uint	buffer		= 0;
		uint	image		= 0;
		uint	sampler		= 0;
	};

	static bool  AddDescriptorSetLayoutMapping (INOUT Array<VkDescriptorSetAndBindingMappingEXT> &mapping, INOUT HeapOffsets &heapOffset,
												uint dsIndex, const VDescriptorSetLayout::Uniforms_t &uniforms,
												const VkPhysicalDeviceDescriptorHeapPropertiesEXT &props,
												ArrayView<const VkSamplerCreateInfo *> samplersCI)
	{
		for (uint i = 0, cnt = uniforms.Get<0>(); i < cnt; ++i)
		{
			auto&	name	= uniforms.Get<1>()[i];
			auto&	un		= uniforms.Get<2>()[i];

			CHECK_ERR( name.IsDefined() );
			CHECK_ERR( un.arraySize > 0 );

			auto&	entry		= mapping.emplace_back();
			entry				= {};
			entry.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_AND_BINDING_MAPPING_EXT;
			entry.descriptorSet	= dsIndex;
			entry.firstBinding	= un.binding.vkIndex;
			entry.bindingCount	= un.arraySize;
			entry.source		= VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_CONSTANT_OFFSET_EXT;

			const auto	AddImage = [&] (VkSpirvResourceTypeFlagBitsEXT resType)
			{{
				entry.resourceMask								= resType;
				entry.sourceData.constantOffset.heapOffset		= heapOffset.image;
				entry.sourceData.constantOffset.heapArrayStride	= uint(props.imageDescriptorSize);
				heapOffset.image								+= uint(props.imageDescriptorSize) * un.arraySize;	// TODO: alignment
			}};

			const auto	AddBuffer = [&] (VkSpirvResourceTypeFlagBitsEXT resType)
			{{
				entry.resourceMask								= resType;
				entry.sourceData.constantOffset.heapOffset		= heapOffset.buffer;
				entry.sourceData.constantOffset.heapArrayStride	= uint(props.bufferDescriptorSize);
				heapOffset.buffer								+= uint(props.bufferDescriptorSize) * un.arraySize;	// TODO: alignment
			}};

			const auto	CheckImtblSamplers = [&un, &samplersCI] (uint offsetInStorage) -> bool
			{{
				CHECK_ERR( offsetInStorage < samplersCI.size() );
				CHECK_ERR( offsetInStorage + un.arraySize <= samplersCI.size() );

				// TODO: replace immutable sampler by sampler descriptors?

				bool	all_equal	= true;
				auto*	first		= samplersCI[ offsetInStorage ];
				for (usize i = offsetInStorage + 1, end = offsetInStorage + un.arraySize; i < end; ++i) {
					all_equal &= (first == samplersCI[i]);
				}
				CHECK_ERR_MSG( all_equal, "descriptor heap supports only one immutable sampler per array" );
				return true;
			}};

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
					AddBuffer( VK_SPIRV_RESOURCE_TYPE_UNIFORM_BUFFER_BIT_EXT );
					break;

				case EDescriptorType::StorageBuffer :
					AddBuffer( VK_SPIRV_RESOURCE_TYPE_READ_WRITE_STORAGE_BUFFER_BIT_EXT );	// TODO: or VK_SPIRV_RESOURCE_TYPE_READ_ONLY_STORAGE_BUFFER_BIT_EXT
					break;

				case EDescriptorType::RayTracingScene :
					AddBuffer( VK_SPIRV_RESOURCE_TYPE_ACCELERATION_STRUCTURE_BIT_EXT );
					break;

				case EDescriptorType::StorageImage :
					AddImage( VK_SPIRV_RESOURCE_TYPE_READ_WRITE_IMAGE_BIT_EXT );	// TODO: or VK_SPIRV_RESOURCE_TYPE_READ_ONLY_IMAGE_BIT_EXT
					break;

				case EDescriptorType::SampledImage :
					AddImage( VK_SPIRV_RESOURCE_TYPE_SAMPLED_IMAGE_BIT_EXT );
					break;

				case EDescriptorType::CombinedImage :
					AddImage( VK_SPIRV_RESOURCE_TYPE_COMBINED_SAMPLED_IMAGE_BIT_EXT );

					entry.sourceData.constantOffset.samplerHeapOffset		= heapOffset.sampler;
					entry.sourceData.constantOffset.samplerHeapArrayStride	= uint(props.samplerDescriptorSize);
					heapOffset.sampler										+= uint(props.samplerDescriptorSize) * un.arraySize;	// TODO: alignment
					break;

				case EDescriptorType::CombinedImage_ImmutableSampler :
					AddImage( VK_SPIRV_RESOURCE_TYPE_COMBINED_SAMPLED_IMAGE_BIT_EXT );

					CHECK_ERR( CheckImtblSamplers( un.image.samplerOffsetInStorage ));
					entry.sourceData.constantOffset.pEmbeddedSampler	= samplersCI[ un.image.samplerOffsetInStorage ];
					break;

				case EDescriptorType::ImmutableSampler :
					CHECK_ERR( CheckImtblSamplers( un.immutableSampler.offsetInStorage ));
					entry.resourceMask									= VK_SPIRV_RESOURCE_TYPE_SAMPLER_BIT_EXT;
					entry.sourceData.constantOffset.pEmbeddedSampler	= samplersCI[ un.immutableSampler.offsetInStorage ];
					break;

				case EDescriptorType::Sampler :
					entry.resourceMask								= VK_SPIRV_RESOURCE_TYPE_SAMPLER_BIT_EXT;
					entry.sourceData.constantOffset.heapOffset		= heapOffset.sampler;
					entry.sourceData.constantOffset.heapArrayStride	= uint(props.samplerDescriptorSize);
					heapOffset.sampler								+= uint(props.samplerDescriptorSize) * un.arraySize;	// TODO: alignment
					break;

				case EDescriptorType::SubpassInput :
					AddImage( VK_SPIRV_RESOURCE_TYPE_READ_ONLY_IMAGE_BIT_EXT );
					break;

				case EDescriptorType::UniformTexelBuffer :
					AddImage( VK_SPIRV_RESOURCE_TYPE_READ_ONLY_IMAGE_BIT_EXT );
					break;

				case EDescriptorType::StorageTexelBuffer :
					AddImage( VK_SPIRV_RESOURCE_TYPE_READ_WRITE_IMAGE_BIT_EXT );	// TODO: or VK_SPIRV_RESOURCE_TYPE_READ_ONLY_IMAGE_BIT_EXT
					break;

				case EDescriptorType::RayTracingPartitionedScene :
				case EDescriptorType::_Count :
				default_unlikely :
					RETURN_ERR( "unsupported descriptor type" );
			}
			switch_end

		}
		return true;
	}
}
/*
=================================================
	_CreateHeapMapping
----
	[proposal](https://github.com/KhronosGroup/Vulkan-Docs/blob/main/proposals/VK_EXT_descriptor_heap.adoc)
	[guide](https://docs.vulkan.org/guide/latest/descriptor_heap.html)
=================================================
*/
	bool  VPipelineLayout::_CreateHeapMapping (const ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		CHECK_ERR( ci.allocator != null );

		Array<VkDescriptorSetAndBindingMappingEXT>		mapping;
		BitSet<GraphicsConfig::MaxDescriptorSets>		bound_dsl;
		Array<const VkSamplerCreateInfo *>				samplers_ci;

		auto&		dev		= resMngr.GetDevice();
		auto&		props	= dev.GetVProperties().descriptorHeapProps;
		HeapOffsets	offsets;
		uint		min_set		= GraphicsConfig::MaxDescriptorSets;
		uint		max_set		= 1;

		for (auto [name, ds] : _descriptorSets)
		{
			CHECK_ERR( ds.index.vkIndex != UMax );
			CHECK_ERR( not bound_dsl.test( ds.index.vkIndex ));	// already set

			auto*	ds_layout = resMngr.GetResource( ds.layoutId );
			CHECK_ERR( ds_layout != null );

			bound_dsl.set( ds.index.vkIndex );
			min_set = Min( min_set, ds.index.vkIndex );
			max_set = Max( max_set, ds.index.vkIndex + 1 );

			auto	sampler_ids = ds_layout->GetSamplerStorage();
			NOTHROW_ERR( samplers_ci.resize( sampler_ids.size() ));

			for (usize i = 0; i < samplers_ci.size(); ++i)
			{
				auto*	sampler = resMngr.GetResource( sampler_ids[i] );
				CHECK_ERR( sampler != null );

				samplers_ci[i] = sampler->GetCreateInfo();
				CHECK_ERR( samplers_ci[i] != null );
			}

			CHECK_ERR( AddDescriptorSetLayoutMapping( INOUT mapping, INOUT offsets, ds.index.vkIndex, ds_layout->GetUniforms(), props, samplers_ci ));
		}

		for (auto pc : _pushConstants)
		{
			// TODO
		}

		auto*	p_mapping_info	= ci.allocator->Allocate<VkShaderDescriptorSetAndBindingMappingInfoEXT>();
		auto*	p_mapping		= mapping.size() ? ci.allocator->Allocate<VkDescriptorSetAndBindingMappingEXT>( mapping.size() ) : null;
		CHECK_ERR( p_mapping_info != null );
		CHECK_ERR( mapping.empty() or p_mapping != null );

		VkShaderDescriptorSetAndBindingMappingInfoEXT	mapping_info = {};
		mapping_info.sType			= VK_STRUCTURE_TYPE_SHADER_DESCRIPTOR_SET_AND_BINDING_MAPPING_INFO_EXT;
		mapping_info.mappingCount	= uint(mapping.size());
		mapping_info.pMappings		= p_mapping;

		MemCopy( OUT p_mapping_info, &mapping_info, Sizeof(mapping_info) );
		MemCopy( OUT p_mapping, mapping.data(), ArraySizeOf(mapping) );

		_firstDescSet	= min_set;
		_layout			= p_mapping_info;
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VPipelineLayout::Destroy (ResourceManager &resMngr) __NE___
	{
		if ( auto* layout = UnionGet<VkPipelineLayout>( _layout ))
		{
			auto&	dev = resMngr.GetDevice();
			dev.vkDestroyPipelineLayout( dev.GetVkDevice(), *layout, null );
		}

		_descriptorSets.clear();
		_pushConstants.clear();

		_layout			= Default;
		_firstDescSet	= UMax;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetDescriptorSetLayout
=================================================
*/
	bool  VPipelineLayout::GetDescriptorSetLayout (DescriptorSetName::Ref id, OUT DescriptorSetLayoutID &layout, OUT DescSetBinding &binding) C_NE___
	{
		auto	it = _descriptorSets.find( id );

		if_likely( it != _descriptorSets.end() )
		{
			layout	= it->second.layoutId;
			binding	= it->second.index;
			return true;
		}

		return false;
	}

/*
=================================================
	Handle
=================================================
*/
	VkPipelineLayout  VPipelineLayout::Handle () C_NE___
	{
		if ( auto* layout = UnionGet<VkPipelineLayout>( _layout ))
			return *layout;

		return Default;
	}

/*
=================================================
	GetHeapMapping
=================================================
*/
	auto  VPipelineLayout::GetHeapMapping () C_NE___ -> VkShaderDescriptorSetAndBindingMappingInfoEXT const*
	{
		if ( auto* mapping = UnionGet<VkShaderDescriptorSetAndBindingMappingInfoEXT const*>( _layout ))
			return *mapping;

		return null;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
