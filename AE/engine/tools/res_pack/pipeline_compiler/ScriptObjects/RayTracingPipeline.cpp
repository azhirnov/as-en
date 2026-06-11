// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/ScriptObjects/RayTracingPipeline.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static RayTracingPipelineScriptBinding*  RayTracingPipelineScriptBinding_Ctor (const String &name) {
		return RayTracingPipelineScriptBinding::Create( name ).Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	_Init
=================================================
*/
	void  RayTracingPipelineScriptBinding::_Init () __Th___
	{
		BasePipelineTmpl::_Init();  // throw

		CHECK_THROW_MSG( ObjectStorage::Instance()->rtpipelines.emplace( _name, RayTracingPipelinePtr{this} ).second,
			"RayTracingPipeline with name '"s << _nameStr << "' is already defined" );
	}

/*
=================================================
	Create
=================================================
*/
	RayTracingPipelinePtr  RayTracingPipelineScriptBinding::Create (const String &name) __Th___
	{
		RayTracingPipelinePtr	result{ new RayTracingPipelineScriptBinding{ name }};
		result->_Init();  // throw
		return result;
	}

/*
=================================================
	_CheckSupport
=================================================
*/
	void  RayTracingPipelineScriptBinding::_CheckSupport () __Th___
	{
		if ( _isSupported > 1 )
			return;	// OK

		_isSupported = 1;

		TEST_FEATURE( GetFeatures(), rayTracingPipeline );

		_isSupported = 2;
	}

/*
=================================================
	AddGeneralShader
=================================================
*/
	void  RayTracingPipelineScriptBinding::AddGeneralShader (const String &name, const ScriptShaderPtr &inShader) __Th___
	{
		_CheckSupport();
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::RayGen, EShader::RayCallable, EShader::RayMiss ));
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second );

		ObjectStorage::Instance()->AddName<RayTracingGroupName>( name );

		auto&	gen_sh	= _generalShaders.emplace_back();
		gen_sh.name		= name;

		_CompileShader( INOUT gen_sh.shader, inShader, Default, Default );
		_stages |= inShader->type;
	}

/*
=================================================
	AddTriangleHitGroup
=================================================
*/
	void  RayTracingPipelineScriptBinding::AddTriangleHitGroup (const String &name, const ScriptShaderPtr &closestHit, const ScriptShaderPtr &anyHit) __Th___
	{
		_CheckSupport();
		CHECK_THROW_MSG( closestHit );
		CHECK_THROW_MSG( closestHit->type == EShader::RayClosestHit );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second );

		ObjectStorage::Instance()->AddName<RayTracingGroupName>( name );

		auto&	tri_group	= _triangleGroups.emplace_back();
		tri_group.name		= name;

		_CompileShader( INOUT tri_group.closestHit, closestHit, Default, Default );
		_stages |= closestHit->type;

		if ( anyHit )
		{
			CHECK_THROW_MSG( anyHit->type == EShader::RayAnyHit );

			_CompileShader( INOUT tri_group.anyHit, anyHit, Default, Default );
			_stages |= anyHit->type;
		}
	}

/*
=================================================
	AddProceduralHitGroup
=================================================
*/
	void  RayTracingPipelineScriptBinding::AddProceduralHitGroup (const String &name, const ScriptShaderPtr &intersection, const ScriptShaderPtr &closestHit, const ScriptShaderPtr &anyHit) __Th___
	{
		_CheckSupport();
		CHECK_THROW_MSG( intersection );
		CHECK_THROW_MSG( intersection->type == EShader::RayIntersection );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second );

		ObjectStorage::Instance()->AddName<RayTracingGroupName>( name );

		auto&	proc_group	= _proceduralGroups.emplace_back();
		proc_group.name		= name;

		_CompileShader( INOUT proc_group.intersection, intersection, Default, Default );
		_stages |= intersection->type;

		if ( closestHit )
		{
			CHECK_THROW_MSG( closestHit->type == EShader::RayClosestHit );
			_CompileShader( INOUT proc_group.closestHit, closestHit, Default, Default );
			_stages |= closestHit->type;
		}

		if ( anyHit )
		{
			CHECK_THROW_MSG( anyHit->type == EShader::RayAnyHit );
			_CompileShader( INOUT proc_group.anyHit, anyHit, Default, Default );
			_stages |= anyHit->type;
		}
	}

/*
=================================================
	AddSpecialization
=================================================
*/
	RayTracingPipelineSpecPtr  RayTracingPipelineScriptBinding::AddSpecialization2 (const String &name) __Th___
	{
		_Prepare();

		CHECK_THROW_MSG( _isPrepared );
		_OnAddSpecialization();

		return _pplnSpec.emplace_back( RayTracingPipelineSpecPtr{new RayTracingPipelineSpecScriptBinding{ this, name }} );
	}

	RayTracingPipelineSpecScriptBinding*  RayTracingPipelineScriptBinding::AddSpecialization (const String &name) __Th___
	{
		return AddSpecialization2( name ).Detach();
	}

