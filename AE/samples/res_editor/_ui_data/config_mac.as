// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
    GlobalConfig    cfg;
    cfg.SetTarget( ECompilationTarget::Metal_Mac );
    cfg.SetShaderVersion( EShaderVersion::SPIRV_1_3 );
    cfg.SetSpirvToMslVersion( EShaderVersion::Metal_Mac_2_2 );
    cfg.SetShaderOptions( EShaderOpt::Optimize );
    cfg.SetDefaultLayout( EStructLayout::Compatible_Std140 );
    cfg.SetPipelineOptions( EPipelineOpt::Optimize );
    cfg.SetPreprocessor( EShaderProprocessor::AEStyle );
    cfg.SetDefaultFeatureSet( "MinDesktop" );
}
