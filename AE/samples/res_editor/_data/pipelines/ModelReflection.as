// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>


void  BufferTypes ()
{
    // push constant
    {
        RC<ShaderStructType>    st = ShaderStructType( "model.pc" );
        st.Set( EStructLayout::Compatible_Std140,
                "uint               nodeIdx;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );     // enable c++ reflection
    }

    // scene node
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelNode" );
        st.Set( EStructLayout::Compatible_Std430,
                "float4x4           transform;" +
                "float3x3           normalMat;" +
                "uint               meshIdx;" +
                "uint               materialIdx;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );     // enable c++ reflection
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelNodeArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelNode          data [];" );
    }

    // material
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterial" );
        st.Set( EStructLayout::Compatible_Std430,
                "uint               albedoMap;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );     // enable c++ reflection
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterialArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelMaterial      data [];" );
    }
}


void  ASmain ()
{
    BufferTypes();
}
