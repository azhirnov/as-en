// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphicsRHI.pch.h"

namespace
{
	static void  EResourceState_Test1 ()
	{
		TEST( EResourceState_RequireShaderStage( EResourceState::ShaderUniform ));
		TEST( not EResourceState_RequireShaderStage( EResourceState::Host_Read ));

		TEST( not EResourceState_IsReadOnly( EResourceState::ShaderStorage_Write ));
		TEST( EResourceState_IsReadOnly( EResourceState::ShaderStorage_Read ));
		TEST( EResourceState_HasReadAccess( EResourceState::ShaderStorage_ReadWrite ));
		TEST( EResourceState_HasWriteAccess( EResourceState::ShaderStorage_ReadWrite ));
		TEST( not EResourceState_HasWriteAccess( EResourceState::ShaderStorage_Read ));

		//TEST( EResourceState_IsDepthReadOnly( EResourceState::DepthStencilTest ));
		//TEST( not EResourceState_IsDepthReadOnly( EResourceState::DepthStencilAttachment_RW ));
		//TEST( EResourceState_IsStencilReadOnly( EResourceState::DepthStencilTest ));
		//TEST( not EResourceState_IsStencilReadOnly( EResourceState::DepthTest_StencilRW ));
	}
}


extern void  UnitTest_EResourceState ()
{
	EResourceState_Test1();

	TEST_PASSED();
}
