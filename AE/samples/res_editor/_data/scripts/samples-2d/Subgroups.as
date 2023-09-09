// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

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
        RC<Image>   rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );       rt.Name( "RT" );

        // render loop
        {
            RC<ComputePass>     pass = ComputePass( "", EPassFlags::Enable_ShaderTrace );

            pass.ArgOut( "un_OutImage", rt );
            pass.LocalSize( 8, 8 );
            pass.DispatchThreads( rt.Dimension() );

            pass.Slider( "iMode", 0, 10 );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "GlobalIndex.glsl"
    #include "Color.glsl"

    float4  QuadGroupId ()
    {
        const float4    colors [] = {
            float4( 1.0, 0.0, 0.0, 1.0 ),
            float4( 1.0, 1.0, 0.0, 1.0 ),
            float4( 0.0, 0.0, 1.0, 1.0 ),
            float4( 1.0, 0.0, 1.0, 1.0 )
        };
        return colors[ gl.subgroup.Index & 3 ];
    }


    float4  SubgroupId ()
    {
        return Rainbow( float(gl.subgroup.Index) / float(gl.subgroup.Size) );
    }


    float4  SubgroupGroupId ()
    {
        const float group_count = un_PerPass.resolution.x * un_PerPass.resolution.y / float(gl.subgroup.Size);
        return RainbowWrap( float(gl.subgroup.GroupIndex) / group_count );
    }


    void  Main ()
    {
        float4  col = float4(0.0);
        switch ( iMode )
        {
            case 0 :    col = QuadGroupId();        break;
            case 1 :    col = SubgroupId();         break;
            case 2 :    col = SubgroupGroupId();    break;
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
    }

#endif
//-----------------------------------------------------------------------------
