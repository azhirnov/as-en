#include <pipeline_compiler.as>

void ASmain ()
{
	{
		// Intel, NV, iOS
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_BGRA8_sRGB_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::BGRA8_sRGB_nonlinear });
	}
	{
		// Android, AMD, Intel
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA8_sRGB_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA8_sRGB_nonlinear });
	}
	{
		// ...
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_BGRA8_BT709_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::BGRA8_BT709_nonlinear });
	}

	{
		// Android, AMD, NV
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGB10A2_sRGB_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGB10A2_sRGB_nonlinear });
	}
	{
		// ...
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGB10A2_HDR10_ST2084" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGB10A2_HDR10_ST2084 });
	}

	{
		// Android, AMD, iOS, NV
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA16F_Extended_sRGB_linear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA16F_Extended_sRGB_linear });
	}
	{
		// Android
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA16F_Extended_sRGB_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA16F_Extended_sRGB_nonlinear });
	}
	{
		// ...
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA16F_BT709_nonlinear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA16F_BT709_nonlinear });
	}
	{
		// ...
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA16F_HDR10_ST2084" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA16F_HDR10_ST2084 });
	}
	{
		// ...
		RC<FeatureSet>  fset = FeatureSet( "part.Surface_RGBA16F_BT2020_linear" );
		fset.AddSurfaceFormats({ ESurfaceFormat::RGBA16F_BT2020_linear });
	}
}
