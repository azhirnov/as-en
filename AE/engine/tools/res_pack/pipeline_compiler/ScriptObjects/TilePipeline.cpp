// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/TilePipeline.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static TilePipelineScriptBinding*  TilePipelineScriptBinding_Ctor (const String &name) {
		return TilePipelinePtr{ new TilePipelineScriptBinding{ name }}.Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	TilePipelineScriptBinding::TilePipelineScriptBinding (const String &name) __Th___ : BasePipelineTmpl{name}
	{
		CHECK_THROW_MSG( ObjectStorage::Instance()->tpipelines.emplace( _name, TilePipelinePtr{this} ).second,
			"TilePipeline with name '"s << name << "' is already defined" );
	}

	TilePipelineScriptBinding::TilePipelineScriptBinding () :
		TilePipelineScriptBinding{ "<unknown>" }
	{}

/*
=================================================
	SetShader
=================================================
*/
	void  TilePipelineScriptBinding::SetShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Tile, Default ));
		inShader->type = EShader::Tile;

		TEST_FEATURE( GetFeatures(), tileShader );

		_CompileShader( INOUT shader, inShader, Default, Default );
		_stages |= inShader->type;
	}

/*
=================================================
	AddSpecialization
=================================================
*/
	TilePipelineSpecPtr  TilePipelineScriptBinding::AddSpecialization2 (const String &name) __Th___
	{
		_Prepare();

		CHECK_THROW_MSG( _isPrepared );
		_OnAddSpecialization();

		return _pplnSpec.emplace_back( TilePipelineSpecPtr{new TilePipelineSpecScriptBinding{ this, name }} );
	}

	TilePipelineSpecScriptBinding*  TilePipelineScriptBinding::AddSpecialization (const String &name) __Th___
	{
		return AddSpecialization2( name ).Detach();
	}

/*
=================================================
	Build
=================================================
*/
	bool  TilePipelineScriptBinding::Build () __NE___
	{
		if ( not _enabled )
			return true;

		try {
			_Prepare();
		}
		catch (...) {
			return false;
		}

		for (auto& src : _pplnSpec)
		{
			CHECK_ERR( src->Build( *_pipelineUID ));
		}

		return true;
	}

