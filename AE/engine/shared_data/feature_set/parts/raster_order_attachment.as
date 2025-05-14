#include <pipeline_compiler.as>

void ASmain ()
{
	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "part.RasterOrderAttachment" );

	fset.rasterizationOrderColorAttachmentAccess (True);
	fset.rasterizationOrderDepthAttachmentAccess (True);
	fset.rasterizationOrderStencilAttachmentAccess (True);
}
