// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/ScriptBasePass.cpp.h"

AE_DECL_SCRIPT_OBJ(	AE::ResEditor::RTInstanceIndex,	"InstanceIndex"	);
AE_DECL_SCRIPT_OBJ(	AE::ResEditor::RTRayIndex,		"RayIndex"		);
AE_DECL_SCRIPT_OBJ(	AE::ResEditor::RTCallableIndex,	"CallableIndex"	);
AE_DECL_SCRIPT_OBJ(	AE::ResEditor::RTShader,		"RTShader"		);

namespace AE::ResEditor
{
namespace
{
	static ScriptRayTracingPass*  ScriptRayTracingPass_Ctor1 () {
		return ScriptRayTracingPassPtr{ new ScriptRayTracingPass{ String{} }}.Detach();
	}

	static ScriptRayTracingPass*  ScriptRayTracingPass_Ctor2 (const String &defines) {
		return ScriptRayTracingPassPtr{ new ScriptRayTracingPass{ defines }}.Detach();
	}

	static void  RTInstanceIndex_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTInstanceIndex>( OUT mem, value );
	}

	static void  RTRayIndex_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTRayIndex>( OUT mem, value );
	}

	static void  RTCallableIndex_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTCallableIndex>( OUT mem, value );
	}

	static void  RTShader_Ctor1 (OUT void* mem, const String &filename) {
		PlacementNew<RTShader>( OUT mem, filename, "" );
	}

	static void  RTShader_Ctor2 (OUT void* mem, const String &filename, const String &defines) {
		PlacementNew<RTShader>( OUT mem, filename, defines );
	}

} // namespace


/*
=================================================
	operator Iteration
=================================================
*/
	ScriptRayTracingPass::Iteration::operator RayTracingPass::Iteration () C_Th___
	{
		RayTracingPass::Iteration	result;
		result.dim	= dim;

		if ( indirect )
		{
			result.indirect			= indirect->ToResource();	// throw
			result.indirectOffset	= indirectOffset;

			if ( not indirectCmdField.empty() )
			{
				ASSERT( indirectOffset == 0 );
				result.indirectOffset = indirect->GetFieldOffset( indirectCmdField );	// throw
			}
		}
		return result;
	}

/*
=================================================
	_Shader ctor
=================================================
*/
	ScriptRayTracingPass::_Shader::_Shader (const RTShader &sh, bool isRequired) __Th___
	{
		if ( not sh.isDefined )
			return;

		shaderPath	= ScriptExe::ScriptPassApi::ToShaderPath( sh.filename );
		defines		= sh.defines;

		FindAndReplace( INOUT defines, '=', ' ' );

		isDefined = FileSystem::IsFile( shaderPath );

		if ( isRequired )
		{
			CHECK_THROW_MSG( isDefined,
				"File '"s << sh.filename << "' is not exists" );
		}
	}

/*
=================================================
	constructor
=================================================
*/
	ScriptRayTracingPass::ScriptRayTracingPass (const String &defines) __Th___
	{
		_defines = defines;
		StringToColor( OUT _dbgColor, StringView{_dbgName} );
		FindAndReplace( INOUT _defines, '=', ' ' );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptRayTracingPass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) C_Th___
	{
		CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

		arg.state |= EResourceState::RayTracingShaders;
	}

/*
=================================================
	DispatchThreads*
=================================================
*/
	void  ScriptRayTracingPass::DispatchThreads3v (const packed_uint3 &threads) __Th___
	{
		auto&	it	= _iterations.emplace_back();
		it.dim		= threads;
	}

	void  ScriptRayTracingPass::DispatchThreadsDS (const ScriptDynamicDimPtr &ds) __Th___
	{
		CHECK_THROW_MSG( ds and ds->Get() );

		auto&	it	= _iterations.emplace_back();
		it.dim		= ds->Get();

		_SetDynamicDimension( ds );
	}

	void  ScriptRayTracingPass::DispatchThreads1D (const ScriptDynamicUIntPtr &dyn) __Th___
	{
		CHECK_THROW_MSG( dyn );

		auto&	it	= _iterations.emplace_back();
		it.dim		= dyn->Get();
	}

