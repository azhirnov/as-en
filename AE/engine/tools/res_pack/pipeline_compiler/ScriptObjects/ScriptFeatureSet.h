// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Rules:
	* feature is supported if one of feature set in array marked as 'True'.
	* feature is not supported if one of feature set in array marked as 'False'.
	* feature sets are incompatible if feature marked as 'True' and 'False'.
	* min limit is allowed if one of feature set in array is >= than this limit.
	* max limit is allowed if one of feature set in array is <= than this limit.
	* 'Ignore' feature or zero value - means value is undefined and must be ignored.
*/

#pragma once

#include "res_pack/pipeline_compiler/ScriptObjects/Common.h"

namespace AE::PipelineCompiler
{
	struct ScriptFeatureSet;
	using ScriptFeatureSetPtr = ScriptRC< ScriptFeatureSet >;


	//
	// Feature Set
	//
	struct ScriptFeatureSet final : EnableScriptRC
	{
	// variables
	public:
		FeatureSet							fs;
	private:
		const String						_name;
		const FeatureSetName::Optimized_t	_hash;


	// methods
	public:
		ScriptFeatureSet () {}
		ND_ static ScriptFeatureSetPtr  Create (const String &name)	__Th___;

		static void  Bind (const ScriptEnginePtr &se)		__Th___;

		void  Build ()										__NE___;

		static ScriptFeatureSet*  Find (const String &name)	__Th___;

		ND_ StringView  Name ()								const	{ return _name; }
		ND_ auto		Hash ()								const	{ return _hash; }
		ND_ bool		IsIncomplete ()						const	{ return StartsWith( _name, "part." ); }

		static void  Minimize (INOUT Array<ScriptFeatureSetPtr> &feats);

		ND_ static String  GetNames (ArrayView<ScriptFeatureSetPtr>);

	private:
		explicit ScriptFeatureSet (const String &name)		__NE___;
	};

} // AE::PipelineCompiler


namespace AE::Base
{
/*
=================================================
	Max (ShaderVersion)
=================================================
*/
	Nd__In Graphics::FeatureSet::ShaderVersion
		Max (const Graphics::FeatureSet::ShaderVersion &lhs, const Graphics::FeatureSet::ShaderVersion &rhs) __NE___
	{
		Graphics::FeatureSet::ShaderVersion	result;
		result.spirv	= Max( lhs.spirv, rhs.spirv );
		result.metal	= Max( lhs.metal, rhs.metal );
		return result;
	}

} // AE::Base


namespace AE::PipelineCompiler
{
	void  TestFeature_PixelFormat (ArrayView<ScriptFeatureSetPtr> features, EnumSet<EPixelFormat> FeatureSet::*member,
								   EPixelFormat fmt, StringView memberName, StringView message = Default) __Th___;

	void  TestFeature_VertexType (ArrayView<ScriptFeatureSetPtr> features, EnumSet<EVertexType> FeatureSet::*member,
								  EVertexType fmt, StringView memberName, StringView message = Default) __Th___;

/*
=================================================
	TEST_FEATURE
=================================================
*/
#	define TEST_FEATURE_MSG( _featArr_, _feature_, _msg_ )																\
	{																													\
		CHECK_THROW_MSG( not _featArr_.empty(), "empty FeatureSet array" );												\
																														\
		bool	has_feat = false;																						\
		for (auto& feat : _featArr_)																					\
		{																												\
			has_feat |= (feat->fs._feature_ == FeatureSet::EFeature::RequireTrue);										\
			CHECK_THROW_MSG( feat->fs._feature_ != FeatureSet::EFeature::RequireFalse,									\
				"Feature '" # _feature_ "' is not supported in FS '"s << feat->Name() << "'" _msg_ );					\
		}																												\
		CHECK_THROW_MSG( has_feat,																						\
			"Feature '" # _feature_ "' is not marked as RequireTrue in at least one feature set" _msg_ ""s <<			\
			ScriptFeatureSet::GetNames( _featArr_ ));																	\
	}

#	define TEST_FEATURE( _featArr_, _feature_ )\
		TEST_FEATURE_MSG( _featArr_, _feature_, "" )

/*
=================================================
	HAS_FEATURE
=================================================
*/
#	define HAS_FEATURE( _outHasFeature_, _featArr_, _feature_ )												\
	{																										\
		ASSERT_MSG( not _featArr_.empty(), "empty FeatureSet array" );										\
																											\
		_outHasFeature_ = false;																			\
		for (auto& feat : _featArr_)																		\
		{																									\
			_outHasFeature_ |= (feat->fs._feature_ == FeatureSet::EFeature::RequireTrue);					\
																											\
			if ( feat->fs._feature_ == FeatureSet::EFeature::RequireFalse ) {								\
				_outHasFeature_ = false;																	\
				break;																						\
			}																								\
		}																									\
	}

/*
=================================================
	TestFeature_Min
=================================================
*/
	template <typename A, typename B>
	void  TestFeature_Min (ArrayView<ScriptFeatureSetPtr> features, A FeatureSet::*member, B inValue, StringView memberName, StringView valueName) __Th___
	{
		CHECK_THROW_MSG( not features.empty(), "empty FeatureSet array" );

		A	max_value = Zero;
		for (auto& feat : features) {
			max_value = Max( max_value, feat->fs.*member );
		}

		// 0 - limits is not specified
		if ( max_value > Zero )
		{
			using T = decltype(A(1u) * B(1u));

			T	value;
			CHECK_THROW_MSG( CastAndCheck( OUT value, inValue ),
				"Failed to cast ("s << ToString(inValue) << ") to '" << memberName << "' member type in feature set with limits: [" <<
				ToString( MinValue<A>() ) << "; " << ToString( MaxValue<A>() ) << "]" );

			CHECK_THROW_MSG( value <= max_value,
				"Specified '"s << valueName << "' (" << ToString(value) << ") must be <= than '" << memberName << "' in feature sets, " <<
				"maximum allowed value (" << ToString(max_value) << ")" <<
				ScriptFeatureSet::GetNames( features ));
		}
	}

/*
=================================================
	TestFeature_Supported
=================================================
*/
	inline void  TestFeature_Supported (ArrayView<ScriptFeatureSetPtr> features, const EResourceState state) __Th___
	{
		CHECK_THROW_MSG( not features.empty(), "empty FeatureSet array" );

		bool	supported = false;
		for (auto& feat : features) {
			supported |= feat->fs.IsSupported( state );
		}

		CHECK_THROW_MSG( supported,
			"Resource state ("s << ToString( state ) << ") is not supported" << ScriptFeatureSet::GetNames( features ));
	}

/*
=================================================
	GetMaxValueFromFeatures
=================================================
*/
	template <typename A>
	ND_ A  GetMaxValueFromFeatures (ArrayView<ScriptFeatureSetPtr> features, A FeatureSet::*member)
	{
		CHECK( not features.empty() );

		A	max_value = {};
		for (auto& feat : features) {
			max_value = Max( max_value, feat->fs.*member );
		}
		return max_value;
	}

	template <typename A>
	ND_ A  GetMaxValueFromFeatures (ArrayView<ScriptFeatureSetPtr> features,
									FeatureSet::PerPipeline FeatureSet::*base,
									A FeatureSet::PerPipeline::*member)
	{
		CHECK( not features.empty() );

		A	max_value = {};
		for (auto& feat : features) {
			max_value = Max( max_value, feat->fs.*base.*member );
		}
		return max_value;
	}


} // AE::PipelineCompiler

