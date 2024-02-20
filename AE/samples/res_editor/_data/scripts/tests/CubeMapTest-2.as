// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
    #include "CubeMap.glsl"
    #include "Ray.glsl"

    float3  CubeFaceToNormal (ECubeFace face)
    {
        switch ( face )
        {
            case ECubeFace_XPos :   return float3(+1.0,  0.0,  0.0 );
            case ECubeFace_XNeg :   return float3(-1.0,  0.0,  0.0 );
            case ECubeFace_YPos :   return float3( 0.0, +1.0,  0.0 );
            case ECubeFace_YNeg :   return float3( 0.0, -1.0,  0.0 );
            case ECubeFace_ZPos :   return float3( 0.0,  0.0, +1.0 );
            case ECubeFace_ZNeg :   return float3( 0.0,  0.0, -1.0 );
        }
        return float3(0.0);
    }

    void  Main ()
    {
        float4      col     = float4(0.0);
        const float y_max   = 7.0;
        const float y       = Floor( GetGroupCoordUNorm().y * y_max );
        const float scale   = 1000.0;

        float2      uv      = GetGlobalCoordUNorm().xy;     uv.y = (uv.y - y/y_max) * y_max;
        float3      dir     = Ray_PlaneTo360( float3(0.0), 0.1, uv ).dir;

        switch ( int(y) )
        {
            case 0 : {
                float3  uv_f    = CM_IdentitySC_Inverse( dir );
                float3  dir2    = CM_IdentitySC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 1 : {
                float3  uv_f    = CM_TangentialSC_Inverse( dir );
                float3  dir2    = CM_TangentialSC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 2 : {
                float3  uv_f    = CM_EverittSC_Inverse( dir );
                float3  dir2    = CM_EverittSC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 3 : {
                float3  uv_f    = CM_5thPolySC_Inverse( dir );
                float3  dir2    = CM_5thPolySC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 4 : {
                float3  uv_f    = CM_COBE_SC_Inverse( dir );
                float3  dir2    = CM_COBE_SC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 5 : {
                float3  uv_f    = CM_ArvoSC_Inverse( dir );
                float3  dir2    = CM_ArvoSC_Forward( uv_f.xy, int(uv_f.z) );
                col.rgb = Abs( dir - dir2 ) * scale;
                break;
            }
            case 6 : {
                float3  uv_f    = CM_IdentitySC_Inverse( dir );
                col.rgb = Abs( CubeFaceToNormal( ECubeFace(uv_f.z) ) - dir );
                break;
            }
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
    }

#endif
//-----------------------------------------------------------------------------
