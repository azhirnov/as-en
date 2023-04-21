
void ASmain ()
{
	// Position
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_Position_f2" );
		st.Set( "packed_float2	Position;" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_Position_s2" );
		st.Set( "packed_short_norm2		Position;" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_Position_f3" );
		st.Set( "packed_float3	Position;" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_Position_f4" );
		st.Set( "packed_float4	Position;" );
	}


	// Attributes
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_UVf2_Col8" );
		st.Set( "packed_float2		UV;" +
				"packed_ubyte_norm4	Color;" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_UVs2_Col8" );
		st.Set( "packed_ushort_norm2	UV;" +
				"packed_ubyte_norm4		Color;" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_UVs2_SCs1_Col8" );
		st.Set( "packed_ushort_norm4	UV_Scale;"	+
				"packed_ubyte_norm4		Color;"		);
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "VB_Color8" );
		st.Set( "packed_ubyte_norm4	Color;" );
	}


	// Vertex buffers
	{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB_Position_f2, VB_UVf2_Col8" );
		vb.Add( "Position",	"VB_Position_f2" );
		vb.Add( "Attribs",	"VB_UVf2_Col8"	 );
	}
	{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB_Position_f2, VB_UVs2_Col8" );
		vb.Add( "Position",	"VB_Position_f2" );
		vb.Add( "Attribs",	"VB_UVs2_Col8"	 );
	}
	{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
		vb.Add( "Position",	"VB_Position_f2"	);
		vb.Add( "Attribs",	"VB_UVs2_SCs1_Col8"	);
	}
	{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB_Position_f2, VB_Color8" );
		vb.Add( "Position",	"VB_Position_f2" );
		vb.Add( "Attribs",	"VB_Color8"		 );
	}
}
