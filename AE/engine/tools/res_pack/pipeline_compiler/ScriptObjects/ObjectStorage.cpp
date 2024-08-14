// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/Common.inl.h"

#include "Packer/FeatureSetPack.h"
#include "Packer/RenderPassPack.h"
#include "Packer/SamplerPack.h"
#include "Packer/PipelinePack.h"

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#else
# include "Packer/ShaderTraceDummy.h"
#endif

namespace AE::PipelineCompiler
{

/*
=================================================
	CompiledShader
=================================================
*/
	CompiledShader::CompiledShader () __NE___ {}

	CompiledShader::~CompiledShader () __NE___ {}

	CompiledShader::CompiledShader (CompiledShader &&other) __NE___ :
		uid{ other.uid },
		version{ other.version },
		type{ other.type },
		data{ RVRef(other.data) },
		reflection{ RVRef(other.reflection) },
		trace{ RVRef(other.trace) }
	{}

	bool  CompiledShader::operator == (const CompiledShader &rhs) C_NE___
	{
		return	version		== rhs.version	and
				type		== rhs.type		and
				data		== rhs.data		and
				reflection	== rhs.reflection;
				// ignore 'trace'
	}

	HashVal  CompiledShader::CalcHash () C_NE___
	{
		return HashOf(version) + HashOf(type) + HashOf(data) + reflection.CalcHash();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  ObjectStorage::ShaderSrcKey::operator == (const ShaderSrcKey &rhs) const
	{
		return	type		== rhs.type			and
				options		== rhs.options		and
				resources	== rhs.resources	and
				version		== rhs.version		and
				source		== rhs.source		and
				include		== rhs.include		and
				defines		== rhs.defines;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  ObjectStorage::ShaderSrcKey::CalcHash () const
	{
		return	HashOf(source) + HashOf(resources) + HashOf(include) + HashOf(type) +
				HashOf(version) + HashOf(options) + HashOf(defines);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ObjectStorage::ObjectStorage () :
		_compatRPConstPtr{ new CompatRTConsts{} },
		_structTypeConstPtr{ new StructTypeConsts{} }
	{
		gpipelines.reserve( 128 );
		mpipelines.reserve( 128 );
		cpipelines.reserve( 128 );
		tpipelines.reserve( 128 );
		rtpipelines.reserve( 128 );
	}

/*
=================================================
	destructor
=================================================
*/
	ObjectStorage::~ObjectStorage ()
	{}

/*
=================================================
	TestRenderPass
=================================================
*/
	void  ObjectStorage::TestRenderPass (const String &compatRP, const String &subpass, const SubpassShaderIO &fragIO,
										 bool requireDepth, bool requireStencil) __Th___
	{
		AddName<CompatRenderPassName>( compatRP );
		AddName<SubpassName>( subpass );

		return TestRenderPass( CompatRenderPassName{compatRP}, SubpassName{subpass}, fragIO, requireDepth, requireStencil );  // throw
	}

/*
=================================================
	TestRenderPass
----
	not supported for Metal, only for SPIRV
=================================================
*/
	void  ObjectStorage::TestRenderPass (const CompatRenderPassName::Optimized_t &compatRP, const SubpassName::Optimized_t &subpass,
										 const SubpassShaderIO &fragIO, bool requireDepth, bool requireStencil) __Th___
	{
		CHECK( HasHashName( compatRP ) and HasHashName( subpass ));

		auto	rp_it = serializedRPs.find( compatRP );
		CHECK_THROW_MSG( rp_it != serializedRPs.end(),
			"Compatible render pass '"s << GetName( compatRP ) << "' is not found" );

		auto	sp_it = rp_it->second.subpasses.find( subpass );
		CHECK_THROW_MSG( sp_it != rp_it->second.subpasses.end(),
			"Compatible render pass '"s << GetName( compatRP ) << "' does not have subpass '" << GetName( subpass ) << "'" );

		// Metal: no way to extract fragment output from shader, use 'SetFragmentOutputFromRenderPass()' in script to avoid it.
		CHECK_THROW_MSG( sp_it->second.colorAttachments.size() == fragIO.colorAttachments.size(),
			"Color attachments in render pass '"s << GetName( compatRP ) << "' subpass '" <<
			GetName( subpass ) << "' doesn't match with color outputs in shader  (" <<
			ToString(sp_it->second.colorAttachments.size()) << " != " <<
			ToString(fragIO.colorAttachments.size()) << ") " );

		CHECK_THROW_MSG( not (requireDepth and not sp_it->second.hasDepth),
			"Pipeline uses depth test/write but depth attachment is not exists in render pass '"s <<
			GetName( compatRP ) << "' subpass '" << GetName( subpass ) << "'" );

		CHECK_THROW_MSG( not (requireStencil and not sp_it->second.hasStencil),
			"Pipeline uses stencil test/write but stencil attachment is not exists in render pass '"s <<
			GetName( compatRP ) << "' subpass '" << GetName( subpass ) << "'" );

		for (usize i = 0; i < fragIO.colorAttachments.size(); ++i)
		{
			auto	lhs = sp_it->second.colorAttachments[i];
			auto	rhs = fragIO.colorAttachments[i];
			CHECK_THROW_MSG( lhs.Get<0>() == rhs.Get<0>(),	// name
				"color attachment ["s << ToString(i) << "] name mismatch: RP('" <<
				ToString<16>(usize(lhs.Get<0>().GetHash())) << "') != Shader('" <<
				ToString<16>(usize(rhs.Get<0>().GetHash())) << "')" );
			CHECK_THROW_MSG( EShaderIO_IsConvertible( lhs.Get<1>(), rhs.Get<1>() ),	// type
				"color attachment ["s << ToString(i) << "] type mismatch: RP(" <<
				ToString( lhs.Get<1>() ) << ") != Shader(" << ToString( rhs.Get<1>() ) << ")" );
		}

		CHECK_THROW_MSG( sp_it->second.inputAttachments.size() == fragIO.inputAttachments.size(),
			"Input attachments in render pass '"s << GetName( compatRP ) << "' subpass '" <<
			GetName( subpass ) << "' doesn't match with input attachments in shader  (" <<
			ToString(sp_it->second.inputAttachments.size()) << " != " <<
			ToString(fragIO.inputAttachments.size()) << ") " );

		for (usize i = 0; i < fragIO.inputAttachments.size(); ++i)
		{
			auto	lhs = sp_it->second.inputAttachments[i];
			auto	rhs = fragIO.inputAttachments[i];
			CHECK_THROW_MSG( lhs.Get<0>() == rhs.Get<0>(),	// name
				"input attachment ["s << ToString(i) << "] name mismatch: RP(" <<
				ToString<16>(usize(lhs.Get<0>().GetHash())) << ") != Shader(" <<
				ToString<16>(usize(rhs.Get<0>().GetHash())) << ")" );
			CHECK_THROW_MSG( EShaderIO_IsConvertible( lhs.Get<1>(), rhs.Get<1>() ),	// type
				"input attachment ["s << ToString(i) << "] type mismatch: RP(" <<
				ToString( lhs.Get<1>() ) << ") != Shader(" << ToString( rhs.Get<1>() ) << ")" );
		}
	}

/*
=================================================
	RenderPassExists
=================================================
*/
	CompatibleRenderPassDescPtr  ObjectStorage::RenderPassExists (const String &rpName, const String &subpass) __Th___
	{
		AddName<RenderPassName>( rpName );	// throw
		AddName<SubpassName>( subpass );	// throw

		auto	rp_it = renderPassToCompat.find( RenderPassName{rpName} );
		CHECK_THROW_MSG( rp_it != renderPassToCompat.end(),
			"Render pass '"s << rpName << "' is not found" );

		auto	ser_rp_it = serializedRPs.find( rp_it->second );
		CHECK_THROW_MSG( ser_rp_it != serializedRPs.end(),
			"Compatible render pass '"s << GetName( rp_it->second ) << "' is not serialized" );

		auto	sp_it = ser_rp_it->second.subpasses.find( SubpassName{subpass} );
		CHECK_THROW_MSG( sp_it != ser_rp_it->second.subpasses.end(),
			"Render pass '"s << rpName << "' does not have subpass '" << subpass << "'" );

		auto	compat_it = compatibleRPs.find( rp_it->second );
		CHECK_THROW_MSG( compat_it != compatibleRPs.end(),
			"Compatible render pass '"s << GetName( rp_it->second ) << "' is not exists" );

		return compat_it->second;
	}

/*
=================================================
	RenderPassExists
=================================================
*/
	CompatibleRenderPassDescPtr  ObjectStorage::RenderPassExists (RenderPassName::Ref rpName, SubpassName::Ref subpass) C_Th___
	{
		auto	rp_it = renderPassToCompat.find( rpName );
		CHECK_THROW_MSG( rp_it != renderPassToCompat.end(),
			"Render pass '"s << GetName( rpName ) << "' is not found" );

		auto	ser_rp_it = serializedRPs.find( rp_it->second );
		CHECK_THROW_MSG( ser_rp_it != serializedRPs.end(),
			"Compatible render pass '"s << GetName( rp_it->second ) << "' is not serialized" );

		auto	sp_it = ser_rp_it->second.subpasses.find( subpass );
		CHECK_THROW_MSG( sp_it != ser_rp_it->second.subpasses.end(),
			"Render pass '"s << GetName( rpName ) << "' does not have subpass '" << GetName( subpass ) << "'" );

		auto	compat_it = compatibleRPs.find( rp_it->second );
		CHECK_THROW_MSG( compat_it != compatibleRPs.end(),
			"Compatible render pass '"s << GetName( rp_it->second ) << "' is not exists" );

		return compat_it->second;
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	RenderPassSpecPtr  ObjectStorage::GetRenderPass (RenderPassName::Ref rpName) C_Th___
	{
		auto	rp_it = renderPassToCompat.find( rpName );
		CHECK_THROW_MSG( rp_it != renderPassToCompat.end(),
			"Render pass '"s << GetName( rpName ) << "' is not found" );

		auto	compat_it = compatibleRPs.find( rp_it->second );
		CHECK_THROW_MSG( compat_it != compatibleRPs.end(),
			"Compatible render pass '"s << GetName( rp_it->second ) << "' is not exists" );

		auto	rp = compat_it->second->GetRenderPass( rpName );
		CHECK_THROW_MSG( rp, "(internal error)" );

		return rp;
	}

/*
=================================================
	Build
=================================================
*/
	bool  ObjectStorage::Build () __NE___
	{
		for (auto& [name, src_rp] : compatibleRPs)
		{
			if ( not serializedRPs.contains( name ))
			{
				CHECK( src_rp->Validate() );

				SerializableRenderPassInfo	dst_rp;
				CHECK_ERR( dst_rp.Create( *src_rp ));
				CHECK_ERR( serializedRPs.emplace( name, dst_rp ).second );
			}
		}

		for (auto& ds : dsLayouts) {
			CHECK_ERR( ds.second->Build() );
		}
		for (auto& pl : pplnLayouts) {
			CHECK_ERR( pl.second->Build() );
		}

		for (auto& ppln : gpipelines) {
			CHECK_ERR( ppln.second->Build() );
		}
		for (auto& ppln : mpipelines) {
			CHECK_ERR( ppln.second->Build() );
		}
		for (auto& ppln : cpipelines) {
			CHECK_ERR( ppln.second->Build() );
		}
		for (auto& ppln : tpipelines) {
			CHECK_ERR( ppln.second->Build() );
		}
		for (auto& ppln : rtpipelines) {
			CHECK_ERR( ppln.second->Build() );
		}
		for (auto& sbt : rtShaderBindings) {
			CHECK_ERR( sbt.second->Build() );
		}

		return true;
	}

/*
=================================================
	Clear
=================================================
*/
	void  ObjectStorage::Clear ()
	{
		featureSets.clear();
		dsLayouts.clear();
		pplnLayouts.clear();

		gpipelines.clear();
		mpipelines.clear();
		cpipelines.clear();
		tpipelines.clear();
		rtpipelines.clear();
		rtechMap.clear();
		rtShaderBindings.clear();

		samplerRefs.clear();
		samplerMap.clear();

		compatibleRPs.clear();
		serializedRPs.clear();
		renderPassToCompat.clear();

		pplnTmplNames.clear();
		pplnSpecNames.clear();
		hashToName.clear();

		pipelineFilename.clear();
		renderStatesMap.clear();
		vbInputMap.clear();
		structTypes.clear();

		_hashCollisionCheck.Clear();
	}

/*
=================================================
	_SetAndSortDefines
=================================================
*/
	void  ObjectStorage::_SetAndSortDefines (OUT ShaderDefines_t &defines, const String &def)
	{
		Array<StringView>	lines;
		Parser::DivideLines( def, OUT lines );

		defines.clear();
		for (auto& line : lines)
		{
			String		str {"#define "};
			const usize	pos1 = Min( line.find( '=' ), line.find( ' ' ));
			const usize	pos2 = Max( line.rfind( '=' )+1, line.rfind( ' ' )+1 );

			if ( pos1 == UMax ){
				str << line;
			}else{
				str << line.substr( 0, pos1 ) << ' ' << line.substr( pos2 );
			}

			defines.push_back( RVRef(str) );
		}

		SortDefines( INOUT defines );
	}

/*
=================================================
	CopyFeatures
=================================================
*/
	FSNameArr_t  ObjectStorage::CopyFeatures (ArrayView<ScriptFeatureSetPtr> inFeats)
	{
		if ( inFeats.empty() )
			return Default;

		Array<ScriptFeatureSetPtr>	feats {inFeats};
		RemoveDuplicates( INOUT feats );

		// sort by name
		std::sort( feats.begin(), feats.end(), [](auto& lhs, auto& rhs){ return lhs->Hash() < rhs->Hash(); });

		auto*		dst = allocator.Allocate< FeatureSetName::Optimized_t >( feats.size() );
		FSNameArr_t	res{ dst, feats.size() };

		for (usize i = 0; i < feats.size(); ++i) {
			dst[i] = FeatureSetName::Optimized_t{ feats[i]->Name() };
		}
		return res;
	}

/*
=================================================
	GetDefaultFeatureSets
=================================================
*/
	Array<ScriptFeatureSetPtr>  ObjectStorage::GetDefaultFeatureSets () __Th___
	{
		if ( defaultFeatureSet.empty() )
			return {};

		auto	it = featureSets.find( FeatureSetName{defaultFeatureSet} );
		CHECK_THROW_MSG( it != featureSets.end(),
			"Can't find default feature set '"s << defaultFeatureSet << "'" );

		return {it->second};
	}

/*
=================================================
	SavePack
=================================================
*/
	bool  ObjectStorage::SavePack (const Path &filename, bool addNameMapping) const
	{
		FileSystem::CreateDirectories( filename.parent_path() );

		PipelinePackOffsets		offsets		= {};
		auto					file		= MakeRC<FileWStream>( filename );

		CHECK_ERR( SavePack( *file, addNameMapping, OUT offsets ));

		file = null;

		// update offsets
		{
			file = MakeRC<FileWStream>( filename, FileWStream::EMode::OpenUpdate );
			CHECK_ERR( file->IsOpen() );

			CHECK_ERR( file->Write( PackOffsets_Name ) and file->Write( offsets ));
		}

		AE_LOGI( "Store pipeline pack: '"s << ToString(filename) << "'" );
		return true;
	}

/*
=================================================
	SavePack
=================================================
*/
	bool  ObjectStorage::SavePack (WStream &stream, bool addNameMapping, OUT PipelinePackOffsets &offsets) const
	{
		const ulong		align		= 8;
		const usize		id_size		= 4;	// sizeof(ID<>)
		const usize		name_size	= 4;	// sizeof(***Name::Optimized_t)

		CHECK_ERR( stream.IsOpen() );
		CHECK_ERR( stream.Write( PackOffsets_Name ) and stream.Write( offsets ));


		// feature sets
		if ( not this->featureSets.empty() )
		{
			offsets.featureSetOffset = stream.Position();

			auto	mem = MakeRC<ArrayWStream>();
			{
				Serializing::Serializer		ser{ mem };
				CHECK_ERR( FeatureSetPacker::Serialize( ser ));
			}
			CHECK_ERR( mem->StoreTo( stream ));

			offsets.featureSetDataSize = stream.Position() - offsets.featureSetOffset;
			CHECK( offsets.featureSetDataSize > 0 );

			offsets.allocSize = AlignUp( offsets.allocSize + this->featureSets.size() * /*FeatureSetName*/name_size, align );
		}

		// samplers
		if ( not this->samplerRefs.empty() )
		{
			offsets.samplerOffset = stream.Position();

			auto	mem = MakeRC<ArrayWStream>();
			{
				Serializing::Serializer		ser{ mem };
				CHECK_ERR( SamplerPacker::Serialize( ser ));
			}
			CHECK_ERR( mem->StoreTo( stream ));

			offsets.samplerDataSize = stream.Position() - offsets.samplerOffset;
			CHECK( offsets.samplerDataSize > 0 );

			offsets.allocSize = AlignUp( offsets.allocSize + this->samplerRefs.size() * (/*SamplerName*/name_size + /*SamplerID*/id_size), align );
		}

		// render passes
		if ( not this->compatibleRPs.empty() )
		{
			offsets.renderPassOffset = stream.Position();

			auto	mem = MakeRC<ArrayWStream>();
			{
				Serializing::Serializer		ser{ mem };
				CHECK_ERR( RenderPassPacker::Serialize( ser ));
			}
			CHECK_ERR( mem->StoreTo( stream ));

			offsets.renderPassDataSize = stream.Position() - offsets.renderPassOffset;
			CHECK( offsets.renderPassDataSize > 0 );

			//offsets.allocSize = AlignUp( offsets.allocSize +	// TODO
		}

		// hash to name
		if ( addNameMapping )
		{
			offsets.nameMappingOffset = stream.Position();

			CHECK_ERR( stream.Write( NameMapping_Name ));

			auto	mem = MakeRC<ArrayWStream>();
			{
				Serializing::Serializer		ser{ mem };
				CHECK_ERR( ser( uint(this->hashToName.size()) ));

				for (auto& [info, name] : this->hashToName)
				{
					CHECK_ERR( ser( info.hash, info.uid, name ));
				}
			}
			CHECK_ERR( mem->StoreTo( stream ));

			offsets.nameMappingDataSize = stream.Position() - offsets.nameMappingOffset;
		}

		// pipeline & shaders
		if ( not this->pplnStorage->Empty() )
		{
			offsets.allocSize		= AlignUp( this->pplnStorage->CalcAllocationSize( Bytes{align} ), align );
			offsets.pipelineOffset	= stream.Position();

			CHECK_ERR( this->pplnStorage->SerializePipelines( stream ));

			offsets.pipelineDataSize = stream.Position() - offsets.pipelineOffset;
			CHECK( offsets.pipelineDataSize > 0 );

			offsets.shaderOffset = stream.Position();
			CHECK_ERR( this->pplnStorage->WriteShaders( stream ));

			offsets.shaderDataSize = stream.Position() - offsets.shaderOffset;
			CHECK( offsets.shaderDataSize > 0 );
		}

		return true;
	}

/*
=================================================
	SaveCppStructs
=================================================
*/
	bool  ObjectStorage::SaveCppStructs (const Path &filename) const
	{
		using EUsage = ShaderStructType::EUsage;

		ShaderStructType::UniqueTypes_t	unique;
		String							types;

		FileSystem::CreateDirectories( filename.parent_path() );

		for (auto& [name, st] : this->structTypes)
		{
			if ( not AnyBits( st->Usage(), EUsage::BufferLayout | EUsage::VertexLayout ))
				continue;

			CHECK_ERR( st->ToCPP( INOUT types, INOUT unique ));
		}

		auto	file = MakeRC<FileWStream>( filename );
		CHECK_ERR( file->IsOpen() );
		CHECK_ERR( file->Write( StringView{types} ));

		AE_LOGI( "Store C++ types for pipelines to '"s << ToString(filename) << "'" );
		return true;
	}

/*
=================================================
	SaveCppNames
=================================================
*/
	bool  ObjectStorage::SaveCppNames (const Path &filename, const EReflectionFlags flags) const
	{
		CHECK( flags != Default );
		FileSystem::CreateDirectories( filename.parent_path() );

		const auto	ValidateName = [] (StringView name) -> String
		{{
			return Parser::ValidateVarName_CPP( name );
		}};

		String	str;

		// render technique names
		if ( AllBits( flags, EReflectionFlags::RenderTechniques ))
		{
			Array<RenderTechniquePtr>	rtech_arr;
			for (auto& [rtech_name, rtech] : rtechMap) {
				rtech_arr.push_back( rtech );
			}
			std::sort( rtech_arr.begin(), rtech_arr.end(), [](auto& lhs, auto &rhs) { return lhs->Name() < rhs->Name(); });

			String	tmp;
			for (auto& rtech : rtech_arr)
			{
				tmp << "\n\tstatic constexpr struct _" << ValidateName(rtech->Name()) << "\n\t{\n";
				tmp << "\t\tconstexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x"
					<< ToString<16>( uint{RenderTechName{rtech->Name()}} ) << "u}};}  // '" << rtech->Name() << "'\n";

				for (auto& pass : rtech->GetPasses())
				{
					tmp << "\n";
					if ( auto* gpass = DynCast<RTGraphicsPass>( pass.Get() ))
						tmp << "\t\t// graphics (" << ToString(pass->PassIndex()) << ")\n";
					else
					if ( auto* cpass = DynCast<RTComputePass>( pass.Get() ))
						tmp << "\t\t// compute (" << ToString(pass->PassIndex()) << ")\n";
					else
						tmp << "\t\t// pass (" << ToString(pass->PassIndex()) << ")\n";

					tmp << "\t\tstatic constexpr struct _" << ValidateName(pass->Name()) << "\n\t\t{\n";
					tmp << "\t\t\tconstexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x"
						<< ToString<16>( uint{RenderTechPassName{pass->Name()}} ) << "u}};}  // '" << pass->Name() << "'\n";

					if ( pass->GetDSLayout() )
					{
						tmp << "\t\t\tstatic constexpr DescriptorSetName_t  dsLayout {Hash_t{0x"
							<< ToString<16>( uint{DescriptorSetName{pass->GetDSLayout()->Name()}} )
							<< "u}};  // '" << pass->GetDSLayout()->Name() << "'\n";
					}

					if ( auto* gpass = DynCast<RTGraphicsPass>( pass.Get() ))
					{
						tmp << "\n\t\t\t// RenderPass '" << GetName(gpass->GetRenderPassName()) << "' subpass '" << GetName(gpass->GetSubpassName()) << "'\n";

						if ( gpass->IsFirstSubpass() )
						{
							CompatibleRenderPassDescPtr	compat_rp	= gpass->GetCompatRenderPass();
							CHECK_ERR( compat_rp );

							tmp << "\n\t\t\tstatic constexpr uint  attachmentsCount = " << ToString(compat_rp->_attachments.size()) << ";\n";
							for (auto& [att_name, att] : compat_rp->_attachments)
							{
								const auto	name = GetName(att_name);
								tmp << "\t\t\tstatic constexpr AttachmentName_t  att_" << ValidateName(name) << " {Hash_t{0x"
									<< ToString<16>( uint{AttachmentName{name}} ) << "u}};  // '" << name << "'\n";
							}
						}
					}

					if ( AllBits( flags, EReflectionFlags::RTechPass_Pipelines ) and
						 not pass->GetPipelines().empty() )
					{
						Array<String>	ppln_names;
						for (auto& ppln : pass->GetPipelines()) {
							ppln_names.push_back( GetName(ppln->Name()) );
						}
						std::sort( ppln_names.begin(), ppln_names.end() );

						tmp << "\n\t\t\t// pipelines\n";
						//tmp << "\n\t\t\tstatic constexpr struct {\n";
						for (auto& name : ppln_names) {
							tmp << "\t\t\tstatic constexpr PipelineName_t  " << ValidateName(name) << " {Hash_t{0x"
								<< ToString<16>( uint{PipelineName{name}} ) << "u}};  // '" << name << "'\n";
						}
						//tmp << "\t\t\t} ppln;\n";
					}

					tmp << "\t\t} " << ValidateName(pass->Name()) << " = {};\n";
				}

				if ( AllBits( flags, EReflectionFlags::RTech_ShaderBindingTable ) and
					 not rtech->GetSBTs().empty() )
				{
					tmp << "\n\t\tstatic constexpr struct _SBT {\n";
					for (auto& sbt : rtech->GetSBTs())
					{
						const auto	name = sbt->Name();
						tmp << "\t\t\tstatic constexpr RTShaderBindingName_t  " << ValidateName(name) << " {Hash_t{0x"
							<< ToString<16>( uint{RTShaderBindingName{name}} ) << "u}};  // '" << name << "'\n";
					}
					tmp << "\t\t} sbt = {};\n";
				}

				tmp << "\t} " << ValidateName(rtech->Name()) << ";\n";
			}

			if ( not tmp.empty() ) {
				str << "namespace RenderTechs\n{\n"
					<< "\tusing Hash_t                = AE::Base::HashVal32;\n"
					<< "\tusing RenderTechName_t      = AE::Graphics::RenderTechName;\n"
					<< "\tusing RenderTechPassName_t  = AE::Graphics::RenderTechPassName;\n"
					<< "\tusing AttachmentName_t      = AE::Graphics::AttachmentName;\n"
					<< "\tusing PipelineName_t        = AE::Graphics::PipelineName;\n"
					<< "\tusing RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;\n"
					<< "\tusing DescriptorSetName_t   = AE::Graphics::DescriptorSetName;\n"
					<< tmp
					<< "}\n";
			}
		}

		auto	file = MakeRC<FileWStream>( filename );
		CHECK_ERR( file->IsOpen() );
		CHECK_ERR( file->Write( StringView{str} ));

		AE_LOGI( "Store C++ RP & RTech names to '"s << ToString(filename) << "'" );
		return true;
	}

/*
=================================================
	CompilePipeline
=================================================
*/
	bool  ObjectStorage::CompilePipeline (const ScriptEnginePtr &scriptEngine, const Path &path, ArrayView<Path> includeDirs)
	{
		const String	ansi_path	= ToString(path);
		String			script;

		{
			FileRStream		file{ path };

			if ( not file.IsOpen() )
			{
				AE_LOGI( "Failed to open pipeline file: '"s << ansi_path << "'" );
				return false;
			}

			if ( not file.Read( file.RemainingSize(), OUT script ))
			{
				AE_LOGI( "Failed to read pipeline file: '"s << ansi_path << "'" );
				return false;
			}
		}

		return CompilePipelineFromSource( scriptEngine, path, script, includeDirs );
	}

/*
=================================================
	CompilePipelineFromSource
=================================================
*/
	bool  ObjectStorage::CompilePipelineFromSource (const ScriptEnginePtr &scriptEngine, const Path &path, StringView source, ArrayView<Path> includeDirs)
	{
		const String	ansi_path = ToString(path);

		ScriptEngine::ModuleSource	src;
		src.name			= ToString( path.stem() );
		src.script			= source;
		src.dbgLocation		= SourceLoc{ ansi_path, 0 };
		src.usePreprocessor	= true;

		ScriptModulePtr		module = scriptEngine->CreateModule( {src}, {"SCRIPT"}, includeDirs );
		if ( not module )
		{
			AE_LOGI( "Failed to parse pipeline file: '"s << ansi_path << "'" );
			return false;
		}

		auto	fn = scriptEngine->CreateScript< void() >( "ASmain", module );
		if ( not fn )
		{
			AE_LOGI( "Failed to create script context for file: '"s << ansi_path << "'" );
			return false;
		}

		this->pipelineFilename = path;

		if ( not fn->Run() )
			return false;

		if ( not this->Build() )
		{
			AE_LOGI( "Failed to process resources for file: '"s << ansi_path << "'" );
		}

		this->pipelineFilename.clear();
		return true;
	}

/*
=================================================
	BuildRenderTechniques
=================================================
*/
	bool  ObjectStorage::BuildRenderTechniques ()
	{
		for (auto& [name, rt] : this->rtechMap) {
			CHECK_ERR( rt->Build() );
		}
		return true;
	}

/*
=================================================
	ShaderStorage_Instance
=================================================
*/
	ObjectStorage*&  ShaderStorage_Instance ()
	{
		static thread_local ObjectStorage*	inst = null;
		return inst;
	}

	Ptr<ObjectStorage>  ObjectStorage::Instance ()
	{
		return ShaderStorage_Instance();
	}

	void  ObjectStorage::SetInstance (ObjectStorage* inst)
	{
		ShaderStorage_Instance() = inst;
	}

/*
=================================================
	config getter
=================================================
*/
namespace {
	static bool  Cfg_IsVulkan ()		{ return ObjectStorage::Instance()->target == ECompilationTarget::Vulkan; }
	static bool  Cfg_IsMetal_iOS ()		{ return ObjectStorage::Instance()->target == ECompilationTarget::Metal_iOS; }
	static bool  Cfg_IsMetal_Mac ()		{ return ObjectStorage::Instance()->target == ECompilationTarget::Metal_Mac; }
	static bool  Cfg_IsMetal ()			{ return Cfg_IsMetal_iOS() or Cfg_IsMetal_Mac(); }

	static bool  Cfg_IsShaderTraceSupported ()
	{
	#ifdef AE_ENABLE_GLSL_TRACE
		return true;
	#else
		return false;
	#endif
	}

	static bool  Cfg_IsMetalCompilerSupported ()
	{
	#if defined(AE_METAL_TOOLS) and defined(AE_ENABLE_SPIRV_CROSS)
		return true;
	#else
		return false;
	#endif
	}

	static ShaderStructType*  GetShaderStructType (const String &name) __Th___
	{
		auto&	struct_types	= ObjectStorage::Instance()->structTypes;
		auto	it				= struct_types.find( name );

		if ( it != struct_types.end() )
			return ShaderStructTypePtr{it->second}.Detach();

		CHECK_THROW_MSG( false,
			"ShaderStructType '"s << name << "' is not exists" );
	}

	static bool  HasRenderTech (const String &name) __Th___
	{
		auto&	rtech_map	= ObjectStorage::Instance()->rtechMap;
		auto	it			= rtech_map.find( name );

		return it != rtech_map.end();
	}

	static String  CurPipelineFileName () __Th___
	{
		return ObjectStorage::Instance()->pipelineFilename.stem().string();
	}

	static ScriptFeatureSet*  GetDefaultFeatureSet () __Th___
	{
		auto	arr = ObjectStorage::Instance()->GetDefaultFeatureSets();
		if ( arr.empty() )
			return null;
		return arr.front().Detach();
	}
}

/*
=================================================
	Bind
=================================================
*/
	void  ObjectStorage::Bind (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW_MSG( se->Create( True{"gen cpp header"} ));

		CoreBindings::BindStdTypes( se );
		CoreBindings::BindScalarMath( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindColor( se );
		CoreBindings::BindString( se );
		CoreBindings::BindArray( se );
		CoreBindings::BindToString( se, true, true, false, true );
		CoreBindings::BindLog( se );
		CoreBindings::BindFileSystem( se, true );
		GraphicsBindings::BindEnums( se );
		GraphicsBindings::BindTypes( se );
		GraphicsBindings::BindRenderState( se );

		Bind_EShaderVersion( se );
		Bind_EShaderOpt( se );
		Bind_EAccessType( se );
		Bind_EImageType( se );
		Bind_ECompilationTarget( se );
		Bind_EStructLayout( se );
		Bind_EValueType( se );
		Bind_EShaderPreprocessor( se );

		ScriptConfig::Bind( se );
		ScriptRenderState::Bind( se );
		ScriptFeatureSet::Bind( se );
		Align::Bind( se );
		ArraySize::Bind( se );
		ScriptShader::Bind( se );
		ShaderStructType::Bind( se );
		ScriptVertexBufferInput::Bind( se );
		DescriptorSetLayout::Bind( se );
		PipelineLayout::Bind( se );
		RenderTechnique::Bind( se );
		GraphicsPipelineSpecScriptBinding::Bind( se );
		GraphicsPipelineScriptBinding::Bind( se );
		MeshPipelineSpecScriptBinding::Bind( se );
		MeshPipelineScriptBinding::Bind( se );
		ComputePipelineSpecScriptBinding::Bind( se );
		ComputePipelineScriptBinding::Bind( se );
		TilePipelineSpecScriptBinding::Bind( se );
		TilePipelineScriptBinding::Bind( se );
		RayTracingPipelineSpecScriptBinding::Bind( se );
		RayTracingPipelineScriptBinding::Bind( se );
		CompatibleRenderPassDesc::Bind( se );
		ScriptSampler::Bind( se );
		RayTracingShaderBinding::Bind( se );

		se->AddFunction( &Cfg_IsShaderTraceSupported,	"IsShaderTraceSupported",	{} );
		se->AddFunction( &Cfg_IsMetalCompilerSupported,	"IsMetalCompilerSupported",	{} );
		se->AddFunction( &Cfg_IsVulkan,					"IsVulkan",					{} );
		se->AddFunction( &Cfg_IsMetal_iOS,				"IsMetal_iOS",				{} );
		se->AddFunction( &Cfg_IsMetal_Mac,				"IsMetal_Mac",				{} );
		se->AddFunction( &Cfg_IsMetal,					"IsMetal",					{} );
		se->AddFunction( &GetShaderStructType,			"GetShaderStructType",		{"name"} );
		se->AddFunction( &HasRenderTech,				"HasRenderTech",			{} );
		se->AddFunction( &CurPipelineFileName,			"FileName",					{} );
		se->AddFunction( &GetDefaultFeatureSet,			"GetDefaultFeatureSet",		{} );

		se->AddCppHeader( "", "#define SCRIPT\n\n", 0 );
	}

	void  ObjectStorage::Bind_EShaderVersion (const ScriptEnginePtr &se)
	{
		EnumBinder<EShaderVersion>	binder{ se };
		binder.Create();
		binder.Comment( "Vulkan 1.0" );
		binder.AddValue( "SPIRV_1_0",		EShaderVersion::SPIRV_1_0 );
		binder.AddValue( "SPIRV_1_1",		EShaderVersion::SPIRV_1_1 );
		binder.AddValue( "SPIRV_1_2",		EShaderVersion::SPIRV_1_2 );
		binder.Comment( "Vulkan 1.1" );
		binder.AddValue( "SPIRV_1_3",		EShaderVersion::SPIRV_1_3 );
		binder.Comment( "Vulkan 1.1 extension" );
		binder.AddValue( "SPIRV_1_4",		EShaderVersion::SPIRV_1_4 );
		binder.Comment( "Vulkan 1.2" );
		binder.AddValue( "SPIRV_1_5",		EShaderVersion::SPIRV_1_5 );
		binder.Comment( "Vulkan 1.3" );
		binder.AddValue( "SPIRV_1_6",		EShaderVersion::SPIRV_1_6 );
		StaticAssert( EShaderVersion::_SPIRV_Last == EShaderVersion::SPIRV_1_6 );

		binder.Comment( "Metal API" );
		binder.AddValue( "Metal_2_0",		EShaderVersion::Metal_2_0 );
		binder.AddValue( "Metal_2_1",		EShaderVersion::Metal_2_1 );
		binder.AddValue( "Metal_2_2",		EShaderVersion::Metal_2_2 );
		binder.Comment( "Added ray tracing." );
		binder.AddValue( "Metal_2_3",		EShaderVersion::Metal_2_3 );
		binder.AddValue( "Metal_2_4",		EShaderVersion::Metal_2_4 );
		binder.Comment( "Added mesh shading." );
		binder.AddValue( "Metal_3_0",		EShaderVersion::Metal_3_0 );
		binder.AddValue( "Metal_3_1",		EShaderVersion::Metal_3_1 );
		StaticAssert( EShaderVersion::_Metal_Last == EShaderVersion::Metal_3_1 );

		binder.Comment( "Compile for iOS." );
		binder.AddValue( "Metal_iOS_2_0",	EShaderVersion::Metal_iOS_2_0 );
		binder.AddValue( "Metal_iOS_2_1",	EShaderVersion::Metal_iOS_2_1 );
		binder.AddValue( "Metal_iOS_2_2",	EShaderVersion::Metal_iOS_2_2 );
		binder.AddValue( "Metal_iOS_2_3",	EShaderVersion::Metal_iOS_2_3 );
		binder.AddValue( "Metal_iOS_2_4",	EShaderVersion::Metal_iOS_2_4 );
		binder.AddValue( "Metal_iOS_3_0",	EShaderVersion::Metal_iOS_3_0 );
		binder.AddValue( "Metal_iOS_3_1",	EShaderVersion::Metal_iOS_3_1 );
		StaticAssert( EShaderVersion::_Metal_iOS_Last == EShaderVersion::Metal_iOS_3_1 );

		binder.Comment( "Compile for MacOS." );
		binder.AddValue( "Metal_Mac_2_0",	EShaderVersion::Metal_Mac_2_0 );
		binder.AddValue( "Metal_Mac_2_1",	EShaderVersion::Metal_Mac_2_1 );
		binder.AddValue( "Metal_Mac_2_2",	EShaderVersion::Metal_Mac_2_2 );
		binder.AddValue( "Metal_Mac_2_3",	EShaderVersion::Metal_Mac_2_3 );
		binder.AddValue( "Metal_Mac_2_4",	EShaderVersion::Metal_Mac_2_4 );
		binder.AddValue( "Metal_Mac_3_0",	EShaderVersion::Metal_Mac_3_0 );
		binder.AddValue( "Metal_Mac_3_1",	EShaderVersion::Metal_Mac_3_1 );
		StaticAssert( EShaderVersion::_Metal_Mac_Last == EShaderVersion::Metal_Mac_3_1 );
	}

	void  ObjectStorage::Bind_EShaderOpt (const ScriptEnginePtr &se)
	{
		EnumBinder<EShaderOpt>	binder{ se };
		binder.Create();
		binder.AddValue( "None",				EShaderOpt::Unknown );
		// debug
		binder.Comment( "Add debug information. Used in RenderDoc shader debugger." );
		binder.AddValue( "DebugInfo",			EShaderOpt::DebugInfo );
		binder.Comment( "Insert shader trace recording. Shader will be very slow." );
		binder.AddValue( "Trace",				EShaderOpt::Trace );
		binder.Comment( "Insert shader function profiling. Shader will be very slow." );
		binder.AddValue( "FnProfiling",			EShaderOpt::FnProfiling );
		binder.Comment( "Insert whole shader time measurement. Shader will be a bit slow." );
		binder.AddValue( "TimeHeatMap",			EShaderOpt::TimeHeatMap );
		// optimize
		binder.Comment( "Enable optimizations. Take a lot of CPU time at shader compilation." );
		binder.AddValue( "Optimize",			EShaderOpt::Optimize );
		binder.Comment( "Enable bytecode size optimizations. Take a lot of CPU time at shader compilation." );
		binder.AddValue( "OptimizeSize",		EShaderOpt::OptimizeSize );
		binder.Comment( "Enable strong optimizations. Take a lot of CPU time at shader compilation." );
		binder.AddValue( "StrongOptimization",	EShaderOpt::StrongOptimization );
		// options
		binder.AddValue( "WarnAsError",			EShaderOpt::WarnAsError );
		StaticAssert( uint(EShaderOpt::All) == 0x7FF );
	}

	void  ObjectStorage::Bind_EAccessType (const ScriptEnginePtr &se)
	{
		EnumBinder<EAccessType>	binder{ se };
		binder.Create();
		binder.AddValue( "Coherent",			EAccessType::Coherent );
		binder.AddValue( "Volatile",			EAccessType::Volatile );
		binder.AddValue( "Restrict",			EAccessType::Restrict );
		// memory model
		binder.Comment( "Require 'vulkanMemoryModel' feature." );
		binder.AddValue( "DeviceCoherent",		EAccessType::DeviceCoherent );
		binder.AddValue( "QueueFamilyCoherent",	EAccessType::QueueFamilyCoherent );
		binder.AddValue( "WorkgroupCoherent",	EAccessType::WorkgroupCoherent );
		binder.AddValue( "SubgroupCoherent",	EAccessType::SubgroupCoherent );
		binder.AddValue( "NonPrivate",			EAccessType::NonPrivate );
		StaticAssert( uint(EAccessType::_Count) == 10 );
	}

	void  ObjectStorage::Bind_EImageType (const ScriptEnginePtr &se)
	{
		EnumBinder<EImageType>	binder{ se };
		binder.Create();
		binder.AddValue( "1D",					EImageType::Img1D );
		binder.AddValue( "1DArray",				EImageType::Img1DArray );
		binder.AddValue( "2D",					EImageType::Img2D );
		binder.AddValue( "2DArray",				EImageType::Img2DArray );
		binder.AddValue( "2DMS",				EImageType::Img2DMS );
		binder.AddValue( "2DMSArray",			EImageType::Img2DMSArray );
		binder.AddValue( "Cube",				EImageType::ImgCube );
		binder.AddValue( "CubeArray",			EImageType::ImgCubeArray );
		binder.AddValue( "3D",					EImageType::Img3D );
		binder.AddValue( "Buffer",				EImageType::Buffer );

		binder.AddValue( "Float",				EImageType::Float );
		binder.AddValue( "Half",				EImageType::Half );
		binder.AddValue( "SNorm",				EImageType::SNorm );
		binder.AddValue( "UNorm",				EImageType::UNorm );
		binder.AddValue( "Int",					EImageType::Int );
		binder.AddValue( "UInt",				EImageType::UInt );
		binder.AddValue( "sRGB",				EImageType::sRGB );
		binder.AddValue( "Depth",				EImageType::Depth );
		binder.AddValue( "Stencil",				EImageType::Stencil );
		binder.AddValue( "DepthStencil",		EImageType::DepthStencil );

		binder.AddValue( "Shadow",				EImageType::Shadow );

		// float
		binder.AddValue( "FImage1D",			EImageType::Img1D			| EImageType::Float );
		binder.AddValue( "FImage2D",			EImageType::Img2D			| EImageType::Float );
		binder.AddValue( "FImage2D_sRGB",		EImageType::Img2D			| EImageType::sRGB );
		binder.AddValue( "FImage3D",			EImageType::Img3D			| EImageType::Float );
		binder.AddValue( "FImage1DArray",		EImageType::Img1DArray		| EImageType::Float );
		binder.AddValue( "FImage2DArray",		EImageType::Img2DArray		| EImageType::Float );
		binder.AddValue( "FImageCube",			EImageType::ImgCube			| EImageType::Float );
		binder.AddValue( "FImageCubeArray",		EImageType::ImgCubeArray	| EImageType::Float );
		binder.AddValue( "FImage2DMS",			EImageType::Img2DMS			| EImageType::Float );
		binder.AddValue( "FImage2DMSArray",		EImageType::Img2DMSArray	| EImageType::Float );
		binder.AddValue( "FImageBuffer",		EImageType::Buffer			| EImageType::Float );
		// half
		binder.AddValue( "HImage1D",			EImageType::Img1D			| EImageType::Half );
		binder.AddValue( "HImage2D",			EImageType::Img2D			| EImageType::Half );
		binder.AddValue( "HImage3D",			EImageType::Img3D			| EImageType::Half );
		binder.AddValue( "HImage1DArray",		EImageType::Img1DArray		| EImageType::Half );
		binder.AddValue( "HImage2DArray",		EImageType::Img2DArray		| EImageType::Half );
		binder.AddValue( "HImageCube",			EImageType::ImgCube			| EImageType::Half );
		binder.AddValue( "HImageCubeArray",		EImageType::ImgCubeArray	| EImageType::Half );
		binder.AddValue( "HImage2DMS",			EImageType::Img2DMS			| EImageType::Half );
		binder.AddValue( "HImage2DMSArray",		EImageType::Img2DMSArray	| EImageType::Half );
		binder.AddValue( "HImageBuffer",		EImageType::Buffer			| EImageType::Half );
		// depth
		binder.AddValue( "Image1DShadow",		EImageType::Img1D			| EImageType::Depth | EImageType::Shadow );
		binder.AddValue( "Image2DShadow",		EImageType::Img2D			| EImageType::Depth | EImageType::Shadow );
		binder.AddValue( "Image1DArrayShadow",	EImageType::Img1DArray		| EImageType::Depth | EImageType::Shadow );
		binder.AddValue( "Image2DArrayShadow",	EImageType::Img2DArray		| EImageType::Depth | EImageType::Shadow );
		binder.AddValue( "ImageCubeShadow",		EImageType::ImgCube			| EImageType::Depth | EImageType::Shadow );
		binder.AddValue( "ImageCubeArrayShadow",EImageType::ImgCubeArray	| EImageType::Depth | EImageType::Shadow );
		// int
		binder.AddValue( "IImage1D",			EImageType::Img1D			| EImageType::Int );
		binder.AddValue( "IImage2D",			EImageType::Img2D			| EImageType::Int );
		binder.AddValue( "IImage3D",			EImageType::Img3D			| EImageType::Int );
		binder.AddValue( "IImage1DArray",		EImageType::Img1DArray		| EImageType::Int );
		binder.AddValue( "IImage2DArray",		EImageType::Img2DArray		| EImageType::Int );
		binder.AddValue( "IImageCube",			EImageType::ImgCube			| EImageType::Int );
		binder.AddValue( "IImageCubeArray",		EImageType::ImgCubeArray	| EImageType::Int );
		binder.AddValue( "IImage2DMS",			EImageType::Img2DMS			| EImageType::Int );
		binder.AddValue( "IImage2DMSArray",		EImageType::Img2DMSArray	| EImageType::Int );
		binder.AddValue( "IImageBuffer",		EImageType::Buffer			| EImageType::Int );
		// uint
		binder.AddValue( "UImage1D",			EImageType::Img1D			| EImageType::UInt );
		binder.AddValue( "UImage2D",			EImageType::Img2D			| EImageType::UInt );
		binder.AddValue( "UImage3D",			EImageType::Img3D			| EImageType::UInt );
		binder.AddValue( "UImage1DArray",		EImageType::Img1DArray		| EImageType::UInt );
		binder.AddValue( "UImage2DArray",		EImageType::Img2DArray		| EImageType::UInt );
		binder.AddValue( "UImageCube",			EImageType::ImgCube			| EImageType::UInt );
		binder.AddValue( "UImageCubeArray",		EImageType::ImgCubeArray	| EImageType::UInt );
		binder.AddValue( "UImage2DMS",			EImageType::Img2DMS			| EImageType::UInt );
		binder.AddValue( "UImage2DMSArray",		EImageType::Img2DMSArray	| EImageType::UInt );
		binder.AddValue( "UImageBuffer",		EImageType::Buffer			| EImageType::UInt );
		// int64 / slong
		binder.AddValue( "SLongImage1D",		EImageType::Img1D			| EImageType::SLong );
		binder.AddValue( "SLongImage2D",		EImageType::Img2D			| EImageType::SLong );
		binder.AddValue( "SLongImage3D",		EImageType::Img3D			| EImageType::SLong );
		binder.AddValue( "SLongImage1DArray",	EImageType::Img1DArray		| EImageType::SLong );
		binder.AddValue( "SLongImage2DArray",	EImageType::Img2DArray		| EImageType::SLong );
		binder.AddValue( "SLongImageCube",		EImageType::ImgCube			| EImageType::SLong );
		binder.AddValue( "SLongImageCubeArray",	EImageType::ImgCubeArray	| EImageType::SLong );
		binder.AddValue( "SLongImage2DMS",		EImageType::Img2DMS			| EImageType::SLong );
		binder.AddValue( "SLongImage2DMSArray",	EImageType::Img2DMSArray	| EImageType::SLong );
		binder.AddValue( "SLongImageBuffer",	EImageType::Buffer			| EImageType::SLong );
		// uint64 / ulong
		binder.AddValue( "ULongImage1D",		EImageType::Img1D			| EImageType::ULong );
		binder.AddValue( "ULongImage2D",		EImageType::Img2D			| EImageType::ULong );
		binder.AddValue( "ULongImage3D",		EImageType::Img3D			| EImageType::ULong );
		binder.AddValue( "ULongImage1DArray",	EImageType::Img1DArray		| EImageType::ULong );
		binder.AddValue( "ULongImage2DArray",	EImageType::Img2DArray		| EImageType::ULong );
		binder.AddValue( "ULongImageCube",		EImageType::ImgCube			| EImageType::ULong );
		binder.AddValue( "ULongImageCubeArray",	EImageType::ImgCubeArray	| EImageType::ULong );
		binder.AddValue( "ULongImage2DMS",		EImageType::Img2DMS			| EImageType::ULong );
		binder.AddValue( "ULongImage2DMSArray",	EImageType::Img2DMSArray	| EImageType::ULong );
		binder.AddValue( "ULongImageBuffer",	EImageType::Buffer			| EImageType::ULong );

		StaticAssert( uint(EImageType::_TexCount)   == 11 );
		StaticAssert( uint(EImageType::_LastVal)-1  == 0xC0 );
		StaticAssert( uint(EImageType::_LastQual)-1 == 0x100 );
	}

	void  ObjectStorage::Bind_ECompilationTarget (const ScriptEnginePtr &se)
	{
		EnumBinder<ECompilationTarget>	binder{ se };
		binder.Create();
		binder.AddValue( "Vulkan",		ECompilationTarget::Vulkan );
		binder.AddValue( "Metal_iOS",	ECompilationTarget::Metal_iOS );
		binder.AddValue( "Metal_Mac",	ECompilationTarget::Metal_Mac );
		StaticAssert( uint(ECompilationTarget::_Count) == 4 );
	}

	void  ObjectStorage::Bind_EStructLayout (const ScriptEnginePtr &se)
	{
		EnumBinder<EStructLayout>	binder{ se };
		binder.Create();
		binder.Comment( "Apply GLSL std140 rules but structure must be compatible with Metal otherwise will throw exception." );
		binder.AddValue( "Compatible_Std140",	EStructLayout::Compatible_Std140 );
		binder.Comment( "Apply GLSL std430 rules but structure must be compatible with Metal otherwise will throw exception." );
		binder.AddValue( "Compatible_Std430",	EStructLayout::Compatible_Std430 );
		binder.Comment( "Apply MSL rules." );
		binder.AddValue( "Metal",				EStructLayout::Metal );
		binder.Comment( "Apply GLSL std140 rules." );
		binder.AddValue( "Std140",				EStructLayout::Std140 );
		binder.Comment( "Apply GLSL std430 rules." );
		binder.AddValue( "Std430",				EStructLayout::Std430 );
		binder.Comment( "Platform depended layout." );
		binder.AddValue( "InternalIO",			EStructLayout::InternalIO );
		StaticAssert( uint(EStructLayout::_Count) == 6 );
	}

	void  ObjectStorage::Bind_EValueType (const ScriptEnginePtr &se)
	{
		EnumBinder<EValueType>	binder{ se };
		binder.Create();
		switch_enum( EValueType::Unknown )
		{
			case EValueType::Unknown :
			case EValueType::_Count :
			#define BIND( _name_ )		case EValueType::_name_ : binder.AddValue( #_name_, EValueType::_name_ );
			BIND( Bool8 )
			BIND( Bool32 )
			BIND( Int8 )
			BIND( Int16 )
			BIND( Int32 )
			BIND( Int64 )
			BIND( UInt8 )
			BIND( UInt16 )
			BIND( UInt32 )
			BIND( UInt64 )
			BIND( Float16 )
			BIND( Float32 )
			BIND( Float64 )
			BIND( Int8_Norm )
			BIND( Int16_Norm )
			BIND( UInt8_Norm )
			BIND( UInt16_Norm )
			BIND( DeviceAddress )
			#undef BIND
			default : break;
		}
		switch_end
	}

	void  ObjectStorage::Bind_EShaderPreprocessor (const ScriptEnginePtr &se)
	{
		EnumBinder<EShaderPreprocessor>	binder{ se };
		binder.Create();
		binder.AddValue( "None",	EShaderPreprocessor::None );
		binder.Comment( "Use <aestyle.glsl.h> for auto-complete in IDE." );
		binder.AddValue( "AEStyle",	EShaderPreprocessor::AEStyle );
		StaticAssert( uint(EShaderPreprocessor::_Count) == 2 );
	}


} // AE::PipelineCompiler
