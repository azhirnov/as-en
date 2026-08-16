// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"

#include "Scripting/ScriptBasePass.cpp.h"
#include "_data/cpp/types.h"

#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
	using DebugModeBits = EnumSet<IPass::EDebugMode>;

/*
=================================================
	constructor
=================================================
*/
	ScriptSceneGraphicsPass::ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName) __Th___ :
		_scene{scene}, _subpassName{passName}, _passName{passName}
	{
		_dbgName = passName;
		StringToColor( OUT _dbgColor, StringView{_dbgName} );

		ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
	}

/*
=================================================
	AddPipeline
=================================================
*/
	void  ScriptSceneGraphicsPass::AddPipeline (const String &pplnFile) __Th___
	{
		Path	file_path = ScriptExe::ScriptPassApi::ToPipelinePath( Path{pplnFile} );  // throw

		if ( _uniquePplns.insert( file_path ).second )
			_pipelines.push_back( file_path );
	}

/*
=================================================
	AddPipelines
=================================================
*/
	void  ScriptSceneGraphicsPass::AddPipelines (const String &pplnsFolder) __Th___
	{
		Path	folder = ScriptExe::ScriptPassApi::ToPipelineFolder( Path{pplnsFolder} );  // throw

		for (auto& path : FileSystem::EnumRecursive( folder )) {
			if ( _uniquePplns.insert( path ).second )
				_pipelines.push_back( path );
		}
	}

/*
=================================================
	SetLayer
=================================================
*/
	void  ScriptSceneGraphicsPass::SetLayer (ERenderLayer layer) __Th___
	{
		_renderLayer = layer;
	}

/*
=================================================
	SetFragmentShadingRate
=================================================
*/
	void  ScriptSceneGraphicsPass::SetFragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp) __Th___
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		CHECK_THROW_MSG( fs.pipelineFragmentShadingRate		== FeatureSet::EFeature::RequireTrue	or
						 fs.primitiveFragmentShadingRate	== FeatureSet::EFeature::RequireTrue	or
						 fs.attachmentFragmentShadingRate	== FeatureSet::EFeature::RequireTrue,
			"Fragment shading rate is not supported." );

		_shadingRate.rate			= rate;
		_shadingRate.primitiveOp	= primitiveOp;
		_shadingRate.textureOp		= textureOp;
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  ScriptSceneGraphicsPass::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) __Th___
	{
		arg.state |= EResourceState::FragmentShader;
	}

/*
=================================================
	NextSubpass
=================================================
*/
	void  ScriptSceneGraphicsPass::NextSubpass1 () __Th___
	{
		NextSubpass2( String{_passName} << '-' << ToString(_subpasses.size()+1) );
	}

	void  ScriptSceneGraphicsPass::NextSubpass2 (const String &passName) __Th___
	{
		auto&	dst = _subpasses.emplace_back();
		dst = ScriptSceneGraphicsSubpassPtr{ new ScriptSceneGraphicsSubpass{} };

		_MoveTo( OUT *dst );

		_subpassName = passName;
	}

