// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/GraphicsPipeline.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static GraphicsPipelineScriptBinding*  GraphicsPipelineScriptBinding_Ctor (const String &name) {
		return GraphicsPipelinePtr{ new GraphicsPipelineScriptBinding{ name }}.Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	GraphicsPipelineScriptBinding::GraphicsPipelineScriptBinding (const String &name) __Th___ : BasePipelineTmpl{name}
	{
		CHECK_THROW_MSG( ObjectStorage::Instance()->gpipelines.emplace( _name, GraphicsPipelinePtr{this} ).second,
			"GraphicsPipeline with name '"s << name << "' is already defined" );
	}

	GraphicsPipelineScriptBinding::GraphicsPipelineScriptBinding () :
		GraphicsPipelineScriptBinding{ "<unknown>" }
	{}

/*
=================================================
	SetShaderIO
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetShaderIO (EShader output, EShader input, const String &typeName) __Th___
	{
		CHECK_THROW_MSG( AnyBits( EShaderStages::GraphicsStages, (EShaderStages{0} | output) ));
		CHECK_THROW_MSG( AnyBits( EShaderStages::GraphicsStages, (EShaderStages{0} | input) ));

		CHECK_THROW_MSG( not _shaderIO[output].second, String{ToString(output)} << " shader input is already defined" );
		CHECK_THROW_MSG( not _shaderIO[input].first, String{ToString(input)} << " shader input is already defined" );

		auto&	struct_types = ObjectStorage::Instance()->structTypes;
		auto	st_it = struct_types.find( typeName );
		CHECK_THROW_MSG( st_it != struct_types.end(),
			"StructureType with name '"s << typeName << "' is not found" );

		if ( st_it->second->Layout() != EStructLayout::InternalIO )
			AE_LOGW( "StructureType '"s << typeName << "' should use 'InternalIO' layout" );

		_shaderIO[input].first		= st_it->second;
		_shaderIO[output].second	= st_it->second;
	}

/*
=================================================
	_GetShaderInput
=================================================
*/
	ShaderStructTypePtr  GraphicsPipelineScriptBinding::_GetShaderInput (EShader shader) const
	{
		auto	io_it = _shaderIO.find( shader );
		return	io_it != _shaderIO.end() ? io_it->second.first : Default;
	}

/*
=================================================
	_GetShaderOutput
=================================================
*/
	ShaderStructTypePtr  GraphicsPipelineScriptBinding::_GetShaderOutput (EShader shader) const
	{
		auto	io_it = _shaderIO.find( shader );
		return	io_it != _shaderIO.end() ? io_it->second.second : Default;
	}

/*
=================================================
	SetVertexShader
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetVertexShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Vertex, Default ));

		inShader->type = EShader::Vertex;
		CHECK_THROW_MSG( _GetShaderInput( inShader->type ) == Default );

		_CompileShader( INOUT vertex, inShader, Default, _GetShaderOutput( inShader->type ), NullOptional, _vertexBuffers );
		_stages |= inShader->type;
	}

/*
=================================================
	SetTessControlShader
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetTessControlShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::TessControl, Default ));
		inShader->type = EShader::TessControl;

		TEST_FEATURE( GetFeatures(), tessellationShader );

		_CompileShader( INOUT tessControl, inShader, _GetShaderInput( inShader->type ), _GetShaderOutput( inShader->type ));
		_stages |= inShader->type;
	}

/*
=================================================
	SetTessEvalShader
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetTessEvalShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::TessEvaluation, Default ));
		inShader->type = EShader::TessEvaluation;

		TEST_FEATURE( GetFeatures(), tessellationShader );

		_CompileShader( INOUT tessEval, inShader, _GetShaderInput( inShader->type ), _GetShaderOutput( inShader->type ));
		_stages |= inShader->type;
	}

/*
=================================================
	SetGeometryShader
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetGeometryShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Geometry, Default ));
		inShader->type = EShader::Geometry;

		TEST_FEATURE( GetFeatures(), geometryShader );

		_CompileShader( INOUT geometry, inShader, _GetShaderInput( inShader->type ), _GetShaderOutput( inShader->type ));
		_stages |= inShader->type;
	}

/*
=================================================
	SetFragmentShader
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetFragmentShader (const ScriptShaderPtr &inShader) __Th___
	{
		CHECK_THROW_MSG( inShader );
		CHECK_THROW_MSG( AnyEqual( inShader->type, EShader::Fragment, Default ));

		inShader->type = EShader::Fragment;
		CHECK_THROW_MSG( _GetShaderOutput( inShader->type ) == Default );

		_CompileShader( INOUT fragment, inShader, _GetShaderInput( inShader->type ), Default, _fragOutput );
		_stages |= inShader->type;
	}

/*
=================================================
	TestRenderPass
=================================================
*/
	void  GraphicsPipelineScriptBinding::TestRenderPass (const String &compatRP, const String &subpass) __Th___
	{
		SubpassShaderIO		frag_io;
		GetSubpassShaderIO( OUT frag_io );

		ObjectStorage::Instance()->TestRenderPass( compatRP, subpass, frag_io, false, false );  // throw
	}

