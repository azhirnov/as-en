// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Compiler/AEStyleGLSLPreprocessor.h"
#include "Compiler/AEStyleMSLPreprocessor.h"

#include "ScriptObjects/ScriptConfig.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	destructor
=================================================
*/
	ScriptConfig::~ScriptConfig () __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( storage.target != Default );
		CHECK_THROW_MSG( storage.target < ECompilationTarget::_Count );

		switch_enum( storage.target )
		{
			case ECompilationTarget::Vulkan :
			{
				CHECK_THROW_MSG( AllBits( storage.shaderVersion, EShaderVersion::_SPIRV, EShaderVersion::_Mask ));
				CHECK_THROW_MSG( storage.sprvToMslVersion == Default );
				break;
			}
			case ECompilationTarget::Metal_iOS :
			{
				CHECK_THROW_MSG( (AllBits( storage.shaderVersion, EShaderVersion::_SPIRV, EShaderVersion::_Mask ) and
								  AllBits( storage.sprvToMslVersion, EShaderVersion::_Metal_iOS, EShaderVersion::_Mask )) or
								 AllBits( storage.shaderVersion, EShaderVersion::_Metal_iOS, EShaderVersion::_Mask ));
				break;
			}
			case ECompilationTarget::Metal_Mac :
			{
				CHECK_THROW_MSG( (AllBits( storage.shaderVersion, EShaderVersion::_SPIRV, EShaderVersion::_Mask ) and
								  AllBits( storage.sprvToMslVersion, EShaderVersion::_Metal_Mac, EShaderVersion::_Mask )) or
								 AllBits( storage.shaderVersion, EShaderVersion::_Metal_Mac, EShaderVersion::_Mask ));
				break;
			}
			case ECompilationTarget::Unknown :
			case ECompilationTarget::_Count :
				break;
		}
		switch_end
	}

/*
=================================================
	SetTarget
=================================================
*/
	void  ScriptConfig::SetTarget (ECompilationTarget value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.target == Default );
		storage.target = value;
	}

/*
=================================================
	SetShaderVersion
=================================================
*/
	void  ScriptConfig::SetShaderVersion (EShaderVersion value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.shaderVersion == Default );
		storage.shaderVersion = value;
	}

/*
=================================================
	SetSpirvToMslVersion
=================================================
*/
	void  ScriptConfig::SetSpirvToMslVersion (EShaderVersion value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.sprvToMslVersion == Default );
		storage.sprvToMslVersion = value;
	}

/*
=================================================
	SetShaderOptions
=================================================
*/
	void  ScriptConfig::SetShaderOptions (EShaderOpt value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.shaderOptions == Default );
		storage.shaderOptions = value;
	}

/*
=================================================
	SetDefaultLayout
=================================================
*/
	void  ScriptConfig::SetDefaultLayout (EStructLayout value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		//CHECK( storage.defaultLayout == Default );
		storage.defaultLayout = value;
	}

/*
=================================================
	SetPipelineOptions
=================================================
*/
	void  ScriptConfig::SetPipelineOptions (EPipelineOpt value) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.defaultPipelineOpt == Default );
		storage.defaultPipelineOpt = value;
	}

/*
=================================================
	SetPreprocessor
=================================================
*/
	void  ScriptConfig::SetPreprocessor (EShaderPreprocessor value) __Th___
	{
		auto&	spv = ObjectStorage::Instance()->spirvCompiler;
		auto&	msl	= ObjectStorage::Instance()->metalCompiler;
		CHECK_THROW_MSG( spv or msl );

		switch_enum( value )
		{
			case EShaderPreprocessor::None :
				if ( spv ) spv->SetPreprocessor( null );
				if ( msl ) msl->SetPreprocessor( null );
				break;

			case EShaderPreprocessor::AEStyle :
				if ( spv ) spv->SetPreprocessor( new AEStyleGLSLPreprocessor{} );
				if ( msl ) msl->SetPreprocessor( new AEStyleMSLPreprocessor{} );
				break;

			case EShaderPreprocessor::_Count :
			default :
				CHECK_THROW_MSG( false, "unknown shader preprocessor type" );
		}
		switch_end
	}

/*
=================================================
	SetDefaultFeatureSet
=================================================
*/
	void  ScriptConfig::SetDefaultFeatureSet (const String &value) __Th___
	{
		CHECK_THROW_MSG( not value.empty() );

		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.defaultFeatureSet.empty() );
		storage.defaultFeatureSet = value;
	}

/*
=================================================
	SetShaderDefines
=================================================
*/
	void  ScriptConfig::SetShaderDefines (const String &value) __Th___
	{
		CHECK_THROW_MSG( not value.empty() );

		auto&	storage = *ObjectStorage::Instance();

		CHECK( storage.defaultShaderDefines.empty() );
		storage.defaultShaderDefines.clear();
		storage.defaultShaderDefines << '\n' << value;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptConfig::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptConfig>	binder{ se };
		binder.CreateClassValue();

		binder.Comment( "Set target GAPI and platform." );
		binder.AddMethod( &ScriptConfig::SetTarget,				"SetTarget",			{} );

		binder.Comment( "Set default shader version." );
		binder.AddMethod( &ScriptConfig::SetShaderVersion,		"SetShaderVersion",		{} );

		binder.Comment( "Set MSL version which is used when cross compiling from SPIRV." );
		binder.AddMethod( &ScriptConfig::SetSpirvToMslVersion,	"SetSpirvToMslVersion",	{} );

		binder.Comment( "Set default shader options." );
		binder.AddMethod( &ScriptConfig::SetShaderOptions,		"SetShaderOptions",		{} );

		binder.Comment( "Set default layout (align rules) for shader structure." );
		binder.AddMethod( &ScriptConfig::SetDefaultLayout,		"SetDefaultLayout",		{} );

		binder.Comment( "Set default pipeline options." );
		binder.AddMethod( &ScriptConfig::SetPipelineOptions,	"SetPipelineOptions",	{} );

		binder.Comment( "Set shader preprocessor.\n"
						"Can transform any source to GLSL/MSL code." );
		binder.AddMethod( &ScriptConfig::SetPreprocessor,		"SetPreprocessor",		{} );

		binder.Comment( "Set FeatureSet which will be added to all resources." );
		binder.AddMethod( &ScriptConfig::SetDefaultFeatureSet,	"SetDefaultFeatureSet",	{"fsName"} );

		binder.Comment( "Set defines which will be used in all shaders.\n"
						"Format: DEF=1\nDEF2" );
		binder.AddMethod( &ScriptConfig::SetShaderDefines,		"SetShaderDefines",		{} );
	}


} // AE::PipelineCompiler