/*
=================================================
	_MoveTo
=================================================
*/
	void  ScriptSceneGraphicsPass::_MoveTo (OUT ScriptSceneGraphicsSubpass &dst) __NE___
	{
		// save state
		ScriptBaseControllerPtr	tmp_controller	= _controller;
		ScriptDynamicDimPtr		tmp_dynamicDim	= RVRef( _dynamicDim );
		ScriptDynamicUIntPtr	tmp_repeatCount	= RVRef( _repeatCount );
		auto					tmp_enablePass	= RVRef( _enablePass );
		String					tmp_dbgName		= RVRef( _dbgName );
		RGBA8u					tmp_dbgColor	= _dbgColor;
		ScriptDynamicFloatPtr	tmp_passTime	= RVRef( _passTime );

		// move
		_dbgName = this->_subpassName;
		ScriptBaseRenderPass::_MoveTo( OUT dst );

		dst._scene			= this->_scene;
		dst._passName		= RVRef( this->_subpassName );	this->_subpassName.clear();

		dst._pipelines		= RVRef( this->_pipelines );	this->_pipelines.clear();
		dst._uniquePplns	= RVRef( this->_uniquePplns );	this->_uniquePplns.clear();

		dst._renderLayer	= this->_renderLayer;			this->_renderLayer	= ERenderLayer::Opaque;
		dst._shadingRate	= this->_shadingRate;			this->_shadingRate	= Default;

		// restore state
		_controller		= RVRef( tmp_controller );
		_dynamicDim		= RVRef( tmp_dynamicDim );
		_repeatCount	= RVRef( tmp_repeatCount );
		_enablePass		= RVRef( tmp_enablePass );
		_dbgName		= RVRef( tmp_dbgName );
		_dbgColor		= tmp_dbgColor;
		_passTime		= RVRef( tmp_passTime );
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptSceneGraphicsPass::ToPass () __Th___
	{
		// add last subpass
		{
			auto&	dst = _subpasses.emplace_back();
			dst = ScriptSceneGraphicsSubpassPtr{ new ScriptSceneGraphicsSubpass{} };
			_MoveTo( OUT *dst );
		}
		ASSERT( _output.empty() );

		// merge attachments between subpasses
		{
			HashMap< String, Output* >	unique_rt;
			_output.reserve( GraphicsConfig::MaxAttachments );

			for (auto& subpass : _subpasses)
			{
				for (auto& out : subpass->_output)
				{
					CHECK_THROW( not out.name.empty() );

					auto	it = unique_rt.find( out.name );
					if ( it == unique_rt.end() )
					{
						it = unique_rt.emplace( out.name, &_output.emplace_back() ).first;

						it->second->name		= out.name;
						it->second->inName		= subpass->_passName;
						it->second->rt			= out.rt;
						it->second->layer		= out.layer;
						it->second->layerCount	= out.layerCount;
						it->second->mipmap		= out.mipmap;

						if ( IsFirstElement( subpass, _subpasses ))
						{
							it->second->loadOp	= out.loadOp;
							it->second->clear	= out.clear;
						}

						if ( IsLastElement( subpass, _subpasses ))
							it->second->storeOp = out.storeOp;
					}
					else
					{
						// merge
						const String	msg =	"Attachment '"s << out.name << "' in subpass '" << subpass->_passName << "' doesn't match with same attachment name, "
												"which previously added by subpass '" << it->second->inName << "'.\n";

						CHECK_THROW_MSG( it->second->rt == out.rt,
							String{msg} << "New render target '" << out.rt->GetName() << "' is not equal to previous '" << it->second->rt->GetName() << "'." );

						CHECK_THROW_MSG( it->second->layer == out.layer,
							String{msg} << "New base layer (" << ToString( out.layer.Get() ) << ") is not equal to previous (" << ToString( it->second->layer.Get() ) << ")." );

						CHECK_THROW_MSG( it->second->layerCount == out.layerCount,
							String{msg} << "New layer count (" << ToString( out.layerCount ) << ") is not equal to previous (" << ToString( it->second->layerCount ) << ")." );

						CHECK_THROW_MSG( it->second->mipmap == out.mipmap,
							String{msg} << "New mipmap level (" << ToString( out.mipmap.Get() ) << ") is not equal to previous (" << ToString( it->second->mipmap.Get() ) << ")." );

						if ( not IsFirstElement( subpass, _subpasses ))
						{
							CHECK_THROW_MSG( out.loadOp == EAttachmentLoadOp::Load,
								"Attachment '"s << out.name << "' in subpass '" << subpass->_passName << "' must have loadOp = Load, other ops will be ignored" );
						}

						if ( not IsLastElement( subpass, _subpasses ))
						{
							CHECK_THROW_MSG( out.storeOp == EAttachmentStoreOp::Store,
								"Attachment '"s << out.name << "' in subpass '" << subpass->_passName << "' must have storeOp = Store, other ops will be ignored" );
						}
					}
				}
			}
			ASSERT( _output.size() == unique_rt.size() );
		}

		RC<SceneGraphicsPass>	result = MakeRC<SceneGraphicsPass>();

		result->_rtech = _CompilePipelines( OUT result->_scene );	// throw

		// create subpasses
		for (auto& src : _subpasses)
		{
			if ( not src->_dynamicDim )
				src->_dynamicDim = this->_dynamicDim;

			auto	subpass = src->_ToPass2( *result );	// throw

			result->_subpasses.push_back( RVRef(subpass) );
		}

		// init render pass description
		uint	min_layer_count = UMax;
		for (auto& src : _output)
		{
			const uint	layers	= src.rt->Description().arrayLayers.Get();
			const uint	count	= (src.layerCount == UMax ? (layers - src.layer.Get()) : src.layerCount);

			CHECK_THROW( src.layer.Get() < layers );
			CHECK_THROW( src.layer.Get() + count <= layers );

			AssignMin( INOUT min_layer_count, count );
		}
		CHECK_THROW( min_layer_count > 0 );

		result->_rpDesc.renderPassName	= RenderPassName{"rp"};
		result->_rpDesc.subpassName		= SubpassName{ _subpasses.size() == 1 ? "main" : "pass-0" };
		result->_rpDesc.packId			= result->_rtech.packId;
		result->_rpDesc.layerCount		= ImageLayer{min_layer_count};
		result->_rpDesc.area			= RectI{0,0,1,1};

		// create render targets
		for (usize i = 0; i < _output.size(); ++i)
		{
			auto&	src	= _output[i];
			auto	rt	= src.rt->ToResource();
			CHECK_THROW( rt );

			ImageViewDesc		view;
			view.viewType		= (min_layer_count > 1 ? EImage_2DArray : EImage_2D);
			view.baseLayer		= src.layer;
			view.layerCount		= ushort(min_layer_count);
			view.baseMipmap		= src.mipmap;
			view.mipmapCount	= 1;

			rt = rt->CreateView( view, rt->GetName() );
			CHECK_THROW( rt );

			auto&	dst = result->_renderTargets.emplace_back();
			dst.name	= AttachmentName{src.name};
			dst.image	= rt;
			dst.clear	= src.clear;
		}
		CHECK_THROW( not result->_renderTargets.empty() );

		_Init( *result, _scene->GetController() );
		return result;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSceneGraphicsPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptSceneGraphicsPass>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );

		_BindBase( binder, True{"with args"} );
		_BindBaseRenderPass( binder, False{"without blending"}, True{"with RWAttachment"} );

		binder.Comment( "Add path to single pipeline or folder with pipelines.\n"
						"Scene geometry will be linked with compatible pipeline or error will be generated.\n"
						"Use star symbol: '*<file-suffix>' to add current file path without extension to suffix." );
		AS_METHOD( binder, ScriptSceneGraphicsPass::AddPipeline,			"AddPipeline",			{"pplnFile"} );
		AS_METHOD( binder, ScriptSceneGraphicsPass::AddPipelines,			"AddPipelines",			{"pplnFolder"} );

		AS_METHOD( binder, ScriptSceneGraphicsPass::SetLayer,				"Layer",				{} );
		AS_METHOD( binder, ScriptSceneGraphicsPass::SetFragmentShadingRate,	"FragmentShadingRate",	{"rate", "primitiveOp", "textureOp"} );

		binder.Comment( "Can be used only if pass hasn't attachments." );
		AS_METHOD( binder, ScriptSceneGraphicsPass::_SetDynamicDimension2,	"SetDimension",			{} );

		//binder.Comment( "" );
		AS_METHOD( binder, ScriptSceneGraphicsPass::NextSubpass1,			"NextSubpass",			{} );
		AS_METHOD( binder, ScriptSceneGraphicsPass::NextSubpass2,			"NextSubpass",			{"passName"} );
	}