/*
=================================================
	Build
=================================================
*/
	bool  RayTracingPipelineScriptBinding::Build () __NE___
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
	void  RayTracingPipelineScriptBinding::_Prepare () __Th___
	{
		if ( _pipelineUID.has_value() )
			return;

		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( not _generalShaders.empty() );
		CHECK_THROW_MSG( AllBits( _stages, EShaderStages::RayGen ));

		for (auto& group : _generalShaders) {
			CHECK_THROW_MSG( _AddLayout( group.shader ));
		}
		for (auto& group : _triangleGroups) {
			CHECK_THROW_MSG( _AddLayout( group.closestHit ));
			CHECK_THROW_MSG( _AddLayout( group.anyHit ));
		}
		for (auto& group : _proceduralGroups) {
			CHECK_THROW_MSG( _AddLayout( group.intersection ));
			CHECK_THROW_MSG( _AddLayout( group.closestHit ));
			CHECK_THROW_MSG( _AddLayout( group.anyHit ));
		}
		CHECK_THROW_MSG( _BuildLayout() );

		SerializableRayTracingPipeline	desc;

		desc.layout = _GetLayout();
		CHECK_THROW_MSG( desc.layout != Default );

		desc.features = storage.CopyFeatures( _features );

		HashMap< ShaderUID, uint >			unique_shaders;
		Array< Tuple< ShaderUID, EShader >>	shader_arr;

		const auto	AddShader = [&unique_shaders, &shader_arr] (const CompiledShaderPtr &ptr) -> uint
		{{
			if ( ptr )
			{
				auto [iter, inserted] = unique_shaders.emplace( ptr->uid, uint(shader_arr.size()) );
				if ( inserted )
					shader_arr.emplace_back( ptr->uid, ptr->type );
				return iter->second;
			}
			return UMax;
		}};

		// general shaders
		if ( not _generalShaders.empty() )
		{
			std::sort( _generalShaders.begin(), _generalShaders.end(), [](auto& lhs, auto& rhs) { return lhs.name < rhs.name; });

			auto*	dst_ptr = storage.allocator.Allocate< SerializableRayTracingPipeline::GeneralShader >( _generalShaders.size() );
			CHECK_THROW_MSG( dst_ptr != null );

			desc.generalShaders = ArrayView< SerializableRayTracingPipeline::GeneralShader >{ dst_ptr, _generalShaders.size() };
			for (usize i = 0; i < _generalShaders.size(); ++i)
			{
				const auto&	group	= _generalShaders[i];
				auto&		dst		= dst_ptr[i];
				dst.name			= RayTracingGroupName::Optimized_t{ group.name };
				dst.shader			= AddShader( group.shader );
			}
		}

		// triangle hit groups
		if ( not _triangleGroups.empty() )
		{
			std::sort( _triangleGroups.begin(), _triangleGroups.end(), [](auto& lhs, auto& rhs) { return lhs.name < rhs.name; });

			auto*	dst_ptr = storage.allocator.Allocate< SerializableRayTracingPipeline::TriangleHitGroup >( _triangleGroups.size() );
			CHECK_THROW_MSG( dst_ptr != null );

			desc.triangleGroups = ArrayView< SerializableRayTracingPipeline::TriangleHitGroup >{ dst_ptr, _triangleGroups.size() };
			for (usize i = 0; i < _triangleGroups.size(); ++i)
			{
				const auto&	group	= _triangleGroups[i];
				auto&		dst		= dst_ptr[i];
				dst.name			= RayTracingGroupName::Optimized_t{ group.name };
				dst.closestHit		= AddShader( group.closestHit );
				dst.anyHit			= AddShader( group.anyHit );
			}
		}

		// procedural hit groups
		if ( not _proceduralGroups.empty() )
		{
			std::sort( _proceduralGroups.begin(), _proceduralGroups.end(), [](auto& lhs, auto& rhs) { return lhs.name < rhs.name; });

			auto*	dst_ptr = storage.allocator.Allocate< SerializableRayTracingPipeline::ProceduralHitGroup >( _proceduralGroups.size() );
			CHECK_THROW_MSG( dst_ptr != null );

			desc.proceduralGroups = ArrayView< SerializableRayTracingPipeline::ProceduralHitGroup >{ dst_ptr, _proceduralGroups.size() };
			for (usize i = 0; i < _proceduralGroups.size(); ++i)
			{
				const auto&	group	= _proceduralGroups[i];
				auto&		dst		= dst_ptr[i];
				dst.name			= RayTracingGroupName::Optimized_t{ group.name };
				dst.intersection	= AddShader( group.intersection );
				dst.closestHit		= AddShader( group.closestHit );
				dst.anyHit			= AddShader( group.anyHit );
			}
		}

		{
			auto*	dst		= storage.allocator.Allocate< Tuple< ShaderUID, EShader >>( shader_arr.size() );
			desc.shaderArr	= ArrayView<Tuple< ShaderUID, EShader >>{ dst, shader_arr.size() };
			MemCopy( OUT dst, shader_arr.data(), ArraySizeOf(shader_arr) );
		}

		auto [uid, is_unique] = storage.pplnStorage->AddPipeline( _name, RVRef(desc) );

		CHECK_MSG( is_unique,
					"Pipeline name '"s << GetName() << "' is already used and will be overriden" );

		_pipelineUID	= uid;
		_isPrepared		= true;
	}

