// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "SamplerPack.h"
#include "graphics/Private/EnumToString.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ObjectStorage.h"
#endif


#ifdef AE_BUILD_PIPELINE_COMPILER
namespace AE::Graphics
{
/*
=================================================
	operator ==
=================================================
*/
	bool  SamplerDesc::operator == (const SamplerDesc &rhs) C_NE___
	{
		return	this->options				==	rhs.options				and
				this->magFilter				==	rhs.magFilter			and
				this->minFilter				==	rhs.minFilter			and
				this->mipmapMode			==	rhs.mipmapMode			and
				All( this->addressMode		==	rhs.addressMode )		and
				Equal( this->mipLodBias,		rhs.mipLodBias )		and
				Equal( this->minLod,			rhs.minLod )			and
				Equal( this->maxLod,			rhs.maxLod )			and
				Equal( this->maxAnisotropy,		rhs.maxAnisotropy )		and
				Equal( this->compareOp,			rhs.compareOp )			and
				this->borderColor			==	rhs.borderColor			and
				this->reductionMode			==	rhs.reductionMode;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SamplerYcbcrConversionDesc::operator == (const SamplerYcbcrConversionDesc &rhs) C_NE___
	{
		return	this->format			== rhs.format			and
				this->extFormat			== rhs.extFormat		and
				this->ycbcrModel		== rhs.ycbcrModel		and
				this->ycbcrRange		== rhs.ycbcrRange		and
				this->components		== rhs.components		and
				this->xChromaOffset		== rhs.xChromaOffset	and
				this->yChromaOffset		== rhs.yChromaOffset	and
				this->chromaFilter		== rhs.chromaFilter		and
				this->forceExplicitReconstruction	== rhs.forceExplicitReconstruction;
	}
}
#endif


namespace AE::PipelineCompiler
{

#ifdef AE_BUILD_PIPELINE_COMPILER
/*
=================================================
	Create
=================================================
*/
	bool  SamplerSerializer::Create (const ScriptSampler &samp)
	{
		CHECK_ERR( samp.IsValid() );

		_features	= ObjectStorage::Instance()->CopyFeatures( samp._features );
		_desc		= samp._desc;

		if ( samp._hasYcbcr )
			_ycbcrDesc = samp._ycbcrDesc;

		return true;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SamplerSerializer::operator == (const ScriptSampler &rhs) const
	{
		return	GetDesc()	== rhs.Desc()		and
				HasYcbcr()	== rhs.HasYcbcr()	and
				(HasYcbcr() and (YcbcrDesc() == rhs.YcbcrDesc()));
	}
#endif

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_TEST_PIPELINE_COMPILER
	String  SamplerSerializer::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "Sampler{"
			<< "\n  options:       " << Base::ToString( _desc.options )
			<< "\n  magFilter:     " << Base::ToString( _desc.magFilter )
			<< "\n  minFilter:     " << Base::ToString( _desc.minFilter )
			<< "\n  mipmapMode:    " << Base::ToString( _desc.mipmapMode )
			<< "\n  addressMode:   ( " << Base::ToString( _desc.addressMode.x ) << ", "
				<< Base::ToString( _desc.addressMode.y ) << ", " << Base::ToString( _desc.addressMode.z ) << " )"
			<< "\n  mipLodBias:    " << Base::ToString( _desc.mipLodBias );

		if ( _desc.HasAnisotropy() )
			str << "\n  maxAnisotropy: " << Base::ToString( _desc.maxAnisotropy );

		if ( _desc.compareOp.has_value() )
			str << "\n  compareOp:     " << Base::ToString( *_desc.compareOp );

		str << "\n  minLod:        " << Base::ToString( _desc.minLod )
			<< "\n  maxLod:        " << Base::ToString( _desc.maxLod )
			<< "\n  borderColor:   " << Base::ToString( _desc.borderColor )
			<< "\n  reductionMode: " << Base::ToString( _desc.reductionMode );

		if ( HasYcbcr() )
		{
			auto&	ycbcr	= YcbcrDesc();
			bool	is_ext	= (ycbcr.extFormat != Default);

			str << (is_ext ?
				   "\n  ycbcr.extFormat:     "s << Base::ToString( ycbcr.extFormat ) :
				   "\n  ycbcr.format:        "s << Base::ToString( ycbcr.format ))
				<< "\n  ycbcr.model:         " << ((is_ext and ycbcr.ycbcrModel == Default) ? "suggested" : Base::ToString( ycbcr.ycbcrModel ))
				<< "\n  ycbcr.range:         " << ((is_ext and ycbcr.ycbcrRange == Default) ? "suggested" : Base::ToString( ycbcr.ycbcrRange ))
				<< "\n  ycbcr.components:    " << ((is_ext and ycbcr.components.IsUndefined()) ? "suggested" :
												   (ycbcr.components.IsUndefined() ? "identity" : Base::ToString( ycbcr.components )))
				<< "\n  ycbcr.xChromaOffset: " << ((is_ext and ycbcr.xChromaOffset == Default) ? "suggested" : Base::ToString( ycbcr.xChromaOffset ))
				<< "\n  ycbcr.yChromaOffset: " << ((is_ext and ycbcr.yChromaOffset == Default) ? "suggested" : Base::ToString( ycbcr.yChromaOffset ))
				<< "\n  ycbcr.chromaFilter:  " << Base::ToString( ycbcr.chromaFilter )
				<< "\n  ycbcr.forceExplicitReconstruction: " << Base::ToString( ycbcr.forceExplicitReconstruction );
		}

		if ( not _features.empty() )
		{
			str << "\n  features:      { ";

			for (auto feat : _features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		str << '\n';
		return str;
	}
#endif

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  SamplerSerializer::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	result = true;
		// sampler desc
		result &= ser( _desc.magFilter, _desc.minFilter, _desc.mipmapMode );
		result &= ser( _desc.addressMode );
		result &= ser( _desc.mipLodBias, _desc.maxAnisotropy, _desc.compareOp );
		result &= ser( _desc.minLod, _desc.maxLod );
		result &= ser( _desc.borderColor );
		result &= ser( _desc.options, _desc.reductionMode );
		// sampler ycbcr desc
		result &= ser( HasYcbcr() );
		if ( HasYcbcr() )
		{
			SamplerYcbcrConversionDesc const&	ycbcr_desc = YcbcrDesc();
			result &= ser( ycbcr_desc.format, ycbcr_desc.extFormat );
			result &= ser( ycbcr_desc.ycbcrModel, ycbcr_desc.ycbcrRange );
			result &= ser( ycbcr_desc.components );
			result &= ser( ycbcr_desc.xChromaOffset, ycbcr_desc.yChromaOffset );
			result &= ser( ycbcr_desc.chromaFilter, ycbcr_desc.forceExplicitReconstruction );
		}
		// features
		result &= ser( _features );
		return result;
	}

	bool  SamplerSerializer::Deserialize (Serializing::Deserializer &des) __NE___
	{
		bool	result = true;
		// sampler desc
		result &= des( OUT _desc.magFilter, OUT _desc.minFilter, OUT _desc.mipmapMode );
		result &= des( OUT _desc.addressMode );
		result &= des( OUT _desc.mipLodBias, OUT _desc.maxAnisotropy, OUT _desc.compareOp );
		result &= des( OUT _desc.minLod, OUT _desc.maxLod );
		result &= des( OUT _desc.borderColor	);
		result &= des( OUT _desc.options, OUT _desc.reductionMode );
		// sampler ycbcr desc
		bool	has_ycbcr = false;
		result &= des( OUT has_ycbcr );
		if ( has_ycbcr )
		{
			SamplerYcbcrConversionDesc	ycbcr_desc;
			result &= des( OUT ycbcr_desc.format, OUT ycbcr_desc.extFormat );
			result &= des( OUT ycbcr_desc.ycbcrModel, OUT ycbcr_desc.ycbcrRange );
			result &= des( OUT ycbcr_desc.components );
			result &= des( OUT ycbcr_desc.xChromaOffset, OUT ycbcr_desc.yChromaOffset );
			result &= des( OUT ycbcr_desc.chromaFilter, OUT ycbcr_desc.forceExplicitReconstruction );
			_ycbcrDesc = ycbcr_desc;
		}
		// features
		result &= des( OUT _features );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
#ifdef AE_BUILD_PIPELINE_COMPILER
	bool  SamplerPacker::Serialize (Serializing::Serializer &ser) __NE___
	{
		using SamplerMap_t		= HashMultiMap< HashVal, SamplerUID >;
		using SamplerArr_t		= Array< SamplerSerializer >;
		using SamplerNameMap_t	= HashMap< SamplerName, SamplerUID >;

		SamplerMap_t		unique_samplers;
		SamplerArr_t		sampler_arr;
		SamplerNameMap_t	sampler_names;
		const auto&			sampler_refs	= ObjectStorage::Instance()->samplerRefs;

		CHECK_ERR( not sampler_refs.empty() );
		CHECK_ERR( sampler_refs.size() <= SamplerSerializer::MaxCount );

		const auto	AddSampler = [&] (const ScriptSamplerPtr &samp) -> bool
		{{
			const HashVal	hash	= samp->CalcHash();
			auto			iter	= unique_samplers.find( hash );

			for (; iter != unique_samplers.end() and iter->first == hash; ++iter)
			{
				auto&	lhs = sampler_arr[ usize(iter->second) ];

				if ( lhs == *samp )
				{
					CHECK_ERR( sampler_names.insert_or_assign( samp->_name, iter->second ).second );
					return true;
				}
			}

			auto				uid = SamplerUID( sampler_arr.size() );
			SamplerSerializer	samp_ser;

			CHECK_ERR( samp_ser.Create( *samp ));

			unique_samplers.emplace( hash, uid );
			sampler_arr.push_back( samp_ser );

			CHECK_ERR( sampler_names.insert_or_assign( samp->_name, uid ).second );
			return true;
		}};

		bool	is_valid = true;
		for (auto& samp : sampler_refs)
		{
			is_valid &= samp->Validate();
			CHECK_ERR( AddSampler( samp ));
		}
		CHECK_ERR( is_valid );

		Array<Pair< SamplerName, SamplerUID >>	sampler_names2;
		for (auto& item : sampler_names) {
			sampler_names2.push_back( item );
		}
		std::sort( sampler_names2.begin(), sampler_names2.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });


		bool	result = true;
		result &= ser( SamplerPack_Name );
		result &= ser( SamplerPack_Version );
		result &= ser( sampler_names2 );
		result &= ser( sampler_arr );
		CHECK_ERR( result );

		AE_LOG_DBG( "Serialized samplers: "s << ToString(sampler_names2.size()) );
		return true;
	}
#endif

} // AE::PipelineCompiler