/*
=================================================
	_CreateUBType
=================================================
*/
	auto  ScriptSceneGraphicsPass::_CreateUBType () __Th___
	{
		using namespace AE::PipelineCompiler;

		auto&	obj_storage = *ObjectStorage::Instance();
		auto	it			= obj_storage.structTypes.find( "SceneGraphicsPassUB" );

		if ( it != obj_storage.structTypes.end() )
			return it->second;

		ShaderStructTypePtr	st = ShaderStructType::Create( "SceneGraphicsPassUB" );
		st->Set( EStructLayout::Compatible_Std140, R"#(
				// view //
				float2		resolution;				// viewport resolution (in pixels)
				float2		invResolution;			// 1.0/resolution, used for optimization
				float		time;					// shader playback time (in seconds)
				float		timeDelta;				// frame render time (in seconds), max value: 1/30s
				uint		frame;					// shader playback frame, global frame counter
				uint		seed;					// unique value, updated on each shader reloading
				float4		mouse;					// mouse unorm coords. xy: current (if MRB down), zw: click
				float2		customKeys;
				float		pixPerMm;				// pix / mm
				float		mmPerPix;				// mm / pix

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
		StaticAssert( IPass::CustomKeys_t{}.max_size() == 2 );
		return st;
	}

/*
=================================================
	GetShaderTypes
=================================================
*/
	void  ScriptSceneGraphicsPass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
	{
		auto	st = _CreateUBType();	// throw

		CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
	}

/*
=================================================
	_CompilePipelines2
=================================================
*/
	void  ScriptSceneGraphicsPass::_CompilePipelines2 (ScriptEnginePtr se, ArrayView<Output> attachments,
														ArrayView<ScriptSceneGraphicsSubpassPtr> subpasses,
														const ScriptScene &scene,
														const ScriptDynamicDim* dynamicDim) __Th___
	{
		using namespace AE::PipelineCompiler;

		const auto	SubpassName = [&subpasses] (auto& subpass) -> String
		{{
			if ( subpasses.size() == 1 )
				return "main"s;
			else
				return "pass-"s << ToString( std::distance( subpasses.data(), &subpass ));
		}};

		for (auto& subpass : subpasses)
		{
			subpass->_args.ValidateArgs();
		}

		auto&	storage = *ObjectStorage::Instance();

		// compatible render pass
		CompatibleRenderPassDescPtr	compat_rp = CompatibleRenderPassDesc::Create( "compat.rp" );

		for (auto& subpass : subpasses) {
			compat_rp->AddSubpass( SubpassName( subpass ));
		}

		for (auto [out2, i] : WithIndex(attachments))
		{
			RPAttachmentPtr		att		= compat_rp->AddAttachment2( out2.name );
			const auto			desc	= out2.rt->ToResource()->GetImageDesc();

			att->format		= desc.format;
			att->samples	= desc.samples;

			for (auto& subpass : subpasses)
			{
				const auto	sp_name = SubpassName( subpass );

				for (auto [out, _] : WithIndex(subpass->_output))
				{
					if ( out.name != out2.name )
						continue;

					if ( not out.inName.empty() and out.usage == EResourceUsage::InputAttachment )
					{
						att->AddUsage2( sp_name, EAttachment::Input,
										RPAttachment::ShaderIO{ out.inName, Default, uint(i) });
						break;
					}

					if ( not out.inName.empty() )
					{
						ASSERT( out.usage == (EResourceUsage::InputAttachment | EResourceUsage::DepthStencil) or
								out.usage == (EResourceUsage::InputAttachment | EResourceUsage::ColorAttachment) );
						att->AddUsage3( sp_name, EAttachment::ReadWrite,
										RPAttachment::ShaderIO{ out.inName, Default, uint(i) },
										RPAttachment::ShaderIO{ out.name,   Default, uint(i) });
						break;
					}

					if ( out.usage == EResourceUsage::FragShadingRate )
					{
						auto	dim = out.rt->DynamicDimension();
						CHECK_THROW( dim );
						CHECK_THROW( dynamicDim != null and dynamicDim->Get() );
						CHECK_THROW( dynamicDim->Get() == dim->BaseDimensionRC() );	// scale must depends on other attachments dimension

						int2	scale {-dim->Scale()};
						CHECK_THROW( All( scale > Zero ));

						att->AddUsage4( sp_name, EAttachment::ShadingRate, packed_uint2(scale) );
						break;
					}

					EAttachment		type = out.rt->IsDepthOrStencil() ? EAttachment::DepthStencil : EAttachment::Color;
					if ( out.usage == EResourceUsage::FragDensityMap )
						type = EAttachment::FragmentDensity;

					att->AddUsage( sp_name, type );
					break;
				}

				if ( not att->usageMap.contains( Graphics::SubpassName{sp_name} ))
					att->AddUsage( sp_name, EAttachment::Preserve );
			}
		}


		// render pass specialization
		RenderPassSpecPtr	rp_spec = compat_rp->AddSpecialization2( "rp" );

		for (auto [out2, __] : WithIndex(attachments))
		{
			RPAttachmentSpecPtr	att = rp_spec->AddAttachment2( out2.name );

			att->loadOp		= out2.loadOp;
			att->storeOp	= out2.storeOp;

			for (auto& subpass : subpasses)
			{
				const auto	sp_name = SubpassName( subpass );

				for (auto [out, _] : WithIndex(subpass->_output))
				{
					if ( out.name != out2.name )
						continue;

					auto		state			= EResourceState::ColorAttachment;
					const bool	is_ds			= out.rt->IsDepthOrStencil();
					const bool	is_rw_att		= AllBits( out.usage, EResourceUsage::InputAttachment ) and
												  AnyBits( out.usage, EResourceUsage::ColorAttachment | EResourceUsage::DepthStencil );
					const bool	is_input_att	= AllBits( out.usage, EResourceUsage::InputAttachment ) and not is_rw_att;

					if ( is_ds )
					{
						state = EResourceState::DepthStencilAttachment_RW;

						if ( subpasses.size() == 1 and out.loadOp == EAttachmentLoadOp::Load and out.storeOp == EAttachmentStoreOp::None )
							state = EResourceState::DepthStencilTest;	// read-only
					}

					// input attachment
					if ( not out.inName.empty() )
					{
						ASSERT( is_input_att or is_rw_att );
						state = is_rw_att ?
								(is_ds ? EResourceState::InputDepthStencilAttachment_RW : EResourceState::InputColorAttachment_RW) :
								(is_ds ? EResourceState::InputDepthStencilAttachment : EResourceState::InputColorAttachment);
						state |= EResourceState::FragmentShader;
					}

					if ( is_ds and not is_input_att )
					{
						state |= EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS;

						// can be read-only
						for (auto& arg : subpass->_args.Args())
						{
							if ( auto* ds_arg = UnionGet<ScriptImagePtr>( arg.res );  ds_arg and ScriptImage::IsSame( out.rt, *ds_arg ))
							{
								EResourceState	state_only = arg.state & ~EResourceState::AllStages;
								CHECK_THROW_MSG( state_only == EResourceState::ShaderSample					or
												 state_only == EResourceState::DepthStencilTest				or
												 state_only == EResourceState::DepthStencilTest_ShaderSample,
									"Can not use depth attachment '"s << arg.name << "' in shader when access is not 'ShaderSample'." );

								CHECK_THROW_MSG( out.loadOp == EAttachmentLoadOp::Load and
												 (out.storeOp == EAttachmentStoreOp::None or out.storeOp == EAttachmentStoreOp::Store),
									"For depth attachment '"s << arg.name << "', which is accessed in shader, loadOp must be 'Load' and storeOp must be 'None' or 'Store'." );

								arg.state	= EResourceState::DepthStencilTest_ShaderSample | EResourceState::FragmentShader |
											  EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS;
								state		= arg.state;
							}
						}
					}

					switch ( out.usage ) {
						case EResourceUsage::FragShadingRate :	state = EResourceState::ShadingRateImage;		break;
						case EResourceUsage::FragDensityMap :	state = EResourceState::FragmentDensityMap;		break;
					}

					if ( IsFirstElement( subpass, subpasses ) and out.loadOp == EAttachmentLoadOp::Clear )
						att->AddLayout( "ExternalIn", EResourceState::Invalidate | state );

					att->AddLayout( sp_name, state );
					break;
				}

				if ( not att->layouts.contains( Graphics::SubpassName{sp_name} ))
					att->AddLayout( sp_name, EResourceState::Preserve );
			}
		}

		RenderTechniquePtr	rtech = RenderTechnique::Create( "rtech" );
		{
			for (auto& subpass : subpasses)
			{
				const auto			sp_name = SubpassName( subpass );
				RTGraphicsPassPtr	pass	= rtech->AddGraphicsPass2( sp_name );
				pass->SetRenderPass( "rp", sp_name );
			}
		}

		// descriptor set layout
		{
			Unused( _CreateUBType() );	// throw

			DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( "pass.ds" );
			const auto				stage		= EShaderStages::AllGraphics;

			ds_layout->AddUniformBuffer( EShaderStages::AllGraphics, "un_PerPass", ArraySize{1}, "SceneGraphicsPassUB", EResourceState::ShaderUniform, False{} );

			if ( subpasses.size() == 1 )
				subpasses.front()->_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw
		}

		// descriptor set layout with input attachment
		for (auto& subpass : subpasses)
		{
			bool	skip = subpasses.size() == 1 or subpass->_args.Empty();

			for (auto [out, _] : WithIndex(subpass->_output))
			{
				if ( not out.inName.empty() )
				{
					skip = false;
					break;
				}
			}
			if ( skip )
			{
				subpass->_dslName = "pass.ds";
				continue;
			}

			subpass->_dslName = "subpass-"s << ToString( std::distance( subpasses.data(), &subpass )) << ".ds";

			DescriptorSetLayoutPtr	ds_layout	= DescriptorSetLayout::Create( subpass->_dslName );
			const auto				stage		= EShaderStages::AllGraphics;

			ds_layout->AddUniformBuffer( EShaderStages::AllGraphics, "un_PerPass", ArraySize{1}, "SceneGraphicsPassUB", EResourceState::ShaderUniform, False{} );
			subpass->_args.ArgsToDescSet( stage, ds_layout, ArraySize{1} );  // throw

			for (auto [out, _] : WithIndex(subpass->_output))
			{
				if ( out.inName.empty() ) continue;

				usize	i = 0;
				for (; i < attachments.size(); ++i) {
					if ( attachments[i].name == out.name )
						break;
				}

				bool	is_ds			= out.rt->IsDepthOrStencil();
				bool	is_rw_att		= AllBits( out.usage, EResourceUsage::InputAttachment ) and
										  AnyBits( out.usage, EResourceUsage::ColorAttachment | EResourceUsage::DepthStencil );
				bool	is_input_att	= AllBits( out.usage, EResourceUsage::InputAttachment ) and not is_rw_att;
				ASSERT( is_input_att or is_rw_att );

				auto	state			= is_rw_att ?
											(is_ds ? EResourceState::InputDepthStencilAttachment_RW : EResourceState::InputColorAttachment_RW) :
											(is_ds ? EResourceState::InputDepthStencilAttachment : EResourceState::InputColorAttachment);

				if ( is_ds and not is_input_att )
					state |= EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS;

				ds_layout->AddSubpassInput( EShaderStages::Fragment, out.inName, uint(i), out.rt->ImageType(), state | EResourceState::FragmentShader );
			}
		}

		for (auto& inst : scene._geomInstances) {
			inst.geom->AddLayoutReflection();  // throw
		}

		auto	include_dirs = ScriptExe::ScriptPassApi::GetPipelineIncludeDirs();
		CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "VertexInput.as" ), include_dirs ));

		if ( GraphicsScheduler().GetFeatureSet().bufferDeviceAddress == FeatureSet::EFeature::RequireTrue )
			CHECK_THROW( storage.CompilePipeline( se, ScriptExe::ScriptPassApi::ToPipelinePath( "ModelShared.as" ), include_dirs ));

		String	prev_defs = RVRef(storage.defaultShaderDefines);
		for (auto& subpass : subpasses)
		{
			storage.defaultShaderDefines = prev_defs;
			subpass->_AddSlidersAsMacros( INOUT storage.defaultShaderDefines );

			for (auto& ppln : subpass->_pipelines) {
				CHECK_THROW( storage.CompilePipeline( se, ppln, include_dirs ));
			}
		}
		storage.defaultShaderDefines = RVRef(prev_defs);

		CHECK_THROW( not storage.gpipelines.empty() or
					 not storage.mpipelines.empty() );

		for (auto& subpass : subpasses)
		{
			auto&	ppln_names = subpass->_pplnPerInst;
			ppln_names.clear();
			ppln_names.reserve( scene._geomInstances.size() );

			for (auto& inst : scene._geomInstances)
			{
				uint	sp_idx	= uint(std::distance( subpasses.data(), &subpass ));
				auto	names	= inst.geom->FindMaterialGraphicsPipelines( subpass->_renderLayer, sp_idx );  // throw
				ppln_names.push_back( RVRef(names) );
			}
			ASSERT( scene._geomInstances.size() == ppln_names.size() );

			// check if DSL was renamed
			auto	it = storage.dsLayouts.find( subpass->_dslName );
			CHECK_THROW( it != storage.dsLayouts.end() );

			auto*	dsl = storage.pplnStorage->GetDescriptorSetLayout( it->second->UID() );
			CHECK_THROW( dsl != null );

			subpass->_dslName = storage.GetName( dsl->name );
		}
	}

