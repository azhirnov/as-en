// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VBuffer.h"

namespace AE::Graphics
{

	//
	// Vulkan Buffer view immutable data
	//

	class VBufferView final
	{
	// variables
	private:
		VkBufferView				_bufferView			= Default;
		BufferViewDesc				_desc;
		bool						_canBeDestroyed		= true;

		Strong<BufferID>			_bufferId;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VBufferView ()									__NE___	{}
		~VBufferView ()									__NE___;

		ND_ bool  Create (VResourceManager &, const BufferViewDesc &, BufferID, StringView dbgName)			__NE___;
		ND_ bool  Create (VResourceManager &, const VulkanBufferViewDesc &, BufferID, StringView dbgName)	__NE___;
			void  Destroy (VResourceManager &)																__NE___;

		ND_ VkBufferView			Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferView; }
		ND_ BufferViewDesc const&	Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ BufferID				BufferId ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }

		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
