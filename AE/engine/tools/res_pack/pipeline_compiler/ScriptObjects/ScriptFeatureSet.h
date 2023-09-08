// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Rules:
    * feature is supported if one of feature set in array marked as 'True'.
    * feature is not supported if one of feature set in array marked as 'False'.
    * feature sets are incompatible if feature marked as 'True' and 'False'.
    * min limit is allowed if one of feature set in array is >= than this limit.
    * max limit is allowed if one of feature set in array is <= than this limit.

    TODO: review all FS checks
*/

#pragma once

#include "ScriptObjects/Common.h"

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
        FeatureSet      fs;
    private:
        const String                        _name;
        const FeatureSetName::Optimized_t   _hash;


    // methods
    public:
        ScriptFeatureSet () {}
        explicit ScriptFeatureSet (const String &name)      __Th___;

        static void  Bind (const ScriptEnginePtr &se)       __Th___;

        void  Build ();

        static ScriptFeatureSet*  Find (const String &name) __Th___;

        ND_ StringView  Name ()                             const   { return _name; }
        ND_ auto        Hash ()                             const   { return _hash; }

        static void  Minimize (INOUT Array<ScriptFeatureSetPtr> &feats);
    };

} // AE::PipelineCompiler


namespace AE::Math
{
/*
=================================================
    Max (ShaderVersion)
=================================================
*/
    ND_ inline Graphics::FeatureSet::ShaderVersion
        Max (const Graphics::FeatureSet::ShaderVersion &lhs, const Graphics::FeatureSet::ShaderVersion &rhs) __NE___
    {
        Graphics::FeatureSet::ShaderVersion result;
        result.spirv    = Max( lhs.spirv, rhs.spirv );
        result.metal    = Max( lhs.metal, rhs.metal );
        return result;
    }

} // AE::Math


namespace AE::PipelineCompiler
{
    void  TestFeature_PixelFormat (ArrayView<ScriptFeatureSetPtr> features, EnumBitSet<EPixelFormat> FeatureSet::*member,
                                   EPixelFormat fmt, StringView memberName, StringView message = Default) __Th___;

    void  TestFeature_VertexType (ArrayView<ScriptFeatureSetPtr> features, EnumBitSet<EVertexType> FeatureSet::*member,
                                  EVertexType fmt, StringView memberName, StringView message = Default) __Th___;

/*
=================================================
    TEST_FEATURE
=================================================
*/
#   define TEST_FEATURE( _featArr_, _feature_, ... )                                                                    \
    {                                                                                                                   \
        CHECK( not _featArr_.empty() );                                                                                 \
                                                                                                                        \
        bool    has_feat = false;                                                                                       \
        for (auto& feat : _featArr_)                                                                                    \
        {                                                                                                               \
            has_feat |= (feat->fs._feature_ == EFeature::RequireTrue);                                                  \
            CHECK_THROW_MSG( feat->fs._feature_ != EFeature::RequireFalse,                                              \
                "Feature '" # _feature_ "' required to be unsupported in FS '"s << feat->Name() << "'" __VA_ARGS__ );   \
        }                                                                                                               \
        CHECK_THROW_MSG( has_feat,                                                                                      \
            "Feature '" # _feature_ "' is not marked as RequireTrue in at least one feature set" __VA_ARGS__ );         \
    }

/*
=================================================
    TestFeature_Min
=================================================
*/
    template <typename A>
    void  TestFeature_Min (ArrayView<ScriptFeatureSetPtr> features, A FeatureSet::*member, A value, StringView memberName, StringView valueName) __Th___
    {
        CHECK( not features.empty() );

        A   max_value = 0;
        for (auto& feat : features) {
            max_value = Max( max_value, feat->fs.*member );
        }

        // 0 - limits is not specified
        if ( max_value > 0 )
        {
            CHECK_THROW_MSG( value <= max_value,
                "Specified '"s << valueName << "' (" << ToString(value) << ") must be <= than '" << memberName << "' in feature sets, " <<
                "maximum allowed value (" << ToString(max_value) << ")" );
        }
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

        A   max_value = {};
        for (auto& feat : features) {
            max_value = Max( max_value, feat->fs.*member );
        }
        return max_value;
    }

    template <typename A>
    ND_ A  GetMaxValueFromFeatures (ArrayView<ScriptFeatureSetPtr> features,
                                    FeatureSet::PerDescriptorSet FeatureSet::*base,
                                    A FeatureSet::PerDescriptorSet::*member)
    {
        CHECK( not features.empty() );

        A   max_value = {};
        for (auto& feat : features) {
            max_value = Max( max_value, feat->fs.*base.*member );
        }
        return max_value;
    }


} // AE::PipelineCompiler

