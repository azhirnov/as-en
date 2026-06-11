// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "remote_graphics_device/RemoteDevice.h"

namespace AE::RemoteGraphics
{

/*
=================================================
	_RunTests
=================================================
*/
	void  RmGAppListener::_RunTests ()
	{
		using namespace AE::Serializing;

		auto	stream = MakeRC<ArrayWStream>();
		{
			Msg::CmdBuf_Bake::ASBuild_BuildGeometryCmd	cmd;

			Serializer	ser{ stream };
			ser.factory	= &_cmdFactory;

			cmd.build.options		= ERTASOptions::AllowCompaction | ERTASOptions::LowMemory;
			cmd.build.scratch.id	= BufferID{ 0x11, 0x22 };
			cmd.dstId				= RmRTGeometryID{ 0x33, 0x44 };

			CHECK( ser( &cmd ));
		}
		{
			auto				rstream = MakeRC<MemRefRStream>( stream->GetData() );
			Deserializer		des{ rstream };
			LinearAllocator<>	alloc;
			des.allocator	= &alloc;
			des.factory		= &_cmdFactory;

			Unique<Msg::CmdBuf_Bake::ASBuild_BuildGeometryCmd>  cmd;
			CHECK( des( cmd ));
			CHECK( cmd->build.options == (ERTASOptions::AllowCompaction | ERTASOptions::LowMemory) );
			CHECK( cmd->build.scratch.id == BufferID{ 0x11, 0x22 });
			CHECK( cmd->dstId == RmRTGeometryID{ 0x33, 0x44 });
		}
	}


} // AE::RemoteGraphics