/*
=================================================
	AddSpecialization
=================================================
*/
	GraphicsPipelineSpecPtr  GraphicsPipelineScriptBinding::AddSpecialization2 (const String &name) __Th___
	{
		_Prepare();

		CHECK_THROW_MSG( _isPrepared );
		_OnAddSpecialization();

		return _pplnSpec.emplace_back( GraphicsPipelineSpecPtr{new GraphicsPipelineSpecScriptBinding{ this, name }} );
	}

	GraphicsPipelineSpecScriptBinding*  GraphicsPipelineScriptBinding::AddSpecialization (const String &name) __Th___
	{
		return AddSpecialization2( name ).Detach();
	}

/*
=================================================
	ValidatePrimitiveTopology
=================================================
*/
	static void  ValidatePrimitiveTopology (INOUT SerializableGraphicsPipeline::TopologyBits_t &topology)
	{
		if ( topology.contains( EPrimitive::Patch ))
		{
			topology.clear().insert( EPrimitive::Patch );
			return;
		}

		if ( topology.contains( EPrimitive::TriangleListAdjacency ) or
			 topology.contains( EPrimitive::TriangleStripAdjacency ))
		{
			topology.clear()
				.insert( EPrimitive::TriangleListAdjacency )
				.insert( EPrimitive::TriangleStripAdjacency );
			return;
		}

		if ( topology.contains( EPrimitive::LineListAdjacency ) or
			 topology.contains( EPrimitive::LineStripAdjacency ))
		{
			topology.clear()
				.insert( EPrimitive::LineListAdjacency )
				.insert( EPrimitive::LineStripAdjacency );
			return;
		}

		if ( topology.None() )
		{
			topology.clear()
				.insert( EPrimitive::Point )
				.insert( EPrimitive::LineList )
				.insert( EPrimitive::LineStrip )
				.insert( EPrimitive::TriangleList )
				.insert( EPrimitive::TriangleStrip )
				.insert( EPrimitive::TriangleFan );
			return;
		}
	}

