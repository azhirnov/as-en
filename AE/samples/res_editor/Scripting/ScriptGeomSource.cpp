// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"

#include "res_editor/GeomSource/SphericalCube.h"
#include "res_editor/GeomSource/UnifiedGeometry.h"
#include "res_editor/GeomSource/ModelGeomSource.h"

#include "res_editor/_data/cpp/types.h"
#include "res_editor/_ui_data/cpp/types.h"

#include "res_editor/Scripting/PipelineCompiler.inl.h"

#include "res_loaders/Intermediate/IntermScene.h"
#include "res_loaders/Public/ModelLoader.h"
#include "res_loaders/Assimp/AssimpLoader.h"

namespace AE::ResEditor
{
	using namespace AE::PipelineCompiler;

	using EDebugMode = IGeomSource::EDebugMode;

namespace
{
	static ScriptModelGeometrySrc*  ScriptSceneGeometry_Ctor1 (const String &filename) {
		return ScriptRC<ScriptModelGeometrySrc>{ new ScriptModelGeometrySrc{ filename }}.Detach();
	}

	static const StringView		c_MtrDS		= "material";
	static const StringView		c_PassDS	= "pass";

} // namespace
//-----------------------------------------------------------------------------


namespace
{
/*
=================================================
	ScriptGeomSource_ToBase
=================================================
*/
	template <typename T>
	static ScriptGeomSource*  ScriptGeomSource_ToBase (T* ptr)
	{
		StaticAssert( IsBaseOf< ScriptGeomSource, T >);

		ScriptGeomSourcePtr  result{ ptr };
		return result.Detach();
	}


/*
=================================================
	GetDescSetBinding
=================================================
*/
	template <typename ResMngr, typename PplnID>
	ND_ static DescSetBinding  GetDescSetBinding (ResMngr &resMngr, PplnID pplnId, DescriptorSetName::Ref name) __Th___
	{
		auto*	ppln = resMngr.GetResource( pplnId );
		CHECK_THROW( ppln != null );

		auto*	layout = resMngr.GetResource( ppln->LayoutId() );
		CHECK_THROW( layout != null );

		DescriptorSetLayoutID	dsl;
		DescSetBinding			binding;
		CHECK_THROW( layout->GetDescriptorSetLayout( name, OUT dsl, OUT binding ));

		return binding;
	}

/*
=================================================
	_FindPipelinesByVB
=================================================
*/
	static void  _FindPipelinesByVB (StringView vbName, OUT Array<GraphicsPipelineSpecPtr> &pipelines) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		pipelines.clear();

		for (auto& [tname, ppln] : storage.gpipelines)
		{
			for (auto& spec : ppln->GetSpecializations())
			{
				if ( spec->GetVertexBuffers() == null )
					continue;

				if ( vbName == spec->GetVertexBuffers()->Name() )
					pipelines.push_back( spec );
			}
		}
		CHECK_THROW_MSG( not pipelines.empty(),
			"Can't find pipelines with vertex buffer type '"s << vbName << "'" );
	}

/*
=================================================
	_FindPipelinesWithoutVB
=================================================
*/
	static void  _FindPipelinesWithoutVB (OUT Array<GraphicsPipelineSpecPtr> &pipelines) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		pipelines.clear();

		for (auto& [tname, ppln] : storage.gpipelines)
		{
			for (auto& spec : ppln->GetSpecializations())
			{
				if ( spec->GetVertexBuffers() == null )
					pipelines.push_back( spec );
			}
		}
		CHECK_THROW_MSG( not pipelines.empty(),
			"Can't find graphics pipelines without vertex buffer" );
	}

/*
=================================================
	_GetMeshPipelines
=================================================
*/
	static void  _GetMeshPipelines (OUT Array<MeshPipelineSpecPtr> &pipelines) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		pipelines.clear();

		for (auto& [tname, ppln] : storage.mpipelines)
		{
			for (auto& spec : ppln->GetSpecializations())
			{
				pipelines.push_back( spec );
			}
		}
		CHECK_THROW_MSG( not pipelines.empty(), "Can't find mesh pipelines" );
	}
/*
=================================================
	_FindPipelinesByBuf
=================================================
*/
	template <typename PplnSpec>
	static void  _FindPipelinesByBuf (StringView dsName, StringView bufTypeName, EDescriptorType descType, INOUT Array<PplnSpec> &inoutPipelines) __Th___
	{
		Array<PplnSpec>				out_pplns;
		const DescriptorSetName		req_ds_name		{dsName};
		const ShaderStructName		req_buf_name	{bufTypeName};

		for (auto& ppln : inoutPipelines)
		{
			auto	ppln_layout = ppln->GetBase()->GetLayout();
			if ( not ppln_layout )
				continue;

			for (auto& [dsl, ds_name] : ppln_layout->Layouts())
			{
				if ( ds_name != req_ds_name )
					continue;

				for (auto& [un_name, un] : dsl->GetUniforms())
				{
					if ( un.type != descType )
						continue;

					if ( un.buffer.typeName == req_buf_name ) {
						out_pplns.push_back( ppln );
						break;
					}
				}
			}
		}

		CHECK_THROW_MSG( not out_pplns.empty(),
			"Can't find pipelines with DS '"s << dsName << "' and " <<
			(descType == EDescriptorType::UniformBuffer ? "UniformBuffer" :
			 descType == EDescriptorType::StorageBuffer ? "StorageBuffer" : "<unknown>") <<
			" with type '" << bufTypeName << "'" );

		inoutPipelines = RVRef(out_pplns);
	}

	template <typename PplnSpec>
	static void  _FindPipelinesByUB (StringView dsName, StringView ubTypeName, INOUT Array<PplnSpec> &pipelines) __Th___
	{
		_FindPipelinesByBuf( dsName, ubTypeName, EDescriptorType::UniformBuffer, INOUT pipelines );
	}

	template <typename PplnSpec>
	static void  _FindPipelinesBySB (StringView dsName, StringView sbTypeName, INOUT Array<PplnSpec> &pipelines) __Th___
	{
		_FindPipelinesByBuf( dsName, sbTypeName, EDescriptorType::StorageBuffer, INOUT pipelines );
	}

/*
=================================================
	_FindPipelinesByResources
=================================================
*/
	template <typename PplnSpec>
	static void  _FindPipelinesByResources (StringView dsName, const ScriptPassArgs::Arguments_t &args, INOUT Array<PplnSpec> &inoutPipelines) __Th___
	{
		if ( args.empty() )
			return;	// keep all pipelines

		auto&												storage		= *ObjectStorage::Instance();
		Array<PplnSpec>										out_pplns;
		const DescriptorSetName								req_ds_name {dsName};
		FlatHashMap< UniformName::Optimized_t, StringView >	tex_names;
		FlatHashSet< UniformName::Optimized_t >				buf_names;
		FlatHashSet< UniformName::Optimized_t >				texbuf_names;
		FlatHashSet< UniformName::Optimized_t >				img_names;
		FlatHashSet< UniformName::Optimized_t >				rtas_names;

		for (auto& arg : args)
		{
			Visit( arg.res,
				[&] (ScriptBufferPtr buf) {
					if ( buf->HasLayout() )
						buf_names.insert( UniformName::Optimized_t{arg.name} );
					else
						texbuf_names.insert( UniformName::Optimized_t{arg.name} );
				},
				[&] (ScriptImagePtr) {
					if ( arg.samplerName.empty() )
						img_names.emplace( UniformName::Optimized_t{arg.name} );
					else
						tex_names.emplace( UniformName::Optimized_t{arg.name}, arg.samplerName );
				},
				[&] (ScriptVideoImagePtr video) {
					tex_names.emplace( UniformName::Optimized_t{arg.name}, arg.samplerName );
				},
				[&] (ScriptRTScenePtr) {
					rtas_names.insert( UniformName::Optimized_t{arg.name} );
				},
				[] (const Array<ScriptImagePtr> &) {
					// skip
				},
				[] (NullUnion) {
					CHECK_THROW_MSG( false, "unsupported argument type" );
				}
			);
		}

		for (auto& ppln : inoutPipelines)
		{
			auto	ppln_layout = ppln->GetBase()->GetLayout();
			if ( not ppln_layout )
				continue;

			for (auto& [dsl, ds_name] : ppln_layout->Layouts())
			{
				if ( ds_name != req_ds_name )
					continue;

				usize	tex_counter		= 0;
				usize	buf_counter		= 0;
				usize	texbuf_counter	= 0;
				usize	img_counter		= 0;
				usize	rtas_counter	= 0;
				usize	unbound_counter	= 0;

				const auto	IncCounter = [&unbound_counter] (bool found, INOUT usize &resCount)
				{{
					if ( found )	++resCount;
					else			++unbound_counter;
				}};

				for (auto& [un_name, un] : dsl->GetUniforms())
				{
					// storage image
					if ( un.type == EDescriptorType::StorageImage ) {
						IncCounter( img_names.contains( un_name ), img_counter );
						continue;
					}else{
						CHECK_THROW_MSG( not img_names.contains( un_name ),
							"Required storage image '"s << storage.GetName( un_name ) << "' declared with different type" );
					}

					// storage buffer
					if ( un.type == EDescriptorType::StorageBuffer ) {
						IncCounter( buf_names.contains( un_name ), buf_counter );
						continue;
					}else{
						CHECK_THROW_MSG( not buf_names.contains( un_name ),
							"Required storage buffer '"s << storage.GetName( un_name ) << "' declared with different type" );
					}

					// storage texel buffer
					if ( un.type == EDescriptorType::StorageTexelBuffer ) {
						IncCounter( texbuf_names.contains( un_name ), texbuf_counter );
						continue;
					}else{
						CHECK_THROW_MSG( not texbuf_names.contains( un_name ),
							"Required storage texel buffer '"s << storage.GetName( un_name ) << "' declared with different type" );
					}

					// ray tracing scene
					if ( un.type == EDescriptorType::RayTracingScene ) {
						IncCounter( rtas_names.contains( un_name ), rtas_counter );
						continue;
					}else{
						CHECK_THROW_MSG( not rtas_names.contains( un_name ),
							"Required ray tracing scene '"s << storage.GetName( un_name ) << "' declared with different type" );
					}

					// texture
					if ( un.type == EDescriptorType::CombinedImage_ImmutableSampler )
					{
						if ( auto it = tex_names.find( un_name );  it != tex_names.end() )
						{
							String	samp_name = storage.GetName(dsl->GetSampler( un ));
							CHECK_THROW_MSG( it->second == samp_name,
								"Texture '"s << storage.GetName( un_name ) << "' requires sampler '" << it->second << "' but declared sampler is '" << samp_name << "'" );
							++tex_counter;
						}
						else
							++unbound_counter;
						continue;
					}else{
						CHECK_THROW_MSG( not tex_names.contains( un_name ),
							"Required combined image '"s << storage.GetName( un_name ) << "' declared with different type" );
					}
				}

				if ( tex_counter	== tex_names.size()		and
					 buf_counter	== buf_names.size()		and
					 texbuf_counter	== texbuf_names.size()	and
					 img_counter	== img_names.size()		and
					 rtas_counter	== rtas_names.size()	)
				{
					CHECK_THROW_MSG( unbound_counter == 0,
						"Pipeline '"s << ppln->NameStr() << "' with DS '" << dsName << "' has all bound resources but has " << ToString(unbound_counter) << " unbound resources" );

					out_pplns.push_back( ppln );
					break;
				}
			}
		}

		if ( out_pplns.empty() )
		{
			String	str = "Can't find pipelines with DS '"s << dsName << "' and all resources:";
			for (auto& arg : args)
			{
				Visit( arg.res,
					[&] (ScriptBufferPtr buf) {
						if ( buf->HasLayout() )
							str << "\n  StorageBuffer '" << arg.name << "'";
						else
							str << "\n  TexelStorageBuffer '" << arg.name << "'";
					},
					[&] (ScriptImagePtr) {
						if ( arg.samplerName.empty() )
							str << "\n  StorageImage '" << arg.name << "'";
						else
							str << "\n  CombinedImage '" << arg.name << "' with immutable sampler '" << arg.samplerName << "'";
					},
					[&] (ScriptVideoImagePtr video) {
						str << "\n  CombinedImage '" << arg.name << "' with immutable sampler '" << arg.samplerName << "'";
					},
					[&] (ScriptRTScenePtr) {
						str << "\n  RayTracingScene '" << arg.name << "'";
					},
					[] (const Array<ScriptImagePtr> &) {
						// skip
					},
					[] (NullUnion) {
						CHECK_THROW_MSG( false, "unsupported argument type" );
					}
				);
			}
			CHECK_THROW_MSG( false, str );
		}

		inoutPipelines = RVRef(out_pplns);
	}

