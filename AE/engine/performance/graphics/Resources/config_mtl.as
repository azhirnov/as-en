#include <pipeline_compiler.as>

void ASmain ()
{
	GlobalConfig	cfg;
	cfg.SetTarget( ECompilationTarget::Metal_Mac );
	cfg.SetShaderVersion( EShaderVersion::SPIRV_1_0 );
	cfg.SetSpirvToMslVersion( EShaderVersion::Metal_Mac_2_2 );
	cfg.SetShaderOptions( EShaderOpt::None );
	cfg.SetDefaultLayout( EStructLayout::Compatible_Std140 );
	cfg.SetPipelineOptions( EPipelineOpt::Optimize );
	cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );
	cfg.SetDefaultFeatureSet( "MinimalFS" );
}