/*
=================================================
	DispatchThreadsIndirect*
=================================================
*/
	void  ScriptRayTracingPass::DispatchThreadsIndirect1 (const ScriptBufferPtr &ibuf) __Th___
	{
		DispatchThreadsIndirect2( ibuf, 0 );
	}

	void  ScriptRayTracingPass::DispatchThreadsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( ibuf );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectOffset	= Bytes{offset};
	}

	void  ScriptRayTracingPass::DispatchThreadsIndirect3 (const ScriptBufferPtr &ibuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not field.empty() );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectCmdField	= field;
	}

/*
=================================================
	MaxRayTypes
=================================================
*/
	void  ScriptRayTracingPass::MaxRayTypes (uint count) __Th___
	{
		CHECK_THROW_MSG( _maxRayTypes == 0, "MaxRayTypes() is already used" );
		CHECK_THROW_MSG( count > 0 );

		_maxRayTypes = count;
	}

/*
=================================================
	SetRayGen
=================================================
*/
	void  ScriptRayTracingPass::SetRayGen (const RTShader &sh) __Th___
	{
		CHECK_THROW_MSG( sh.isDefined );
		CHECK_THROW_MSG( not _rayGen.IsDefined(), "RayGen shader is already set" );

		_rayGen.shader	= _Shader{ sh, true };
		_rayGen.name	= "Main";

		if ( _dbgName.empty() )
		{
			_dbgName = ToString( _rayGen.shader.shaderPath.stem() );

			if ( not _defines.empty() )
				_dbgName << "|" << _defines;
		}
	}

/*
=================================================
	SetRayMiss
=================================================
*/
	void  ScriptRayTracingPass::SetRayMiss (const RTRayIndex &missIndex, const RTShader &sh) __Th___
	{
		CHECK_THROW_MSG( sh.isDefined );

		_missShaders.resize( Max( _missShaders.size(), missIndex.value+1 ), Default );

		auto&	dst = _missShaders[ missIndex.value ];

		CHECK_THROW_MSG( not dst.IsDefined(),
			"Miss shader ("s << ToString(missIndex.value) << ") is already set" );

		dst.shader	= _Shader{ sh, true };
		dst.name	= "Miss-"s << ToString(missIndex.value);
	}

/*
=================================================
	SetCallable
=================================================
*/
	void  ScriptRayTracingPass::SetCallable (const RTCallableIndex &callIndex, const RTShader &sh) __Th___
	{
		CHECK_THROW_MSG( sh.isDefined );

		_callableShaders.resize( Max( _callableShaders.size(), callIndex.value+1 ), Default );

		auto&	dst = _callableShaders[ callIndex.value ];

		CHECK_THROW_MSG( not dst.IsDefined(),
			"Callable shader ("s << ToString(callIndex.value) << ") is already set" );

		dst.shader	= _Shader{ sh, true };
		dst.name	= "Call-"s << ToString(callIndex.value);
	}

/*
=================================================
	SetTriangleHit*
=================================================
*/
	void  ScriptRayTracingPass::SetTriangleHit1 (const RTRayIndex &rayIndex, const RTInstanceIndex &instanceIndex, const RTShader &closestHit) __Th___
	{
		SetTriangleHit2( rayIndex, instanceIndex, closestHit, RTShader() );
	}

	void  ScriptRayTracingPass::SetTriangleHit2 (const RTRayIndex &rayIndex, const RTInstanceIndex &instanceIndex, const RTShader &closestHit, const RTShader &anyHit) __Th___
	{
		CHECK_THROW_MSG( rayIndex.value < _maxRayTypes );
		CHECK_THROW_MSG( closestHit.isDefined );

		_hitGroups.resize( Max( _hitGroups.size(), instanceIndex.value+1 ), Default );
		auto&	groups = _hitGroups[ instanceIndex.value ];

		groups.resize( Max( groups.size(), rayIndex.value+1 ), Default );

		auto&	dst = groups[ rayIndex.value ];
		CHECK_THROW_MSG( not dst.IsDefined(),
			"TriangleHit shader with Ray("s << ToString(rayIndex.value) << "), Instance(" << ToString(instanceIndex.value) << ") is already set" );

		dst.closestHit	= _Shader{ closestHit, true };
		dst.anyHit		= _Shader{ anyHit, false };
		dst.name		= "Hit-r"s << ToString(rayIndex.value) << "-i" << ToString(instanceIndex.value);
	}