/*
=================================================
	_FindPipelinesByMaterial
=================================================
*/
	template <typename PplnSpec>
	static void  _FindPipelinesByMaterial (const ResLoader::IntermMaterial &mtr, INOUT Array<PplnSpec> &inoutPipelines) __Th___
	{
		Array<PplnSpec>		out_pplns;
		const auto			settings	= mtr.GetSettings();

		for (auto& ppln : inoutPipelines)
		{
			const auto&	rs = ppln->renderState;

			if ( not ((settings.wireframe and rs.rasterization.polygonMode == EPolygonMode::Line) or
					  (not settings.wireframe and rs.rasterization.polygonMode == EPolygonMode::Fill) ))
				continue;

			if ( not (rs.rasterization.cullMode == settings.cullMode) )
				continue;

			if ( settings.blendMode )
			{
				if ( not (rs.color.buffers[0].blend												and
						  rs.color.buffers[0].srcBlendFactor.color	== settings.blendMode.src	and
						  rs.color.buffers[0].dstBlendFactor.color	== settings.blendMode.dst	and
						  rs.color.buffers[0].blendOp.color			== settings.blendMode.op)	)
					continue;
			}
			else
			{
				if ( rs.color.buffers[0].blend )
					continue;
			}

			out_pplns.push_back( ppln );
		}

		if ( out_pplns.empty() )
		{
			String	str = "Can't find pipelines with states:";
			str << "\n  wireframe: " << ToString(settings.wireframe);
			str << "\n  cullMode:  " << ToString(settings.cullMode);
			str << "\n  blendMode: " << (settings.blendMode ?
										 (String{ToString(settings.blendMode.src)} << " (" << ToString(settings.blendMode.op) << ") " << ToString(settings.blendMode.dst)) :
										 "none"s);
			CHECK_THROW_MSG( false, str );
		}
		inoutPipelines = RVRef(out_pplns);
	}

/*
=================================================
	_FindPipelinesByLayout
=================================================
*/
	template <typename PplnSpec>
	static void  _FindPipelinesByLayout (StringView plName, INOUT Array<PplnSpec> &inoutPipelines) __Th___
	{
		Array<PplnSpec>		out_pplns;

		for (auto& ppln : inoutPipelines)
		{
			if ( auto pl = ppln->GetBase()->GetLayout() )
			{
				if ( pl->Name() == plName )
					out_pplns.push_back( ppln );
			}
		}

		CHECK_THROW_MSG( not out_pplns.empty(),
			"Can't find pipelines with layout '"s << plName << "'" );

		inoutPipelines = RVRef(out_pplns);
	}

/*
=================================================
	_GetSuitablePipeline
=================================================
*/
	template <typename PplnSpec>
	ND_ static ScriptGeomSource::PipelineNames_t  _GetSuitablePipeline (Array<PplnSpec> &pipelines, StringView hint, usize objId, EDebugMode dbgMode, EShaderStages dbgStages)
	{
		CHECK_THROW_MSG( not pipelines.empty(), "Failed to find suitable pipeline" );
		CHECK_THROW_MSG( (dbgMode == Default) == (dbgStages == Default), "Both 'dbgMode' and 'dbgStages' must be defined or undefined" );

		const auto	MatchHint = [hint] (StringView name)
		{{
			if_likely( hint.empty() )
				return true;

			return HasSubString( name, hint );
		}};

		ScriptGeomSource::PipelineNames_t	result;
		String								log;
		EShaderOpt							opt = Default;

		switch_enum( dbgMode )
		{
			case EDebugMode::Trace :		opt = EShaderOpt::Trace;		break;
			case EDebugMode::FnProfiling :	opt = EShaderOpt::FnProfiling;	break;
			case EDebugMode::TimeHeatMap :	opt = EShaderOpt::TimeHeatMap;	break;
			case EDebugMode::Unknown :		break;
			case EDebugMode::_Count :
			default :						CHECK_THROW_MSG( false, "Unsupported EDebugMode" );
		}
		switch_end

		for (auto& ppln : pipelines)
		{
			auto	pl = ppln->GetBase()->GetLayout();

			if ( pl												and
				 pl->GetDebugDS().mode == opt					and
				 AllBits( pl->GetDebugDS().stages, dbgStages )	and
				 MatchHint( ppln->NameStr() ))
			{
				if ( result.empty() )
					result.emplace_back( ppln->Name(), objId, dbgMode, dbgStages );
				else
					log << "\n\t" << ppln->NameStr();
			}
		}

		if ( not log.empty() )
			AE_LOGW( "More than one pipeline are match the requirements, skip:"s << log );

		return result;
	}

	template <typename PplnSpec>
	ND_ static ScriptGeomSource::PipelineNames_t  _GetSuitablePipeline (Array<PplnSpec> &pipelines, StringView hint = Default, usize objId = 0)
	{
		return _GetSuitablePipeline( pipelines, hint, objId, Default, Default );
	}

/*
=================================================
	_GetAllSuitablePipelines
=================================================
*/
	template <typename PplnSpec>
	ND_ static ScriptGeomSource::PipelineNames_t  _GetAllSuitablePipelines (Array<PplnSpec> &pipelines, EShaderStages stages, StringView hint = Default, usize objId = 0)
	{
		ScriptGeomSource::PipelineNames_t	result;
		{
			result = _GetSuitablePipeline( pipelines, hint, objId );
			CHECK_THROW_MSG( result.size() == 1 );
		}

		for (EShaderStages stage : BitfieldIterate( stages ))
		{
			auto	tmp = _GetSuitablePipeline( pipelines, hint, objId, EDebugMode::Trace, stage );
			if ( not tmp.empty() )
				result.push_back( tmp.front() );
		}
		return result;
	}

/*
=================================================
	_GetSuitablePipelineAndDS
=================================================
*/
	template <typename PplnSpec>
	static void  _GetSuitablePipelineAndDS (Array<PplnSpec> &pipelines, StringView dsName,
											OUT ScriptGeomSource::PipelineNames_t &name, OUT DSLayoutName &dslName)
	{
		CHECK_THROW_MSG( not pipelines.empty(),
			"Failed to find suitable pipeline" );

		if ( pipelines.size() > 1 )
			AE_LOGI( "More than one pipeline are match the requirements" );

		if ( not pipelines.empty() )
		{
			name = ScriptGeomSource::PipelineNames_t{ ScriptGeomSource::PplnNameAndObjectId{ pipelines.front()->Name() }};

			auto	pl = pipelines.front()->GetBase()->GetLayout();
			CHECK_THROW( pl );

			const DescriptorSetName	req_ds_name {dsName};
			dslName = Default;

			for (auto& [dsl, ds_name] : pl->Layouts())
			{
				if ( ds_name == req_ds_name )
				{
					dslName = DSLayoutName{dsl->Name()};
					return;
				}
			}
			CHECK_THROW_MSG( false, "Can't find DS with name '"s << dsName << "'" );
		}
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptGeomSource::ScriptGeomSource () :
		_args{ [this](ScriptPassArgs::Argument &arg) { _OnAddArg( arg ); }}
	{}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptGeomSource::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptGeomSource>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
	}

