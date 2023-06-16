#include <pipeline_compiler>

void ASmain ()
{
	// Compatible
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.compat" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::Ignore );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}

	// NV
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.NV" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::RequireTrue );

		fset.fragmentShadingRateTexelSize( {16, 16}, {16, 16}, 1 );

		fset.AddShadingRate( {1,1}, {1,2,4,8,16} );
		fset.AddShadingRate( {1,2}, {1,2,4,8} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
		fset.AddShadingRate( {2,4}, {1,2} );
		fset.AddShadingRate( {4,2}, {1} );
		fset.AddShadingRate( {4,4}, {1} );
	}
	
	// AMD
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.AMD" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::RequireTrue );

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}
	
	// Intel Xe
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.IntelXe" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::Ignore );
		
		fset.AddShadingRate( {1,1}, {1,2,4,8,16} );
		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
		fset.AddShadingRate( {2,4}, {1,2} );
		fset.AddShadingRate( {4,2}, {1} );
		fset.AddShadingRate( {4,4}, {1} );
	}

	// Intel Arc
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.IntelArc" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::RequireTrue );

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}

	// Adreno 7xx
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.Adreno7xx" );

		fset.pipelineFragmentShadingRate( EFeature::RequireTrue );
		fset.primitiveFragmentShadingRate( EFeature::RequireTrue );
		fset.attachmentFragmentShadingRate( EFeature::RequireTrue );

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
		fset.AddShadingRate( {2,4}, {1,2} );
		fset.AddShadingRate( {4,2}, {1} );
		fset.AddShadingRate( {4,4}, {1} );
	}
}