/*
=================================================
	SetProceduralHit*
=================================================
*/
	void  ScriptRayTracingPass::SetProceduralHit1 (const RTRayIndex &rayIndex, const RTInstanceIndex &instanceIndex,
												   const RTShader &intersection, const RTShader &closestHit) __Th___
	{
		SetProceduralHit2( rayIndex, instanceIndex, intersection, closestHit, RTShader() );
	}

	void  ScriptRayTracingPass::SetProceduralHit2 (const RTRayIndex &rayIndex, const RTInstanceIndex &instanceIndex,
												   const RTShader &intersection, const RTShader &closestHit, const RTShader &anyHit) __Th___
	{
		CHECK_THROW_MSG( intersection.isDefined );
		CHECK_THROW_MSG( closestHit.isDefined );
		CHECK_THROW_MSG( rayIndex.value < _maxRayTypes );

		_hitGroups.resize( Max( _hitGroups.size(), instanceIndex.value+1 ), Default );
		auto&	groups = _hitGroups[ instanceIndex.value ];

		groups.resize( Max( groups.size(), rayIndex.value+1 ), Default );

		auto&	dst = groups[ rayIndex.value ];
		CHECK_THROW_MSG( not dst.IsDefined(),
			"ProceduralHit shader with Ray("s << ToString(rayIndex.value) << "), Instance(" << ToString(instanceIndex.value) << ") is already set" );

		dst.closestHit	 = _Shader{ closestHit, true };
		dst.anyHit		 = _Shader{ anyHit, false };
		dst.intersection = _Shader{ intersection, true };
		dst.name		 = "Hit-r"s << ToString(rayIndex.value) << "-i" << ToString(instanceIndex.value);
	}

/*
=================================================
	SetMaxRayRecursion
=================================================
*/
	void  ScriptRayTracingPass::SetMaxRayRecursion1 (uint value) __Th___
	{
		SetMaxRayRecursion2( ScriptDynamicUIntPtr{new ScriptDynamicUInt{ MakeRC<DynamicUInt>( value )}} );
	}

	void  ScriptRayTracingPass::SetMaxRayRecursion2 (const ScriptDynamicUIntPtr &value) __Th___
	{
		CHECK_THROW_MSG( value and value->Get() );
		CHECK_THROW_MSG( not _maxRayRecursion, "'SetMaxRayRecursion()' already used" );

		_maxRayRecursion = value;
	}