/*
=================================================
	_BindBase
=================================================
*/
	template <typename B>
	void  ScriptGeomSource::_BindBase (B &binder, Bool withArgs) __Th___
	{
		using T = typename B::Class_t;

		binder.Operators().ImplCast( &ScriptGeomSource_ToBase<T> );

		if ( withArgs )
		{
			binder.Comment( "Add resource to all shaders in the current pass.\n"
							"In - resource is used for read access.\n"
							"Out - resource is used for write access.\n" );
			binder.AddMethod( &ScriptGeomSource::ArgSceneIn,		"ArgIn",	{"uniformName", "resource"} );

			binder.AddMethod( &ScriptGeomSource::ArgBufferIn,		"ArgIn",	{"uniformName", "resource"} );
			binder.AddMethod( &ScriptGeomSource::ArgBufferOut,		"ArgOut",	{"uniformName", "resource"} );
			binder.AddMethod( &ScriptGeomSource::ArgBufferInOut,	"ArgInOut",	{"uniformName", "resource"} );

			binder.AddMethod( &ScriptGeomSource::ArgImageIn,		"ArgIn",	{"uniformName", "resource"} );
			binder.AddMethod( &ScriptGeomSource::ArgImageOut,		"ArgOut",	{"uniformName", "resource"} );
			binder.AddMethod( &ScriptGeomSource::ArgImageInOut,		"ArgInOut",	{"uniformName", "resource"} );

			binder.AddMethod( &ScriptGeomSource::ArgImageArrIn,		"ArgIn",	{"uniformName", "resources"} );
			binder.AddMethod( &ScriptGeomSource::ArgImageArrOut,	"ArgOut",	{"uniformName", "resources"} );
			binder.AddMethod( &ScriptGeomSource::ArgImageArrInOut,	"ArgInOut",	{"uniformName", "resources"} );

			binder.AddMethod( &ScriptGeomSource::ArgTextureIn,		"ArgTex",	{"uniformName", "resource"} );
			binder.AddMethod( &ScriptGeomSource::ArgTextureIn2,		"ArgIn",	{"uniformName", "resource", "samplerName"} );
			binder.AddMethod( &ScriptGeomSource::ArgTextureArrIn,	"ArgTex",	{"uniformName", "resources"} );
			binder.AddMethod( &ScriptGeomSource::ArgTextureArrIn2,	"ArgIn",	{"uniformName", "resources", "samplerName"} );

			binder.AddMethod( &ScriptGeomSource::ArgVideoIn,		"ArgIn",	{"uniformName", "resource", "samplerName"} );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	ScriptSphericalCube::~ScriptSphericalCube ()
	{
		if ( not _geomSrc )
			AE_LOGW( "Unused SphericalCube" );
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptSphericalCube::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
	{
		CHECK_THROW_MSG( not _geomSrc );
	}

/*
=================================================
	SetDetailLevel*
=================================================
*/
	void  ScriptSphericalCube::SetDetailLevel1 (uint maxLod) __Th___
	{
		return SetDetailLevel2( 0, maxLod );
	}

	void  ScriptSphericalCube::SetDetailLevel2 (uint minLod, uint maxLod) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc );
		CHECK_THROW_MSG( _minLod == 0 and _maxLod == 0, "already specified" );

		_minLod	= minLod;
		_maxLod	= maxLod;
	}

/*
=================================================
	SetInstanceCount
=================================================
*/
	void  ScriptSphericalCube::SetInstanceCount (uint count) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc );
		CHECK_THROW_MSG( count > 0 );

		_instCount = count;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSphericalCube::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptSphericalCube>	binder{ se };
		binder.CreateRef();
		ScriptGeomSource::_BindBase( binder );

		binder.Comment( "Set detail level of the sphere.\n"
						"Vertex count: (lod+2)^2, index count: 6*(lod+1)^2." );
		binder.AddMethod( &ScriptSphericalCube::SetDetailLevel1,	"DetailLevel",		{"maxLOD"} );
		binder.AddMethod( &ScriptSphericalCube::SetDetailLevel2,	"DetailLevel",		{"minLOD", "maxLOD"} );
		binder.AddMethod( &ScriptSphericalCube::SetInstanceCount,	"InstanceCount",	{} );
	}

/*
=================================================
	ToGeomSource
=================================================
*/
	RC<IGeomSource>  ScriptSphericalCube::ToGeomSource () __Th___
	{
		if ( _geomSrc )
			return _geomSrc;

		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer();  // throw
		auto		result		= MakeRC<SphericalCube>( renderer, _minLod, _maxLod, _instCount );

		_args.InitResources( result->_resources, Default );

		_geomSrc = result;
		return _geomSrc;
	}

/*
=================================================
	FindMaterialGraphicsPipelines
=================================================
*/
	ScriptGeomSource::PipelineNames_t  ScriptSphericalCube::FindMaterialGraphicsPipelines (ERenderLayer layer) C_Th___
	{
		CHECK_THROW( layer == ERenderLayer::Opaque );

		Array<GraphicsPipelineSpecPtr>	pipelines;
		_FindPipelinesByVB( "VB{SphericalCubeVertex}", OUT pipelines );				// throw
		_FindPipelinesByUB( c_MtrDS, "SphericalCubeMaterialUB", INOUT pipelines );	// throw
		_FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );		// throw

		return _GetAllSuitablePipelines( pipelines, EShaderStages::GraphicsPipeStages );
	}

/*
=================================================
	ToMaterial
=================================================
*/
	RC<IGSMaterials>  ScriptSphericalCube::ToMaterial (const ERenderLayer layer, RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
	{
		CHECK_THROW( _geomSrc );
		CHECK_THROW( rtech );
		CHECK_THROW( not names.empty() );
		CHECK_THROW( layer == ERenderLayer::Opaque );

		auto		result		= MakeRC<SphericalCube::Material>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer();  // throw
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		auto	ppln = rtech->GetGraphicsPipeline( names[0].pplnName );
		CHECK_THROW( ppln );
		CHECK_THROW( names[0].dbgMode == Default );

		result->rtech = rtech;

		#if PIPELINE_STATISTICS
		{
			auto&	res = res_mngr.GetResourcesOrThrow( ppln );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( "SphericalCube", res.Handle(), res.Options() ));
		}
		#endif

		for (auto& src : names)
		{
			auto	p = rtech->GetGraphicsPipeline( src.pplnName );
			CHECK_THROW( p );
			result->pplnMap.emplace( Tuple{ src.dbgMode, src.dbgStages }, p );
		}

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), max_frames, ppln,
													DescriptorSetName{c_MtrDS}, null, "MaterialDS" ));

		result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );

		result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::SphericalCubeMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
												 "SphericalCubeMaterialUB", renderer.GetStaticAllocator() );
		CHECK_THROW( result->ubuffer );

		return result;
	}

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptSphericalCube::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "SphericalCubeMaterialUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"SphericalCubeMaterialUB"}};
		st->Set( EStructLayout::Std140, R"#(
				float4x4	transform;
				float3x3	normalMat;
			)#");

		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSphericalCube::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
	DrawCmd_SetIndexBuffer*
=================================================
*/
	template <typename DrawCmd>
	void  DrawCmd_SetIndexBuffer2 (DrawCmd &cmd, EIndex type, const ScriptBufferPtr &ibuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not cmd._indexBuffer );
		CHECK_THROW_MSG( type == EIndex::UShort or type == EIndex::UInt );
		CHECK_THROW_MSG( (type == EIndex::UShort and IsMultipleOf( offset, 2 )) or
						 (type == EIndex::UInt   and IsMultipleOf( offset, 4 )) );

		ibuf->AddUsage( EResourceUsage::VertexInput );

		cmd._indexType			= type;
		cmd._indexBuffer		= ibuf;
		cmd._indexBufferOffset	= offset;
		cmd._indexBufferField	= "";
	}

	template <typename DrawCmd>
	void  DrawCmd_SetIndexBuffer1 (DrawCmd &cmd, EIndex type, const ScriptBufferPtr &ibuf) __Th___
	{
		DrawCmd_SetIndexBuffer2( cmd, type, ibuf, 0 );
	}

	template <typename DrawCmd>
	void  DrawCmd_SetIndexBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not cmd._indexBuffer );
		CHECK_THROW_MSG( not field.empty() );

		ibuf->AddUsage( EResourceUsage::VertexInput );

		cmd._indexType			= Default;
		cmd._indexBuffer		= ibuf;
		cmd._indexBufferOffset	= 0;
		cmd._indexBufferField	= field;
	}

/*
=================================================
	DrawCmd_GetIndexBufferOffset
=================================================
*/
	template <typename DrawCmd>
	ND_ Bytes  DrawCmd_GetIndexBufferOffset (DrawCmd &cmd) __Th___
	{
		CHECK_THROW_MSG( cmd._indexBuffer );

		if ( not cmd._indexBufferField.empty() )
		{
			CHECK_THROW_MSG( cmd._indexBufferOffset == 0 );
			cmd._indexBuffer->AddLayoutReflection();
			return cmd._indexBuffer->GetFieldOffset( cmd._indexBufferField );
		}
		else
			return Bytes{cmd._indexBufferOffset};
	}

