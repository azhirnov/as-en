// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Ray Tracing Shader Binding Table
	//

	class RRTShaderBindingTable final
	{
	// types
	public:
		struct CreateInfo
		{
			RTShaderBindingTable		sbt;
		};


	// variables
	private:
		RTShaderBindingTable			_sbt;
		RTShaderBindingDesc				_desc;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RRTShaderBindingTable ()									__NE___	{}
		~RRTShaderBindingTable ()									__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)	__NE___;
			void  Destroy (RResourceManager &)						__NE___;

		ND_ RmRTShaderBindingID			Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sbt.id; }
		ND_ RTShaderBindingTable const&	GetSBT ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sbt; }
		ND_ RTShaderBindingDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
