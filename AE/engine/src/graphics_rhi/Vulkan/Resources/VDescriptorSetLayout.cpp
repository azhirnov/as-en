// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VDescriptorSetLayout.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
/*
=================================================
	VEnumCast
=================================================
*/
	ND_ inline VkDescriptorBindingFlags  VEnumCast (EDescriptorFlags flags) __NE___
	{
		VkDescriptorBindingFlags	res = 0;

		for (auto t : BitfieldIterate( flags ))
		{
			switch_enum( t )
			{
				case EDescriptorFlags::VariableSize :	res |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;	break;
				case EDescriptorFlags::_BITOPS_ :
				case EDescriptorFlags::_Last :
				default_unlikely :						RETURN_ERR( "unknown descriptor flags!", Zero );
			}
			switch_end
		}
		return res;
	}

/*
=================================================
	destructor
=================================================
*/
	VDescriptorSetLayout::~VDescriptorSetLayout () __NE___
	{
		CHECK( not _layout );
	}

/*
=================================================
	Create
----
	create empty layout
=================================================
*/
	bool  VDescriptorSetLayout::Create (const VDevice &dev, StringView dbgName) __NE___
	{
		CHECK_ERR( not _layout );

		// Don't use old descriptor model when enabled descriptor heap
		if ( dev.GetVExtensions().descriptorHeap )
			return true;

		VkDescriptorSetLayoutCreateInfo	descriptor_info = {};
		descriptor_info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_info.pBindings		= null;
		descriptor_info.bindingCount	= 0;
		VK_CHECK_ERR( dev.vkCreateDescriptorSetLayout( dev.GetVkDevice(), &descriptor_info, null, OUT &_layout ));

		dev.SetObjectName( _layout, dbgName, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT );
		GFX_DBG_ONLY( _debugName = dbgName; )

		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VDescriptorSetLayout::Create (const ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		CHECK_ERR( not _layout );

		_usage		= ci.usage;
		_bindCount	= 0;
		_uniforms	= ci.uniforms;
		_unOffsets	= ci.unOffsets;

		GFX_DBG_ONLY( _debugName = ci.dbgName; )

		// Don't use old descriptor model when enabled descriptor heap
		if ( resMngr.GetDevice().GetVExtensions().descriptorHeap )
		{
			_samplerStorage	= ci.samplerStorage;
			return true;
		}

		return _CreateDSL( resMngr, ci );
	}

/*
=================================================
	_CreateDSL
=================================================
*/
	bool  VDescriptorSetLayout::_CreateDSL (const ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		Array<VkSampler>	vk_samplers;
		NOTHROW_ERR( vk_samplers.resize( ci.samplerStorage.size() ));

		for (usize i = 0; i < ci.samplerStorage.size(); ++i)
		{
			const VSampler*		samp = resMngr.GetResource( ci.samplerStorage[i] );
			CHECK_ERR( samp != null );

			vk_samplers[i] = samp->Handle();
		}

		DescriptorBinding	binding;
		binding.allowUpdateTmpl	= AllBits( _usage, EDescSetUsage::UpdateTemplate );

		CHECK_ERR( binding.allowUpdateTmpl == (_uniforms.Get<3>() != null) );

		for (uint i = 0, cnt = _uniforms.Get<0>(); i < cnt; ++i)
		{
			auto&	name	= _uniforms.Get<1>()[i];
			auto&	un		= _uniforms.Get<2>()[i];
			auto*	off		= binding.allowUpdateTmpl ? _uniforms.Get<3>() + i : null;

			CHECK_ERR( name.IsDefined() );
			CHECK_ERR( _AddUniform( un, vk_samplers, OUT off, INOUT binding ));

			_bindCount = Max( _bindCount, un.binding.vkIndex + 1u );
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfo	ext_flags = {};
		ext_flags.sType	= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;

		VkDescriptorSetLayoutCreateInfo	descriptor_info = {};
		descriptor_info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_info.pBindings		= binding.desc.data();
		descriptor_info.bindingCount	= uint(binding.desc.size());

		auto&	dev = resMngr.GetDevice();
		if ( dev.GetVExtensions().descriptorIndexing and not binding.flags.empty() )
		{
			binding.flags.resize( binding.desc.size(), 0u );

			descriptor_info.pNext	= &ext_flags;

			ext_flags.bindingCount	= uint(binding.flags.size());
			ext_flags.pBindingFlags	= binding.flags.data();
		}

		#if AE_DBG_GRAPHICS
		if ( dev.GetVExtensions().maintenance3 or dev.GetVkVersion() >= Version2{1,1} )
		{
			VkDescriptorSetLayoutSupport	dsl_support = {};
			dsl_support.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;
			dev.vkGetDescriptorSetLayoutSupportKHR( dev.GetVkDevice(), &descriptor_info, OUT &dsl_support );
			GRES_CHECK( dsl_support.supported == VK_TRUE );
		}
		#endif

		VK_CHECK_ERR( dev.vkCreateDescriptorSetLayout( dev.GetVkDevice(), &descriptor_info, null, OUT &_layout ));

		if ( binding.allowUpdateTmpl )
		{
			CHECK_ERR( not binding.entries.empty() );

			VkDescriptorUpdateTemplateCreateInfo	tmpl_info = {};
			tmpl_info.sType							= VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
			tmpl_info.descriptorUpdateEntryCount	= uint(binding.entries.size());
			tmpl_info.pDescriptorUpdateEntries		= binding.entries.data();
			tmpl_info.templateType					= VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
			tmpl_info.descriptorSetLayout			= _layout;

			VK_CHECK_ERR( dev.vkCreateDescriptorUpdateTemplateKHR( dev.GetVkDevice(), &tmpl_info, null, OUT &_updateTmpl ));

			_updateTmplSize = binding.tmplEntryOffset;
		}

		dev.SetObjectName( _layout, ci.dbgName, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT );
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VDescriptorSetLayout::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		if ( _layout != Default )
			dev.vkDestroyDescriptorSetLayout( dev.GetVkDevice(), _layout, null );

		if ( _updateTmpl != Default )
			dev.vkDestroyDescriptorUpdateTemplateKHR( dev.GetVkDevice(), _updateTmpl, null );

		_unOffsets.fill( 0 );
		_uniforms		= Default;
		_updateTmplSize	= Default;
		_updateTmpl		= Default;
		_layout			= Default;
		_usage			= Default;
		_bindCount		= 0;
		_samplerStorage	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	_AddUniform
=================================================
*/
	bool  VDescriptorSetLayout::_AddUniform (const Uniform_t &un, ArrayView<VkSampler> samplerStorage, OUT Bytes16u* offset, INOUT DescriptorBinding &binding) C_NE___
	{
		static constexpr Bytes	align = Max( Max( AlignOf<VkDescriptorBufferInfo>, AlignOf<VkBufferView> ),
											 Max( AlignOf<VkDescriptorImageInfo>, AlignOf<VkWriteDescriptorSetAccelerationStructureKHR> ));

		CHECK_ERR( un.binding.vkIndex != UMax );
		CHECK_ERR( un.arraySize > 0 );	// array size must be specialized at compile or creation time

		Bytes	entry_stride;

		VkDescriptorSetLayoutBinding	bind = {};
		bind.stageFlags			= VEnumCast( un.stages );
		bind.binding			= un.binding.vkIndex;
		bind.descriptorCount	= un.arraySize;

		switch_enum( un.type )
		{
			case EDescriptorType::UniformBuffer :
				entry_stride		= SizeOf<VkDescriptorBufferInfo>;
				bind.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				_AddBuffer( un.buffer, INOUT bind, INOUT binding );
				break;

			case EDescriptorType::StorageBuffer :
				entry_stride		= SizeOf<VkDescriptorBufferInfo>;
				bind.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				_AddBuffer( un.buffer, INOUT bind, INOUT binding );
				break;

			case EDescriptorType::UniformTexelBuffer :
				entry_stride		= SizeOf<VkBufferView>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				break;

			case EDescriptorType::StorageTexelBuffer :
				entry_stride		= SizeOf<VkBufferView>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
				break;

			case EDescriptorType::StorageImage :
				entry_stride		= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;

			case EDescriptorType::SubpassInput :
				entry_stride		= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				break;

			case EDescriptorType::SampledImage :
				entry_stride		= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;

			case EDescriptorType::CombinedImage :
				entry_stride		= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;

			case EDescriptorType::Sampler :
				entry_stride		= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;

			case EDescriptorType::ImmutableSampler :
				bind.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLER;
				bind.pImmutableSamplers	= samplerStorage.data() + un.immutableSampler.offsetInStorage;
				break;

			case EDescriptorType::CombinedImage_ImmutableSampler :
				entry_stride			= SizeOf<VkDescriptorImageInfo>;
				bind.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bind.pImmutableSamplers	= samplerStorage.data() + un.image.samplerOffsetInStorage;
				break;

			case EDescriptorType::RayTracingScene :
				entry_stride		= SizeOf<VkWriteDescriptorSetAccelerationStructureKHR>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
				break;

			case EDescriptorType::RayTracingPartitionedScene :
				entry_stride		= SizeOf<VkWriteDescriptorSetPartitionedAccelerationStructureNV>;
				bind.descriptorType = VK_DESCRIPTOR_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_NV;
				break;

			case EDescriptorType::_Count :
			default_unlikely :
				RETURN_ERR( "unsupported descriptor type" );
		}
		switch_end

		binding.desc.push_back( bind );

		if ( un.flags != Default )
		{
			binding.flags.resize( binding.desc.size(), 0u );
			binding.flags.back() = VEnumCast( un.flags );
		}

		// some descriptors may be not supported
		if ( binding.allowUpdateTmpl and entry_stride > 0 )
		{
			NonNull( offset );
			binding.tmplEntryOffset = AlignUp( binding.tmplEntryOffset, align );

			*offset = binding.tmplEntryOffset;

			auto&	entry			= binding.entries.emplace_back();
			entry.dstBinding		= bind.binding;
			entry.dstArrayElement	= 0;
			entry.descriptorCount	= bind.descriptorCount;
			entry.descriptorType	= bind.descriptorType;	// TODO image with immutable sampler must be combined or separated?
			entry.offset			= usize(binding.tmplEntryOffset);
			entry.stride			= usize(entry_stride);

			binding.tmplEntryOffset += bind.descriptorCount * entry_stride;
		}

		return true;
	}

/*
=================================================
	_AddBuffer
=================================================
*/
	inline void  VDescriptorSetLayout::_AddBuffer (const Buffer &buf, INOUT VkDescriptorSetLayoutBinding &bind, INOUT DescriptorBinding &binding) C_NE___
	{
		const bool	is_dynamic = buf.dynamicOffsetIndex != UMax;

		if ( is_dynamic ) {
			bind.descriptorType =
				bind.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC :
				bind.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}

		binding.dbCount += (is_dynamic ? bind.descriptorCount : 0);
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
