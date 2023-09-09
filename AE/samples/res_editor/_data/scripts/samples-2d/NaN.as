// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Test result of passing NaN to some math functions.
*/
#ifdef __INTELLISENSE__
#   define SH_COMPUTE
#   include <res_editor.as>
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
            RC<ComputePass>     pass = ComputePass( "", EPassFlags::Enable_ShaderTrace );

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

    void  Main ()
    {
        float4      col = float4(0.0);
        float       nan = uintBitsToFloat( 0xFFFFFFFF );
        const bool  x   = (GetGroupCoordUNorm().x > 0.5);

        switch ( int(GetGroupCoordUNorm().y * 6.0) )
        {
            case 0 :    col = float4( x ? Min( nan, 0.5f ) : Min( 1.0f, nan ));                 break;
            case 1 :    col = float4( x ? Max( nan, 0.5f ) : Max( 1.0f, nan ));                 break;
            case 2 :    col = float4(Saturate( nan ));                                          break;
            case 3 :    col = float4(x ? Clamp( 0.5f, nan, 1.f ) : Clamp( 0.5f, 1.f, nan ));    break;
            case 4 :    col = float4( bool(nan) ? 0.5f : 1.f );                                 break;
            case 5 :    col = float4(x ? Sign(nan) : SignOrZero(nan) );                         break;
        }

        if ( Any(IsNaN( col )))
            col = float4(1.0, 0.0, 0.0, 1.0);

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
    }

#endif
//-----------------------------------------------------------------------------