/*
=================================================
	_Prepare
=================================================
*/
	void  TilePipelineScriptBinding::_Prepare () __Th___
	{
		if ( _pipelineUID.has_value() )
			return;

		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( _AddLayout( shader ));
		CHECK_THROW_MSG( _BuildLayout() );

		SerializableTilePipeline		desc;

		desc.layout = _GetLayout();
		CHECK_THROW_MSG( desc.layout != Default );

		CHECK_THROW_MSG( shader );

		desc.shader		= shader->uid;
		desc.features	= storage.CopyFeatures( _features );

		auto [uid, is_unique] = storage.pplnStorage->AddPipeline( _name, RVRef(desc) );

		CHECK_MSG( is_unique,
					"Pipeline name '"s << GetName() << "' is already used and will be overriden" );

		_pipelineUID	= uid;
		_isPrepared		= true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  TilePipelineScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<TilePipelineScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Create pipeline template.\n"
						"Name is used in C++ code to create pipeline." );
		binder.AddFactoryCtor( &TilePipelineScriptBinding_Ctor,	{"name"} );

		binder.Comment( "Add macros which will be used in shader.\n"
						"Format: MACROS = value \\n DEF \\n ..." );
		binder.AddMethod( &TilePipelineScriptBinding::Define,				"Define",			{} );

		binder.Comment( "Set tile shader.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &TilePipelineScriptBinding::SetShader,			"SetShader",		{} );

		binder.Comment( "Create specialization for pipeline template.\n"
						"Name is used in C++ code to get pipeline from render technique.\n"
						"Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters." );
		binder.AddMethod( &TilePipelineScriptBinding::AddSpecialization,	"AddSpecialization", {"specName"} );

		binder.Comment( "Add FeatureSet to the pipeline." );
		binder.AddMethod( &TilePipelineScriptBinding::AddFeatureSet,		"AddFeatureSet",	{"fsName"} );

		binder.Comment( "Set pipeline layout.\n"
						"Pipeline will inherit layout feature sets." );
		binder.AddMethod( &TilePipelineScriptBinding::SetLayout,			"SetLayout",		{"plName"} );
		binder.AddMethod( &TilePipelineScriptBinding::SetLayout2,			"SetLayout",		{"pl"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	TilePipelineSpecScriptBinding::TilePipelineSpecScriptBinding (TilePipelineScriptBinding* base, const String &name) __Th___ :
		BasePipelineSpec{ base, name }
	{}

/*
=================================================
	GetBase
=================================================
*/
	const TilePipelineScriptBinding*  TilePipelineSpecScriptBinding::GetBase () const
	{
		return Cast<TilePipelineScriptBinding>( BasePipelineSpec::GetBase() );
	}

/*
=================================================
	SetDynamicState
=================================================
*/
	void  TilePipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
	{
		auto	ds = EPipelineDynamicState(states);
		CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::TilePipelineMask) == Default, "unsupported dynamic state for compute pipeline" );

		desc.dynamicState = ds;
	}

/*
=================================================
	SetLocalGroupSize2
=================================================
*/
	void  TilePipelineSpecScriptBinding::SetLocalGroupSize2 (uint x, uint y) __Th___
	{
		CHECK_THROW_MSG( GetBase() != null and GetBase()->shader, "shader is not compiled" );

		// TODO: tile local size instead of compute ?
		const auto&	spec		= GetBase()->shader->reflection.compute.localGroupSpec;
		uint		total_size	= Max( 1u, GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupInvocations ));
		uint3		max_threads	= uint3{ GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeX ),
										 GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeY ),
										 GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeZ )};
		max_threads = Max( max_threads, uint3{1} );

		packed_ushort3	local_size;
		_SetLocalGroupSize( "tile localSize ", spec, max_threads, total_size, uint3{x,y,1}, OUT local_size );
		desc.localSize = ushort2{local_size};
	}

/*
=================================================
	Build
=================================================
*/
	bool  TilePipelineSpecScriptBinding::Build (PipelineTemplUID templUID) __NE___
	{
		if ( not _enabled )
			return true;

		if ( IsBuilded() )
			return true;

		auto&	ppln_storage = *ObjectStorage::Instance()->pplnStorage;

		SerializableTilePipelineSpec	spec;
		spec.templUID		= templUID;
		spec.desc			= desc;
		spec.desc.options	= _options;

		_OnBuild( ppln_storage.AddPipeline( Name(), RVRef(spec) ));
		return true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  TilePipelineSpecScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<TilePipelineSpecScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set specialization value.\n"
						"Specialization constant must be previously defined in shader by 'Shader::AddSpec()'." );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetSpecValueU,		"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetSpecValueI,		"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetSpecValueF,		"SetSpecValue",		{"name", "value"} );

		binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
						"None of the states are supported for tile pipeline." );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetDynamicState,		"SetDynamicState",	{"states"} );

		binder.Comment( "Set tile shader tile size. All threads in tile can use same (tile) memory." );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetLocalGroupSize1,	"SetLocalSize",		{"x"} );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetLocalGroupSize2,	"SetLocalSize",		{"x", "y"} );

		binder.Comment( "Attach pipeline to the render technique.\n"
						"When rtech is created it will create all attached pipelines." );
		binder.AddMethod( &TilePipelineSpecScriptBinding::AddToRenderTech,		"AddToRenderTech",	{"rtech", "gpass"} );

		binder.Comment( "Set pipeline options (EPipelineOpt).\n"
						"Supported: 'Optimize'.\n"
						"By default used value from 'GlobalConfig::SetPipelineOptions()'." );
		binder.AddMethod( &TilePipelineSpecScriptBinding::SetOptions,			"SetOptions",		{"opts"} );
	}


} // AE::PipelineCompiler
