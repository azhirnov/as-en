#include <pipeline_compiler.as>

void VertexBuffers ()
{
    // layouts
    {
        RC<ShaderStructType>    st = ShaderStructType( "vb_layout1" );
        st.Set( EStructLayout::InternalIO,
                "float3     Position;" +
                "float2     Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "vb_layout1" );
        vb.Add( "vb", st );
    }
    {
        RC<ShaderStructType>    st = ShaderStructType( "vb_layout2" );
        st.Set( EStructLayout::InternalIO,
                "float2     Position;" +
                "float2     Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "vb_layout2" );
        vb.Add( "vb", st );
    }

    // definition
    {
        RC<ShaderStructType>    st = ShaderStructType( "vb_input1" );
        st.Set( EStructLayout::InternalIO,
                "packed_float3          Position;" +
                "packed_ushort_norm2    Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "vb_input1" );
        vb.AddFeatureSet( "MinDesktopAMD" );
        vb.AddFeatureSet( "MinDesktopNV" );
        vb.Add( "vb", st, VertexDivisor(1) );
        SameAttribs( "vb_layout1", "vb_input1" ); // throw
    }
    {
        RC<ShaderStructType>    st = ShaderStructType( "vb_input2" );
        st.Set( EStructLayout::InternalIO,
                "packed_float2          Position;" +
                "packed_ushort_norm2    Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "vb_input2" );
        vb.Add( "vb", st );
        SameAttribs( "vb_layout2", "vb_input2" ); // throw
    }
    {
        RC<ShaderStructType>    st1 = ShaderStructType( "VB_3_Pos" );
        st1.Set( EStructLayout::InternalIO,
                 "packed_float3     Position;" );

        RC<ShaderStructType>    st2 = ShaderStructType( "VB_3_Attribs" );
        st2.Set( EStructLayout::InternalIO,
                 "packed_float2     Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "vb_input3" );
        vb.Add( "Position", st1 );
        vb.Add( "Attribs",  st2 );
        SameAttribs( "vb_layout1", "vb_input3" ); // throw
    }
}


void ASmain ()
{
    VertexBuffers();
}