/*
=================================================
	DrawCmd_GetIndexBufferType
=================================================
*/
	template <typename DrawCmd>
	ND_ EIndex  DrawCmd_GetIndexBufferType  (DrawCmd &cmd) __Th___
	{
		using namespace AE::PipelineCompiler;
		CHECK_THROW_MSG( cmd._indexBuffer );

		if ( not cmd._indexBufferField.empty() )
		{
			CHECK_THROW_MSG( cmd._indexType == Default );
			cmd._indexBuffer->AddLayoutReflection();

			auto*	field = cmd._indexBuffer->GetField( cmd._indexBufferField ).template GetIf< ShaderStructType::Field >();
			CHECK_THROW_MSG( field != null );
			CHECK_THROW_MSG( field->IsArray() );
			CHECK_THROW_MSG( field->IsScalar() or field->IsVec() );
			CHECK_THROW_MSG( AnyEqual( field->rows, 1, 2, 4 ));

			switch ( field->type ) {
				case EValueType::UInt16 :	return EIndex::UShort;
				case EValueType::UInt32 :	return EIndex::UInt;
			}

			CHECK_THROW_MSG( false,
				"IndexBuffer '"s << cmd._indexBuffer->GetName() << "' field '" << cmd._indexBufferField <<
				"' must be array of scalar/vec1/vec2/vec4 with uint16/uint32 type" );
		}
		else
			return cmd._indexType;
	}

/*
=================================================
	DrawCmd_SetIndirectBuffer*
=================================================
*/
	template <typename DrawCmd>
	void  DrawCmd_SetIndirectBuffer2 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not cmd._indirectBuffer );

		ibuf->AddUsage( EResourceUsage::IndirectBuffer );

		cmd._indirectBuffer			= ibuf;
		cmd._indirectBufferOffset	= offset;
	}

	template <typename DrawCmd>
	void  DrawCmd_SetIndirectBuffer1 (DrawCmd &cmd, const ScriptBufferPtr &ibuf) __Th___
	{
		DrawCmd_SetIndirectBuffer2( cmd, ibuf, 0 );
	}

	template <typename DrawCmd>
	void  DrawCmd_SetIndirectBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not cmd._indirectBuffer );
		CHECK_THROW_MSG( not field.empty() );

		ibuf->AddUsage( EResourceUsage::IndirectBuffer );

		cmd._indirectBuffer			= ibuf;
		cmd._indirectBufferOffset	= 0;
		cmd._indirectBufferField	= field;
	}

/*
=================================================
	DrawCmd_GetIndirectBufferOffset
=================================================
*/
	template <typename DrawCmd>
	ND_ Bytes  DrawCmd_GetIndirectBufferOffset (DrawCmd &cmd, StringView cmdName) __Th___
	{
		CHECK_THROW_MSG( cmd._indirectBuffer );

		Bytes	result;
		if ( not cmd._indirectBufferField.empty() )
		{
			CHECK_THROW_MSG( cmd._indirectBufferOffset == 0 );
			cmd._indirectBuffer->AddLayoutReflection();

			CHECK_THROW_MSG( cmd._indirectBuffer->GetFieldStructName( cmd._indirectBufferField ) == cmdName,
				"Buffer '"s << cmd._indirectBuffer->GetName() << "' field '" << cmd._indirectBufferField <<
				"' must have '" << cmdName << "' type to use it as IndirectBuffer" );

			result = cmd._indirectBuffer->GetFieldOffset( cmd._indirectBufferField );
		}
		else
			result = Bytes{cmd._indirectBufferOffset};

		CHECK_THROW_MSG( IsMultipleOf( result, 4 ));
		return result;
	}

/*
=================================================
	DrawCmd_SetCountBuffer*
=================================================
*/
	template <typename DrawCmd>
	void  DrawCmd_SetCountBuffer2 (DrawCmd &cmd, const ScriptBufferPtr &cbuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( cbuf );
		CHECK_THROW_MSG( not cmd._countBuffer );

		cbuf->AddUsage( EResourceUsage::IndirectBuffer );

		cmd._countBuffer		= cbuf;
		cmd._countBufferOffset	= offset;
	}

	template <typename DrawCmd>
	void  DrawCmd_SetCountBuffer1 (DrawCmd &cmd, const ScriptBufferPtr &cbuf) __Th___
	{
		DrawCmd_SetCountBuffer2( cmd, cbuf, 0 );
	}

	template <typename DrawCmd>
	void  DrawCmd_SetCountBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &cbuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( cbuf );
		CHECK_THROW_MSG( not cmd._countBuffer );
		CHECK_THROW_MSG( not field.empty() );

		cbuf->AddUsage( EResourceUsage::IndirectBuffer );

		cmd._countBuffer		= cbuf;
		cmd._countBufferOffset	= 0;
		cmd._countBufferField	= field;
	}

/*
=================================================
	DrawCmd_GetCountBufferOffset
=================================================
*/
	template <typename DrawCmd>
	ND_ Bytes  DrawCmd_GetCountBufferOffset (DrawCmd &cmd) __Th___
	{
		CHECK_THROW_MSG( cmd._countBuffer );

		Bytes	result;
		if ( not cmd._countBufferField.empty() )
		{
			CHECK_THROW_MSG( cmd._countBufferOffset == 0 );
			cmd._countBuffer->AddLayoutReflection();

			CHECK_THROW_MSG( cmd._countBuffer->GetFieldType( cmd._countBufferField ) == uint(PipelineCompiler::EValueType::UInt32),
				"Buffer '"s << cmd._countBuffer->GetName() << "' field '" << cmd._countBufferField <<
				"' must have 'Uint32' type to use it as CountBuffer" );

			result = cmd._countBuffer->GetFieldOffset( cmd._countBufferField );
		}
		else
			result = Bytes{cmd._countBufferOffset};

		CHECK_THROW_MSG( IsMultipleOf( result, 4 ));
		return result;
	}

/*
=================================================
	DrawCmd_SetPipelineHint
=================================================
*/
	template <typename DrawCmd>
	void  DrawCmd_SetPipelineHint (DrawCmd &cmd, const String &hint) __Th___
	{
		cmd._pplnHint = hint;
	}


} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	DrawCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawCmd3::SetDynVertexCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynVertexCount );
		dynVertexCount = ptr;
	}

	void  ScriptUniGeometry::DrawCmd3::SetDynInstanceCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynInstanceCount );
		dynInstanceCount = ptr;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawIndexedCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawIndexedCmd3::SetDynIndexCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynIndexCount );
		dynIndexCount = ptr;
	}

	void  ScriptUniGeometry::DrawIndexedCmd3::SetDynInstanceCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynInstanceCount );
		dynInstanceCount = ptr;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawIndirectCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynDrawCount );
		dynDrawCount	= ptr;
		drawCount		= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawIndexedIndirectCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynDrawCount );
		dynDrawCount	= ptr;
		drawCount		= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawMeshTasksCmd3::SetDynTaskCount
=================================================
*/
	void  ScriptUniGeometry::DrawMeshTasksCmd3::SetDynTaskCount (const ScriptDynamicUInt3Ptr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynTaskCount );
		dynTaskCount = ptr;
	}

	void  ScriptUniGeometry::DrawMeshTasksCmd3::SetDynTaskCount1 (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynTaskCount );
		dynTaskCount = ScriptDynamicUInt3Ptr{ new ScriptDynamicUInt3{ ptr->Get()->ToX11() }};
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawMeshTasksIndirectCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynDrawCount );
		dynDrawCount	= ptr;
		drawCount		= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawIndirectCountCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynMaxDrawCount );
		dynMaxDrawCount	= ptr;
		maxDrawCount	= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawIndexedIndirectCountCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynMaxDrawCount );
		dynMaxDrawCount	= ptr;
		maxDrawCount	= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DrawMeshTasksIndirectCountCmd3::SetDyn*
=================================================
*/
	void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
	{
		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not dynMaxDrawCount );
		dynMaxDrawCount	= ptr;
		maxDrawCount	= 0;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Draw*
