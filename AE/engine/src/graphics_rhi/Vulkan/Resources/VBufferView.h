// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VBuffer.h"

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

		GFX_DBG_ONLY( DebugName_t	_debugName;	)


	// methods
	public:
		VBufferView ()									__NE___	{}
		~VBufferView ()									__NE___;

		ND_ bool  Create (ResourceManager &, const BufferViewDesc &, BufferID, StringView dbgName)			__NE___;
		ND_ bool  Create (ResourceManager &, const VulkanBufferViewDesc &, BufferID, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)																__NE___;

		ND_ VkBufferView			Handle ()			C_NE___	{ return _bufferView; }
		ND_ BufferViewDesc const&	Description ()		C_NE___	{ return _desc; }
		ND_ BufferID				BufferId ()			C_NE___	{ return _bufferId; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()	C_NE___	{ return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
