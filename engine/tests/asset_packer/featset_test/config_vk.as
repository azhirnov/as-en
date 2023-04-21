
void ASmain ()
{
	GlobalConfig	cfg;
	cfg.SetTarget( ECompilationTarget::Vulkan );
	cfg.SetShaderVersion( EShaderVersion::SPIRV_1_0 );
	cfg.SetShaderOptions( EShaderOpt::Optimize );
	cfg.SetDefaultLayout( EStructLayout::Compatible_Std140 );
	cfg.SetPipelineOptions( EPipelineOpt::Optimize );
}