=================================================
*/
	void  ScriptUniGeometry::Draw1 (const DrawCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		CHECK_THROW_MSG( cmd.vertexCount > 0 or cmd.dynVertexCount );
		CHECK_THROW_MSG( cmd.instanceCount > 0 or cmd.dynInstanceCount );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw2 (const DrawIndexedCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		CHECK_THROW_MSG( cmd._indexBuffer );
		CHECK_THROW_MSG( cmd.indexCount > 0 );
		CHECK_THROW_MSG( cmd.instanceCount > 0 );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw3 (const DrawIndirectCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw4 (const DrawIndexedIndirectCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		CHECK_THROW_MSG( cmd._indexBuffer );
		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw5 (const DrawMeshTasksCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.meshShader == FeatureSet::EFeature::RequireTrue );

		CHECK_THROW_MSG( All( uint3{cmd.taskCount} > uint3{0} ));

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw6 (const DrawMeshTasksIndirectCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.meshShader == FeatureSet::EFeature::RequireTrue );

		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw7 (const DrawIndirectCountCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.drawIndirectCount == FeatureSet::EFeature::RequireTrue );

		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd._countBuffer );
		CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw8 (const DrawIndexedIndirectCountCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.drawIndirectCount == FeatureSet::EFeature::RequireTrue );

		CHECK_THROW_MSG( cmd._indexBuffer );
		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd._countBuffer );
		CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

	void  ScriptUniGeometry::Draw9 (const DrawMeshTasksIndirectCountCmd3 &cmd)
	{
		CHECK_THROW_MSG( not _geomSrc );

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.drawIndirectCount == FeatureSet::EFeature::RequireTrue and
						 fs.meshShader == FeatureSet::EFeature::RequireTrue );

		CHECK_THROW_MSG( cmd._indirectBuffer );
		CHECK_THROW_MSG( cmd._countBuffer );
		CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
		CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
		CHECK_THROW_MSG( IsMultipleOf( cmd.stride, 4 ), "Stride must be multiple of 4" );

		_drawCommands.push_back( cmd );
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptUniGeometry::~ScriptUniGeometry ()
	{
		if ( not _geomSrc )
			AE_LOGW( "Unused UnifiedGeometry" );
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptUniGeometry::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
	{
		CHECK_THROW_MSG( not _geomSrc );
	}

/*
=================================================
	Clone
=================================================
*/
	ScriptUniGeometry*  ScriptUniGeometry::Clone () C_Th___
	{
		ScriptRC<ScriptUniGeometry>	result{ new ScriptUniGeometry{} };

		result->_args.CopyFrom( this->_args );
		result->_drawCommands	= this->_drawCommands;

		return result.Detach();
	}

/*
=================================================
	Bind*
=================================================
*/
	void  ScriptUniGeometry::DrawCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawCmd3::SetDynVertexCount,								"VertexCount",		{} );
		binder.AddMethod( &DrawCmd3::SetDynInstanceCount,							"InstanceCount",	{} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawCmd3>,				"PipelineHint",		{} );
		binder.AddProperty( &DrawCmd3::vertexCount,									"vertexCount"		);
		binder.AddProperty( &DrawCmd3::instanceCount,								"instanceCount"		);
		binder.AddProperty( &DrawCmd3::firstVertex,									"firstVertex"		);
		binder.AddProperty( &DrawCmd3::firstInstance,								"firstInstance"		);
	}

	void  ScriptUniGeometry::DrawIndexedCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawIndexedCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawIndexedCmd3::SetDynIndexCount,						"IndexCount",		{} );
		binder.AddMethod( &DrawIndexedCmd3::SetDynInstanceCount,					"InstanceCount",	{} );
		binder.Comment( "Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedCmd3>,		"IndexBuffer",		{"type", "buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedCmd3>,		"IndexBuffer",		{"type", "buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedCmd3>,		"IndexBuffer",		{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawIndexedCmd3>,		"PipelineHint",		{} );
		binder.AddProperty( &DrawIndexedCmd3::indexCount,							"indexCount"		);
		binder.AddProperty( &DrawIndexedCmd3::instanceCount,						"instanceCount"		);
		binder.AddProperty( &DrawIndexedCmd3::firstIndex,							"firstIndex"		);
		binder.AddProperty( &DrawIndexedCmd3::vertexOffset,							"vertexOffset"		);
		binder.AddProperty( &DrawIndexedCmd3::firstInstance,						"firstInstance"		);
	}

	void  ScriptUniGeometry::DrawIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawIndirectCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawIndirectCmd3::SetDynDrawCount,						"DrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'DrawIndirectCommand' (16 bytes) structs, array size must be at least 'drawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndirectCmd3>,	"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndirectCmd3>,	"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndirectCmd3>,	"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawIndirectCmd3>,		"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 16 bytes and multiple of 4." );
		binder.AddProperty( &DrawIndirectCmd3::stride,								"stride"			);
		binder.AddProperty( &DrawIndirectCmd3::drawCount,							"drawCount"			);
	}

	void  ScriptUniGeometry::DrawIndexedIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawIndexedIndirectCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawIndexedIndirectCmd3::SetDynDrawCount,						"DrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedIndirectCmd3>,		"IndexBuffer",		{"type", "buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedIndirectCmd3>,		"IndexBuffer",		{"type", "buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedIndirectCmd3>,		"IndexBuffer",		{"buffer", "field"} );
		binder.Comment( "Set buffer which contains array of 'DrawIndexedIndirectCommand' (20 bytes) structs, array size must be at least 'drawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndexedIndirectCmd3>,	"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndexedIndirectCmd3>,	"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndexedIndirectCmd3>,	"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawIndexedIndirectCmd3>,		"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 20 bytes and multiple of 4." );
		binder.AddProperty( &DrawIndexedIndirectCmd3::stride,								"stride"			);
		binder.AddProperty( &DrawIndexedIndirectCmd3::drawCount,							"drawCount"			);
	}

	void  ScriptUniGeometry::DrawMeshTasksCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawMeshTasksCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawMeshTasksCmd3::SetDynTaskCount,								"TaskCount",		{} );
		binder.AddMethod( &DrawMeshTasksCmd3::SetDynTaskCount1,								"TaskCount",		{} );
		binder.AddProperty( &DrawMeshTasksCmd3::taskCount,									"taskCount"			);
	}

	void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawMeshTasksIndirectCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawMeshTasksIndirectCmd3::SetDynDrawCount,						"DrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'DrawMeshTasksIndirectCommand' (12 bytes) structs, array size must be at least 'drawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawMeshTasksIndirectCmd3>,	"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawMeshTasksIndirectCmd3>,	"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawMeshTasksIndirectCmd3>,	"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawMeshTasksIndirectCmd3>,	"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 12 bytes and multiple of 4." );
		binder.AddProperty( &DrawMeshTasksIndirectCmd3::stride,								"stride"			);
		binder.AddProperty( &DrawMeshTasksIndirectCmd3::drawCount,							"drawCount"			);
	}

	void  ScriptUniGeometry::DrawIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawIndirectCountCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawIndirectCountCmd3::SetDynMaxDrawCount,						"MaxDrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'DrawIndirectCommand' (16 bytes) structs, array size must be at least 'maxDrawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndirectCountCmd3>,		"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndirectCountCmd3>,		"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndirectCountCmd3>,		"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Set buffer which contains single 'uint' (4 bytes) value." );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawIndirectCountCmd3>,		"CountBuffer",		{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawIndirectCountCmd3>,		"CountBuffer",		{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawIndirectCountCmd3>,		"CountBuffer",		{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawIndirectCountCmd3>,		"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 16 bytes and multiple of 4." );
		binder.AddProperty( &DrawIndirectCountCmd3::stride,									"stride"			);
		binder.AddProperty( &DrawIndirectCountCmd3::maxDrawCount,							"maxDrawCount"		);
	}

	void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawIndexedIndirectCountCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetDynMaxDrawCount,					"MaxDrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedIndirectCountCmd3>,		"IndexBuffer",		{"type", "buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedIndirectCountCmd3>,		"IndexBuffer",		{"type", "buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedIndirectCountCmd3>,		"IndexBuffer",		{"buffer", "field"} );
		binder.Comment( "Set buffer which contains array of 'DrawIndexedIndirectCommand' (20 bytes) structs, array size must be at least 'maxDrawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndexedIndirectCountCmd3>,	"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndexedIndirectCountCmd3>,	"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndexedIndirectCountCmd3>,	"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Set buffer which contains single 'uint' (4 bytes) value." );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawIndexedIndirectCountCmd3>,		"CountBuffer",		{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawIndexedIndirectCountCmd3>,		"CountBuffer",		{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawIndexedIndirectCountCmd3>,		"CountBuffer",		{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawIndexedIndirectCountCmd3>,		"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 20 bytes and multiple of 4." );
		binder.AddProperty( &DrawIndexedIndirectCountCmd3::stride,								"stride"			);
		binder.AddProperty( &DrawIndexedIndirectCountCmd3::maxDrawCount,						"maxDrawCount"		);
	}

	void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<DrawMeshTasksIndirectCountCmd3>	binder{ se };
		binder.CreateClassValue();
		binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetDynMaxDrawCount,						"MaxDrawCount",		{} );
		binder.Comment( "Set buffer which contains array of 'DrawMeshTasksIndirectCommand' (12 bytes) structs, array size must be at least 'maxDrawCount'." );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawMeshTasksIndirectCountCmd3>,	"IndirectBuffer",	{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawMeshTasksIndirectCountCmd3>,	"IndirectBuffer",	{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawMeshTasksIndirectCountCmd3>,	"IndirectBuffer",	{"buffer", "field"} );
		binder.Comment( "Set buffer which contains single 'uint' (4 bytes) value." );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawMeshTasksIndirectCountCmd3>,		"CountBuffer",		{"buffer"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawMeshTasksIndirectCountCmd3>,		"CountBuffer",		{"buffer", "offset"} );
		binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawMeshTasksIndirectCountCmd3>,		"CountBuffer",		{"buffer", "field"} );
		binder.Comment( "Pattern to choose pipeline if found multiple pipelines." );
		binder.AddMethodFromGlobal( &DrawCmd_SetPipelineHint<DrawMeshTasksIndirectCountCmd3>,		"PipelineHint",		{} );
		binder.Comment( "Stride must be at least 12 bytes and multiple of 4." );
		binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::stride,								"stride"			);
		binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::maxDrawCount,							"maxDrawCount"		);
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUniGeometry::Bind (const ScriptEnginePtr &se) __Th___
	{
		DrawCmd3::Bind( se );
		DrawIndexedCmd3::Bind( se );
		DrawIndirectCmd3::Bind( se );
		DrawIndexedIndirectCmd3::Bind( se );
		DrawMeshTasksCmd3::Bind( se );
		DrawMeshTasksIndirectCmd3::Bind( se );
		DrawIndirectCountCmd3::Bind( se );
		DrawIndexedIndirectCountCmd3::Bind( se );
		DrawMeshTasksIndirectCountCmd3::Bind( se );

		Scripting::ClassBinder<ScriptUniGeometry>	binder{ se };
		binder.CreateRef();
		ScriptGeomSource::_BindBase( binder );

		binder.AddMethod( &ScriptUniGeometry::Draw1,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw2,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw3,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw4,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw5,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw6,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw7,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw8,	"Draw",	{} );
		binder.AddMethod( &ScriptUniGeometry::Draw9,	"Draw",	{} );

		binder.AddMethod( &ScriptUniGeometry::Clone,	"Clone", {} );
	}

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptUniGeometry::_CreateUBType () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "UnifiedGeometryMaterialUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st{ new ShaderStructType{"UnifiedGeometryMaterialUB"}};
		st->Set( EStructLayout::Std140, R"#(
				float4x4	transform;
				float3x3	normalMat;
			)#");

		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptUniGeometry::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	ToGeomSource
