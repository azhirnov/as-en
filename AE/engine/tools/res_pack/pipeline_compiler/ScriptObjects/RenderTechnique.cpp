// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/RenderTechnique.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static RenderTechnique*  RenderTechnique_Ctor (const String &name) {
		return RenderTechniquePtr{ new RenderTechnique{ name }}.Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RTBasePass::RTBasePass (RenderTechnique* rtech, const String &name, usize passIdx) __Th___ :
		_name{name},
		_passIndex{passIdx},
		_rtech{rtech}
	{
		ObjectStorage::Instance()->AddName<RenderTechPassName>( name );
	}

/*
=================================================
	SetRenderState1
=================================================
*/
	void  RTBasePass::_SetDSLayout (const String &typeName) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );
		CHECK_THROW_MSG( _pipelineRefs.empty() );
		CHECK_THROW_MSG( not _dsLayout );

		auto&	storage = *ObjectStorage::Instance();
		auto	iter	= storage.dsLayouts.find( typeName );
		CHECK_THROW_MSG( iter != storage.dsLayouts.end(),
			"DescriptorSetLayout '"s << typeName << "' is not exists" );

		_dsLayout = iter->second;
	}

/*
=================================================
	AddPipeline
=================================================
*/
	void  RTBasePass::AddPipeline (const BasePipelineSpecPtr &ptr) __Th___
	{
		if ( _dsLayout )
		{
			auto	layout = ptr->GetBase()->GetLayout();
			CHECK_THROW_MSG( layout );

			auto&	desc_set = layout->GetDesc().descrSets;
			bool	found	 = false;

			for (auto ds : desc_set)
			{
				if ( ds.second.vkIndex == 0 )
				{
					CHECK_THROW_MSG( ds.second.uid == _dsLayout->UID(),
						"Incompatible Global DS layout in RTech pass '"s << _name << "' in pipeline '" <<
						ObjectStorage::Instance()->GetName( ptr->Name() ) << "'" );
					found = true;
				}
			}
			CHECK_THROW_MSG( found,
				"DescriptorSetLayout at index 0 must match with specified layout: '"s << _dsLayout->Name() << "'" );
		}

		{
			auto	feats = ptr->GetBase()->GetFeatures();

			_rtech->_features.insert( _rtech->_features.end(), feats.begin(), feats.end() );
			ScriptFeatureSet::Minimize( INOUT _rtech->_features );
		}

		_pipelineRefs.emplace( ptr );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetRenderState1
=================================================
*/
	void  RTGraphicsPass::SetRenderState1 (const RenderState &value) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );
		// TODO
		Unused( value );
	}

/*
=================================================
	SetRenderState2
=================================================
*/
	void  RTGraphicsPass::SetRenderState2 (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );
		// TODO
		Unused( name );
	}

/*
=================================================
	SetMutableStates
=================================================
*/
	void  RTGraphicsPass::SetMutableStates (EMutableRenderState values) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );
		// TODO
		Unused( values );
	}

/*
=================================================
	AddPipeline
=================================================
*/
	void  RTGraphicsPass::AddPipeline (const BasePipelineSpecPtr &ptr) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );

		auto*	gppln	= DynCast< GraphicsPipelineSpecScriptBinding >(ptr.Get());
		auto*	mppln	= DynCast< MeshPipelineSpecScriptBinding >(ptr.Get());
		CHECK_THROW_MSG( gppln != null or mppln != null,
			"must be graphics or mesh pipeline" );

		if ( gppln != null )
			gppln->SetRenderPass( _renderPass, _subpass );

		if ( mppln != null )
			mppln->SetRenderPass( _renderPass, _subpass );

		RTBasePass::AddPipeline( ptr );
	}

