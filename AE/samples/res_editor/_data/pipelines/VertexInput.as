// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Always included for GraphicsPass
*/
#include <pipeline_compiler.as>

void ASmain ()
{
	// spherical cube
	{
		RC<ShaderStructType>	st = ShaderStructType( "CubeVertex" );
		st.Set(	EStructLayout::InternalIO,
				"packed_short_norm4		Position;" +
				"packed_short_norm4		Texcoord;" +
			    "packed_short_norm4		Normal;" +
				"packed_short_norm4		Tangent;" +
				"packed_short_norm4		BiTangent;" );
		st.AddUsage( ShaderStructTypeUsage::VertexLayout );

		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{CubeVertex}" );
		vb.Add( "All", st );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "SphericalCubeVertex" );
		st.Set(	EStructLayout::InternalIO,
				"packed_short_norm4		Position;" +
				"packed_short_norm4		Texcoord;" +
				"packed_short_norm4		Tangent;" +
				"packed_short_norm4		BiTangent;" );
		st.AddUsage( ShaderStructTypeUsage::VertexLayout );

		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{SphericalCubeVertex}" );
		vb.Add( "All", st );
	}
}