/*
=================================================
	SetMaxCallableRecursion
=================================================
*/
	void  ScriptRayTracingPass::SetMaxCallableRecursion1 (uint value) __Th___
	{
		SetMaxCallableRecursion2( ScriptDynamicUIntPtr{new ScriptDynamicUInt{ MakeRC<DynamicUInt>( value )}} );
	}

	void  ScriptRayTracingPass::SetMaxCallableRecursion2 (const ScriptDynamicUIntPtr &value) __Th___
	{
		CHECK_THROW_MSG( value and value->Get() );
		CHECK_THROW_MSG( not _maxCallRecursion, "'SetMaxCallableRecursion()' already used" );

		_maxCallRecursion = value;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRayTracingPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace AE::Scripting;

		{
			ClassBinder<RTInstanceIndex>	binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &RTInstanceIndex_Ctor, {} );
		}{
			ClassBinder<RTRayIndex>			binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &RTRayIndex_Ctor, {} );
		}{
			ClassBinder<RTCallableIndex>	binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &RTCallableIndex_Ctor, {} );
		}{
			ClassBinder<RTShader>	binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &RTShader_Ctor1, {"filename"} );
			binder.AddConstructor( &RTShader_Ctor2, {"filename", "defines"} );
		}{
			ClassBinder<ScriptRayTracingPass>	binder{ se };
			binder.CreateRef( 0, False{"no ctor"} );
			ScriptBasePass::_BindBase( binder, True{"withArgs"} );

			binder.AddFactoryCtor( &ScriptRayTracingPass_Ctor1,		{} );
			binder.AddFactoryCtor( &ScriptRayTracingPass_Ctor2,		{"defines"} );

			binder.Comment( "Run RayGen shader with specified number of threads." );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads1,			"Dispatch",			{"threadsX"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads2,			"Dispatch",			{"threadsX", "threadsY"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads3,			"Dispatch",			{"threadsX", "threadsY", "threadsZ"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads2v,			"Dispatch",			{"threads"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads3v,			"Dispatch",			{"threads"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreadsDS,			"Dispatch",			{"dynamicThreadCount"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreads1D,			"Dispatch",			{"dynamicThreadCount"} );

			binder.Comment( "Run RayGen shader with number of threads from indirect command." );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreadsIndirect1,	"DispatchIndirect",	{"indirectBuffer"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreadsIndirect2,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferOffset"} );
			binder.AddMethod( &ScriptRayTracingPass::DispatchThreadsIndirect3,	"DispatchIndirect",	{"indirectBuffer", "indirectBufferFieldName"} );

			binder.AddMethod( &ScriptRayTracingPass::SetRayGen,					"RayGen",			{} );
			binder.AddMethod( &ScriptRayTracingPass::MaxRayTypes,				"MaxRayTypes",		{} );
			binder.AddMethod( &ScriptRayTracingPass::SetRayMiss,				"RayMiss",			{"missIndex", "missShader"} );
			binder.AddMethod( &ScriptRayTracingPass::SetCallable,				"Callable",			{"callableIndex", "callableShader"} );
			binder.AddMethod( &ScriptRayTracingPass::SetTriangleHit1,			"TriangleHit",		{"rayIndex", "instanceIndex", "closestHit"} );
			binder.AddMethod( &ScriptRayTracingPass::SetTriangleHit2,			"TriangleHit",		{"rayIndex", "instanceIndex", "closestHit", "anyHit"} );
			binder.AddMethod( &ScriptRayTracingPass::SetProceduralHit1,			"ProceduralHit",	{"rayIndex", "instanceIndex", "intersection", "closestHit"} );
			binder.AddMethod( &ScriptRayTracingPass::SetProceduralHit2,			"ProceduralHit",	{"rayIndex", "instanceIndex", "intersection", "closestHit", "anyHit"} );

			binder.AddMethod( &ScriptRayTracingPass::SetMaxRayRecursion1,		"MaxRayRecursion",		{} );
			binder.AddMethod( &ScriptRayTracingPass::SetMaxRayRecursion2,		"MaxRayRecursion",		{} );
			binder.AddMethod( &ScriptRayTracingPass::SetMaxCallableRecursion1,	"MaxCallableRecursion",	{} );
			binder.AddMethod( &ScriptRayTracingPass::SetMaxCallableRecursion2,	"MaxCallableRecursion",	{} );
		}
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptRayTracingPass::_CompilePipeline (OUT Bytes &ubSize) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this, &ubSize] (ScriptEnginePtr) {
						_CompilePipeline2( OUT ubSize );	// throw
					});
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptRayTracingPass::ToPass () C_Th___
	{
		CHECK_THROW_MSG( not _iterations.empty(), "add at least one Dispatch() call" );
		CHECK_THROW_MSG( not _args.Empty(), "empty argument list" );
		CHECK_THROW_MSG( _rayGen.IsDefined(), "RayGen shader must be defined" );

		auto		result		= MakeRC<RayTracingPass>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();
		Bytes		ub_size;

		// validate
		if ( not _hitGroups.empty() )
		{
			for (auto& arg : _args.Args())
			{
				if ( auto* rt_scene = UnionGet<ScriptRTScenePtr>( arg.res ))
				{
					CHECK_THROW_MSG( _hitGroups.size() == (*rt_scene)->GetInstanceCount(),
						"Number of HitGroups ("s << ToString(_hitGroups.size()) << ") must equal to InstanceCount (" <<
						ToString((*rt_scene)->GetInstanceCount()) << ") in RTScene '" << (*rt_scene)->GetName() <<
						"' in arg '" << arg.name << "'" );

					CHECK_THROW_MSG( _maxRayTypes == (*rt_scene)->GetMaxRayTypes(),
						"MaxRayTypes ("s << ToString(_maxRayTypes) << ") must equal to MaxRayTypes (" <<
						ToString((*rt_scene)->GetMaxRayTypes()) << ") in RTScene '" << (*rt_scene)->GetName() <<
						"' in arg '" << arg.name << "'" );
				}
			}
		}

		result->_rtech = _CompilePipeline( OUT ub_size );	// throw

		EnumSet<IPass::EDebugMode>	dbg_modes;

		const auto	AddPpln = [this, cp = result.get(), &dbg_modes]
								(IPass::EDebugMode mode, EFlags flag, PipelineName::Ref pplnName, RTShaderBindingName::Ref sbtName)
		{{
			if ( AllBits( _baseFlags, flag ))
			{
				auto	ppln	= cp->_rtech.rtech->GetRayTracingPipeline( pplnName );
				auto	sbt		= cp->_rtech.rtech->GetRTShaderBinding( sbtName );
				ASSERT( bool{ppln} == bool{sbt} );

				if ( ppln and sbt )
				{
					cp->_pipelines.insert_or_assign( mode, Tuple{ ppln, sbt });
					dbg_modes.insert( mode );
				}
			}
		}};

		AddPpln( IPass::EDebugMode::Unknown,		EFlags::Unknown,				PipelineName{"raytrace"},			RTShaderBindingName{"raytrace.sbt"} );
		AddPpln( IPass::EDebugMode::Trace,			EFlags::Enable_ShaderTrace,		PipelineName{"raytrace.Trace"},		RTShaderBindingName{"raytrace.Trace.sbt"} );
		AddPpln( IPass::EDebugMode::FnProfiling,	EFlags::Enable_ShaderFnProf,	PipelineName{"raytrace.FnProf"},	RTShaderBindingName{"raytrace.FnProf.sbt"} );
		AddPpln( IPass::EDebugMode::TimeHeatMap,	EFlags::Enable_ShaderTmProf,	PipelineName{"raytrace.TmProf"},	RTShaderBindingName{"raytrace.TmProf.sbt"} );

		auto	ppln = result->_pipelines.find( IPass::EDebugMode::Unknown )->second.Get<0>();

		#if PIPELINE_STATISTICS
		{
			auto&	res = res_mngr.GetResourcesOrThrow( ppln );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		result->_iterations.assign( this->_iterations.begin(), this->_iterations.end() );

		result->_maxRayRecursion	= this->_maxRayRecursion  ? this->_maxRayRecursion->Get()  : null;
		result->_maxCallRecursion	= this->_maxCallRecursion ? this->_maxCallRecursion->Get() : null;

		result->_ubuffer = _CreateUBuffer( ub_size, "RayTracingPassUB", EResourceState::UniformRead | EResourceState::RayTracingShaders );  // throw

		// create descriptor set
		{
			CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_dsIndex, OUT result->_descSets.data(), max_frames,
														ppln, DescriptorSetName{"ds0"}, null, _dbgName ));
			_args.InitResources( OUT result->_resources, result->_rtech.packId );  // throw
		}

		{
			Bytes	ray_gen_stack_max;
			Bytes	closest_hit_stack_max;
			Bytes	miss_stack_max;
			Bytes	intersection_stack_max;
			Bytes	any_hit_stack_max;
			Bytes	callable_stack_max;

			Array<RayTracingGroupName>	names;
			Array<RayTracingGroupName>	names1;
			Array<RayTracingGroupName>	names2;

			for (auto [mode, ppln_sbt] : result->_pipelines)
			{
				auto*	res = res_mngr.GetResource( ppln_sbt.Get<0>() );

				AssignMax( INOUT ray_gen_stack_max, res->GetShaderGroupStackSize( res_mngr.GetDevice(), {RayTracingGroupName{_rayGen.name}}, ERTShaderGroup::General ));

				names.clear();
				for (auto& miss : _missShaders) {
					names.push_back( RayTracingGroupName{ miss.name });
				}
				AssignMax( INOUT miss_stack_max, res->GetShaderGroupStackSize( res_mngr.GetDevice(), names, ERTShaderGroup::General ));

				names.clear();
				for (auto& call : _callableShaders) {
					names.push_back( RayTracingGroupName{ call.name });
				}
				AssignMax( INOUT callable_stack_max, res->GetShaderGroupStackSize( res_mngr.GetDevice(), names, ERTShaderGroup::General ));

				names.clear();
				names1.clear();
				names2.clear();

				for (auto& per_inst : _hitGroups)
				{
					for (auto& hit : per_inst)
					{
						if ( hit.closestHit.isDefined )
							names.push_back( RayTracingGroupName{ hit.name });

						if ( hit.anyHit.isDefined )
							names1.push_back( RayTracingGroupName{ hit.name });

						if ( hit.intersection.isDefined )
							names2.push_back( RayTracingGroupName{ hit.name });
					}
				}

				AssignMax( INOUT closest_hit_stack_max,		res->GetShaderGroupStackSize( res_mngr.GetDevice(), names, ERTShaderGroup::ClosestHit ));
				AssignMax( INOUT any_hit_stack_max,			res->GetShaderGroupStackSize( res_mngr.GetDevice(), names1, ERTShaderGroup::AnyHit ));
				AssignMax( INOUT intersection_stack_max,	res->GetShaderGroupStackSize( res_mngr.GetDevice(), names2, ERTShaderGroup::Intersection ));
			}

			result->_rayGenStackMax			= ray_gen_stack_max;
			result->_closestHitStackMax		= closest_hit_stack_max;
			result->_missStackMax			= miss_stack_max;
			result->_intersectionStackMax	= intersection_stack_max;
			result->_anyHitStackMax			= any_hit_stack_max;
			result->_callableStackMax		= callable_stack_max;
		}

		_Init( *result, null );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::AllRayTracing );

		return result;
	}


} // AE::ResEditor


#include "res_editor/Scripting/PipelineCompiler.inl.h"

#include "base/DataSource/File.h"
#include "base/Algorithms/Parser.h"

#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"

namespace AE::ResEditor
{
	using namespace AE::PipelineCompiler;

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptRayTracingPass::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "RayTracingPassUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"RayTracingPassUB"}};
		st->Set( EStructLayout::Std140, R"#(
				float		time;			// shader playback time (in seconds)
				float		timeDelta;		// frame render time (in seconds), max value: 1/30s
				uint		frame;			// shader playback frame, global frame counter
				uint		passFrameId;	// current pass frame index
				uint		seed;			// unique value, updated on each shader reloading
				float4		mouse;			// mouse unorm coords. xy: current (if MRB down), zw: click
				float		customKeys;

				// controller //
				CameraData	camera;

				// sliders //
				float4		floatSliders [8];
				int4		intSliders [8];
				float4		colors [8];

				// constants //
				float4		floatConst [8];
				int4		intConst [8];
			)#");

		StaticAssert( UIInteraction::MaxSlidersPerType == 8 );
		StaticAssert( IPass::Constants::MaxCount == 8 );
		StaticAssert( IPass::CustomKeys_t{}.max_size() == 1 );
		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptRayTracingPass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	_CompilePipeline2
=================================================
*/
	void  ScriptRayTracingPass::_CompilePipeline2 (OUT Bytes &ubSize) C_Th___
	{
		_args.ValidateArgs();

		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}

		const auto				stage	= EShaderStages::AllRayTracing;
		DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
		{
			ShaderStructTypePtr	st = _CreateUBType();	// throw
			ubSize = st->StaticSize();

			ds_layout->AddUniformBuffer( stage, "un_PerPass", ArraySize{1}, "RayTracingPassUB", EResourceState::ShaderUniform, False{} );
		}
		_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw


		String	header;
		_AddDefines( _defines, INOUT header );
		_AddSliders( INOUT header );

		EShaderOpt		sh_opt	 = Default;		//EShaderOpt::DebugInfo;	// for shader debugging in RenderDoc
		EPipelineOpt	ppln_opt = Default;

	  #if OPTIMIZE_SHADER
		sh_opt   = EShaderOpt::Optimize;
		ppln_opt |= EPipelineOpt::Optimize;
	  #endif
	  #if PIPELINE_STATISTICS
		ppln_opt |= EPipelineOpt::CaptureStatistics | EPipelineOpt::CaptureInternalRepresentation;
	  #endif

		_CompilePipeline3( header, "raytrace", uint(sh_opt), ppln_opt );

	  #ifdef AE_ENABLE_GLSL_TRACE
		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTrace ))
			NOTHROW( _CompilePipeline3( header, "raytrace.Trace", uint(sh_opt | EShaderOpt::Trace), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderFnProf ))
			NOTHROW( _CompilePipeline3( header, "raytrace.FnProf", uint(sh_opt | EShaderOpt::FnProfiling), Default ));

		if ( AllBits( _baseFlags, EFlags::Enable_ShaderTmProf ))
			NOTHROW( _CompilePipeline3( header, "raytrace.TmProf", uint(sh_opt | EShaderOpt::TimeHeatMap), Default ));
	  #endif
	}

