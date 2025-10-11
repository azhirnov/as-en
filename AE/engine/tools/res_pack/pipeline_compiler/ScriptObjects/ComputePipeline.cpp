// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/ScriptObjects/ComputePipeline.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

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
	bool  ComputePipelineScriptBinding::Build () __NE___
	{
		if ( not IsEnabled() )
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

		desc.defaultLocalSize	= WGLocalSize_t{shader->reflection.compute.localGroupSize};
		desc.localSizeSpec		= WGLocalSizeSpec_t{shader->reflection.compute.localGroupSpec};
		desc.shader				= shader->uid;
		desc.features			= storage.CopyFeatures( _features );

		auto [uid, is_unique] = storage.pplnStorage->AddPipeline( _name, RVRef(desc) );

		CHECK_MSG( is_unique,
			"Pipeline name '"s << GetName() << "' is already used and will be overriden" );

		CHECK_THROW_MSG( All( (desc.defaultLocalSize > WGLocalSize_t{0}) or (desc.localSizeSpec != WGLocalSizeSpec_t{SerializableComputePipeline::UndefinedSpecConst}) ),
			"'defaultLocalSize' is zero and specialization is not defined" );

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
						"Format: MACROS = value \\n DEF \\n ..." );
		AS_METHOD( binder, ComputePipelineScriptBinding::Define,			"Define",				{} );

		binder.Comment( "Set compute shader.\n"
						"Pipeline will inherit shader feature sets." );
		AS_METHOD( binder, ComputePipelineScriptBinding::SetShader,			"SetShader",			{} );

		binder.Comment( "Create specialization for pipeline template.\n"
						"Name is used in C++ code to get pipeline from render technique.\n"
						"Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters." );
		AS_METHOD( binder, ComputePipelineScriptBinding::AddSpecialization,	"AddSpecialization",	{"specName"} );

		binder.Comment( "Add FeatureSet to the pipeline." );
		AS_METHOD( binder, ComputePipelineScriptBinding::AddFeatureSet,		"AddFeatureSet",		{"fsName"} );

		binder.Comment( "Set pipeline layout.\n"
						"Pipeline will inherit layout feature sets." );
		AS_METHOD( binder, ComputePipelineScriptBinding::SetLayout,			"SetLayout",			{"plName"} );
		AS_METHOD( binder, ComputePipelineScriptBinding::SetLayout2,		"SetLayout",			{"pl"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ComputePipelineSpecScriptBinding::ComputePipelineSpecScriptBinding (ComputePipelineScriptBinding* base, const String &name) __Th___ :
		BasePipelineSpec{ base, name }
	{}

/*
=================================================
	GetBase
=================================================
*/
	const ComputePipelineScriptBinding*  ComputePipelineSpecScriptBinding::GetBase () const
	{
		return Cast<ComputePipelineScriptBinding>( BasePipelineSpec::GetBase() );
	}

/*
=================================================
	SetDynamicState
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
	{
		auto	ds = EPipelineDynamicState(states);
		CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::ComputePipelineMask) == Default, "unsupported dynamic state for compute pipeline" );

		desc.dynamicState = ds;
	}

/*
=================================================
	SetLocalGroupSize3
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetLocalGroupSize3 (uint x, uint y, uint z) __Th___
	{
		CHECK_THROW_MSG( GetBase() != null and GetBase()->shader, "shader is not compiled" );
		CHECK_THROW_MSG( desc.subgroupSize == 0, "Set local group size before setting subgroup size." );

		const auto&	spec		= GetBase()->shader->reflection.compute.localGroupSpec;
		const uint	inv_count	= x * y * z;
		uint		total_size	= 0;
		uint3		max_threads;

		for (auto& feat : GetFeatures())
		{
			if ( inv_count <= feat->fs.maxComputeWorkGroupInvocations and
				 x <= feat->fs.maxComputeWorkGroupSizeX and
				 y <= feat->fs.maxComputeWorkGroupSizeY and
				 z <= feat->fs.maxComputeWorkGroupSizeZ )
			{
				total_size  = uint{feat->fs.maxComputeWorkGroupInvocations};
				max_threads = uint3{uint{feat->fs.maxComputeWorkGroupSizeX},
									uint{feat->fs.maxComputeWorkGroupSizeY},
									uint{feat->fs.maxComputeWorkGroupSizeZ}};
				break;
			}
		}

		_SetLocalGroupSize( "compute localSize ", spec, max_threads, total_size, uint3{x,y,z}, OUT desc.localSize );
	}

/*
=================================================
	SetLocalGroupSizeAtLoadTime
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetLocalGroupSizeAtLoadTime () __Th___
	{
		CHECK_THROW_MSG( GetBase() != null and GetBase()->shader, "shader is not compiled" );
		CHECK_THROW_MSG( All( desc.localSize == BasePipelineDesc::UndefinedLocalSize ), "Workgroup size is already set" );
		CHECK_THROW_MSG( desc.subgroupSize == 0, "Explicit subgroup size is not compatible with load time workgroup size." );

		const auto&	spec = GetBase()->shader->reflection.compute.localGroupSpec;
		CHECK_THROW_MSG( All( spec != uint3{~0u} ),
			"All specialization constants must be enabled to use load time local size" );

		desc.localSize = WGLocalSize_t{BasePipelineDesc::LoadTimeLocalSize};
	}

/*
=================================================
	SetSubgroupSize
=================================================
*/
	void  ComputePipelineSpecScriptBinding::SetSubgroupSize (uint value) __Th___
	{
		CHECK_THROW_MSG( GetBase() != null and GetBase()->shader, "shader is not compiled" );

		CHECK_THROW_MSG( All( desc.localSize != WGLocalSize_t{BasePipelineDesc::LoadTimeLocalSize} ),
			"Workgroup size (local size) which will be set at load time is not compatible with explicit subgroup size" );

		TEST_FEATURE( GetFeatures(), subgroupSizeControl );

		const auto&		def_size	= GetBase()->shader->reflection.compute.localGroupSize;
		const auto&		spec		= GetBase()->shader->reflection.compute.localGroupSpec;
		const uint3		local_dim	{ (spec.x == UMax or desc.localSize.x == UMax) ? def_size.x : desc.localSize.x,
									  (spec.y == UMax or desc.localSize.y == UMax) ? def_size.y : desc.localSize.y,
									  (spec.z == UMax or desc.localSize.z == UMax) ? def_size.z : desc.localSize.z };
		bool			supported	= false;

		CHECK_THROW_MSG( All( local_dim > 0u ));

		for (auto& feat : GetFeatures())
		{
			if ( value >= feat->fs.minSubgroupSize and
				 value <= feat->fs.maxSubgroupSize and
				 AllBits( feat->fs.requiredSubgroupSizeStages, EShaderStages::Compute ))
			{
				supported = true;
				break;
			}
		}
		CHECK_THROW_MSG( supported,
			"Subgroup size ("s << ToString(value) << ") must be in range [minSubgroupSize, maxSubgroupSize] in at least one feature set" );

		CHECK_THROW_MSG( IsMultipleOf( local_dim.x, value ),
			"Local size X ("s << ToString( local_dim.x ) << ") must be multiple of subgroup size (" << ToString( value ) << ")" );

		desc.subgroupSize = ushort(value);
	}

/*
=================================================
	Build
=================================================
*/
	bool  ComputePipelineSpecScriptBinding::Build (PipelineTemplUID templUID) __NE___
	{
		if ( not IsEnabled() )
			return true;

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
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetSpecValueU,			"SetSpecValue",		{"name", "value"} );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetSpecValueI,			"SetSpecValue",		{"name", "value"} );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetSpecValueF,			"SetSpecValue",		{"name", "value"} );

		binder.Comment( "Set subgroup size.\n"
						"Requires 'subgroupSizeControl' feature, value must be in range [minSubgroupSize, maxSubgroupSize]." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetSubgroupSize,		"SubgroupSize",		{} );

		binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
						"None of the states are supported for compute pipeline." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetDynamicState,		"SetDynamicState",	{"states"} );

		binder.Comment( "Set compute shader workgroup size. All threads in workgroup can use same (shared) memory.\n"
						"Shader must use 'ComputeSpec1/2/3()' to define specialization constant." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetLocalGroupSize1,	"SetLocalSize",		{"x"} );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetLocalGroupSize2,	"SetLocalSize",		{"x", "y"} );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetLocalGroupSize3,	"SetLocalSize",		{"x", "y", "z"});

		binder.Comment( "Compute shader workgroup size will be set at load time in 'RenderTechDesc::computeLocalSize'." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetLocalGroupSizeAtLoadTime,	"LoadTimeLocalSize",	{} );

		binder.Comment( "Attach pipeline to the render technique.\n"
						"Render technique will create all attached pipelines during its creation." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::AddToRenderTech,		"AddToRenderTech",	{"rtech", "gpass"} );

		binder.Comment( "Set pipeline options (EPipelineOpt).\n"
						"Supported: 'Optimize', 'CS_DispatchBase'.\n"
						"By default used value from 'GlobalConfig::SetPipelineOptions()'." );
		AS_METHOD( binder, ComputePipelineSpecScriptBinding::SetOptions,			"SetOptions",		{"opts"} );
	}


} // AE::PipelineCompiler