/*
=================================================
	_CompilePipelines
=================================================
*/
	RTechInfo  ScriptSceneGraphicsPass::_CompilePipelines (OUT RC<SceneData> &outScene) C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this, &outScene] (ScriptEnginePtr se)
					{
						outScene = _scene->ToScene();												// throw
						_CompilePipelines2( se, _output, _subpasses, *_scene, _dynamicDim.Get() );	// throw
					},
					_baseFlags );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_ToPass2
=================================================
*/
	RC<SceneGraphicsSubpass>  ScriptSceneGraphicsSubpass::_ToPass2 (SceneGraphicsPass &graphicsPass) __Th___
	{
		CHECK_THROW_MSG( not _pipelines.empty(), "pipelines must be defined" );

		RC<SceneGraphicsSubpass>	result		= MakeRC<SceneGraphicsSubpass>();
		auto&						res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&						materials	= result->_materials;
		const auto					max_frames	= GraphicsScheduler().GetMaxFrames();
		DebugModeBits				dbg_modes;

		CHECK_THROW( _pplnPerInst.size() == _scene->_geomInstances.size() );
		materials.reserve( _pplnPerInst.size() );

		for (usize i = 0; i < _pplnPerInst.size(); ++i)
		{
			const auto&		geom	= _scene->_geomInstances[i].geom;
			const auto&		pplns	= _pplnPerInst[i];

			if ( pplns.empty() )
			{
				materials.emplace_back();
				continue;
			}

			auto	mtr = geom->ToMaterial( _renderLayer, graphicsPass._rtech.rtech, pplns );  // throw
			CHECK_THROW( mtr );

			materials.push_back( mtr );
			dbg_modes |= mtr->GetDebugModeBits();
		}

		CHECK_THROW( materials.size() == _scene->_geomInstances.size() );

		result->_scene			= graphicsPass._scene;
		result->_renderLayer	= this->_renderLayer;
		result->_shadingRate	= this->_shadingRate;
		result->_scissors		= this->_scissors;
		result->_viewports		= this->_viewports;

		if ( result->_viewports.empty() )
			result->_viewports.push_back( Viewport{ RectF{0.f, 0.f, 1.f, 1.f}, this->_depthRange.x, this->_depthRange.y });

		if ( result->_scissors.empty() )
			result->_scissors.push_back( RectF{0.f, 0.f, 1.f, 1.f} );

		result->_ubuffer = _CreateUBuffer( SizeOf<ShaderTypes::SceneGraphicsPassUB>, "SceneGraphicsPassUB",
											EResourceState::UniformRead | EResourceState::AllGraphicsShaders );  // throw

		// create descriptor set
		CHECK_THROW_MSG( res_mngr.CreateDescriptorSets( OUT result->_descSets.data(), max_frames, graphicsPass._rtech.packId, DSLayoutName{_dslName}, null, _dbgName ),
			"Failed to create descriptor set with layout '"s << _dslName << "' for pass '" << _passName << "'." );

		_args.InitResources( OUT result->_resources, graphicsPass._rtech.packId );  // throw

		for (auto [out, i] : WithIndex(_output))
		{
			if ( out.inName.empty() ) continue;

			bool	is_ds = out.rt->IsDepthOrStencil();
			auto	state = out.usage == EResourceUsage::InputAttachment ?
								(is_ds ? EResourceState::InputDepthStencilAttachment : EResourceState::InputColorAttachment) :
								(is_ds ? EResourceState::InputDepthStencilAttachment_RW : EResourceState::InputColorAttachment_RW);

			result->_resources.Add( UniformName{out.inName}, out.rt->ToResource(), state | EResourceState::FragmentShader );
		}

		_Init( *result, _scene->GetController(), False{"disable log"} );
		UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::AllGraphics );

		return result;
	}


} // AE::ResEditor
