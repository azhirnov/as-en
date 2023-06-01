// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  VertexBufferInput_Test1 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "vb.vertex" }};
		st->Set( EStructLayout::InternalIO,
				 "packed_ubyte_norm4	a;" );

		VertexBufferInputPtr vb{ new ScriptVertexBufferInput{} };
		vb->Add4( "All", st );

		const String	src = vb->ToMSL();
		const String	ref = R"#(struct VertexInput
{
  // All
  rgba8unorm<half4>  a  [[attribute(0)]];
};

)#";
		TEST( src == ref );
	}
}


extern void  UnitTest_VertexBufferInput_MSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.target			= ECompilationTarget::Vulkan;
	obj.pplnStorage		= &ppln;
	obj.metalCompiler	= MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler	= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );
	
	try {
		VertexBufferInput_Test1();
	} catch(...) {
		TEST( false );
	}
	
	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}
