// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain (RC<Collection> collection)
    {
        RC<Image>       dst     = collection.Image("dst");

        RC<ComputePass> pass    = ComputePass( "", EPassFlags::None );
        pass.ArgOut( "un_OutImage", dst );
        pass.LocalSize( 8, 8 );
        pass.DispatchThreads( dst.Dimension2_Layers() );

        GenMipmaps( dst );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "GlobalIndex.glsl"
    #include "Color.glsl"
    #include "CubeMap.glsl"
    #include "Noise.glsl"
    #define PROJECTION  CM_TangentialSC_Forward

    int  FaceIdx () {
        return int(gl.WorkGroupID.z);
    }

    float3  PosOnSphere ()
    {
    //  float2  ncoord      = ToSNorm( float2(GetGlobalCoord().xy) / float2(gl.image.GetSize( un_OutImage ).xy - 1) );
        float2  ncoord      = ToSNorm( (float2(GetGlobalCoord().xy) + 0.5) / float2(gl.image.GetSize( un_OutImage ).xy) );
        float3  sphere_pos  = PROJECTION( ncoord, FaceIdx() );
        return sphere_pos;
    }

    void  Main ()
    {
        const int3      coord   = GetGlobalCoord();
        const float3    pos     = PosOnSphere();
        float           hash    = DHash13( Voronoi( Turbulence( pos * 8.0, 1.0, 2.0, 0.6, 7 ), float2(3.9672) ).icenter );

        gl.image.Store( un_OutImage, coord, Rainbow(hash) );
    }

#endif
//-----------------------------------------------------------------------------