/*
=================================================
	Build
=================================================
*/
	bool  GraphicsPipelineScriptBinding::Build () __NE___
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
	void  GraphicsPipelineScriptBinding::_Prepare () __Th___
	{
		if ( _pipelineUID.has_value() )
			return;

		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( vertex );
		CHECK_THROW_MSG( fragment );

		CHECK_THROW_MSG( _AddLayout( vertex		 ));
		CHECK_THROW_MSG( _AddLayout( tessControl ));
		CHECK_THROW_MSG( _AddLayout( tessEval	 ));
		CHECK_THROW_MSG( _AddLayout( geometry	 ));
		CHECK_THROW_MSG( _AddLayout( fragment	 ));
		CHECK_THROW_MSG( _BuildLayout() );

		SerializableGraphicsPipeline	desc;

		const bool	is_metal = AnyEqual( storage.target, ECompilationTarget::Metal_Mac, ECompilationTarget::Metal_iOS );

		desc.layout = _GetLayout();
		CHECK_THROW_MSG( desc.layout != Default );

		if ( vertex )
		{
			const auto&	va = vertex->reflection.vertex.vertexAttribs;
			CHECK_THROW_MSG( is_metal or (va.empty() == (_vertexBuffers == Default)) );

			if ( _vertexBuffers )
			{
				CHECK_THROW_MSG( is_metal or (va.size() == _vertexBuffers->GlslLocations()) );
				_vertexBuffers->Get( OUT desc.vertexAttribs );
			}

			desc.supportedTopology	= vertex->reflection.vertex.supportedTopology;
			CHECK( desc.shaders.insert_or_assign( EShader::Vertex, vertex->uid ).second );
		}

		if ( tessControl )
		{
			desc.patchControlPoints	= tessControl->reflection.tessellation.patchControlPoints;
			CHECK( desc.shaders.insert_or_assign( EShader::TessControl, tessControl->uid ).second );

			desc.supportedTopology |= tessControl->reflection.vertex.supportedTopology;	// TODO: check
		}

		if ( tessEval )
		{
			CHECK( desc.shaders.insert_or_assign( EShader::TessEvaluation, tessEval->uid ).second );
		}

		if ( geometry )
		{
			CHECK( desc.shaders.insert_or_assign( EShader::Geometry, geometry->uid ).second );

			desc.supportedTopology |= geometry->reflection.vertex.supportedTopology;	// TODO: check
		}

		if ( fragment )
		{
			desc.earlyFragmentTests	= fragment->reflection.fragment.earlyFragmentTests;
			CHECK( desc.shaders.insert_or_assign( EShader::Fragment, fragment->uid ).second );
		}

		ValidatePrimitiveTopology( INOUT desc.supportedTopology );

		desc.features = storage.CopyFeatures( _features );

		auto [uid, is_unique] = storage.pplnStorage->AddPipeline( _name, RVRef(desc) );

		CHECK_MSG( is_unique,
					"Pipeline name '"s << GetName() << "' is already used and will be overriden" );

		_pipelineUID	= uid;
		_isPrepared		= true;
	}

/*
=================================================
	SetFragmentOutputFromRenderTech
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetFragmentOutputFromRenderTech (const String &renTechName, const String &passName) __Th___
	{
		CHECK_THROW_MSG( not _fragOutput.has_value() );
		CHECK_THROW_MSG( not AllBits( _states, EStateBits::HasShaders ));

		_fragOutput = FragOutput_t{};
		CHECK_THROW_MSG( BasePipelineTmpl::_FragmentOutputFromRenderTech( renTechName, passName, OUT *_fragOutput ));
	}

/*
=================================================
	SetFragmentOutputFromRenderPass
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetFragmentOutputFromRenderPass (const String &compatRPassName, const String &subpassName) __Th___
	{
		CHECK_THROW_MSG( not _fragOutput.has_value() );
		CHECK_THROW_MSG( not AllBits( _states, EStateBits::HasShaders ));

		_fragOutput = FragOutput_t{};
		CHECK_THROW_MSG( BasePipelineTmpl::_FragmentOutputFromCompatRenderPass( compatRPassName, subpassName, OUT *_fragOutput ));
	}

/*
=================================================
	SetVertexInput
=================================================
*/
	void  GraphicsPipelineScriptBinding::SetVertexInput (const String &name) __Th___
	{
		const auto&	map = ObjectStorage::Instance()->vbInputMap;

		auto	iter = map.find( name );
		CHECK_THROW_MSG( iter != map.end(),
			"VertexInput '"s << name << "' is not exists" );

		_vertexBuffers = iter->second;
	}

	void  GraphicsPipelineScriptBinding::SetVertexInput2 (const VertexBufferInputPtr &ptr) __Th___
	{
		CHECK_THROW_MSG( ptr );
		_vertexBuffers = ptr;
	}

