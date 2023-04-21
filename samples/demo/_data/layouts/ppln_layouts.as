// pipeline descriptor set layouts
#include <pipeline_compiler>

void VertexBuffers ()
{
	{
		//RC<ShaderStructType>	st1 = ShaderStructType( "VB_Position_f3" );
		//st1.Set( "packed_float3		Position;" );

		RC<ShaderStructType>	st2 = ShaderStructType( "VB_UVf2_Normf3" );
		st2.Set( "packed_float3		Normal;" +
				 "packed_float2		Texcoord;" );

		RC<VertexBufferInput>	vb = VertexBufferInput( "VB_Position_f3, VB_UVf2_Normf3" );
		vb.Add( "Position", "VB_Position_f3" );
		vb.Add( "Attribs",  st2 );
	}
}


void ASmain ()
{
	VertexBuffers();
}
