// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <pipeline_compiler.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        const string    prefix = "model-2.";

        {
            RC<ShaderStructType>    st = ShaderStructType( prefix+"io.vs-fs" );
            st.Set( EStructLayout::InternalIO,
                    "float3     normal;" +
                    "uint       materialIdx;" );
        }
        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( prefix+"draw" );
            ppln.SetLayout( "model.pl" );
            ppln.SetVertexInput( "VB{Posf3, Normf3}" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
            ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+"io.vs-fs" );

            {
                RC<Shader>  vs = Shader();
                vs.LoadSelf();
                ppln.SetVertexShader( vs );
            }{
                RC<Shader>  fs = Shader();
                fs.LoadSelf();
                ppln.SetFragmentShader( fs );
            }

            // specialization
            {
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( prefix+"draw" );
                spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

                RenderState rs;

                rs.depth.test                   = true;
                rs.depth.write                  = true;

                rs.inputAssembly.topology       = EPrimitive::TriangleList;

                rs.rasterization.frontFaceCCW   = true;
                rs.rasterization.cullMode       = ECullMode::Back;

                spec.SetRenderState( rs );
            }
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    void Main ()
    {
        ModelNode   node    = un_Nodes .data[ pc.nodeIdx ];

        gl.Position     = un_PerPass.camera.viewProj * (node.transform * float4( in_Position, 1.0f ) - float4(un_PerPass.camera.pos, 0.0f));
        Out.normal      = node.normalMat * in_Normal;
        Out.materialIdx = node.materialIdx;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Math.glsl"

    void Main ()
    {
        out_Color = float4(1.0);    // TODO: read from material
    }

#endif
//-----------------------------------------------------------------------------
