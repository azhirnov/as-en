// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Descriptor Set
	//

	class MDescriptorSet final
	{
	// types
	private:
		using DescrSetStorage_t	= IDescriptorAllocator::Storage;
		using DynamicBuffers_t	= TupleArrayView< MetalBuffer, Bytes >;


	// variables
	private:
		DescrSetStorage_t				_descrSet;
		Strong<DescriptorSetLayoutID>	_layoutId;
		
		EShaderStages					_stages		= Default;
		EDescSetUsage					_usage		= Default;

		DescriptorAllocatorPtr			_allocator;
		
		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)


	// methods
	public:
		MDescriptorSet ()									__NE___	{}
		~MDescriptorSet ()									__NE___;

		ND_ bool  Create (MResourceManager &, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___;
			void  Destroy (MResourceManager &)				__NE___;

		ND_ MetalBuffer				Handle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return IsArgumentBuffer() ? _descrSet.handle : Default; }
		ND_ DescriptorSetLayoutID	LayoutID ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		
		ND_ DynamicBuffers_t		GetDynamicBuffers ()	C_NE___;
		ND_ EShaderStages			ShaderStages ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _stages; }
		ND_ bool					IsArgumentBuffer ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _usage, EDescSetUsage::ArgumentBuffer ); }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
