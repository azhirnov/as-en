// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
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
    #include "Matrix.glsl"

    void  Main ()
    {
        float2  uv = GetGlobalCoordUNorm().xy;
        uv = Transform2D( ub.camera.viewProj, uv );

        float4  col = float4(uv, 0.0, 1.0);

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
    }

#endif
//-----------------------------------------------------------------------------
