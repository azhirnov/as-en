// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void VertexBuffers ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "Pos3_Norm3_UV2" );
        st.Set( "packed_float3      Position;" +
                "packed_float3      Normal;" +
                "packed_float2      Texcoord;" );
        st.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{Pos3_Norm3_UV2}" );
        vb.Add( "All", st );
    }
    {
        RC<ShaderStructType>    st1 = ShaderStructType( "Posf3" );
        st1.Set( "packed_float3     Position;" );
        st1.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<ShaderStructType>    st2 = ShaderStructType( "Normf3" );
        st2.Set( "packed_float3     Normal;" );
        st2.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<ShaderStructType>    st3 = ShaderStructType( "UVf2" );
        st3.Set( "packed_float2     Texcoord;" );
        st3.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{Posf3, Normf3, UVf2}" );
        vb.Add( "Position", st1 );
        vb.Add( "Normals",  st2 );
        vb.Add( "UVs",      st3 );
    }
}


void ASmain ()
{
    VertexBuffers();

    // push const
    {
        RC<ShaderStructType>    st = ShaderStructType( "draw_model.pc" );
        st.Set( EStructLayout::Compatible_Std140,
                "uint   nodeIdx;" +
                "uint   lod;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }

    // node
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelNode" );
        st.Set( EStructLayout::Compatible_Std140,
                "float4x4           transform;" +
                "uint               meshIdx;" +
                "uint               materialIdx;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }/*{
        RC<ShaderStructType>    st = ShaderStructType( "ModelNodeArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelNode          data [];" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }*/

    // mesh
    /*{
        RC<ShaderStructType>    st = ShaderStructType( "ModelMesh" );
        st.Set( EStructLayout::Compatible_Std430,
                "uint               vertexCount;" +
                "uint               indexCount;" +
                "packed_float3 *    positions;" +   // [vertexCount]
                "packed_float3 *    normals;" +     // [vertexCount]
                "packed_float2 *    texcoords;" +   // [vertexCount]
                "uint *             indices;"       // [indexCount]
        );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelMeshArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelMesh          data [];" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }*/

    // material
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterial" );
        st.Set( EStructLayout::Compatible_Std430,
                "uint               albedoMap;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterialArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelMaterial      data [];" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }
}
