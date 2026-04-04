// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/ScriptObjects/IndirectExecutionSet.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static IndirectExecutionSet*  IndirectExecutionSet_Ctor (const String &name) {
		return IndirectExecutionSet::Create( name ).Detach();
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	IndirectExecutionSet::IndirectExecutionSet (const String &name) __NE___ :
		_name{ IndirectExecutionSetName{name} }, _nameStr{ name }
	{}

	IndirectExecutionSetPtr  IndirectExecutionSet::Create (const String &name) __Th___
	{
		IndirectExecutionSetPtr	result{ new IndirectExecutionSet{ name }};

		ObjectStorage::Instance()->AddName<IndirectExecutionSetName>( name );

		return result;
	}

/*
=================================================
	destructor
=================================================
*/
	IndirectExecutionSet::~IndirectExecutionSet ()
	{
		CHECK_MSG( not _rtechPasses.empty(),
			"IndirectExecutionSet is not attached to any Render technique" );
	}

/*
=================================================
	_AddToRenderTech
=================================================
*/
	void  IndirectExecutionSet::_AddToRenderTech (const String &rtechName, const String &passName) __Th___
	{
		const auto&	rtech_map = ObjectStorage::Instance()->rtechMap;

		CHECK_THROW_MSG( _state == EState::Initial,
			"Can not add to RenderTechnique when 'AddPipeline()' is used or when IndirectExecutionSet is already packed" );

		auto	rt_iter = rtech_map.find( rtechName );
		CHECK_THROW_MSG( rt_iter != rtech_map.end(),
			"Render technique '"s << rtechName << "' is not exists" );

		auto	pass	= rt_iter->second->GetPass( passName );
		CHECK_THROW_MSG( pass,
			"Render technique pass '"s << passName << "' is not exists in render technique '" << rtechName << "'" );

		CHECK_THROW_MSG( _rtechPasses.insert( pass ).second,
			"Already attached to Render technique '"s << rtechName << "' pass '" << passName << "'" );

		auto	feats = rt_iter->second->GetFeatures();
		TEST_FEATURE_MSG( feats, deviceGeneratedCommands, " in RenderTechnique '"s << rtechName << "'" );

		EShaderStages	shader_stages	= Default;
		EShaderStages	pipe_binding	= Default;
		uint			pipe_count		= 0;

		for (auto& fs : feats)
		{
			if ( fs->fs.deviceGeneratedCommands != FeatureSet::EFeature::RequireTrue )
				continue;

			// indicates the stages which can be used to generate indirect commands.
			shader_stages |= fs->fs.supportedIndirectCommandsShaderStages;

			// indicates the stages which can be used within indirect execution sets for indirectly binding shader stages using pipelines.
			pipe_binding  |= fs->fs.supportedIndirectCommandsShaderStagesPipelineBinding;

			pipe_count = Max( pipe_count, fs->fs.maxIndirectPipelineCount );
		}

		_shaderStages	&= shader_stages;
		_pipeBinding	&= pipe_binding;
		_maxPipeCount	= Min( _maxPipeCount, pipe_count );

		CHECK_THROW_MSG( _shaderStages != Default and _pipeBinding != Default and _maxPipeCount != 0,
			"RenderTechnique '"s << rtechName << "' doesn't contains FeatureSet which is compatible with IndirectExecutionSet."
			"All RenderTechniques must have at least one same stage in 'supportedIndirectCommandsShaderStagesPipelineBinding' and 'supportedIndirectCommandsShaderStages',"
			"'maxIndirectPipelineCount' must not be zero." );

		pass->AddExecSet( IndirectExecutionSetPtr{this} );
	}

/*
=================================================
	AddPipeline
=================================================
*/
	void  IndirectExecutionSet::AddPipeline (const String &name) __Th___
	{
		CHECK_THROW_MSG( _state <= EState::AddPipelines,
			"Can not add pipeline when IndirectExecutionSet is already packed" );

		CHECK_THROW_MSG( not _rtechPasses.empty(),
			"IndirectExecutionSet must be attached to RenderTechnique Graphics/Compute/RayTracing pass" );

		_state = EState::AddPipelines;

		BasePipelineSpecPtr	ppln;

		for (auto& pass : _rtechPasses)
		{
			auto&	map = pass->GetPipelineMap();
			auto	it	= map.find( name );

			CHECK_THROW_MSG( it != map.end(),
				"Pipeline '"s << name << "' is not exists in RTech '" << pass->RenTech()->Name() << "' pass '" << pass->Name() << "'" );

			if ( not ppln ){
				ppln = it->second;
			}else{
				CHECK_THROW( ppln == it->second );
			}
		}

		// TODO: add option?
		CHECK_THROW_MSG( AllBits( ppln->Options(), EPipelineOpt::IndirectBindable ));

		CHECK_THROW_MSG( _pipelines.size() < _maxPipeCount,
			"Maximal supported pipeline count is ("s << ToString(_maxPipeCount) << ")" );

		EShaderStages	active_stages = ppln->GetBase()->GetStages();
		_usedStages |= active_stages;

		_pipelines.push_back( ppln );
	}

