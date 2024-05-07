#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<FeatureSet>  fset = FeatureSet( "part.FragmentShaderTrace" );
		fset.fragmentStoresAndAtomics (EFeature::RequireTrue);
	}
	{
		RC<FeatureSet>  fset = FeatureSet( "part.VertexShaderTrace" );
		fset.vertexPipelineStoresAndAtomics (EFeature::RequireTrue);
	}
}
