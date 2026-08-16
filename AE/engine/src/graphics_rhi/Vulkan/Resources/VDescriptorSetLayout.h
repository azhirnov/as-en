// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/DescriptorSet.h"
# include "graphics_rhi/Private/EnumUtils.h"
# include "graphics_rhi/Vulkan/VCommon.h"
# include "res_pack/pipeline_compiler/PipelineCompilerImpl.h"

namespace AE::Graphics
{

	//
	// Vulkan Descriptor Set Layout
	//

	class VDescriptorSetLayout final
	{
	// types
	public:
		using UniformOffsets_t	= PipelineCompiler::DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniform_t			= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t		= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		struct CreateInfo
		{
			Uniforms_t				uniforms;
			UniformOffsets_t		unOffsets;
			ArrayView<SamplerID>	samplerStorage;
			EDescSetUsage			usage;
			StringView				dbgName;
		};

	private:
		struct DescriptorBinding
		{
			Array< VkDescriptorSetLayoutBinding >		desc;		// TODO: temp allocator ?
			Array< VkDescriptorUpdateTemplateEntry >	entries;
			Array< VkDescriptorBindingFlags >			flags;
			Bytes										tmplEntryOffset;
			uint										dbCount			= 0;
			bool										allowUpdateTmpl	= false;
		};


	// variables
	private:
		VkDescriptorSetLayout		_layout			= Default;
		VkDescriptorUpdateTemplate	_updateTmpl		= Default;

		uint						_bindCount		= 0;
		Bytes32u					_updateTmplSize;
		ArrayView<SamplerID>		_samplerStorage;			// allocated by PipelinePack allocator

		#include "graphics_rhi/Private/DescriptorSetLayout.h"


	// methods
	public:
		VDescriptorSetLayout ()													__NE___	{}
		~VDescriptorSetLayout ()												__NE___;

		ND_ bool  Create (const VDevice &dev, StringView dbgName)				__NE___;
		ND_ bool  Create (const ResourceManager &resMngr, const CreateInfo &ci)	__NE___;
			void  Destroy (ResourceManager &)									__NE___;

		ND_ VkDescriptorSetLayout		Handle ()								C_NE___	{ return _layout; }
		ND_ VkDescriptorUpdateTemplate	GetUpdateTemplate ()					C_NE___	{ return _updateTmpl; }
		ND_ uint						GetBindingCount ()						C_NE___	{ return _bindCount; }
		ND_ Bytes						GetUpdateTemplateSize ()				C_NE___	{ return Bytes{_updateTmplSize}; }
		ND_ ArrayView<SamplerID>		GetSamplerStorage ()					C_NE___	{ return _samplerStorage; }


	private:
		ND_ bool  _AddUniform (const Uniform_t &un, ArrayView<VkSampler> samplerStorage, OUT Bytes16u* offset, INOUT DescriptorBinding &binding)C_NE___;
			void  _AddBuffer (const Buffer &buf, INOUT VkDescriptorSetLayoutBinding &bind, INOUT DescriptorBinding &binding)					C_NE___;

		ND_ bool  _CreateDSL (const ResourceManager &resMngr, const CreateInfo &ci)	__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
