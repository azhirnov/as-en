#include <pipeline_compiler.as>

void ASmain ()
{
	const EFeature  True = EFeature::RequireTrue;
	
	// Compatible
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.compat" );

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}

	// NV
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.NV" );

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

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

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}

	// Intel Xe
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.IntelXe" );

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

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

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
	}

	// Adreno 7xx
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ShadingRate.Adreno7xx" );

		fset.pipelineFragmentShadingRate (True);
		fset.primitiveFragmentShadingRate (True);
		fset.attachmentFragmentShadingRate (True);

		fset.fragmentShadingRateTexelSize( {8, 8}, {8, 8}, 1 );

		fset.AddShadingRate( {1,2}, {1,2,4} );
		fset.AddShadingRate( {2,1}, {1,2,4} );
		fset.AddShadingRate( {2,2}, {1,2,4} );
		fset.AddShadingRate( {2,4}, {1,2} );
		fset.AddShadingRate( {4,2}, {1} );
		fset.AddShadingRate( {4,4}, {1} );
	}

	// Mali
	{
		RC<FeatureSet>  fset = FeatureSet( "part.FragmentDensityMap" );

		fset.fragmentDensityMap (True);
		fset.fragmentDensityMapDynamic (True);
		fset.fragmentDensityInvocations (True);
		fset.maxSubsampledArrayLayers (4096);
		fset.perPipeline_maxSubsampledSamplers (8);
	}
}