=================================================
*/
	RC<IGeomSource>  ScriptUniGeometry::ToGeomSource () __Th___
	{
		if ( _geomSrc )
			return _geomSrc;

		CHECK_THROW_MSG( not _drawCommands.empty() );

		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer();  // throw
		auto		result		= MakeRC<UnifiedGeometry>( renderer );

		for (auto& src_cmd : _drawCommands)
		{
			auto&	dst = result->_drawCommands.emplace_back();
			Visit( src_cmd,
				[&] (const DrawCmd3 &src) {
					UnifiedGeometry::DrawCmd2	cmd;
					cmd.dynVertexCount		= src.dynVertexCount ? src.dynVertexCount->Get() : null;
					cmd.dynInstanceCount	= src.dynInstanceCount ? src.dynInstanceCount->Get() : null;
					cmd.vertexCount			= src.vertexCount;
					cmd.instanceCount		= src.instanceCount;
					cmd.firstVertex			= src.firstVertex;
					cmd.firstInstance		= src.firstInstance;
					dst = cmd;
				},

				[&] (const DrawIndexedCmd3 &src) {
					UnifiedGeometry::DrawIndexedCmd2	cmd;
					cmd.indexType			= DrawCmd_GetIndexBufferType( src );
					cmd.indexBufferPtr		= src._indexBuffer->ToResource();		CHECK_THROW( cmd.indexBufferPtr );
					cmd.indexBufferOffset	= DrawCmd_GetIndexBufferOffset( src );
					cmd.dynIndexCount		= src.dynIndexCount ? src.dynIndexCount->Get() : null;
					cmd.dynInstanceCount	= src.dynInstanceCount ? src.dynInstanceCount->Get() : null;
					cmd.indexCount			= src.indexCount;
					cmd.instanceCount		= src.instanceCount;
					cmd.firstIndex			= src.firstIndex;
					cmd.vertexOffset		= src.vertexOffset;
					cmd.firstInstance		= src.firstInstance;
					dst = cmd;
				},

				[&] (const DrawMeshTasksCmd3 &src) {
					UnifiedGeometry::DrawMeshTasksCmd2	cmd;
					cmd.dynTaskCount		= src.dynTaskCount ? src.dynTaskCount->Get() : null;
					cmd.taskCount			= uint3{ src.taskCount };
					dst = cmd;
				},

				[&] (const DrawIndirectCmd3 &src) {
					UnifiedGeometry::DrawIndirectCmd2	cmd;
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndirectCommand" );
					cmd.drawCount			= src.drawCount;
					cmd.dynDrawCount		= src.dynDrawCount ? src.dynDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				},

				[&] (const DrawIndexedIndirectCmd3 &src) {
					UnifiedGeometry::DrawIndexedIndirectCmd2	cmd;
					cmd.indexType			= DrawCmd_GetIndexBufferType( src );
					cmd.indexBufferPtr		= src._indexBuffer->ToResource();		CHECK_THROW( cmd.indexBufferPtr );
					cmd.indexBufferOffset	= DrawCmd_GetIndexBufferOffset( src );
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndexedIndirectCommand" );
					cmd.drawCount			= src.drawCount;
					cmd.dynDrawCount		= src.dynDrawCount ? src.dynDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				},

				[&] (const DrawMeshTasksIndirectCmd3 &src) {
					UnifiedGeometry::DrawMeshTasksIndirectCmd2	cmd;
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawMeshTasksIndirectCommand" );
					cmd.drawCount			= src.drawCount;
					cmd.dynDrawCount		= src.dynDrawCount ? src.dynDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				},

				[&] (const DrawIndirectCountCmd3 &src) {
					UnifiedGeometry::DrawIndirectCountCmd2	cmd;
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndirectCommand" );
					cmd.countBufferPtr		= src._countBuffer->ToResource();		CHECK_THROW( cmd.countBufferPtr );
					cmd.countBufferOffset	= DrawCmd_GetCountBufferOffset( src );
					cmd.maxDrawCount		= src.maxDrawCount;
					cmd.dynMaxDrawCount		= src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				},

				[&] (const DrawIndexedIndirectCountCmd3 &src) {
					UnifiedGeometry::DrawIndexedIndirectCountCmd2	cmd;
					cmd.indexType			= DrawCmd_GetIndexBufferType( src );
					cmd.indexBufferPtr		= src._indexBuffer->ToResource();		CHECK_THROW( cmd.indexBufferPtr );
					cmd.indexBufferOffset	= DrawCmd_GetIndexBufferOffset( src );
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndexedIndirectCommand" );
					cmd.countBufferPtr		= src._countBuffer->ToResource();		CHECK_THROW( cmd.countBufferPtr );
					cmd.countBufferOffset	= DrawCmd_GetCountBufferOffset( src );
					cmd.maxDrawCount		= src.maxDrawCount;
					cmd.dynMaxDrawCount		= src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				},

				[&] (const DrawMeshTasksIndirectCountCmd3 &src) {
					UnifiedGeometry::DrawMeshTasksIndirectCountCmd2	cmd;
					cmd.indirectBufferPtr	= src._indirectBuffer->ToResource();	CHECK_THROW( cmd.indirectBufferPtr );
					cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawMeshTasksIndirectCommand" );
					cmd.countBufferPtr		= src._countBuffer->ToResource();		CHECK_THROW( cmd.countBufferPtr );
					cmd.countBufferOffset	= DrawCmd_GetCountBufferOffset( src );
					cmd.maxDrawCount		= src.maxDrawCount;
					cmd.dynMaxDrawCount		= src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
					cmd.stride				= Bytes{src.stride};
					dst = cmd;
				});
		}

		_args.InitResources( result->_resources, Default );

		_geomSrc = result;
		return _geomSrc;
	}

/*
=================================================
	FindMaterialGraphicsPipelines
=================================================
*/
	ScriptGeomSource::PipelineNames_t  ScriptUniGeometry::FindMaterialGraphicsPipelines (ERenderLayer layer) C_Th___
	{
		CHECK_THROW_MSG( layer == ERenderLayer::Opaque );
		CHECK_THROW_MSG( not _drawCommands.empty() );

		const auto	GetMeshPipeline = [this] (PipelineNames_t &result, usize idx, const String &hint) __Th___
		{{
			Array<MeshPipelineSpecPtr>		pipelines;
			_GetMeshPipelines( OUT pipelines );
			_FindPipelinesByUB( c_MtrDS, "UnifiedGeometryMaterialUB", INOUT pipelines );	// throw
			_FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );			// throw
			auto	tmp = _GetAllSuitablePipelines( pipelines, EShaderStages::MeshPipeStages, hint, idx );
			result.insert( result.end(), tmp.begin(), tmp.end() );
		}};

		const auto	GetGraphicsPipeline = [this] (PipelineNames_t &result, usize idx, const String &hint) __Th___
		{{
			Array<GraphicsPipelineSpecPtr>	pipelines;
			_FindPipelinesWithoutVB( OUT pipelines );
			_FindPipelinesByUB( c_MtrDS, "UnifiedGeometryMaterialUB", INOUT pipelines );	// throw
			_FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );			// throw
			auto	tmp = _GetAllSuitablePipelines( pipelines, EShaderStages::GraphicsPipeStages, hint, idx );
			result.insert( result.end(), tmp.begin(), tmp.end() );
		}};

		PipelineNames_t		result;
		result.reserve( _drawCommands.size() );

		for (const auto [src, idx] : WithIndex( _drawCommands ))
		{
			Visit( src,
				[&] (const DrawCmd3 &cmd)						{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawIndexedCmd3 &cmd)				{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawIndirectCmd3 &cmd)				{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawIndexedIndirectCmd3 &cmd)		{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawIndirectCountCmd3 &cmd)			{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawIndexedIndirectCountCmd3 &cmd)	{ GetGraphicsPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawMeshTasksCmd3 &cmd)				{ GetMeshPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawMeshTasksIndirectCmd3 &cmd)		{ GetMeshPipeline( INOUT result, idx, cmd._pplnHint ); },
				[&] (const DrawMeshTasksIndirectCountCmd3 &cmd)	{ GetMeshPipeline( INOUT result, idx, cmd._pplnHint ); }
			);
		}
		return result;
	}

/*
=================================================
	ToMaterial
=================================================
*/
	RC<IGSMaterials>  ScriptUniGeometry::ToMaterial (const ERenderLayer layer, RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
	{
		CHECK_THROW( _geomSrc );
		CHECK_THROW( rtech );
		CHECK_THROW( names.size() >= _drawCommands.size() );
		CHECK_THROW( layer == ERenderLayer::Opaque );

		auto		result		= MakeRC<UnifiedGeometry::Material>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer();  // throw
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();
		bool		ds_inited	= false;

		result->pipelineMap.reserve( names.size() );

		const auto	FindMeshPpln	= [&] (const PplnNameAndObjectId &info) __Th___
		{{
			auto	ppln = rtech->GetMeshPipeline( info.pplnName );
			CHECK_THROW( ppln );

			#if PIPELINE_STATISTICS
			{
				auto&	res = res_mngr.GetResourcesOrThrow( ppln );
				Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( "UnifiedGeometry", res.Handle(), res.Options() ));
			}
			#endif

			if ( ds_inited ){
				CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_MtrDS} ));
				CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} ));
			}else{
				ds_inited = true;
				CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), max_frames, ppln, DescriptorSetName{c_MtrDS} ));
				result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );
			}
			CHECK_THROW( result->pipelineMap.emplace( Tuple{ info.objId, info.dbgMode, info.dbgStages }, ppln ).second );
		}};

		const auto	FindGraphicsPpln = [&] (const PplnNameAndObjectId &info) __Th___
		{{
			auto	ppln = rtech->GetGraphicsPipeline( info.pplnName );
			CHECK_THROW( ppln );

			#if PIPELINE_STATISTICS
			{
				auto&	res = res_mngr.GetResourcesOrThrow( ppln );
				Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( "UnifiedGeometry", res.Handle(), res.Options() ));
			}
			#endif

			if ( ds_inited ){
				CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_MtrDS} ));
				CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} ));
			}else{
				ds_inited = true;
				CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), max_frames, ppln, DescriptorSetName{c_MtrDS} ));
				result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );
			}
			CHECK_THROW( result->pipelineMap.emplace( Tuple{ info.objId, info.dbgMode, info.dbgStages }, ppln ).second );
		}};

		result->rtech = rtech;

		for (auto& info : names)
		{
			Visit( _drawCommands[ info.objId ],
				[&] (const DrawCmd3 &)						{ FindGraphicsPpln( info ); },
				[&] (const DrawIndexedCmd3 &)				{ FindGraphicsPpln( info ); },
				[&] (const DrawIndirectCmd3 &)				{ FindGraphicsPpln( info ); },
				[&] (const DrawIndexedIndirectCmd3 &)		{ FindGraphicsPpln( info ); },
				[&] (const DrawIndirectCountCmd3 &)			{ FindGraphicsPpln( info ); },
				[&] (const DrawIndexedIndirectCountCmd3 &)	{ FindGraphicsPpln( info ); },
				[&] (const DrawMeshTasksCmd3 &)				{ FindMeshPpln( info ); },
				[&] (const DrawMeshTasksIndirectCmd3 &)		{ FindMeshPpln( info ); },
				[&] (const DrawMeshTasksIndirectCountCmd3 &){ FindMeshPpln( info ); }
			);
		}

		result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::UnifiedGeometryMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
												 "UnifiedGeometryMaterialUB", renderer.GetStaticAllocator() );
		CHECK_THROW( result->ubuffer );

		return result;
	}