/*
=================================================
	GetSubpassShaderIO
=================================================
*/
	void  GraphicsPipelineScriptBinding::GetSubpassShaderIO (OUT SubpassShaderIO &fragIO) C_Th___
	{
		if ( _fragOutput.has_value() )
		{
			CHECK_THROW_MSG( _FragOutToSubpassShaderIO( *_fragOutput, OUT fragIO ));
			return;
		}

		CHECK_THROW_MSG( fragment );
		fragIO = fragment->reflection.fragment.fragmentIO;
	}

/*
=================================================
	Bind
=================================================
*/
	void  GraphicsPipelineScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<GraphicsPipelineScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Create pipeline template.\n"
						"Name is used in C++ code to create pipeline." );
		binder.AddFactoryCtor( &GraphicsPipelineScriptBinding_Ctor, {"name"} );

		binder.Comment( "Add macros which will be used in shader.\n"
						"Format: MACROS = value; DEF" );
		binder.AddMethod( &GraphicsPipelineScriptBinding::Define,				"Define",				{} );

		binder.Comment( "Set vertex shader.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetVertexShader,		"SetVertexShader",		{} );

		binder.Comment( "Set tessellation control shader. This shader is optional.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetTessControlShader,	"SetTessControlShader",	{} );

		binder.Comment( "Set tessellation evaluation shader. This shader is optional.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetTessEvalShader,	"SetTessEvalShader",	{} );

		binder.Comment( "Set geometry shader. This shader is optional.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetGeometryShader,	"SetGeometryShader",	{} );

		binder.Comment( "Set vertex shader.\n"
						"Pipeline will inherit shader feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetFragmentShader,	"SetFragmentShader",	{} );

		binder.Comment( "Create specialization for pipeline template.\n"
						"Name is used in C++ code to get pipeline from render technique.\n"
						"Pipeline specialization use the same pipeline layout, same shader binaries, compatible render pass, difference only in some parameters." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::AddSpecialization,	"AddSpecialization",	{"specName"} );

		binder.Comment( "Check is fragment shader compatible with render pass." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::TestRenderPass,		"TestRenderPass",		{"compatRP", "subpass"} );

		binder.Comment( "Add FeatureSet to the pipeline." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::AddFeatureSet,		"AddFeatureSet",		{"fsName"} );

		binder.Comment( "Set pipeline layout.\n"
						"Pipeline will inherit layout feature sets." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetLayout,			"SetLayout",			{"plName"} );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetLayout2,			"SetLayout",			{"pl"} );

		binder.Comment( "Set vertex default attributes." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetVertexInput,		"SetVertexInput",		{"vbName"} );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetVertexInput2,		"SetVertexInput",		{"vb"} );

		binder.Comment( "Set shader interface between stages." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetShaderIO,			"SetShaderIO",			{"output", "input", "typeName"} );

		binder.Comment( "Add fragment shader output variables from render technique graphics pass.\n"
						"Don't use it with explicit shader output." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetFragmentOutputFromRenderTech,	"SetFragmentOutputFromRenderTech", {"renTechName", "passName"} );

		binder.Comment( "Add fragment shader output variables from render pass.\n"
						"Don't use it with explicit shader output." );
		binder.AddMethod( &GraphicsPipelineScriptBinding::SetFragmentOutputFromRenderPass,	"SetFragmentOutputFromRenderPass", {"compatRP", "subpass"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	GraphicsPipelineSpecScriptBinding::GraphicsPipelineSpecScriptBinding (GraphicsPipelineScriptBinding* base, const String &name) __Th___ :
		BasePipelineSpec{ base, name }
	{}

/*
=================================================
	SetRenderPass
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::SetRenderPass (RenderPassName::Ref rpName, SubpassName::Ref subpass) __Th___
	{
		CHECK_THROW_MSG( rpName.IsDefined() and subpass.IsDefined() );

		CHECK_THROW_MSG( GetBase() != null and GetBase()->fragment );
		auto&	storage = *ObjectStorage::Instance();

		auto	iter = storage.renderPassToCompat.find( rpName );
		CHECK_THROW_MSG( iter != storage.renderPassToCompat.end(),
			"RenderPass '"s << storage.GetName( rpName ) << "' is not exists" );

		if ( desc.renderPass.IsDefined() and desc.subpass.IsDefined() )
		{
			CHECK_THROW_MSG( desc.renderPass == iter->second and desc.subpass == subpass,
				"RenderPass is already defined" );
		}

		SubpassShaderIO		frag_io;
		GetBase()->GetSubpassShaderIO( OUT frag_io );

		storage.TestRenderPass( iter->second, subpass, frag_io, false, false );  // throw

		desc.renderPass	= iter->second;
		desc.subpass	= subpass;
	}

/*
=================================================
	SetDynamicState
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::SetDynamicState (/*EPipelineDynamicState*/uint states) __Th___
	{
		auto	ds = EPipelineDynamicState(states);
		CHECK_THROW_MSG( (ds & ~EPipelineDynamicState::GraphicsPipelineMask) == Default, "unsupported dynamic state for graphics pipeline" );

		desc.dynamicState = ds;
	}

/*
=================================================
	SetRenderState
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::SetRenderState (const RenderState &state) __Th___
	{
		CHECK_THROW_MSG( state.inputAssembly.topology < EPrimitive::_Count );

		renderState = state;
		_ValidateRenderState( desc.dynamicState, INOUT renderState, GetFeatures() );

		CHECK_THROW_MSG( _CheckTopology() );

		SubpassShaderIO		frag_io;
		GetBase()->GetSubpassShaderIO( OUT frag_io );
		_CheckDepthStencil( renderState, frag_io, desc.renderPass, desc.subpass );  // throw
	}

	void  GraphicsPipelineSpecScriptBinding::SetRenderState2 (const String &name) __Th___
	{
		const auto&	map = ObjectStorage::Instance()->renderStatesMap;

		auto	iter = map.find( name );
		CHECK_THROW_MSG( iter != map.end(),
			"RenderState '"s << name << "' is not exists" );

		return SetRenderState( iter->second->Get() );
	}

/*
=================================================
	_CheckTopology
=================================================
*/
	bool  GraphicsPipelineSpecScriptBinding::_CheckTopology ()
	{
		SerializableGraphicsPipeline::TopologyBits_t	supported_topology;
		if ( GetBase()->vertex )		supported_topology |= GetBase()->vertex->reflection.vertex.supportedTopology;
		if ( GetBase()->tessControl )	supported_topology |= GetBase()->tessControl->reflection.vertex.supportedTopology;
		if ( GetBase()->geometry )		supported_topology |= GetBase()->geometry->reflection.vertex.supportedTopology;

		ValidatePrimitiveTopology( INOUT supported_topology );

		if ( renderState.inputAssembly.topology == Default )
		{
			RETURN_ERR( "renderState.inputAssembly.topology must be defined" );
		}

		if ( not supported_topology.contains( renderState.inputAssembly.topology ))
		{
			String	str;
			str << "renderState.inputAssembly.topology = " << ToString( renderState.inputAssembly.topology ) << " is not supported.\n";
			str << "supported topologies: ";

			for (uint bits = supported_topology.AsBits(); bits != 0; )
			{
				EPrimitive	value = ExtractBitIndex<EPrimitive>( INOUT bits );
				str << ToString( value ) << (bits ? ", " : "");
			}
			RETURN_ERR( str );
		}
		return true;
	}

/*
=================================================
	SetViewportCount
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::SetViewportCount (uint value) __Th___
	{
		CHECK_THROW_MSG( value > 0 );
		CHECK_THROW_MSG( value <= GraphicsConfig::MaxViewports );

		TestFeature_Min( GetBase()->GetFeatures(), &FeatureSet::maxViewports, value, "maxViewports", "viewportCount" );

		desc.viewportCount = CheckCast<ubyte>(value);
	}

/*
=================================================
	Build
=================================================
*/
	bool  GraphicsPipelineSpecScriptBinding::Build (PipelineTemplUID templUID) __NE___
	{
		if ( not _enabled )
			return true;

		if ( IsBuilded() )
			return true;

		CHECK_ERR( _CheckTopology() );

		auto*	base			= GetBase();
		auto&	ppln_storage	= *ObjectStorage::Instance()->pplnStorage;
		auto	rs_uid			= ppln_storage.AddRenderState( SerializableRenderState{ renderState });
		auto	dss_uid			= ppln_storage.AddDepthStencilState( SerializableDepthStencilState{ renderState });

		if ( not _vertexBuffers )
			_vertexBuffers.Set( base->GetVertexBuffers() );

		SerializableGraphicsPipelineSpec	spec;
		spec.templUID		= templUID;
		spec.rStateUID		= rs_uid;
		spec.dsStateUID		= dss_uid;
		spec.desc			= desc;
		spec.desc.options	= _options;

		if ( _vertexBuffers )
			CHECK_ERR( _vertexBuffers->Get( OUT spec.desc.vertexInput, OUT spec.desc.vertexBuffers ));

		_OnBuild( ppln_storage.AddPipeline( Name(), RVRef(spec) ));
		return true;
	}

/*
=================================================
	SetVertexInput
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::SetVertexInput (const String &name) __Th___
	{
		const auto&	map = ObjectStorage::Instance()->vbInputMap;

		auto	iter = map.find( name );
		CHECK_THROW_MSG( iter != map.end(),
			"VertexInput '"s << name << "' is not exists" );

		_vertexBuffers = iter->second;
	}

	void  GraphicsPipelineSpecScriptBinding::SetVertexInput2 (const VertexBufferInputPtr &ptr) __Th___
	{
		CHECK_THROW_MSG( ptr );
		_vertexBuffers = ptr;
	}

/*
=================================================
	Bind
=================================================
*/
	void  GraphicsPipelineSpecScriptBinding::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<GraphicsPipelineSpecScriptBinding>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set specialization value.\n"
						"Specialization constant must be previously defined in shader by 'Shader::AddSpec()'." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetSpecValueU,	"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetSpecValueI,	"SetSpecValue",		{"name", "value"} );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetSpecValueF,	"SetSpecValue",		{"name", "value"} );

		binder.Comment( "Set dynamic states (EPipelineDynamicState).\n"
						"Supported: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants', FragmentShadingRate." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetDynamicState,	"SetDynamicState",	{"states"} );

		binder.Comment( "Set render state." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetRenderState,	"SetRenderState",	{"rs"} );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetRenderState2,	"SetRenderState",	{"rsName"} );

		binder.Comment( "Set number of viewports. Default is 1.\n"
						"Requires 'multiViewport' feature." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetViewportCount,	"SetViewportCount",	{"count"} );

		binder.Comment( "Attach pipeline to the render technique.\n"
						"When rtech is created it will create all attached pipelines." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::AddToRenderTech,	"AddToRenderTech",	{"rtech", "gpass"} );

		binder.Comment( "Set pipeline options (EPipelineOpt).\n"
						"Supported: 'Optimize'.\n"
						"By default used value from 'GlobalConfig::SetPipelineOptions()'." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetOptions,		"SetOptions",		{"opts"} );

		binder.Comment( "Set vertex attributes." );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetVertexInput,	"SetVertexInput",	{"vbName"} );
		binder.AddMethod( &GraphicsPipelineSpecScriptBinding::SetVertexInput2,	"SetVertexInput",	{"vb"} );
	}


} // AE::PipelineCompiler
