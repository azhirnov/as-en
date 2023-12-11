// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Doesn't make any effect, just compiles.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   define SH_COMPUTE
#   define AE_COOP_MATRIX
#   define AE_MEM_SCOPE
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>   rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );     rt.Name( "RT" );

        // render loop
        {
            RC<ComputePass>     pass = ComputePass();
            pass.ArgOut( "un_OutImage", rt );
            pass.LocalSize( 8, 8 );
            pass.DispatchThreads( rt.Dimension() );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "GlobalIndex.glsl"

    #define CoopMatA    gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::A >
    #define CoopMatB    gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::B >
    #define CoopMatC    gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::C >

    void  Main ()
    {
        CoopMatA    s_MatA = CoopMatA(half(0.0));
        CoopMatB    s_MatB = CoopMatB(half(0.0));
        CoopMatC    s_MatC = CoopMatC(half(0.0));
        CoopMatC    s_MatR;

        s_MatR = gl.CoopMatMulAdd( s_MatA, s_MatB, s_MatC );

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(0.0) );
    }

#endif
//-----------------------------------------------------------------------------
