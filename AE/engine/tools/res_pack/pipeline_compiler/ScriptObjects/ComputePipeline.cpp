// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ComputePipeline.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static ComputePipelineScriptBinding*  ComputePipelineScriptBinding_Ctor (const String &name) {
		return ComputePipelinePtr{ new ComputePipelineScriptBinding{ name }}.Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ComputePipelineScriptBinding::ComputePipelineScriptBinding (const String &name) __Th___ : BasePipelineTmpl{name}
	{
		CHECK_THROW_MSG( ObjectStorage::Instance()->cpipelines.emplace( _name, ComputePipelinePtr{this} ).second,
			"ComputePipeline with name '"s << name << "' is already defined" );
	}

	ComputePipelineScriptBinding::ComputePipelineScriptBinding () :
		ComputePipelineScriptBinding{ "<unknown>" }
	{}

/*
=================================================
	SetShader
=================================================
*/
	void  ComputePipelineScriptBinding::SetShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Compute, Default ));
		inShader->type = EShader::Compute;

		TEST_FEATURE( GetFeatures(), computeShader );

		_CompileShader( INOUT shader, inShader, Default, Default );
		_stages |= inShader->type;
	}

/*
=================================================
	AddSpecialization
=================================================
*/
	ComputePipelineSpecPtr  ComputePipelineScriptBinding::AddSpecialization2 (const String &name) __Th___
	{
		_Prepare();

		CHECK_THROW_MSG( _isPrepared );
		_OnAddSpecialization();

		return _pplnSpec.emplace_back( ComputePipelineSpecPtr{new ComputePipelineSpecScriptBinding{ this, name }} );
	}

	ComputePipelineSpecScriptBinding*  ComputePipelineScriptBinding::AddSpecialization (const String &name) __Th___
	{
		return AddSpecialization2( name ).Detach();
	}

/*
=================================================
	Build
=================================================
*/
	bool  ComputePipelineScriptBinding::Build ()
	{
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
	void  ComputePipelineScriptBinding::_Prepare () __Th___
	{
		if ( _pipelineUID.has_value() )
			return;

		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( _AddLayout( shader ));
		CHECK_THROW_MSG( _BuildLayout() );

		SerializableComputePipeline		desc;

		desc.layout = _GetLayout();
		CHECK_THROW_MSG( desc.layout != Default );

		CHECK_THROW_MSG( shader );

		desc.defaultLocalSize	= ushort3{shader->reflection.compute.localGroupSize};
		desc.localSizeSpec		= ushort3{shader->reflection.compute.localGroupSpec};
		desc.shader				= shader->uid;
		desc.features			= storage.CopyFeatures( _features );

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
	void  ComputePipelineScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ComputePipelineScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Create pipeline template.\n"
						"Name is used in C++ code to create pipeline." );
		binder.AddFactoryCtor( &ComputePipelineScriptBinding_Ctor,	{"name"} );

		binder.Comment( "Add macros which will be used in shader.\n"
						"Format: MACROS = value; DEF" );
		binder.AddMethod( &ComputePipelineScriptBinding::Define,			"Define",				{} );

		binder.Comment( "Set compute shader.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &ComputePipelineScriptBinding::SetShader,			"SetShader",			{} );

		binder.Comment( "Create specialization for pipeline template.\n"
						"Name is used in C++ code to get pipeline from render technique.\n"
						"Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters." );
		binder.AddMethod( &ComputePipelineScriptBinding::AddSpecialization,	"AddSpecialization",	{"specName"} );

		binder.Comment( "Add FeatureSet to the pipeline." );
		binder.AddMethod( &ComputePipelineScriptBinding::AddFeatureSet,		"AddFeatureSet",		{"fsName"} );

		binder.Comment( "Set pipeline layout.\n"
						"Pipeline will inherit layout feature sets." );
		binder.AddMethod( &ComputePipelineScriptBinding::SetLayout,			"SetLayout",			{"plName"} );
		binder.AddMethod( &ComputePipelineScriptBinding::SetLayout2,		"SetLayout",			{"pl"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ComputePipelineSpecScriptBinding::ComputePipelineSpecScriptBinding (const ComputePipelineScriptBinding* base, const String &name) __Th___ :
		BasePipelineSpec{ base, name }
	{}

/*
=================================================
	SetDynamicState
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
	{
		auto	ds = EPipelineDynamicState(states);
		CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::ComputePipelineMask) == Default, "unsupported dynamic state for compute pipeline" );

		//desc.dynamicState = ds;
	}

/*
=================================================
	SetLocalGroupSize3
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetLocalGroupSize3 (uint x, uint y, uint z) __Th___
	{
		CHECK_THROW_MSG( GetBase() != null and GetBase()->shader, "shader is not compiled" );

		const auto&	spec		= GetBase()->shader->reflection.compute.localGroupSpec;
		uint		total_size	= Max( 1u, GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupInvocations ));
		uint3		max_threads	= uint3{ GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeX ),
										 GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeY ),
										 GetMaxValueFromFeatures( GetBase()->GetFeatures(), &FeatureSet::maxComputeWorkGroupSizeZ )};
		max_threads = Max( max_threads, uint3{1} );


		_SetLocalGroupSize( "compute localSize ", spec, max_threads, total_size, uint3{x,y,z}, OUT desc.localSize );
	}

/*
=================================================
	Build
=================================================
*/
	bool  ComputePipelineSpecScriptBinding::Build (PipelineTemplUID templUID)
	{
		if ( IsBuilded() )
			return true;

		auto&	ppln_storage = *ObjectStorage::Instance()->pplnStorage;

		SerializableComputePipelineSpec	spec;
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
	void  ComputePipelineSpecScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ComputePipelineSpecScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set specialization value.\n"
						"Specialization constant must be previously defined in shader by 'Shader::AddSpec()'." );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetSpecValueU,			"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetSpecValueI,			"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetSpecValueF,			"SetSpecValue",		{"name", "value"} );

		binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
						"None of the states are supported for compute pipeline." );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetDynamicState,		"SetDynamicState",	{"states"} );

		binder.Comment( "Set compute shader workgroup size. All threads in workgroup can use same (shared) memory.\n"
						"Shader must use 'ComputeSpec1/2/3()' to define specialization constant." );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetLocalGroupSize1,	"SetLocalSize",		{"x"} );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetLocalGroupSize2,	"SetLocalSize",		{"x", "y"} );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetLocalGroupSize3,	"SetLocalSize",		{"x", "y", "z"});

		binder.Comment( "Attach pipeline to the render technique.\n"
						"When rtech is created it will create all attached pipelines." );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::AddToRenderTech,		"AddToRenderTech",	{"rtech", "gpass"} );

		binder.Comment( "Set pipeline options (EPipelineOpt).\n"
						"Supported: 'Optimize', 'CS_DispatchBase'.\n"
						"By default used value from 'GlobalConfig::SetPipelineOptions()'." );
		binder.AddMethod( &ComputePipelineSpecScriptBinding::SetOptions,			"SetOptions",		{"opts"} );
	}


} // AE::PipelineCompiler
