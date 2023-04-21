#include <pipeline_compiler>

void ShaderInputOutput ()
{
	{
		RC<ShaderStructType>	st = ShaderStructType( "graphics_1.io" );
		st.Set( "float2  Texcoord;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "graphics_4.io" );
		st.Set( "float2  texCoord;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "mesh_1.io" );
		st.Set( "float2  texcoord;" +
				"float4  color;" );
	}
}


void ASmain ()
{
	ShaderInputOutput();
}