/*
=================================================
	Get***Shader/Group
=================================================
*/
	uint  RayTracingPipelineScriptBinding::GetRayGenShader (const String &name) const
	{
		for (auto& sh : _generalShaders) {
			if_unlikely( sh.name == name )
			{
				if ( sh.shader->type == EShader::RayGen )
					return uint(PtrDistance( _generalShaders.data(), &sh ));
				break;
			}
		}
		return UMax;
	}

	uint  RayTracingPipelineScriptBinding::GetMissShader (const String &name) const
	{
		for (auto& sh : _generalShaders) {
			if_unlikely( sh.name == name )
			{
				if ( sh.shader->type == EShader::RayMiss )
					return uint(PtrDistance( _generalShaders.data(), &sh ));
				break;
			}
		}
		return UMax;
	}

	uint  RayTracingPipelineScriptBinding::GetHitGroup (const String &name) const
	{
		for (auto& sh : _triangleGroups) {
			if_unlikely( sh.name == name )
				return uint(_generalShaders.size() + PtrDistance( _triangleGroups.data(), &sh ));
		}
		for (auto& sh : _proceduralGroups) {
			if_unlikely( sh.name == name )
				return uint(_generalShaders.size() + _triangleGroups.size() + PtrDistance( _proceduralGroups.data(), &sh ));
		}
		return UMax;
	}

	uint  RayTracingPipelineScriptBinding::GetCallableShader (const String &name) const
	{
		for (auto& sh : _generalShaders) {
			if_unlikely( sh.name == name )
			{
				if ( sh.shader->type == EShader::RayCallable )
					return uint(PtrDistance( _generalShaders.data(), &sh ));
				break;
			}
		}
		return UMax;
	}

/*
=================================================
	Get***Shader/GroupName
=================================================
*/
	StringView  RayTracingPipelineScriptBinding::GetRayGenShaderName (uint idx) const
	{
		if ( idx < _generalShaders.size() )
			return _generalShaders[idx].name;
		else
			return Default;
	}

	StringView  RayTracingPipelineScriptBinding::GetMissShaderName (uint idx) const
	{
		if ( idx < _generalShaders.size() )
			return _generalShaders[idx].name;
		else
			return Default;
	}

	StringView  RayTracingPipelineScriptBinding::GetHitGroupName (uint idx) const
	{
		usize	i = idx;

		if ( i < _generalShaders.size() )
			return Default;

		i -= _generalShaders.size();

		if ( i < _triangleGroups.size() )
			return _triangleGroups[i].name;

		i -= _triangleGroups.size();

		if ( i < _proceduralGroups.size() )
			return _proceduralGroups[i].name;

		return Default;
	}

	StringView  RayTracingPipelineScriptBinding::GetCallableShaderName (uint idx) const
	{
		if ( idx < _generalShaders.size() )
			return _generalShaders[idx].name;
		else
			return Default;
	}

