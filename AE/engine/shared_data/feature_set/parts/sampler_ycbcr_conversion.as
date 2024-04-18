#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<FeatureSet>  fset = FeatureSet( "part.samplerYcbcrConversion" );

		fset.samplerYcbcrConversion (EFeature::RequireTrue);
	}
	{
		RC<FeatureSet>  fset = FeatureSet( "part.externalFormatAndroid" );

		fset.samplerYcbcrConversion (EFeature::RequireTrue);
		fset.externalFormatAndroid (EFeature::RequireTrue);
	}
}