/*
=================================================
	SetRenderPass
=================================================
*/
	void  RTGraphicsPass::SetRenderPass (const String &rpName, const String &subpass) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );

		auto&	storage		= *ObjectStorage::Instance();
		auto	compat_rp	= storage.RenderPassExists( rpName, subpass ); // throw

		_rtech->_features.insert( _rtech->_features.end(), compat_rp->_features.begin(), compat_rp->_features.end() );
		ScriptFeatureSet::Minimize( INOUT _rtech->_features );

		_renderPass	= RenderPassName{rpName};
		_subpass	= SubpassName{subpass};

		// validate
		{
			auto	sp_it = compat_rp->_subpassMap.find( _subpass );
			CHECK_THROW_MSG( sp_it != compat_rp->_subpassMap.end(),
				"render pass '"s << rpName << "' doesn't have subpass '" << subpass << "'" );

			const uint	sp_index = sp_it->second;
			if_unlikely( sp_index > _passIndex )
			{
				String	str = "Previous subpasses of render pass may be missed:\n";
				str << "Current subpass [" << ToString(sp_index) << "] '" << subpass << "'\n";
				str << "Previous subpasses must be:";

				for (uint i = 0; i < sp_index; ++i) {
					str << "\n  [" << ToString(i) << "] '" << storage.GetName( compat_rp->_subpasses[i].name ) << "'";
				}
				CHECK_THROW_MSG( false, str );
			}

			// check previous subpasses
			for (uint i = 0; i < sp_index; ++i)
			{
				auto&	prev	= _rtech->_passes[ _passIndex - sp_index + i ];
				auto*	gpass	= DynCast<RTGraphicsPass>( prev.Get() );
				CHECK_THROW_MSG( gpass != null,
					"render technique '"s << _rtech->_name << "' pass '" << prev->Name() << "' is not a graphics pass, " <<
					ToString(sp_index) << " passes before must be a graphics passes with render pass '" << rpName << "'" );

				CHECK_THROW_MSG( gpass->_renderPass == _renderPass,
					"render technique '"s << _rtech->_name << "' graphics pass '" << prev->Name() << "' with render pass '" <<
					storage.GetName( gpass->_renderPass ) << "' is not compatible with render pass '" << rpName <<
					"' in graphics pass '" << _name << "'" );

				auto	sp_it2 = compat_rp->_subpassMap.find( gpass->_subpass );
				CHECK_THROW_MSG( sp_it2 != compat_rp->_subpassMap.end() );	// should not happen

				CHECK_THROW_MSG( sp_it2->second == i,
					"render technique '"s << _rtech->_name << "' graphics pass '" << prev->Name() << "' with render pass '" <<
					storage.GetName( gpass->_renderPass ) << "' and subpass '" << storage.GetName( gpass->_subpass ) <<
					"' is not compatible with render pass '" << rpName << "' and subpass '" << subpass <<
					"' in graphics pass '" << _name << "'" );
			}
		}
	}

