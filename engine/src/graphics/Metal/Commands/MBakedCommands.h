// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Baked Commands
	//

	class MBakedCommands final
	{
	// types
	public:
		using Header	= _hidden_::SoftwareCmdBufBase::Header;


	// variables
	protected:
		void *		_root	= null;


	// methods
	public:
		MBakedCommands ()
		{}

		explicit MBakedCommands (void* root) :
			_root{ root }
		{}

		MBakedCommands (MBakedCommands &&other) :
			_root{ other._root }
		{
			other._root = null;
		}

		void  Destroy ()
		{
			_hidden_::SoftwareCmdBufBase::Deallocate( _root );
			_root = null;
		}
		
		bool  Execute (MetalCommandBuffer cmdbuf) const;

		ND_ bool  IsValid () const
		{
			return _root != null;
		}
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL

