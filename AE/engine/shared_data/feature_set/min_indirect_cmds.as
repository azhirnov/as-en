#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Win-NV-RTX5000-1.4.325

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinIndirectCmds" );

	fset.deviceGeneratedCommands (True);
	fset.deviceGeneratedCommandsMultiDrawIndirectCount (True);
	fset.supportedIndirectCommandsShaderStages(EShaderStages(
		EShaderStages::AllGraphics | 
		EShaderStages::Compute
	));
	fset.supportedIndirectCommandsShaderStagesPipelineBinding(EShaderStages(
		EShaderStages::AllGraphics | 
		EShaderStages::Compute
	));
	fset.maxIndirectPipelineCount (16 << 10);
}