/*
=================================================
	IsFirstSubpass
=================================================
*/
	bool  RTGraphicsPass::IsFirstSubpass () C_NE___
	{
		try {
			auto&	storage		= *ObjectStorage::Instance();
			auto	compat_rp	= storage.RenderPassExists( _renderPass, _subpass ); // throw
			return compat_rp->IsFirstSubpass( _subpass );
		}
		catch (...) {}
		return false;
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	RenderPassSpecPtr  RTGraphicsPass::GetRenderPass () C_NE___
	{
		try {
			auto&	storage	= *ObjectStorage::Instance();
			auto	rp		= storage.GetRenderPass( _renderPass ); // throw
			return rp;
		}
		catch (...) {}
		return null;
	}

/*
=================================================
	GetCompatRenderPass
=================================================
*/
	CompatibleRenderPassDescPtr  RTGraphicsPass::GetCompatRenderPass () C_NE___
	{
		try {
			auto&	storage		= *ObjectStorage::Instance();
			auto	compat_rp	= storage.RenderPassExists( _renderPass, _subpass ); // throw
			return compat_rp;
		}
		catch (...) {}
		return null;
	}

/*
=================================================
	Bind
=================================================
*/
	void  RTGraphicsPass::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RTGraphicsPass>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set RenderPass and Subpass which is used in current pass.\n"
						"All pipelines must be compatible with RenderPass and use same Subpass." );
		binder.AddMethod( &RTGraphicsPass::SetRenderPass,		"SetRenderPass",	{"rp", "subpass"} );

		binder.Comment( "Set default render state.\n"
						"Pipelines can override some parameters." );
		binder.AddMethod( &RTGraphicsPass::SetRenderState1,		"SetRenderState",	{"rs"} );
		binder.AddMethod( &RTGraphicsPass::SetRenderState2,		"SetRenderState",	{"rsName"} );

		binder.Comment( "Set render state mutable state which can be overriden by pipelines." );
		binder.AddMethod( &RTGraphicsPass::SetMutableStates,	"SetMutableStates",	{"states"} );

		binder.Comment( "Set per-pass descriptor set layout.\n"
						"All pipelines must contains this DSLayout." );
		binder.AddMethod( &RTGraphicsPass::SetDSLayout,			"SetDSLayout",		{"dsl"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	AddPipeline
=================================================
*/
	void  RTComputePass::AddPipeline (const BasePipelineSpecPtr &ptr) __Th___
	{
		CHECK_THROW_MSG( not _rtech->HasUID() );

		auto*	cppln	= DynCast< ComputePipelineSpecScriptBinding >(ptr.Get());
		auto*	tppln	= DynCast< TilePipelineSpecScriptBinding >(ptr.Get());
		auto*	rtppln	= DynCast< RayTracingPipelineSpecScriptBinding >(ptr.Get());

		CHECK_THROW_MSG( cppln != null or rtppln != null or tppln != null,
			"must be compute, tile or ray tracing pipeline" );

		RTBasePass::AddPipeline( ptr );
	}

/*
=================================================
	Bind
=================================================
*/
	void  RTComputePass::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RTComputePass>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set per-pass descriptor set layout.\n"
						"All pipelines must contains this DSLayout." );
		binder.AddMethod( &RTComputePass::SetDSLayout,	"SetDSLayout",	{"dsl"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RenderTechnique::RenderTechnique (const String &name) __Th___ :
		_name{ name },
		_features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
	{
		ObjectStorage::Instance()->AddName<RenderTechName>( name );
		CHECK_THROW_MSG( ObjectStorage::Instance()->rtechMap.emplace( name, RenderTechniquePtr{this} ).second,
			"RenderTechnique with name '"s << name << "' is already defined" );
		CHECK_THROW_MSG( ObjectStorage::Instance()->Build() );
	}

/*
=================================================
	AddFeatureSet
=================================================
*/
	void  RenderTechnique::AddFeatureSet (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _uid.has_value() );
		CHECK_THROW_MSG( _passMap.empty() );

		auto&	storage = *ObjectStorage::Instance();
		auto	fs_it	= storage.featureSets.find( FeatureSetName{name} );
		CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
			"FeatureSet with name '"s << name << "' is not found" );

		_features.push_back( fs_it->second );
		ScriptFeatureSet::Minimize( INOUT _features );
	}

/*
=================================================
	AddGraphicsPass
=================================================
*/
	RTGraphicsPassPtr  RenderTechnique::AddGraphicsPass2 (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _uid.has_value() );

		RTGraphicsPassPtr	pass{new RTGraphicsPass{ this, name, _passes.size() }};

		CHECK_THROW_MSG( _passMap.emplace( RenderTechPassName{name}, _passes.size() ).second,
			"render technique pass '"s << name << "' already exists" );
		_passes.push_back( pass );

		return pass;
	}

	RTGraphicsPass*  RenderTechnique::AddGraphicsPass (const String &name) __Th___
	{
		return AddGraphicsPass2( name ).Detach();
	}

/*
=================================================
	AddComputePass
=================================================
*/
	RTComputePassPtr  RenderTechnique::AddComputePass2 (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _uid.has_value() );

		RTComputePassPtr	pass{new RTComputePass{ this, name, _passes.size() }};

		CHECK_THROW_MSG( _passMap.emplace( RenderTechPassName{name}, _passMap.size() ).second,
			"render technique pass '"s << name << "' already exists" );
		_passes.push_back( pass );

		return pass;
	}

	RTComputePass*  RenderTechnique::AddComputePass (const String &name) __Th___
	{
		return AddComputePass2( name ).Detach();
	}

/*
=================================================
	CopyGraphicsPass
=================================================
*/
	RTGraphicsPassPtr  RenderTechnique::CopyGraphicsPass2 (const String &newName, const String &rtechName, const String &passName) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		auto	rt_it	= storage.rtechMap.find( rtechName );
		CHECK_THROW_MSG( rt_it != storage.rtechMap.end(), "render technique '"s << rtechName << "' is not found" );

		auto	src_pass = rt_it->second->GetPass( passName );
		CHECK_THROW_MSG( src_pass, "render technique pass '"s << passName << "' is not found in '" << rtechName << "'" );

		auto*	src_gpass = DynCast<RTGraphicsPass>(src_pass.Get());
		CHECK_THROW_MSG( src_gpass != null, "render technique pass '"s << passName << "' in '" << rtechName << "' is not a graphics pass" );

		_features.insert( _features.end(), rt_it->second->_features.begin(), rt_it->second->_features.end() );
		ScriptFeatureSet::Minimize( INOUT _features );

		RTGraphicsPassPtr	dst_pass = AddGraphicsPass2( newName );

		dst_pass->_dsLayout			= src_gpass->_dsLayout;
		dst_pass->_pipelineRefs		= src_gpass->_pipelineRefs;
		dst_pass->_renderState		= src_gpass->_renderState;
		dst_pass->_mutableStates	= src_gpass->_mutableStates;
		dst_pass->SetRenderPass( storage.GetName( src_gpass->_renderPass ),
								 storage.GetName( src_gpass->_subpass ));

		return dst_pass;
	}

	RTGraphicsPass*  RenderTechnique::CopyGraphicsPass (const String &newName, const String &rtechName, const String &passName) __Th___
	{
		return CopyGraphicsPass2( newName, rtechName, passName ).Detach();
	}

