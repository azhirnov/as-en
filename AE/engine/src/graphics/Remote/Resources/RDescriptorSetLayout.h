// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Remote/RCommon.h"
# include "PipelineCompilerImpl.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Descriptor Set Layout
	//

	class RDescriptorSetLayout final
	{
	// types
	public:
		using UniformOffsets_t	= PipelineCompiler::DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniform_t			= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t		= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		struct CreateInfo
		{
			RmDescriptorSetLayoutID	id;
			Uniforms_t				uniforms;
			UniformOffsets_t		unOffsets;
			EDescSetUsage			usage;
			StringView				dbgName;
		};


	// variables
	private:
		#include "graphics/Private/DescriptorSetLayout.h"

		RmDescriptorSetLayoutID		_dslId;


	// methods
	public:
		RDescriptorSetLayout ()										__NE___	{}
		~RDescriptorSetLayout ()									__NE___;

		ND_ bool  Create (const RDevice &dev, StringView dbgName)	__NE___;
		ND_ bool  Create (const RDevice &dev, const CreateInfo &ci)	__NE___;
			void  Destroy (RResourceManager &)						__NE___;

		ND_ RmDescriptorSetLayoutID		Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dslId; }
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