/*
=================================================
	_ValidatePipe2
=================================================
*/
	void  IndirectExecutionSet::_ValidatePipe2 (MeshPipelineSpecScriptBinding const& initial, MeshPipelineSpecScriptBinding const& pending) __Th___
	{
		CHECK_THROW( initial.desc.renderPass == pending.desc.renderPass );
		CHECK_THROW( initial.desc.subpass == pending.desc.subpass );
		CHECK_THROW( initial.desc.viewportCount == pending.desc.viewportCount );
		CHECK_THROW( AllBits( _pipeBinding, EShaderStages::Fragment ));
	}

	void  IndirectExecutionSet::_ValidatePipe2 (GraphicsPipelineSpecScriptBinding const& initial, GraphicsPipelineSpecScriptBinding const& pending) __Th___
	{
		CHECK_THROW( initial.desc.renderPass == pending.desc.renderPass );
		CHECK_THROW( initial.desc.subpass == pending.desc.subpass );
		CHECK_THROW( initial.desc.viewportCount == pending.desc.viewportCount );
		CHECK_THROW( initial.GetVertexBuffers() == pending.GetVertexBuffers() );
		CHECK_THROW( AllBits( _pipeBinding, EShaderStages::Fragment ));
	}

	void  IndirectExecutionSet::_ValidatePipe2 (ComputePipelineSpecScriptBinding const& initial, ComputePipelineSpecScriptBinding const& pending) __Th___
	{
		CHECK_THROW( AllBits( _pipeBinding, EShaderStages::Compute ));
	}

	void  IndirectExecutionSet::_ValidatePipe2 (RayTracingPipelineSpecScriptBinding const& initial, RayTracingPipelineSpecScriptBinding const& pending) __Th___
	{
		CHECK_THROW( AnyBits( _pipeBinding, EShaderStages::AllRayTracing ));
	}

/*
=================================================
	_ValidatePipe
=================================================
*/
	template <typename T>
	void  IndirectExecutionSet::_ValidatePipe (BasePipelineSpec const* initialBase, BasePipelineSpec const* pendingBase) __Th___
	{
		auto*	initial = DynCast< T >( initialBase );
		auto*	pending = DynCast< T >( pendingBase );

		CHECK_THROW( (initial != null) == (pending != null) );

		if ( not initial or not pending )
			return;

		CHECK_THROW( initial->IsBuilded() and pending->IsBuilded() );

		auto*	initial_tmpl	= initial->GetBase();
		auto*	pending_tmpl	= pending->GetBase();

		CHECK_THROW( initial_tmpl->IsEnabled() and pending_tmpl->IsEnabled() );

		auto	initial_pl		= initial_tmpl->GetLayout();
		auto	pending_pl		= pending_tmpl->GetLayout();

		CHECK_THROW( (initial_pl != null) == (pending_pl != null) );

		if ( initial_pl )
		{
			CHECK_THROW( initial_pl->GetLayoutID().has_value() );
			CHECK_THROW( pending_pl->GetLayoutID().has_value() );

			CHECK_THROW_MSG( *initial_pl->GetLayoutID() == *pending_pl->GetLayoutID(),
				"In IndirectExecutionSet '"s << _nameStr << "' pipeline '" << pending->NameStr() << "' layout '" <<
				pending_pl->Name() << "' doesn't match with initial pipeline '" << initial->NameStr() <<
				"' layout '" << initial_pl->Name() << "'." );
		}

		CHECK_THROW_MSG( initial_tmpl->GetStages() == pending_tmpl->GetStages(),
			"In IndirectExecutionSet '"s << _nameStr << "' pipeline '" << pending->NameStr() <<
			"' has stages (" << ToString( pending_tmpl->GetStages() ) << ") which is not match with initial pipeline '" <<
			initial->NameStr() << "' stages (" << ToString( initial_tmpl->GetStages() ) << ")." );

		CHECK_THROW_MSG( initial->desc.dynamicState == pending->desc.dynamicState,
			"In IndirectExecutionSet '"s << _nameStr << "' pipeline '" << pending->NameStr() <<
			"' has dynamic states (" << ToString( pending->desc.dynamicState ) << ") which is not match with initial pipeline '" <<
			initial->NameStr() << "' dynamic states (" << ToString( initial->desc.dynamicState ) << ")." );

		_ValidatePipe2( *initial, *pending );
	}

