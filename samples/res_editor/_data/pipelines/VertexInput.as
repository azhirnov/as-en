// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "CubeVertex" );
        st.Set( "packed_float3      Position;" +
                "packed_float3      Normal;" +
                "packed_float3      Texcoord;" );
        st.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{CubeVertex}" );
        vb.Add( "All", st );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "SphericalCubeVertex" );
        st.Set( "packed_float4      Position;" +
                "packed_float4      Texcoord;" );
        st.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{SphericalCubeVertex}" );
        vb.Add( "All", st );
    }

    /*{
        RC<ShaderStructType>    st = ShaderStructType( "ParticleVertexAttribs" );
        st.Set( "packed_float3          Position;" +
                "float                  Size;" +
                "packed_float3          Velocity;" +
                "packed_ubyte_norm4     Color;" );
        st.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{ParticleVertexAttribs}" );
        vb.Add( "All", st );
    }*/

    /*{
        RC<ShaderStructType>    st = ShaderStructType( "ParticleVertex" );
        st.Set( EStructLayout::Std430,
                "float4     positionSize;" +
                "float4     velocityColor;" +
                "uint4      params;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ParticleVertexArray" );
        st.Set( EStructLayout::Std430,
                "ParticleVertex     particles [];" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }*/
}