/*
=================================================
	_CompilePipeline3
=================================================
*/
	void  ScriptRayTracingPass::_CompilePipeline3 (const String &header, const String &pplnName, uint shaderOpts, EPipelineOpt pplnOpt) C_Th___
	{
		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ pplnName + ".pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

		if ( AnyBits( EShaderOpt(shaderOpts), EShaderOpt::_ShaderTrace_Mask ))
			ppln_layout->AddDebugDSLayout2( 1, EShaderOpt(shaderOpts) & EShaderOpt::_ShaderTrace_Mask, uint(EShaderStages::AllRayTracing) );

		RayTracingPipelinePtr	ppln_templ{ new RayTracingPipelineScriptBinding{ pplnName }};
		ppln_templ->Disable();
		ppln_templ->SetLayout2( ppln_layout );

		const auto	CreateShader = [shaderOpts, &header] (const _Shader &sh, EShader type) -> ScriptShaderPtr
		{{
			if ( not sh.isDefined )
				return null;

			String	src;
			String	hdr = header;
			_AddDefines( sh.defines, INOUT hdr );

			const uint	lines = uint(Parser::CalculateNumberOfLines( hdr )) - 1;

			// load shader source from file
			{
				FileRStream		file {sh.shaderPath};
				CHECK_THROW_MSG( file.IsOpen(),
					"Failed to open shader file: '"s << ToString(sh.shaderPath) << "'" );

				CHECK_THROW_MSG( file.Read( file.RemainingSize(), OUT src ),
					"Failed to read shader file '"s << ToString(sh.shaderPath) << "'" );

				hdr >> src;
			}

			ScriptShaderPtr	shader{ new ScriptShader{}};
			shader->SetSource2( type, src, PathAndLine{sh.shaderPath, lines} );
			shader->options = EShaderOpt(shaderOpts);

			return shader;
		}};

		// compile shaders
		{
			ppln_templ->AddGeneralShader( _rayGen.name, CreateShader( _rayGen.shader, EShader::RayGen ));

			for (auto& miss : _missShaders) {
				if ( miss.IsDefined() )
					ppln_templ->AddGeneralShader( miss.name, CreateShader( miss.shader, EShader::RayMiss ));
			}

			for (auto& call : _callableShaders) {
				if ( call.IsDefined() )
					ppln_templ->AddGeneralShader( call.name, CreateShader( call.shader, EShader::RayCallable ));
			}

			for (auto& per_inst : _hitGroups) {
				for (auto& group : per_inst) {
					if ( group.IsDefined() ) {
						if ( group.intersection.isDefined ) {
							ppln_templ->AddProceduralHitGroup( group.name,
															   CreateShader( group.intersection, EShader::RayIntersection ),
															   CreateShader( group.closestHit,	 EShader::RayClosestHit ),
															   CreateShader( group.anyHit,		 EShader::RayAnyHit ));
						} else {
							ppln_templ->AddTriangleHitGroup( group.name,
															 CreateShader( group.closestHit,	 EShader::RayClosestHit ),
															 CreateShader( group.anyHit,		 EShader::RayAnyHit ));
						}
					}
				}
			}
		}

		// specialization
		{
			const uint	max_recursion = GraphicsScheduler().GetDevice().GetDeviceProperties().rayTracing.maxRecursion;

			RayTracingPipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( pplnName );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", "Compute" );
			ppln_spec->SetOptions( pplnOpt );
			ppln_spec->MaxRecursionDepth( max_recursion );
			ppln_spec->SetDynamicState( uint(EPipelineDynamicState::RTStackSize) );

			// create SBT
			RayTracingShaderBindingPtr	sbt{ new RayTracingShaderBinding{ ppln_spec, pplnName + ".sbt" }};

			if ( _maxRayTypes > 0 )
				sbt->MaxRayTypes( _maxRayTypes );

			sbt->BindRayGen( "Main" );

			for (const auto [miss, i] : WithIndex( _missShaders )) {
				if ( miss.IsDefined() )
					sbt->BindMiss( miss.name, RayIndex(uint(i)) );
			}

			for (const auto [call, i] : WithIndex( _callableShaders )) {
				if ( call.IsDefined() )
					sbt->BindCallable( call.name, CallableIndex(uint(i)) );
			}

			for (const auto [per_inst, inst] : WithIndex( _hitGroups )) {
				for (const auto [group, ray] : WithIndex( per_inst )) {
					if ( group.IsDefined() )
						sbt->BindHitGroup( group.name, InstanceIndex(uint(inst)), RayIndex(uint(ray)) );
				}
			}

			// if successfully compiled
			ppln_spec->Enable();
		}
	}


} // AE::ResEditor