/*
=================================================
	Bind
=================================================
*/
	void  RayTracingPipelineScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RayTracingPipelineScriptBinding>	binder{ se };
		binder.CreateRef( 0, False{} );

		binder.Comment( "Create pipeline template.\n"
						"Name is used in C++ code to create pipeline." );
		binder.AddFactoryCtor( &RayTracingPipelineScriptBinding_Ctor, {"name"} );

		binder.Comment( "Add macros which will be used in shader.\n"
						"Format: MACROS = value \\n DEF \\n ..." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::Define,					"Define",				{} );

		binder.Comment( "Create specialization for pipeline template.\n"
						"Name is used in C++ code to get pipeline from render technique.\n"
						"Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::AddSpecialization,		"AddSpecialization",	{"specName"} );

		binder.Comment( "Add FeatureSet to the pipeline." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::AddFeatureSet,			"AddFeatureSet",		{"fsName"} );

		binder.Comment( "Set pipeline layout.\n"
						"Pipeline will inherit layout feature sets." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::SetLayout,				"SetLayout",			{"plName"} );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::SetLayout2,				"SetLayout",			{"pl"} );

		binder.Comment( "Create general shader group. Can be used as RayGen/Miss/Callable shaders." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::AddGeneralShader,		"AddGeneralShader",		{"groupName", "shader"} );

		binder.Comment( "Create triangle hit group. Can be used only as HitGroup.\n"
						"'closestHit' must be defined, 'anyHit' is optional." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::AddTriangleHitGroup,	"AddTriangleHitGroup",	{"groupName", "closestHit", "anyHit"} );

		binder.Comment( "Create procedural hit group. Can be used only as HitGroup.\n"
						"'intersection' and 'closestHit' must be defined, 'anyHit' is optional." );
		AS_METHOD( binder, RayTracingPipelineScriptBinding::AddProceduralHitGroup,	"AddProceduralHitGroup",{"groupName", "intersection", "closestHit", "anyHit"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RayTracingPipelineSpecScriptBinding::RayTracingPipelineSpecScriptBinding (RayTracingPipelineScriptBinding* base, const String &name) __Th___ :
		BasePipelineSpec{ base, name }
	{}

/*
=================================================
	GetBase
=================================================
*/
	const RayTracingPipelineScriptBinding*  RayTracingPipelineSpecScriptBinding::GetBase () const
	{
		return Cast<RayTracingPipelineScriptBinding>( BasePipelineSpec::GetBase() );
	}

/*
=================================================
	SetDynamicState
=================================================
*/
	void  RayTracingPipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
	{
		auto	ds = EPipelineDynamicState(states);
		CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::RayTracingPipelineMask) == Default,
			"unsupported dynamic state for ray tracing pipeline" );

		desc.dynamicState = ds;
	}

/*
=================================================
	Build
=================================================
*/
	bool  RayTracingPipelineSpecScriptBinding::Build (PipelineTemplUID templUID) __NE___
	{
		if ( not IsEnabled() )
			return true;

		if ( IsBuilded() )
			return true;

		auto&	ppln_storage = *ObjectStorage::Instance()->pplnStorage;

		SerializableRayTracingPipelineSpec	spec;
		spec.templUID		= templUID;
		spec.desc			= desc;
		spec.desc.options	= _options;

		_OnBuild( ppln_storage.AddPipeline( Name(), RVRef(spec) ));
		return true;
	}

/*
=================================================
	Max***
=================================================
*/
	void  RayTracingPipelineSpecScriptBinding::MaxRecursionDepth (uint value) __Th___
	{
		TestFeature_Min( GetBase()->GetFeatures(), &FeatureSet::maxRayRecursionDepth, value, "maxRayRecursionDepth", "maxRecursionDepth" );

		desc.maxRecursionDepth = value;
	}
/*
	void  RayTracingPipelineSpecScriptBinding::MaxPipelineRayPayloadSize (Bytes value) __Th___
	{
		// TODO
	}

	void  RayTracingPipelineSpecScriptBinding::MaxPipelineRayHitAttributeSize (Bytes value) __Th___
	{
		// TODO
	}

/*
=================================================
	Bind
=================================================
*/
	void  RayTracingPipelineSpecScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RayTracingPipelineSpecScriptBinding>	binder{ se };
		binder.CreateRef( 0, False{} );

		binder.Comment( "Set specialization value.\n"
						"Specialization constant must be previously defined in shader by 'Shader::AddSpec()'." );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::SetSpecValueU,		"SetSpecValue",		{"name", "value"} );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::SetSpecValueI,		"SetSpecValue",		{"name", "value"} );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::SetSpecValueF,		"SetSpecValue",		{"name", "value"} );

		binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
						"Supported: 'RTStackSize'." );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::SetDynamicState,	"SetDynamicState",	{"states"} );

		binder.Comment( "Attach pipeline to the render technique.\n"
						"Render technique will create all attached pipelines during its creation." );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::AddToRenderTech,	"AddToRenderTech",	{"rtech", "cpass"} );

		binder.Comment( "Set pipeline options (EPipelineOpt).\n"
						"Supported: Optimize, RT_NoNullAnyHitShaders, RT_NoNullClosestHitShaders, RT_NoNullMissShaders, RT_NoNullIntersectionShaders, RT_SkipTriangles, RT_SkipAABBs.\n"
						"By default used value from 'GlobalConfig::SetPipelineOptions()'." );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::SetOptions,			"SetOptions",		{"opts"} );

		binder.Comment( "Set max recursion.\n"
						"User must check recursion depth in shader and avoid exceeding this limit." );
		AS_METHOD( binder, RayTracingPipelineSpecScriptBinding::MaxRecursionDepth,	"MaxRecursionDepth", {} );
	}


} // AE::PipelineCompiler
