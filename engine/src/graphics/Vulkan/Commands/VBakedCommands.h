// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Baked Commands
	//

	class VBakedCommands final
	{
	// types
	public:
		using Header	= _hidden_::SoftwareCmdBufBase::Header;



	// variables
	protected:
		void *		_root	= null;


	// methods
	public:
		VBakedCommands ()
		{}

		explicit VBakedCommands (void* root) :
			_root{ root }
		{}

		VBakedCommands (VBakedCommands &&other) :
			_root{ other._root }
		{
			other._root = null;
		}

		void  Destroy ()
		{
			_hidden_::SoftwareCmdBufBase::Deallocate( _root );
			_root = null;
		}
		
		ND_ bool  Execute (VulkanDeviceFn fn, VkCommandBuffer cmdbuf) const;

		ND_ bool  IsValid () const
		{
			return _root != null;
		}
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
