// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>   rt                  = Image( EPixelFormat::RGBA16F, SurfaceSize() );    rt.Name( "RT" );
        RC<Image>   sdf_font_32         = Image( EImageType::FImage2D, "res/font/sdf-32.dds" );
        RC<Image>   mc_sdf_font_32      = Image( EImageType::FImage2D, "res/font/mc-sdf-32.dds" );
        RC<Image>   mc_sdf_font_64      = Image( EImageType::FImage2D, "res/font/mc-sdf-64.dds" );
        RC<Image>   mc_sdf_font_32_4    = Image( EImageType::FImage2D, "res/font/mc-sdf-32-4.dds" );

        // render loop
        {
            RC<Postprocess>     pass = Postprocess( EPostprocess::None );

            pass.ArgIn( "un_SdfFont_32",        sdf_font_32,        Sampler_LinearRepeat );
            pass.ArgIn( "un_McSdfFont_32",      mc_sdf_font_32,     Sampler_LinearRepeat );
            pass.ArgIn( "un_McSdfFont_64",      mc_sdf_font_64,     Sampler_LinearRepeat );
            pass.ArgIn( "un_McSdfFont_32_4",    mc_sdf_font_32_4,   Sampler_LinearRepeat );
            pass.Output( "out_Color",           rt );

            pass.Slider( "iSdfTex",     0,      3 );
            pass.Slider( "iMode",       0,      4 );
            pass.Slider( "iScale",      0.01,   4.0,    1.0 );
            pass.Slider( "iOffset",     0.0,    1.0 );
            pass.Slider( "iRotate",     0.0,    90.0 );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "GlobalIndex.glsl"
    #include "SDF.glsl"


    // Result must be >= 1, AA will work if >= 2
    float  ScreenPxRange (gl::CombinedTex2D<float> msdfTex, float2 uv, float pxRange)
    {
        float2  unit_range      = float2(pxRange) / float2(gl.texture.GetSize( msdfTex, 0 ));
        float2  src_tex_size    = float2(1.0) / gl.fwidth( uv );
        return Max( 0.5 * Dot( unit_range, src_tex_size ), 1.0 );
    }


    float4  SdfFont (gl::CombinedTex2D<float> sdfTex, const float2 uv, float sdfScale, float sdfBias, float pxRange)
    {
        float   sd  = gl.texture.SampleLod( sdfTex, uv, 0.0 ).r;
                sd  = FusedMulAdd( sd, sdfScale, sdfBias );

        switch ( iMode )
        {
            case 0 :
                return float4(1.0, 0.0, 0.0, 1.0) * SmoothStep( 0.0, 1.0, sd );

            case 1 : {
                float   px_dist = ScreenPxRange( sdfTex, uv, pxRange );
                        sd      = px_dist * (sd - 0.5);
                return float4(sd, 0.0, 0.0, px_dist);
            }
            case 2 : {
                float2  d_uv    = gl.fwidth( uv );
                float   w_uv    = Length( d_uv * float2(gl.texture.GetSize( sdfTex, 0 )) );
                float   px_dist = sd * 2.0 / w_uv;
                return float4(1.0, 0.0, 0.0, 1.0) * Saturate( px_dist );
            }
            case 3 : {
                float2  d_sd    = float2( gl.dFdx(sd), gl.dFdy(sd) );
                float   px_dist = sd / Length(d_sd);
                return float4(1.0, 0.0, 0.0, 1.0) * Saturate( 0.5 + px_dist );
            }
        }
        return float4(gl.texture.SampleLod( sdfTex, uv, 0.0 ).r);
    }


    float4  McSdfFont (gl::CombinedTex2D<float> msdfTex, const float2 uv, float sdfScale, float sdfBias, float pxRange)
    {
        float3  msd     = gl.texture.SampleLod( msdfTex, uv, 0.0 ).rgb;
        float   sd      = MCSDF_Median( msd );
                sd      = FusedMulAdd( sd, sdfScale, sdfBias );

        switch ( iMode )
        {
            case 0 :
                return float4(0.0, 1.0, 0.0, 1.0) * SmoothStep( 0.0, 1.0, sd );

            case 1 : {
                float   px_dist = ScreenPxRange( msdfTex, uv, pxRange );
                        sd      = px_dist * (sd - 0.5);
                return float4(0.0, sd, 0.0, px_dist);
            }
            case 2 : {
                float2  d_uv    = gl.fwidth( uv );
                float   w_uv    = Length( d_uv * float2(gl.texture.GetSize( msdfTex, 0 )) );
                float   px_dist = sd * 2.0 / w_uv;
                return float4(0.0, 1.0, 0.0, 1.0) * Saturate( 0.5 + px_dist );
            }
            case 3 : {
                float2  d_sd    = float2( gl.dFdx(sd), gl.dFdy(sd) );
                float   px_dist = sd / Length(d_sd);
                return float4(0.0, 1.0, 0.0, 1.0) * Saturate( 0.5 + px_dist );
            }
        }
        return float4(msd, 1.0);
    }


    void  Main ()
    {
        float2  uv  = GetGlobalCoordSNormCorrected2();
        uv.x += iOffset;
        uv = SDF_Rotate2D( uv, ToRad(iRotate) );
        uv = ToUNorm( uv * iScale );

        switch ( iSdfTex )
        {
            case 0 :    out_Color = SdfFont(    un_SdfFont_32,      uv, 20.1597252, -17.5723000, 1.0 ); break;
            case 1 :    out_Color = McSdfFont(  un_McSdfFont_32,    uv, 41.1041679, -21.1272316, 1.0 ); break;
            case 2 :    out_Color = McSdfFont(  un_McSdfFont_32_4,  uv, 10.2760420, -4.90680790, 4.0 ); break;
            case 3 :    out_Color = McSdfFont(  un_McSdfFont_64,    uv, 40.4665451, -20.5937500, 1.0 ); break;
        }
    }

#endif
//-----------------------------------------------------------------------------
