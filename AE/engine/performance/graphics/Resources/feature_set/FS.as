#include <pipeline_compiler.as>


void ASmain ()
{
	const EFeature  True = EFeature::RequireTrue;

	{
		RC<FeatureSet>  fset = FeatureSet( "ShaderInt8" );
		fset.shaderInt8 (True);
	}{
		RC<FeatureSet>  fset = FeatureSet( "ShaderInt16" );
		fset.shaderInt16 (True);
	}{
		RC<FeatureSet>  fset = FeatureSet( "ShaderInt64" );
		fset.shaderInt64 (True);
	}

	{
		RC<FeatureSet>  fset = FeatureSet( "ShaderFloat16" );
		fset.shaderFloat16 (True);
	}{
		RC<FeatureSet>  fset = FeatureSet( "ShaderFloat64" );
		fset.shaderFloat64 (True);
	}

	{
		RC<FeatureSet>  fset = FeatureSet( "ShaderFloatInt16" );
		fset.shaderInt16 (True);
		fset.shaderFloat16 (True);
	}{
		RC<FeatureSet>  fset = FeatureSet( "ShaderFloatInt64" );
		fset.shaderInt64 (True);
		fset.shaderFloat64 (True);
	}
}
