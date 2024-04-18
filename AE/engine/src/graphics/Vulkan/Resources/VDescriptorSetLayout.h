// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Vulkan/VCommon.h"
# include "PipelineCompilerImpl.h"

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
			ArrayView<VkSampler>	samplerStorage;
			EDescSetUsage			usage;
			StringView				dbgName;
		};

	private:
		struct DescriptorBinding
		{
			Array< VkDescriptorSetLayoutBinding >		desc;		// TODO: temp allocator ?
			Array< VkDescriptorUpdateTemplateEntry >	entries;
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

		#include "graphics/Private/DescriptorSetLayout.h"


	// methods
	public:
		VDescriptorSetLayout ()										__NE___	{}
		~VDescriptorSetLayout ()									__NE___;

		ND_ bool  Create (const VDevice &dev, StringView dbgName)	__NE___;
		ND_ bool  Create (const VDevice &dev, const CreateInfo &ci)	__NE___;
			void  Destroy (VResourceManager &)						__NE___;

		ND_ VkDescriptorSetLayout		Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkDescriptorUpdateTemplate	GetUpdateTemplate ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _updateTmpl; }
		ND_ uint						GetBindingCount ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bindCount; }
		ND_ Bytes						GetUpdateTemplateSize ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return Bytes{_updateTmplSize}; }


	private:
		ND_ bool  _AddUniform (const Uniform_t &un, ArrayView<VkSampler> samplerStorage, OUT Bytes16u* offset, INOUT DescriptorBinding &binding)C_NE___;
			void  _AddBuffer (const Buffer &buf, INOUT VkDescriptorSetLayoutBinding &bind, INOUT DescriptorBinding &binding)					C_NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