//-----------------------------------------------------------------------------


namespace {
	static constexpr const char*	c_RTGeometrySuffix [] = {
		"-Opaque",
		"-OpaqueDS",
		"-Translucent",
		"-Volumetric"
	};
	StaticAssert( CountOf(c_RTGeometrySuffix) == uint(ScriptGeomSource::EGeometryType::_Count) );
}

/*
=================================================
	constructor
=================================================
*/
	ScriptModelGeometrySrc::ScriptModelGeometrySrc (const String &filename) __Th___ :
		_scenePath{ ScriptExe::ScriptResourceApi::ToAbsolute( filename )}  // throw
	{
		_dbgName = Path{filename}.stem().string().substr( 0, ResNameMaxLen );
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptModelGeometrySrc::~ScriptModelGeometrySrc ()
	{
		if ( not _geomSrc )
			AE_LOGW( "Unused Model '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptModelGeometrySrc::Name (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc,
			"resource is already created, can not change debug name" );

		_dbgName = name;

		for (auto i : IndicesOnly( _rtGeometries ))
		{
			if ( _rtGeometries[i] )
				_rtGeometries[i]->Name( name + c_RTGeometrySuffix[i] );
		}
	}

/*
=================================================
	SetInstanceCount
=================================================
*/
	void  ScriptModelGeometrySrc::SetInstanceCount (uint value) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc,
			"resource is already created, can not change instance count" );
		CHECK_THROW_MSG( value >= 1 );

		_instanceCount = value;
	}

/*
=================================================
	AddTextureSearchDir
=================================================
*/
	void  ScriptModelGeometrySrc::AddTextureSearchDir (const String &value) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc,
			"resource is already created, can not change texture prefix" );

		_texSearchDirs.push_back( FileSystem::ToAbsolute( value ));	// TODO: VFS

		CHECK_THROW_MSG( FileSystem::IsDirectory( _texSearchDirs.back() ),
			"Folder '"s << ToString(_texSearchDirs.back()) << "' is not exists" );
	}

/*
=================================================
	GetRTGeometry
=================================================
*/
	ScriptRTGeometryPtr  ScriptModelGeometrySrc::GetRTGeometry (EGeometryType type) __Th___
	{
		CHECK_THROW_MSG( type < EGeometryType::_Count );

		auto&	rt_geom = _rtGeometries[ uint(type) ];

		if ( _geomSrc and not rt_geom )
			return null;

		if ( not rt_geom )
		{
			rt_geom = ScriptRTGeometryPtr{new ScriptRTGeometry{ True{"dummy"} }};
			rt_geom->Name( _dbgName + c_RTGeometrySuffix[uint(type)] );
			rt_geom->MakeImmutable();
		}

		return rt_geom;
	}

/*
=================================================
	SetInitialTransform
=================================================
*/
	void  ScriptModelGeometrySrc::SetInitialTransform1 (const packed_float4x4 &value) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc,
			"resource is already created, can not set initial transform" );

		_initialTransform = Transformation{ float4x4{ value }};
	}

	void  ScriptModelGeometrySrc::SetInitialTransform2 (const packed_float3 &position, const packed_float3 &rotation, float scale) __Th___
	{
		CHECK_THROW_MSG( not _geomSrc,
			"resource is already created, can not set initial transform" );

		_initialTransform = Transformation{ -position, Quat::Rotate2( RadianVec<float,3>{ rotation }), scale };
	}

/*
=================================================
	AddOmniLight
=================================================
*/
	void  ScriptModelGeometrySrc::AddOmniLight (const packed_float3 &pos, const packed_float3 &atten, const RGBA32f &color) __Th___
	{
		auto&	dst	= _omiLights.emplace_back();
		dst.pos		= pos;
		dst.atten	= atten;
		dst.color	= color;
	}

/*
=================================================
	AddConeLight
=================================================
*/
	void  ScriptModelGeometrySrc::AddConeLight (const packed_float3 &pos, const packed_float3 &dir, const packed_float3 &atten, const packed_float2	&cone, const RGBA32f &color) __Th___
	{
		auto&	dst	= _coneLights.emplace_back();
		dst.pos		= pos;
		dst.dir		= dir;
		dst.atten	= atten;
		dst.cone	= cone;
		dst.color	= color;
	}

/*
=================================================
	AddDirLight
=================================================
*/
	void  ScriptModelGeometrySrc::AddDirLight (const packed_float3 &dir, const packed_float3 &atten, const RGBA32f &color) __Th___
	{
		auto&	dst	= _dirLights.emplace_back();
		dst.dir		= dir;
		dst.atten	= atten;
		dst.color	= color;
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptModelGeometrySrc::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
	{
		CHECK_THROW_MSG( not _geomSrc );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptModelGeometrySrc::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptModelGeometrySrc>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		ScriptGeomSource::_BindBase( binder, False{"without args"} );
		binder.AddFactoryCtor( &ScriptSceneGeometry_Ctor1,	{"scenePathInVFS"} );

		binder.Comment( "Set resource name. It is used for debugging." );
		binder.AddMethod( &ScriptModelGeometrySrc::Name,					"Name",					{} );

		binder.Comment( "Add directory where to search required textures." );
		binder.AddMethod( &ScriptModelGeometrySrc::AddTextureSearchDir,		"TextureSearchDir",		{"folder"} );

		binder.Comment( "Set transformation for model root node." );
		binder.AddMethod( &ScriptModelGeometrySrc::SetInitialTransform1,	"InitialTransform",		{} );
		binder.AddMethod( &ScriptModelGeometrySrc::SetInitialTransform2,	"InitialTransform",		{"position", "rotation", "scale"} );

		binder.AddMethod( &ScriptModelGeometrySrc::SetInstanceCount,		"InstanceCount",		{} );

		binder.Comment( "Add light source." );
		binder.AddMethod( &ScriptModelGeometrySrc::AddOmniLight,			"AddOmniLight",			{"position", "attenuation", "color"} );
		binder.AddMethod( &ScriptModelGeometrySrc::AddConeLight,			"AddConeLight",			{"position", "direction", "coneAngle", "attenuation", "color"} );
		binder.AddMethod( &ScriptModelGeometrySrc::AddDirLight,				"AddDirLight",			{"direction", "attenuation", "color"} );
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptModelGeometrySrc::GetShaderTypes (INOUT CppStructsFromShaders &) __Th___
	{
	}

/*
=================================================
	ToGeomSource
=================================================
*/
	RC<IGeomSource>  ScriptModelGeometrySrc::ToGeomSource () __Th___
	{
		using namespace ResLoader;

		if ( _geomSrc )
			return _geomSrc;

		IModelLoader::Config	cfg;

		_intermScene.reset( new IntermScene{} );

		#ifdef AE_ENABLE_ASSIMP
			AssimpLoader    loader;
			CHECK_THROW_MSG( loader.LoadModel( *_intermScene, _scenePath, cfg ),
				"failed to load model from '"s << ToString(_scenePath) << "'" );
		#endif

		const bool			has_rtas = [this](){ bool b = false;  for (auto& g : _rtGeometries) b |= (g != null);  return b; }();
		RTGeometryTypes_t	geom_types;

		for (auto& src : _omiLights)
		{
			auto	light	= MakeRC<IntermLight>();
			auto&	dst		= light->Edit();

			dst.type		= IntermLight::ELightType::Point;
			dst.position	= src.pos;
			dst.attenuation	= src.atten;
			dst.diffuseColor= float3{float4{src.color}};

			CHECK( _intermScene->EditLights().emplace( RVRef(light), uint(_intermScene->EditLights().size()) ).second );
		}
		for (auto& src : _coneLights)
		{
			auto	light	= MakeRC<IntermLight>();
			auto&	dst		= light->Edit();

			dst.type				= IntermLight::ELightType::Spot;
			dst.position			= src.pos;
			dst.direction			= src.dir;
			dst.attenuation			= src.atten;
			dst.coneAngleInnerOuter	= src.cone;
			dst.diffuseColor		= float3{float4{src.color}};

			CHECK( _intermScene->EditLights().emplace( RVRef(light), uint(_intermScene->EditLights().size()) ).second );
		}
		for (auto& src : _dirLights)
		{
			auto	light	= MakeRC<IntermLight>();
			auto&	dst		= light->Edit();

			dst.type		= IntermLight::ELightType::Directional;
			dst.direction	= src.dir;
			dst.attenuation	= src.atten;
			dst.diffuseColor= float3{float4{src.color}};

			CHECK( _intermScene->EditLights().emplace( RVRef(light), uint(_intermScene->EditLights().size()) ).second );
		}

		if ( has_rtas )
		{
			EnumSet<EGeometryType>	enabled;
			_intermScene->ForEachModel(
				[&enabled] (const ResLoader::IntermScene::ModelData &model)
				{
					auto	mesh	= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
					auto	mtr		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
					CHECK_THROW( mesh and mtr );

					const bool	translucent		= mtr->GetSettings().IsTranslucent();
					const bool	dual_sided		= mtr->GetSettings().cullMode == ECullMode::None;

					if ( translucent ) {
						enabled.insert( EGeometryType::Translucent );
						return;
					}

					if ( dual_sided )
						enabled.insert( EGeometryType::OpaqueDualSided );
					else
						enabled.insert( EGeometryType::Opaque );
				});

			for (auto i : IndicesOnly( geom_types ))
			{
				auto&	src	= _rtGeometries[i];
				auto&	dst = geom_types[i];

				if ( enabled.contains( EGeometryType(i) ))
					dst = src->ToResource();  // throw
				else{
					ASSERT( src.UseCount() == 1 );
					src = null;
				}
			}
		}

		Renderer&	renderer = ScriptExe::ScriptResourceApi::GetRenderer(); // throw

		// search in scene dir
		_texSearchDirs.push_back( _scenePath.parent_path() );

		_geomSrc = MakeRCTh<ModelGeomSource>( renderer, _intermScene, _initialTransform,
											  _texSearchDirs, _maxTextures, RVRef(geom_types), _instanceCount );
		return _geomSrc;
	}

/*
=================================================
	_FindPostProcessPipelines
=================================================
*/
	ScriptGeomSource::PipelineNames_t  ScriptModelGeometrySrc::_FindPostProcessPipelines () C_Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		CHECK_THROW_MSG( storage.gpipelines.size() == 1 );
		CHECK_THROW_MSG( storage.gpipelines.begin()->second->GetSpecializations().size() == 1 );

		PipelineNames_t	result;
		result.emplace_back( storage.gpipelines.begin()->second->GetSpecializations()[0]->Name() );
		return result;
	}

/*
=================================================
	_ToPostProcessMaterial
=================================================
*/
	RC<IGSMaterials>  ScriptModelGeometrySrc::_ToPostProcessMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
	{
		CHECK_THROW( names.size() == 1 );

		auto		result		= MakeRC<ModelGeomSource::Material>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		result->rtech = rtech;

		GraphicsPipelineID	ppln = rtech->GetGraphicsPipeline( names[0].pplnName );
		CHECK_THROW( ppln );

		#if PIPELINE_STATISTICS
		{
			auto&	res = res_mngr.GetResourcesOrThrow( ppln );
			Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
		}
		#endif

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), max_frames, ppln, DescriptorSetName{c_MtrDS} ));

		result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );  // throw
		CHECK_THROW( result->passDSIndex );

		auto&	draw_groups	= result->drawGroups.emplace< ModelGeomSource::Material::GPplnGroups_t >();
		draw_groups.emplace( ppln, Array<uint>{} );

		return result;
	}