/*
=================================================
	CopyComputePass
=================================================
*/
	RTComputePassPtr  RenderTechnique::CopyComputePass2 (const String &newName, const String &rtechName, const String &passName) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();

		auto	rt_it	= storage.rtechMap.find( rtechName );
		CHECK_THROW_MSG( rt_it != storage.rtechMap.end(), "render technique '"s << rtechName << "' is not found" );

		auto	src_pass = rt_it->second->GetPass( passName );
		CHECK_THROW_MSG( src_pass, "render technique pass '"s << passName << "' is not found in '" << rtechName << "'" );

		auto*	src_cpass = DynCast<RTComputePass>(src_pass.Get());
		CHECK_THROW_MSG( src_cpass != null, "render technique pass '"s << passName << "' in '" << rtechName << "' is not a compute pass" );

		_features.insert( _features.end(), rt_it->second->_features.begin(), rt_it->second->_features.end() );
		ScriptFeatureSet::Minimize( INOUT _features );

		RTComputePassPtr	dst_pass = AddComputePass2( newName );

		dst_pass->_dsLayout		= src_cpass->_dsLayout;
		dst_pass->_pipelineRefs	= src_cpass->_pipelineRefs;

		return dst_pass;
	}

	RTComputePass*  RenderTechnique::CopyComputePass (const String &newName, const String &rtechName, const String &passName) __Th___
	{
		return CopyComputePass2( newName, rtechName, passName ).Detach();
	}

/*
=================================================
	GetPass
=================================================
*/
	RTBasePassPtr  RenderTechnique::GetPass (const String &name)
	{
		auto	iter = _passMap.find( RenderTechPassName{name} );
		if ( iter != _passMap.end() )
			return _passes[ iter->second ];

		return {};
	}

