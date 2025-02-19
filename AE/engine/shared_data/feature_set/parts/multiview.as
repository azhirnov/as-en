#include <pipeline_compiler.as>

void ASmain ()
{
	const EFeature  True = EFeature::RequireTrue;

	{
		RC<FeatureSet>  fset = FeatureSet( "part.MultiView" );
		
		fset.multiview (True);
		fset.maxMultiviewViewCount (6);
	}
	{
		RC<FeatureSet>  fset = FeatureSet( "part.ViewportArray" );
		
		fset.multiViewport (True);
		fset.maxViewports (16);
		fset.shaderOutputViewportIndex (True);
	}
}
