// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  VertexBufferInput_Test1 ()
	{
		ShaderStructTypePtr	st = ShaderStructType::Create( "vb.vertex" );
		st->Set( EStructLayout::InternalIO,
				 "packed_ubyte_norm4	a;" );

		VertexBufferInputPtr vb = ScriptVertexBufferInput::Create( "VB" );
		vb->Add4( "All", st );

		const String	src = vb->ToGLSL();
		const String	ref = R"#(  // All
layout(location=0) in mediump vec4  in_a;

)#";
		TEST( src == ref );
	}
}


extern void  UnitTest_VertexBufferInput_GLSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.defaultFeatureSet	= "DefaultFS";
	obj.target				= ECompilationTarget::Vulkan;
	obj.pplnStorage			= &ppln;
	obj.spirvCompiler		= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

	#ifdef AE_METAL_TOOLS
		obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	#endif

	ScriptFeatureSetPtr	fs = ScriptFeatureSet::Create( "DefaultFS" );
	fs->fs.Init( FeatureSet::EFeature::RequireTrue );

	try {
		VertexBufferInput_Test1();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}