/*
=================================================
	FindMaterialGraphicsPipelines
=================================================
*/
	ScriptGeomSource::PipelineNames_t  ScriptModelGeometrySrc::FindMaterialGraphicsPipelines (const ERenderLayer layer) C_Th___
	{
		CHECK_THROW_MSG( _intermScene );

		switch_enum( layer )
		{
			case ERenderLayer::Opaque :			break;
			case ERenderLayer::Translucent :	break;
			case ERenderLayer::PostProcess :	return _FindPostProcessPipelines();
			case ERenderLayer::_Count :
			default :							CHECK_THROW_MSG( false, "unknown layer" );  break;
		}
		switch_end

		using EKey = ResLoader::IntermMaterial::EKey;

		ScriptGeomSource::PipelineNames_t	ppln_per_obj;
		DSLayoutName						shared_mtr_dsl;
		usize								obj_id			= 0;

		_intermScene->ForEachModel(
			[&ppln_per_obj, this, &shared_mtr_dsl, layer, &obj_id] (const ResLoader::IntermScene::ModelData &model)
			{
				++obj_id;

				auto	mesh	= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
				auto	mtr		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
				CHECK_THROW( mesh and mtr );

				const bool	is_translucent	= mtr->GetSettings().IsTranslucent();

				// skip model
				if ( (layer == ERenderLayer::Opaque) and is_translucent )			return;
				if ( (layer == ERenderLayer::Translucent) and not is_translucent )	return;

				CHECK_THROW( mesh->Attribs() != null );
				CHECK_THROW( mesh->Topology() == EPrimitive::TriangleList );

				Array<GraphicsPipelineSpecPtr>	pipelines;
				for (uint i = 0; i < 2; ++i)
				{
					try {
						String	attribs_name;
						switch ( i ) {
							case 0 :	attribs_name = _AttribsToVBName( *mesh->Attribs() );	break;
							default :	attribs_name = "VB{Posf3}";								break;	// flat shading
						}
						_FindPipelinesByVB( attribs_name, OUT pipelines );						// throw
						_FindPipelinesByLayout( "model.pl", INOUT pipelines );					// throw

					//	_FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );	// throw
						_FindPipelinesByMaterial( *mtr, INOUT pipelines );						// throw

						if ( not pipelines.empty() )
							break;
					}
					catch (...)
					{}
				}

				ScriptGeomSource::PipelineNames_t	ppln_name;
				DSLayoutName						mtr_dsl;
				_GetSuitablePipelineAndDS( pipelines, c_MtrDS, OUT ppln_name, OUT mtr_dsl );  // throw

				if ( shared_mtr_dsl == Default )
					shared_mtr_dsl = mtr_dsl;	// init
				else
					CHECK_THROW_MSG( shared_mtr_dsl == mtr_dsl, "All pipelines must use the same DescriptorSetLayout" );

				ppln_per_obj.emplace_back( ppln_name[0].pplnName, obj_id-1 );
			});

		if ( not ppln_per_obj.empty() )
			CHECK_THROW_MSG( shared_mtr_dsl.IsDefined() );

		return ppln_per_obj;
	}

/*
=================================================
	ToMaterial
=================================================
*/
	RC<IGSMaterials>  ScriptModelGeometrySrc::ToMaterial (const ERenderLayer layer, RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
	{
		CHECK_THROW( _geomSrc );
		CHECK_THROW( rtech );

		switch_enum( layer )
		{
			case ERenderLayer::Opaque :			break;
			case ERenderLayer::Translucent :	break;
			case ERenderLayer::PostProcess :	return _ToPostProcessMaterial( rtech, names );
			case ERenderLayer::_Count :
			default :							CHECK_THROW_MSG( false, "unknown layer" );  break;
		}
		switch_end

		auto		result		= MakeRC<ModelGeomSource::Material>();
		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		result->rtech = rtech;

		if ( names.empty() )
			return result;

		Array<GraphicsPipelineID>	pipelines;

		_intermScene->ForEachModel(
			[&, i = 0u, obj_id = 0u, init_ds = true] (const ResLoader::IntermScene::ModelData &model) M_Th___
			{
				++obj_id;

				auto	mesh	= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
				auto	mtr		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
				CHECK_THROW( mesh and mtr );

				if ( i >= names.size() or names[i].objId != obj_id-1 )
					return;

				auto	ppln = rtech->GetGraphicsPipeline( names[i].pplnName );
				CHECK_THROW( ppln );

				#if PIPELINE_STATISTICS
				{
					auto&	res = res_mngr.GetResourcesOrThrow( ppln );
					Unused( res_mngr.GetDevice().PrintPipelineExecutableInfo( _dbgName, res.Handle(), res.Options() ));
				}
				#endif

				if ( init_ds )
				{
					init_ds = false;
					CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), max_frames, ppln, DescriptorSetName{c_MtrDS} ));

					result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );  // throw
				}

				pipelines.push_back( ppln );
				++i;
			});

		CHECK_THROW( pipelines.size() == names.size() );
		CHECK_THROW( result->passDSIndex );
		CHECK_THROW( result->descSets[0] );

		auto&	draw_groups	= result->drawGroups.emplace< ModelGeomSource::Material::GPplnGroups_t >();

		for (usize i = 0; i < names.size(); ++i) {
			draw_groups[ pipelines[i] ].push_back( uint(names[i].objId) );
		}

		return result;
	}

/*
=================================================
	_AttribsToVBName
----
	vertex buffer types defined in
	[file](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/ModelReflection.as)
=================================================
*/
	String  ScriptModelGeometrySrc::_AttribsToVBName (const ResLoader::IntermVertexAttribs &attribs) __Th___
	{
		using namespace AE::ResLoader;

		const bool	has_position	= attribs.HasVertex( VertexAttributeName::Position,		EVertexType::Float3 );
		const bool	has_normal		= attribs.HasVertex( VertexAttributeName::Normal,		EVertexType::Float3 );
	//	const bool	has_tangent		= attribs.HasVertex( VertexAttributeName::Tangent,		EVertexType::Float3 );
	//	const bool	has_bitangent	= attribs.HasVertex( VertexAttributeName::BiTangent,	EVertexType::Float3 );
		const bool	has_uv0_2		= attribs.HasVertex( VertexAttributeName::TextureUVs[0],EVertexType::Float2 );

		if ( has_position and has_normal and has_uv0_2 )
			return "VB{Posf3, Normf3, UVf2}";

		if ( has_position and has_uv0_2 )
			return "VB{Posf3, UVf2}";

		if ( has_position and has_normal )
			return "VB{Posf3, Normf3}";

		if ( has_position )
			return "VB{Posf3}";

		CHECK_THROW_MSG( false, "No compatible attributess found." );
	}


} // AE::ResEditor
