// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Metal/MCommon.h"
# include "PipelineCompilerImpl.h"

namespace AE::Graphics
{

	//
	// Metal Descriptor Set Layout
	//

	class MDescriptorSetLayout final
	{
	// types
	public:
		using UniformOffsets_t	= PipelineCompiler::DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniform_t			= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t		= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		struct CreateInfo
		{
			Uniforms_t				uniforms;
			EDescSetUsage			usage;
			EShaderStages			stages;
			StringView				dbgName;
		};


	// variables
	private:
		MetalArrayOfArgumentDescriptorRC	_descArray;
		Bytes16u::SizeAndAlign				_argbufSizeAlign;
		EShaderStages						_stages			= Default;

		
		#include "graphics/Private/DescriptorSetLayout.h"


	// methods
	public:
		MDescriptorSetLayout ()												__NE___	{}
		~MDescriptorSetLayout ()											__NE___;

		ND_ bool  Create (const MDevice &dev, StringView dbgName)			__NE___;
		ND_ bool  Create (const MDevice &dev, const CreateInfo &ci)			__NE___;
			void  Destroy (MResourceManager &)								__NE___;

		ND_ MetalArrayOfArgumentDescriptor	Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _descArray; }
		ND_ EShaderStages					ShaderStages ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _stages; }
		ND_ EDescSetUsage					DescSetUsage ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _usage; }

		ND_ Bytes							GetArgumentBufferSize ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return Bytes{_argbufSizeAlign.size}; }
		ND_ SizeAndAlign					GetArgumentBufferSizeAlign ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return SizeAndAlign{_argbufSizeAlign}; }


	private:
		ND_ bool  _AddUniform (const Uniform_t &un, OUT Bytes16u* offset) __NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