/*
=================================================
	Build
=================================================
*/
	bool  RenderTechnique::Build () __NE___
	{
		if ( _uid.has_value() )
			return true;

		auto&	storage			= *ObjectStorage::Instance();
		auto&	ppln_storage	= *storage.pplnStorage;

		SerializableRenderTechnique	desc;

		desc.name = RenderTechName{_name};

		uint	ppln_spec_idx	= 0;
		uint	ppln_spec_count = 0;

		for (const auto& ptr : _passes) {
			ppln_spec_count += CheckCast<uint>( ptr.Get()->_pipelineRefs.size() );
		}

		if ( ppln_spec_count == 0 )
		{
			AE_LOG_DBG( "skip empty render technique '"s << _name << "'" );
			return true;
		}

		{
			auto*	ppln_specs = storage.allocator.Allocate< SerializableRenderTechnique::PipelineList_t::value_type >( ppln_spec_count );
			CHECK_ERR( ppln_specs != null );

			auto*	passes = storage.allocator.Allocate< SerializableRenderTechnique::Pass >( _passes.size() );
			CHECK_ERR( passes != null );

			desc.passes		= ArrayView< SerializableRenderTechnique::Pass >{ passes, _passes.size() };
			desc.pipelines	= SerializableRenderTechnique::PipelineList_t{ ppln_specs, ppln_spec_count };

			RenderPassName	last_rp;
			SubpassName		last_sp;
			uint			subpass_idx = 0;

			for (usize i = 0; i < _passes.size(); ++i)
			{
				const auto&	src = _passes[i];
				auto&		dst = passes[i];
				PlacementNew<SerializableRenderTechnique::Pass>( OUT &dst );

				dst.name = RenderTechPassName{src->Name()};

				if  ( src->_dsLayout )
					dst.dsLayout = src->_dsLayout->UID();

				if ( auto* gpass = DynCast<RTGraphicsPass>(src.Get()))
				{
					dst.renderPass	= gpass->_renderPass;
					dst.subpass		= gpass->_subpass;
				}
				else
				if ( auto* cpass = DynCast<RTComputePass>(src.Get()))
				{
				}
				else
					RETURN_ERR( "must be graphics or compute pass" );

				// validation
				if ( last_rp == dst.renderPass )
				{
					++subpass_idx;

					if ( last_rp.IsDefined() )
					{
						CompatibleRenderPassDescPtr	compat_rp;
						NOTHROW_ERR( compat_rp = storage.RenderPassExists( last_rp, last_sp ));

						auto	sp_it = compat_rp->_subpassMap.find( last_sp );
						CHECK_ERR( sp_it != compat_rp->_subpassMap.end() );

						auto	sp2_it = compat_rp->_subpassMap.find( dst.subpass );
						CHECK_ERR( sp2_it != compat_rp->_subpassMap.end() );

						const usize	sp_count = compat_rp->_subpasses.size();
						if ( subpass_idx == sp_count ){
							subpass_idx = 0;
						}else{
							CHECK_ERR( subpass_idx < sp_count );
							CHECK_ERR( sp2_it->second == subpass_idx );
						}
					}
					last_sp = dst.subpass;
				}
				else
				{
					if ( last_rp.IsDefined() )
					{
						CompatibleRenderPassDescPtr	compat_rp;
						NOTHROW_ERR( compat_rp = storage.RenderPassExists( last_rp, last_sp ));

						auto	sp_it = compat_rp->_subpassMap.find( last_sp );
						CHECK_ERR( sp_it != compat_rp->_subpassMap.end() );

						const usize	sp_count = compat_rp->_subpasses.size();
						CHECK_ERR_MSG( sp_it->second+1 == sp_count,
							"not an all subpasses of render pass '"s << storage.GetName( last_rp ) << "' are exists" );
					}

					last_rp		= dst.renderPass;
					last_sp		= dst.subpass;
					subpass_idx	= 0;
				}

				for (auto& ppln : src->_pipelineRefs)
				{
					auto	uid = ppln->UID();
					CHECK_ERR( uid != Default );

					ppln_specs[ppln_spec_idx++] = { ppln->Name(), uid };
				}
			}
			ASSERT( ppln_spec_idx == ppln_spec_count );
		}

		ScriptFeatureSet::Minimize( INOUT _features );
		desc.features = storage.CopyFeatures( _features );

		if ( not _rtSBTs.empty() )
		{
			auto*	sbt_list = storage.allocator.Allocate< SerializableRenderTechnique::SBTList_t::value_type >( _rtSBTs.size() );
			CHECK_ERR( sbt_list != null );

			for (usize i = 0; i < _rtSBTs.size(); ++i)
			{
				sbt_list[i].first	= RTShaderBindingName{_rtSBTs[i]->Name()};
				sbt_list[i].second	= _rtSBTs[i]->UID();
			}

			desc.rtSBTs = SerializableRenderTechnique::SBTList_t{ sbt_list, _rtSBTs.size() };
		}

		_uid = ppln_storage.AddRenderTech( RVRef(desc) );
		return true;
	}

/*
=================================================
	AddSBT
=================================================
*/
	void  RenderTechnique::AddSBT (RayTracingShaderBindingPtr sbt) __Th___
	{
		_rtSBTs.push_back( sbt );
	}

/*
=================================================
	Bind
=================================================
*/
	void  RenderTechnique::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			EnumBinder<EMutableRenderState>	binder{ se };
			binder.Create();
			//binder.AddValue( "", EMutableRenderState:: );
		}

		RTGraphicsPass::Bind( se );
		RTComputePass::Bind( se );

		{
			ClassBinder<RenderTechnique>	binder{ se };
			binder.CreateRef();

			binder.Comment( "Create render technique.\n"
							"Name is used in C++ code to create render technique." );
			binder.AddFactoryCtor( &RenderTechnique_Ctor, {"name"} );

			binder.Comment( "Add FeatureSet to the render technique." );
			binder.AddMethod( &RenderTechnique::AddFeatureSet,		"AddFeatureSet",	{"fsName"} );

			binder.Comment( "Create graphics pass.\n"
							"Name is used in C++ to begin render pass." );
			binder.AddMethod( &RenderTechnique::AddGraphicsPass,	"AddGraphicsPass",	{"passName"} );

			binder.Comment( "Create compute pass." );
			binder.AddMethod( &RenderTechnique::AddComputePass,		"AddComputePass",	{"passName"} );

			binder.Comment( "Copy graphics pass from another render technique." );
			binder.AddMethod( &RenderTechnique::CopyGraphicsPass,	"CopyGraphicsPass",	{"newName", "rtech", "gpass"} );

			binder.Comment( "Copy compute pass from another render technique." );
			binder.AddMethod( &RenderTechnique::CopyComputePass,	"CopyComputePass",	{"newName", "rtech", "cpass"} );
		}
	}


} // AE::PipelineCompiler