/*
=================================================
	_Validate
=================================================
*/
	void  IndirectExecutionSet::_Validate () __Th___
	{
		CHECK_THROW( _usedStages != Default );
		CHECK_THROW( AllBits( _pipeBinding, _usedStages ));

		auto*	initial = _pipelines.front().Get();
		for (auto& pipe : _pipelines)
		{
			if ( initial == pipe.Get() )
				continue;

			_ValidatePipe< MeshPipelineSpecScriptBinding >( initial, pipe.Get() );
			_ValidatePipe< ComputePipelineSpecScriptBinding >( initial, pipe.Get() );
			_ValidatePipe< GraphicsPipelineSpecScriptBinding >( initial, pipe.Get() );
			_ValidatePipe< RayTracingPipelineSpecScriptBinding >( initial, pipe.Get() );
		}
	}

/*
=================================================
	Build
=================================================
*/
	bool  IndirectExecutionSet::Build () __NE___
	{
		if ( _uid.has_value() )
			return true;

		_state = EState::Packing;

		CHECK_ERR( not _pipelines.empty() );
		NOTHROW_ERR( _Validate() );

		auto&	storage = *ObjectStorage::Instance();

		auto*	ppln_names = storage.allocator.Allocate< SerializableIndirectExecutionSet::Pipelines_t::value_type >( _pipelines.size() );
		CHECK_ERR( ppln_names != null );

		for (usize i = 0; i < _pipelines.size(); ++i) {
			ppln_names[i] = _pipelines[i]->Name();
		}

		SerializableIndirectExecutionSet	desc;
		desc.pipelines = SerializableIndirectExecutionSet::Pipelines_t{ ppln_names, _pipelines.size() };

		if ( AllBits( _usedStages, EShaderStages::Vertex ))
			desc.pipeType = PipelineSpecUID::Graphics;
		else
		if ( AllBits( _usedStages, EShaderStages::Mesh ))
			desc.pipeType = PipelineSpecUID::Mesh;
		else
		if ( _usedStages == EShaderStages::Compute )
			desc.pipeType = PipelineSpecUID::Compute;
		else
		if ( AnyBits( _usedStages, EShaderStages::AllRayTracing ))
			desc.pipeType = PipelineSpecUID::RayTracing;
		else
			RETURN_ERR( "unknown pipeline type" );

		_uid = storage.pplnStorage->AddIndirectExecutionSet( Name(), RVRef(desc) );
		return true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  IndirectExecutionSet::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<IndirectExecutionSet>	binder{ se };
		binder.CreateRef( 0, False{} );

		binder.Comment( "Create IndirectExecutionSet - array of compatible pipelines which can be bound from shader by they index in array.\n"
						"Name is used in C++ code get execution set." );
		binder.AddFactoryCtor( &IndirectExecutionSet_Ctor, {"name"} );

		binder.Comment(	"Add IndirectExecutionSet to RenderTechnique."
						"'AddPipeline()' method will search pipelines in all specified RenderTechnique passes." );
		AS_METHOD( binder,	IndirectExecutionSet::_AddToRenderTech,		"AddToRenderTech",	{"renTech", "pass"} );

		binder.Comment( "Add pipeline to execution set.\n"
						"Pipeline must be compatible with first pipeline:\n"
						" * same pipeline layout\n"
						" * same render pass\n"		// "identically defined fragment outputs interface" in specs
						" * same dynamic states\n"	// "must have identically defined static and dynamic state values" in specs (?)
						" * same shader stages\n"
						" * same use of FragDepth, SampleMask, StencilExport" );
		AS_METHOD( binder,	IndirectExecutionSet::AddPipeline,			"AddPipeline",		{} );
	}

/*
=================================================
	ToGlslCppHeader
----
	generate header for C++ and GLSL
=================================================
*/
	String  IndirectExecutionSet::ToGlslCppHeader () C_Th___
	{
		const auto	ValidateName = [] (StringView name) -> String
		{{
			return Parser::ValidateVarName_CPP( name );
		}};

		CHECK_ERR( not _pipelines.empty() );

		String			str, glsl;
		const String	name	= ValidateName( _nameStr );

		str << "#ifdef __cplusplus\n";
		str << "namespace IES {\n";
		str << "\t\tstatic constexpr struct {\n";

		uint	idx = 0;
		for (auto& pipe : _pipelines)
		{
			String	pipe_name	= ValidateName( pipe->NameStr() );
			String	idx_str		= ToString( idx );

			str << "\t\t\tstatic constexpr uint  " << pipe_name << " = " << idx_str << ";\n";
			glsl << "\tconst uint  IES_" << name << "_" << pipe_name << " = " << idx_str << ";\n";
			++idx;
		}

		str << "\t\t}  " << name << ";\n";
		str << "} // IES\n\n";
		str << "#endif // __cplusplus\n\n";

		str << "#if defined(VULKAN) || defined(GL_SPIRV)\n";
		str << glsl;
		str << "#endif // GLSL\n\n";

		// TODO: msl

		return str;
	}


} // AE::PipelineCompiler
