// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "remote_graphics_device/RemoteDevice.h"
#include "graphics/Remote/RMessages.cpp.h"

namespace AE::RemoteGraphics
{

/*
=================================================
	_RegisterMessages
=================================================
*/
	bool  RmGAppListener::_RegisterMessages ()
	{
		using namespace AE::Serializing;

		CHECK_ERR( _objFactory.Register< PipelineCompiler::ShaderTrace >( SerializedID{"ShaderTrace"} ));

		{
			#define REG_MSG( _name_ )	_Register( SerializedID{#_name_}, &RmGAppListener::_Cb_##_name_ )
			#define REG_RESP( _name_ )	_Register< Msg::_name_ >( SerializedID{#_name_} )

			REGISTER_MESSAGES( REG_MSG, REG_RESP )

			#undef REG_MSG
			#undef REG_RESP
		}{
			#define REG( _name_ )		_cmdFactory.Register< Msg::_name_ >( SerializedID{#_name_} )
			REGISTER_CMDBUF_COMMANDS( REG )
			#undef REG
		}{
			#define REG( _name_ )		_descFactory.Register< Msg::_name_ >( SerializedID{#_name_} )
			REGISTER_DESCUPD_COMMANDS( REG )
			#undef REG
		}

		//_RunTests();
		return true;
	}


} // AE::RemoteGraphics
