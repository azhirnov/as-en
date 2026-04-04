#include <pipeline_compiler.as>


void ASmain ()
{
	// include:

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinIndirectCmds" );

	fset.maxDrawIndirectCount (1);
	fset.maxViewports (1);
	fset.maxUniformBufferSize (16 << 10);
	fset.perPipeline_maxSampledImages (1);
	fset.perPipeline_maxUniformBuffers (1);
	fset.perPipeline_maxTotalResources (1);
	fset.perStage_maxSampledImages (1);
	fset.perStage_maxUniformBuffers (1);
	fset.perStage_maxTotalResources (1);
	fset.maxDescriptorSets (1);
	fset.maxFragmentOutputAttachments (1);
	fset.maxFragmentCombinedOutputResources (1);
	fset.maxPushConstantsSize (16);
	fset.maxImageDimension1D (1);
	fset.maxImageDimension2D (1);
	fset.maxImageDimension3D (1);
	fset.maxImageDimensionCube (1);
	fset.maxImageArrayLayers (1);
	fset.maxFramebufferLayers (1);
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
	fset.maxIndirectPipelineCount (16000);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
