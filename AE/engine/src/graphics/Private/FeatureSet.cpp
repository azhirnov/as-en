// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"
#include "graphics/Private/EnumToString.h"
#include "graphics/Public/FeatureSet.h"


template <>
struct std::hash< AE::Graphics::FeatureSet::PerDescriptorSet > :
    AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::PerDescriptorSet >
{};

template <typename E>
struct std::hash< AE::Graphics::FeatureSet::IncludeExcludeBits<E> > :
    AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::IncludeExcludeBits<E> >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::ShaderVersion > :
    AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::ShaderVersion >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::Queues > :
    AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::Queues >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::VRSTexelSize > :
    AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::VRSTexelSize >
{};


namespace AE::Graphics
{
namespace
{
    template <typename T1, typename T2>
    using MajType2 = Conditional< (sizeof(T1) >= sizeof(T2)), T1, T2 >;

    template <typename T1, typename T2>
    using MajType = MajType2< RemoveCVRef<T1>, RemoveCVRef<T2> >;

/*
=================================================
    GetVertexTypeBits
=================================================
*/
    ND_ static constexpr FeatureSet::VertexFormatSet_t  GetVertexTypeBits () __NE___
    {
        FeatureSet::VertexFormatSet_t   bits;
        #define AE_VERTEXTYPE_VISIT( _name_, _value_ )  bits.insert( EVertexType::_name_ );
        AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT );
        #undef AE_VERTEXTYPE_VISIT
        return bits;
    }

    static constexpr FeatureSet::VertexFormatSet_t  VertexTypeBits = GetVertexTypeBits();

/*
=================================================
    FS_Equal
=================================================
*/
    template <typename T>
    ND_ static bool  FS_Equal (const T &lhs, const T &rhs, const char*) __NE___ {
        return lhs == rhs;
    }

    ND_ static bool  FS_Equal (EFeature lhs, EFeature rhs, const char*) __NE___ {
        return  (lhs == rhs) | (lhs == EFeature::Ignore) | (rhs == EFeature::Ignore);
    }

    template <typename E>
    ND_ static bool  FS_Equal (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___ {
        return  lhs.include == rhs.include  and
                lhs.exclude == rhs.exclude;
    }

    ND_ static bool  FS_Equal (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___ {
        return  lhs.spirv == rhs.spirv      and
                lhs.metal == rhs.metal;
    }

    ND_ static bool  FS_Equal (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___ {
        return  lhs.minX    == rhs.minX     and
                lhs.minY    == rhs.minY     and
                lhs.maxX    == rhs.maxX     and
                lhs.maxY    == rhs.maxY     and
                lhs.aspect  == rhs.aspect;
    }

/*
=================================================
    FS_GreaterEqual
=================================================
*/
    ND_ static bool  FS_GreaterEqual (EFeature lhs, EFeature rhs, const char*) __NE___ {
        return  (lhs >= rhs) | (lhs == EFeature::Ignore) | (rhs == EFeature::Ignore);
    }

    ND_ static bool  FS_GreaterEqual (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
        return rhs == Default or AllBits( lhs, rhs );
    }

    ND_ static bool  FS_GreaterEqual (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
        return rhs == Default or AllBits( lhs, rhs );
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___ {
        return  rhs.required == Default or
                (AllBits( lhs.supported | rhs.supported, lhs.required | rhs.required ) and
                 AllBits( lhs.required, rhs.required ));
    }

    ND_ static bool  FS_GreaterEqual (ubyte lhs, ubyte rhs, const char*) __NE___ {
        return lhs >= rhs;
    }

    ND_ static bool  FS_GreaterEqual (ushort lhs, ushort rhs, const char*) __NE___ {
        return lhs >= rhs;
    }

    ND_ static bool  FS_GreaterEqual (uint lhs, uint rhs, const char*) __NE___ {
        return lhs >= rhs;
    }

    ND_ static bool  FS_GreaterEqual (float lhs, float rhs, const char*) __NE___ {
        return lhs >= rhs;
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
        return (uint(lhs) & uint(rhs)) == uint(rhs);
    }

    template <typename E>
    ND_ static bool  FS_GreaterEqual (const EnumBitSet<E> &lhs, const EnumBitSet<E> &rhs, const char*) __NE___ {
        return (lhs & rhs) == rhs;
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___ {
        return lhs >= rhs;
    }

    template <typename E>
    ND_ static bool  FS_GreaterEqual (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___ {
        return  (lhs.include & rhs.include) == rhs.include  and
                (lhs.exclude & rhs.exclude) == rhs.exclude;
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___ {
        return  lhs.spirv >= rhs.spirv  and
                lhs.metal >= rhs.metal;
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
    {
        if ( lhs.empty() and rhs.empty() )
            return true;

        const uint  size_mask   = uint(EShadingRate::_SizeMask);
        const uint  samp_mask   = uint(EShadingRate::_SamplesMask);
        auto        it1         = lhs.begin(), it2 = rhs.begin();

        for (;;)
        {
            if ( it2 == rhs.end() ) break;
            if ( it1 == lhs.end() ) break;

            if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
            {
                if ( (uint(*it1) & samp_mask) < (uint(*it2) & samp_mask) )
                    return false;

                ++it1; ++it2;
            }
            else
            if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
                ++it1;
            else
            if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
                ++it2;
        }
        return true;
    }

    ND_ static bool  FS_GreaterEqual (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
    {
        return  lhs.minX    >= rhs.minX     and
                lhs.minY    >= rhs.minY     and
                lhs.maxX    >= rhs.maxX     and
                lhs.maxY    >= rhs.maxY     and
                lhs.aspect  == rhs.aspect;
    }

/*
=================================================
    FS_IsCompatible
=================================================
*/
    ND_ static bool  FS_IsCompatible (EFeature lhs, EFeature rhs, const char*) __NE___ {
        return  (lhs == rhs) | (lhs == EFeature::Ignore) | (rhs == EFeature::Ignore);   // 'True' is not compatible with 'False'
    }

    template <typename T>
    ND_ static bool  FS_IsCompatible (const T& lhs, const T& rhs, const char* name) __NE___ {
        return FS_GreaterEqual( lhs, rhs, name );
    }

    ND_ static bool  FS_IsCompatible (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
    {
        bool    result = true;
        if ( lhs.spirv != 0 and rhs.spirv != 0 )    result &= (lhs.spirv >= rhs.spirv);
        if ( lhs.metal != 0 and rhs.metal != 0 )    result &= (lhs.metal >= rhs.metal);
        return result;
    }

    template <typename E>
    ND_ static bool  FS_IsCompatible (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
    {
        if ( lhs.None() or rhs.None() )
            return true;

        return  (lhs.include & rhs.include).Any()   and
                (lhs.include & rhs.exclude).None()  and
                (rhs.include & lhs.exclude).None();
    }

/*
=================================================
    FS_Set
=================================================
*/
    template <typename T>
    ND_ static T  FS_Set (EFeature feat) __NE___
    {
        if constexpr( IsSameTypes< T, EFeature >)
            return feat;
        else
            return {};
    }

/*
=================================================
    FS_MergeMin
=================================================
*/
    ND_ static EFeature  FS_MergeMin (EFeature lhs, EFeature rhs, const char*) __NE___
    {
        if ( lhs == rhs )
            return lhs;

        if ( lhs == EFeature::RequireFalse or rhs == EFeature::RequireFalse )
            return EFeature::RequireFalse;

        return EFeature::Ignore;
    }

    template <typename E>
    ND_ static EnumBitSet<E>  FS_MergeMin (const EnumBitSet<E> &lhs, const EnumBitSet<E> &rhs, const char*) __NE___ {
        return lhs & rhs;
    }

    ND_ static ESubgroupTypes  FS_MergeMin (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
        return lhs & rhs;
    }

    ND_ static EShaderStages  FS_MergeMin (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
        return lhs & rhs;
    }

    ND_ static FeatureSet::Queues  FS_MergeMin (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___
    {
        FeatureSet::Queues  res;
        res.supported   = lhs.supported & rhs.supported;
        res.required    = (lhs.required & rhs.required) & res.supported;
        return res;
    }

    ND_ static ubyte  FS_MergeMin (ubyte lhs, ubyte rhs, const char*) __NE___ {
        return Min( lhs, rhs );
    }

    ND_ static ushort  FS_MergeMin (ushort lhs, ushort rhs, const char*) __NE___ {
        return Min( lhs, rhs );
    }

    ND_ static uint  FS_MergeMin (uint lhs, uint rhs, const char*) __NE___ {
        return Min( lhs, rhs );
    }

    ND_ static float  FS_MergeMin (float lhs, float rhs, const char*) __NE___ {
        return Min( lhs, rhs );
    }

    ND_ static FeatureSet::ShadingRateSet_t  FS_MergeMin (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
    {
        if ( lhs.empty() and rhs.empty() )
            return {};

        FeatureSet::ShadingRateSet_t    result;
        const uint                      size_mask   = uint(EShadingRate::_SizeMask);
        const uint                      samp_mask   = uint(EShadingRate::_SamplesMask);
        auto                            it1         = lhs.begin(), it2 = rhs.begin();

        for (;;)
        {
            if ( it1 == lhs.end() ) break;
            if ( it2 == rhs.end() ) break;

            if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
            {
                result.push_back(EShadingRate( (uint(*it1) & size_mask) | ((uint(*it1) & samp_mask) & (uint(*it2) & samp_mask)) ));
                ++it1; ++it2;
                continue;
            }

            if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
                ++it1;
            else
            if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
                ++it2;
        }
        ASSERT( IsSorted( result.begin(), result.end() ));
        return result;
    }

    ND_ static FeatureSet::ShaderVersion  FS_MergeMin (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
    {
        FeatureSet::ShaderVersion   res;
        if ( lhs.spirv != 0 and rhs.spirv != 0 )    res.spirv = Min( lhs.spirv, rhs.spirv );
        if ( lhs.metal != 0 and rhs.metal != 0 )    res.metal = Min( lhs.metal, rhs.metal );
        return res;
    }

    ND_ static FeatureSet::VRSTexelSize  FS_MergeMin (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
    {
        if ( not IsIntersects( lhs.minX, lhs.maxX, rhs.minX, rhs.maxX ) or
             not IsIntersects( lhs.minY, lhs.maxY, rhs.minY, rhs.maxY ))
            return {};

        FeatureSet::VRSTexelSize    res;
        res.minX    = Max( lhs.minX,    rhs.minX );
        res.minY    = Max( lhs.minY,    rhs.minY );
        res.maxX    = Min( lhs.maxX,    rhs.maxX );
        res.maxY    = Min( lhs.maxY,    rhs.maxY );
        res.aspect  = Min( lhs.aspect,  rhs.aspect );
        return res;
    }

    ND_ static FeatureSet::SampleCountBits  FS_MergeMin (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
        return FeatureSet::SampleCountBits{ uint(lhs) & uint(rhs) };
    }

    ND_ static FeatureSet::PerDescriptorSet  FS_MergeMin (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___
    {
        FeatureSet::PerDescriptorSet    res;
        res.maxInputAttachments = FS_MergeMin( lhs.maxInputAttachments, rhs.maxInputAttachments,"maxInputAttachments"   );
        res.maxSampledImages    = FS_MergeMin( lhs.maxSampledImages,    rhs.maxSampledImages,   "maxSampledImages"      );
        res.maxSamplers         = FS_MergeMin( lhs.maxSamplers,         rhs.maxSamplers,        "maxSamplers"           );
        res.maxStorageBuffers   = FS_MergeMin( lhs.maxStorageBuffers,   rhs.maxStorageBuffers,  "maxStorageBuffers"     );
        res.maxStorageImages    = FS_MergeMin( lhs.maxStorageImages,    rhs.maxStorageImages,   "maxStorageImages"      );
        res.maxUniformBuffers   = FS_MergeMin( lhs.maxUniformBuffers,   rhs.maxUniformBuffers,  "maxUniformBuffers"     );
        res.maxAccelStructures  = FS_MergeMin( lhs.maxAccelStructures,  rhs.maxAccelStructures, "maxAccelStructures"    );
        res.maxTotalResources   = FS_MergeMin( lhs.maxTotalResources,   rhs.maxTotalResources,  "maxTotalResources"     );
        STATIC_ASSERT( sizeof(FeatureSet::PerDescriptorSet) == sizeof(uint)*8 );
        return res;
    }

    template <typename E>
    ND_ static FeatureSet::IncludeExcludeBits<E>  FS_MergeMin (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
    {
        FeatureSet::IncludeExcludeBits<E>   res;
        res.include  = lhs.include & rhs.include;
        res.include &= ~(lhs.exclude | rhs.exclude);
        res.exclude  = lhs.exclude & rhs.exclude;
        res.exclude &= ~res.include;
        return res;
    }

/*
=================================================
    FS_MergeMax
=================================================
*/
    ND_ static EFeature  FS_MergeMax (EFeature lhs, EFeature rhs, const char*) __NE___
    {
        if ( lhs == EFeature::RequireTrue or rhs == EFeature::RequireTrue )
            return EFeature::RequireTrue;

        if ( lhs == EFeature::RequireFalse or rhs == EFeature::RequireFalse )
            return EFeature::RequireFalse;

        return EFeature::Ignore;
    }

    template <typename E>
    ND_ static EnumBitSet<E>  FS_MergeMax (const EnumBitSet<E> &lhs, const EnumBitSet<E> &rhs, const char*) __NE___ {
        return lhs | rhs;
    }

    ND_ static ESubgroupTypes  FS_MergeMax (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
        return lhs | rhs;
    }

    ND_ static EShaderStages  FS_MergeMax (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
        return lhs | rhs;
    }

    ND_ static FeatureSet::Queues  FS_MergeMax (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___
    {
        FeatureSet::Queues  res;
        res.supported   = lhs.supported | rhs.supported;
        res.required    = (lhs.required | rhs.required) & res.supported;
        return res;
    }

    ND_ static ubyte  FS_MergeMax (ubyte lhs, ubyte rhs, const char*) __NE___ {
        return Max( lhs, rhs );
    }

    ND_ static ushort  FS_MergeMax (ushort lhs, ushort rhs, const char*) __NE___ {
        return Max( lhs, rhs );
    }

    ND_ static uint  FS_MergeMax (uint lhs, uint rhs, const char*) __NE___ {
        return Max( lhs, rhs );
    }

    ND_ static float  FS_MergeMax (float lhs, float rhs, const char*) __NE___ {
        return Max( lhs, rhs );
    }

    ND_ static FeatureSet::ShaderVersion  FS_MergeMax (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
    {
        FeatureSet::ShaderVersion   res;
        res.spirv = Max( lhs.spirv, rhs.spirv );
        res.metal = Max( lhs.metal, rhs.metal );
        return res;
    }

    ND_ static FeatureSet::VRSTexelSize  FS_MergeMax (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
    {
        FeatureSet::VRSTexelSize    res;
        res.minX    = Min( lhs.minX,    rhs.minX );
        res.minY    = Min( lhs.minY,    rhs.minY );
        res.maxX    = Max( lhs.maxX,    rhs.maxX );
        res.maxY    = Max( lhs.maxY,    rhs.maxY );
        res.aspect  = Max( lhs.aspect,  rhs.aspect );
        return res;
    }

    ND_ static FeatureSet::ShadingRateSet_t  FS_MergeMax (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
    {
        if ( lhs.empty() and rhs.empty() )
            return {};

        FeatureSet::ShadingRateSet_t    result;
        const uint                      size_mask   = uint(EShadingRate::_SizeMask);
        const uint                      samp_mask   = uint(EShadingRate::_SamplesMask);
        auto                            it1         = lhs.begin(), it2 = rhs.begin();

        for (;;)
        {
            if ( it1 == lhs.end() ) break;
            if ( it2 == rhs.end() ) break;

            if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
            {
                result.push_back(EShadingRate( uint(*it1) | (uint(*it2) & samp_mask) ));
                ++it1; ++it2;
                continue;
            }

            if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
                ++it1;
            else
            if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
                ++it2;
        }
        ASSERT( IsSorted( result.begin(), result.end() ));
        return result;
    }

    ND_ static FeatureSet::SampleCountBits  FS_MergeMax (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
        return FeatureSet::SampleCountBits{ uint(lhs) | uint(rhs) };
    }

    ND_ static FeatureSet::PerDescriptorSet  FS_MergeMax (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___
    {
        FeatureSet::PerDescriptorSet    res;
        res.maxInputAttachments = FS_MergeMax( lhs.maxInputAttachments, rhs.maxInputAttachments,    "maxInputAttachments"   );
        res.maxSampledImages    = FS_MergeMax( lhs.maxSampledImages,    rhs.maxSampledImages,       "maxSampledImages"      );
        res.maxSamplers         = FS_MergeMax( lhs.maxSamplers,         rhs.maxSamplers,            "maxSamplers"           );
        res.maxStorageBuffers   = FS_MergeMax( lhs.maxStorageBuffers,   rhs.maxStorageBuffers,      "maxStorageBuffers"     );
        res.maxStorageImages    = FS_MergeMax( lhs.maxStorageImages,    rhs.maxStorageImages,       "maxStorageImages"      );
        res.maxUniformBuffers   = FS_MergeMax( lhs.maxUniformBuffers,   rhs.maxUniformBuffers,      "maxUniformBuffers"     );
        res.maxAccelStructures  = FS_MergeMax( lhs.maxAccelStructures,  rhs.maxAccelStructures,     "maxAccelStructures"    );
        res.maxTotalResources   = FS_MergeMax( lhs.maxTotalResources,   rhs.maxTotalResources,      "maxTotalResources"     );
        return res;
    }

    template <typename E>
    ND_ static FeatureSet::IncludeExcludeBits<E>  FS_MergeMax (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
    {
        FeatureSet::IncludeExcludeBits<E>   res;
        res.include = lhs.include | rhs.include;
        res.exclude = lhs.exclude & rhs.exclude;
        res.exclude &= ~res.include;
        return res;
    }

/*
=================================================
    FS_Validate***
=================================================
*/
    template <typename T1, typename T2>
    ND_ static T1  FS_ValidateEq (const T1 lhs, const T2 rhs) __NE___
    {
        return rhs;
    }

    template <bool Mutable, typename T1, typename T2, typename T3>
    static void  FS_ValidateNotEq (T1 &lhs, const T2 &rhs, const T3 &) __NE___
    {
        if constexpr( Mutable )
        {
            lhs = rhs;
        }
        else
        {
            CHECK_ERRV( lhs != rhs );
        }
    }

} // namespace

/*
=================================================
    operator ==
=================================================
*/
    bool  FeatureSet::PerDescriptorSet::operator == (const PerDescriptorSet &rhs) C_NE___
    {
        return  maxInputAttachments == rhs.maxInputAttachments  and
                maxSampledImages    == rhs.maxSampledImages     and
                maxSamplers         == rhs.maxSamplers          and
                maxStorageBuffers   == rhs.maxStorageBuffers    and
                maxStorageImages    == rhs.maxStorageImages     and
                maxUniformBuffers   == rhs.maxUniformBuffers    and
                maxAccelStructures  == rhs.maxAccelStructures   and
                maxTotalResources   == rhs.maxTotalResources;
    }

/*
=================================================
    operator >=
=================================================
*/
    bool  FeatureSet::PerDescriptorSet::operator >= (const PerDescriptorSet &rhs) C_NE___
    {
        return  maxInputAttachments >= rhs.maxInputAttachments  and
                maxSampledImages    >= rhs.maxSampledImages     and
                maxSamplers         >= rhs.maxSamplers          and
                maxStorageBuffers   >= rhs.maxStorageBuffers    and
                maxStorageImages    >= rhs.maxStorageImages     and
                maxUniformBuffers   >= rhs.maxUniformBuffers    and
                maxAccelStructures  >= rhs.maxAccelStructures   and
                maxTotalResources   >= rhs.maxTotalResources;
    }

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  FeatureSet::PerDescriptorSet::CalcHash () C_NE___
    {
        return  HashOf( maxInputAttachments ) + HashOf( maxSampledImages ) + HashOf( maxSamplers ) +
                HashOf( maxStorageBuffers ) + HashOf( maxStorageImages ) + HashOf( maxUniformBuffers ) +
                HashOf( maxTotalResources );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    FeatureSet::FeatureSet () __NE___
    {
        ZeroMem( OUT this, Sizeof(*this) );
    }

/*
=================================================
    _Validate
=================================================
*/
    template <bool Mutable>
    bool  FeatureSet::_Validate () __NE___
    {
        #define chEqual2( _lhs_, _ref_, _op_ )                      \
            {using A = MajType< decltype(_lhs_), decltype(_ref_)>;  \
             if constexpr( Mutable ){                               \
                if ( not (A(_lhs_) == A(_ref_)) )                   \
                    (_op_);                                         \
            }else{                                                  \
                CHECK_ERR( A(_lhs_) == A(_ref_) );                  \
            }}

        #define chNotEqual2( _lhs_, _ref_, _valid_ )                \
            {using A = MajType< decltype(_lhs_), decltype(_ref_)>;  \
             if constexpr( Mutable ){                               \
                if ( not (A(_lhs_) != A(_ref_)) )                   \
                    (_lhs_) = (_valid_);                            \
            }else{                                                  \
                CHECK_ERR( A(_lhs_) != A(_ref_) );                  \
            }}

        #define chLessEq2( _lhs_, _ref_, _valid_ )                  \
            {using A = MajType< decltype(_lhs_), decltype(_ref_)>;  \
             if constexpr( Mutable ){                               \
                if ( not (A(_lhs_) <= A(_ref_)) )                   \
                    (_lhs_) = (_valid_);                            \
            }else{                                                  \
                CHECK_ERR( A(_lhs_) <= A(_ref_) );                  \
            }}

        #define chGreaterEq2( _lhs_, _ref_, _valid_ )               \
            {using A = MajType< decltype(_lhs_), decltype(_ref_)>;  \
             if constexpr( Mutable ){                               \
                if ( not (A(_lhs_) >= A(_ref_)) )                   \
                    (_lhs_) = (_valid_);                            \
            }else{                                                  \
                CHECK_ERR( A(_lhs_) >= A(_ref_) );                  \
            }}

        #define chEqual(     _lhs_, _ref_ )     chEqual2(     (_lhs_), (_ref_), (_lhs_) = (_ref_) )
        #define chNotEqual(  _lhs_, _ref_ )     chNotEqual2(  (_lhs_), (_ref_), (_ref_) )
        #define chLessEq(    _lhs_, _ref_ )     chLessEq2(    (_lhs_), (_ref_), (_ref_) )
        #define chGreaterEq( _lhs_, _ref_ )     chGreaterEq2( (_lhs_), (_ref_), (_ref_) )

        #define fEqual( _lhs_, _rhs_ )          (EFeature(_lhs_) == (_rhs_))
        #define fNotEq( _lhs_, _rhs_ )          (EFeature(_lhs_) != (_rhs_))

        EShaderStages   all_stages = EShaderStages::All;
        if ( fNotEq( computeShader,      EFeature::RequireTrue ))   all_stages &= ~EShaderStages::Compute;
        if ( fNotEq( geometryShader,     EFeature::RequireTrue ))   all_stages &= ~EShaderStages::Geometry;
        if ( fNotEq( tessellationShader, EFeature::RequireTrue ))   all_stages &= ~(EShaderStages::TessControl | EShaderStages::TessEvaluation);
        if ( fNotEq( tileShader,         EFeature::RequireTrue ))   all_stages &= ~EShaderStages::Tile;
        if ( fNotEq( taskShader,         EFeature::RequireTrue ))   all_stages &= ~EShaderStages::MeshTask;
        if ( fNotEq( meshShader,         EFeature::RequireTrue ))   all_stages &= ~EShaderStages::Mesh;
        if ( fNotEq( rayTracingPipeline, EFeature::RequireTrue ))   all_stages &= ~EShaderStages::AllRayTracing;

        const auto  neg_feat = EFeature::Ignore;

        if ( fEqual( subgroup, EFeature::RequireTrue ))
        {
            chNotEqual2( subgroupOperations, SubgroupOperationBits{},   SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End ));
            chNotEqual2( subgroupTypes,      ESubgroupTypes::Unknown,   ESubgroupTypes::Float32 );
            chNotEqual2( subgroupStages,     EShaderStages::Unknown,    EShaderStages::Fragment | EShaderStages::Compute);

            chEqual2(    AllBits( subgroupTypes,  ESubgroupTypes::Float32 ),    true,   subgroupTypes  |= ESubgroupTypes::Float32 );
            chEqual2(    AllBits( subgroupStages, EShaderStages::Compute ),     true,   subgroupStages |= EShaderStages::Compute  );
            chEqual2(    AnyBits( subgroupStages, ~all_stages ),                false,  subgroupStages &= all_stages  );

            if ( fNotEq( shaderInt8,    EFeature::RequireTrue ))    chEqual2(    AllBits( subgroupTypes, ESubgroupTypes::Int8 ),    false,  subgroupTypes &= ~ESubgroupTypes::Int8 );
            if ( fNotEq( shaderInt16,   EFeature::RequireTrue ))    chEqual2(    AllBits( subgroupTypes, ESubgroupTypes::Int16 ),   false,  subgroupTypes &= ~ESubgroupTypes::Int16 );
            if ( fNotEq( shaderInt64,   EFeature::RequireTrue ))    chEqual2(    AllBits( subgroupTypes, ESubgroupTypes::Int64 ),   false,  subgroupTypes &= ~ESubgroupTypes::Int64 );
            if ( fNotEq( shaderFloat16, EFeature::RequireTrue ))    chEqual2(    AllBits( subgroupTypes, ESubgroupTypes::Float16 ), false,  subgroupTypes &= ~ESubgroupTypes::Float16 );

            chGreaterEq( minSubgroupSize,                   1 );
            chEqual2(    IsPowerOfTwo( minSubgroupSize ),   true,   minSubgroupSize = CeilPOT(  minSubgroupSize ));
            chEqual2(    IsPowerOfTwo( maxSubgroupSize ),   true,   maxSubgroupSize = FloorPOT( maxSubgroupSize ));
            chGreaterEq( maxSubgroupSize,                   minSubgroupSize );

            if ( subgroupOperations.AnyInRange( ESubgroupOperation::_Quad_Begin, ESubgroupOperation::_Quad_End ))
            {
                chNotEqual2( subgroupQuadStages,                            EShaderStages::Unknown, EShaderStages::Fragment | EShaderStages::Compute );
                chEqual2(    AnyBits( subgroupQuadStages, ~all_stages ),    false,                  subgroupQuadStages &= all_stages                 );
            }
        }
        else
        {
            chEqual( subgroupOperations,    SubgroupOperationBits{} );
            chEqual( subgroupTypes,         ESubgroupTypes::Unknown );
            chEqual( subgroupStages,        EShaderStages::Unknown );
            chEqual( subgroupQuadStages,    EShaderStages::Unknown );
            chEqual( minSubgroupSize,       0 );
            chEqual( maxSubgroupSize,       0 );
        }

        if ( fEqual( subgroupSizeControl, EFeature::RequireTrue ))
        {
            chEqual( subgroup, EFeature::RequireTrue );
            // requiredSubgroupSizeStages can be 0
            chEqual2( AnyBits( requiredSubgroupSizeStages, ~all_stages ), false, requiredSubgroupSizeStages &= all_stages );
        }
        else
        {
            chEqual( maxSubgroupSize,               minSubgroupSize );
            chEqual( requiredSubgroupSizeStages,    EShaderStages::Unknown );
        }

        if ( fEqual( attachmentFragmentShadingRate, EFeature::RequireTrue )) {
            if constexpr( Mutable ) {
                if (not fragmentShadingRateTexelSize)
                    attachmentFragmentShadingRate = EFeature::Ignore;
            }else
                CHECK_ERR( fragmentShadingRateTexelSize );
        }else{
            if constexpr( Mutable )
                fragmentShadingRateTexelSize = VRSTexelSize{};
            else
                CHECK_ERR( not fragmentShadingRateTexelSize );
        }

        if ( fEqual( pipelineFragmentShadingRate,   EFeature::RequireTrue ) or
             fEqual( primitiveFragmentShadingRate,  EFeature::RequireTrue ) or
             fEqual( attachmentFragmentShadingRate, EFeature::RequireTrue ))
        {
            if constexpr( Mutable ){
                if ( fragmentShadingRates.empty() )
                    fragmentShadingRates.push_back( EShadingRate::Size1x1 | EShadingRate::Samples1 );
            }else{
                CHECK_ERR( not fragmentShadingRates.empty() );
            }
        }
        else
        {
            chNotEqual2( primitiveFragmentShadingRateWithMultipleViewports, EFeature::RequireTrue, neg_feat );
            chNotEqual2( layeredShadingRateAttachments,                     EFeature::RequireTrue, neg_feat );
            chNotEqual2( fragmentShadingRateWithShaderDepthStencilWrites,   EFeature::RequireTrue, neg_feat );
            chNotEqual2( fragmentShadingRateWithSampleMask,                 EFeature::RequireTrue, neg_feat );
            chNotEqual2( fragmentShadingRateWithShaderSampleMask,           EFeature::RequireTrue, neg_feat );
            chNotEqual2( fragmentShadingRateWithFragmentShaderInterlock,    EFeature::RequireTrue, neg_feat );
            chNotEqual2( fragmentShadingRateWithCustomSampleLocations,      EFeature::RequireTrue, neg_feat );
        }

        if ( fNotEq( bufferDeviceAddress, EFeature::RequireTrue ))
        {
            if constexpr( Mutable ){
                rayTracingPipeline  = neg_feat;
                rayQuery            = neg_feat;
            }
        }

        if ( fEqual( rayTracingPipeline, EFeature::RequireTrue ))
        {
            chGreaterEq( maxShaderVersion.spirv, 140 );
            if constexpr( not Mutable ) { CHECK( fEqual( bufferDeviceAddress, EFeature::RequireTrue )); }
        }
        else
        {
            chNotEqual2( rayTraversalPrimitiveCulling,  EFeature::RequireTrue, neg_feat );
            chEqual(     maxRayRecursionDepth,          0 );
        }

        if ( fEqual( rayQuery, EFeature::RequireTrue ))
        {
            chNotEqual2( rayQueryStages,                            EShaderStages::Unknown, EShaderStages::Fragment | EShaderStages::Compute );
            chEqual2(    AnyBits( rayQueryStages, ~all_stages ),    false,                  rayQueryStages &= all_stages                     );
            if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
            if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 230 ); }
            if constexpr( not Mutable ) { CHECK( fNotEq( bufferDeviceAddress, EFeature::RequireFalse )); }
        }
        else
        {
            chEqual( rayQueryStages, EShaderStages::Unknown );
        }

        if ( fEqual( rayTracingPipeline, EFeature::RequireTrue ) or
             fEqual( rayQuery, EFeature::RequireTrue ))
        {
            chNotEqual2( accelStructVertexFormats,  VertexFormatSet_t{},    VertexFormatSet_t{}.insert( EVertexType::Float3 ));
        }
        else
        {
            chEqual( accelStructVertexFormats,  VertexFormatSet_t{} );
        }

        if ( maxShaderVersion.spirv != 0 )
        {
            chGreaterEq( maxShaderVersion.spirv, MinSpirvVersion );
            chLessEq(    maxShaderVersion.spirv, MaxSpirvVersion );
        }
        if ( maxShaderVersion.metal != 0 )
        {
            chGreaterEq( maxShaderVersion.metal, MinMetalVersion );
            chLessEq(    maxShaderVersion.metal, MaxMetalVersion );
        }

        if ( fEqual( multiview, EFeature::RequireTrue ))
        {
            chGreaterEq( maxMultiviewViewCount, 1 );
        }
        else
        {
            chNotEqual2( multiviewGeometryShader,       EFeature::RequireTrue, neg_feat );
            chNotEqual2( multiviewTessellationShader,   EFeature::RequireTrue, neg_feat );
            chLessEq(    maxMultiviewViewCount,         1 );
        }

        if ( fNotEq( multiViewport, EFeature::RequireTrue ))
        {
            chEqual( maxViewports, 1 );
        }
        chGreaterEq( maxViewports, 1 );

        chGreaterEq( maxUniformBufferSize, 1 );
        chGreaterEq( maxStorageBufferSize, 0 );

        chGreaterEq( perDescrSet.maxUniformBuffers, 1 );
        chGreaterEq( perDescrSet.maxSampledImages,  1 );
        chGreaterEq( perDescrSet.maxTotalResources, 1 );
        chGreaterEq( perStage.maxUniformBuffers, 1 );
        chGreaterEq( perStage.maxSampledImages,  1 );
        chGreaterEq( perStage.maxTotalResources, 1 );

        chGreaterEq( maxDescriptorSets, 1 );
        // maxTexelOffset, maxTexelGatherOffset can be 0
        chGreaterEq( maxFragmentOutputAttachments,       1 );
        chGreaterEq( maxFragmentCombinedOutputResources, 1 );
        // maxFragmentDualSrcAttachments can be 0
        chGreaterEq( maxPushConstantsSize, 16 );

        if ( fEqual( computeShader, EFeature::RequireTrue ))
        {
            chGreaterEq( maxComputeSharedMemorySize,     16 );
            chGreaterEq( maxComputeWorkGroupInvocations, 1 );
            chGreaterEq( maxComputeWorkGroupSizeX,       1 );
            chLessEq(    maxComputeWorkGroupSizeX,       maxComputeWorkGroupInvocations );
            chGreaterEq( maxComputeWorkGroupSizeY,       1 );
            chLessEq(    maxComputeWorkGroupSizeY,       maxComputeWorkGroupInvocations );
            chGreaterEq( maxComputeWorkGroupSizeZ,       1 );
            chLessEq(    maxComputeWorkGroupSizeZ,       maxComputeWorkGroupInvocations );

            chNotEqual2( storageImageFormats, PixelFormatSet_t{},  PixelFormatSet_t{}.insert( EPixelFormat::R32I ));
            //CHECK_ERR( storageImageAtomicFormats.Any() ); // can be empty
        }
        else
        {
            chEqual( maxComputeSharedMemorySize,     0 );
            chEqual( maxComputeWorkGroupInvocations, 0 );
            chEqual( maxComputeWorkGroupSizeX,       0 );
            chEqual( maxComputeWorkGroupSizeY,       0 );
            chEqual( maxComputeWorkGroupSizeZ,       0 );
        }

        if ( fEqual( taskShader, EFeature::RequireTrue ))
        {
            chGreaterEq( maxTaskWorkGroupSize,                  32          );
            chGreaterEq( maxTaskPayloadSize,                    (16<<10)    );
            chGreaterEq( maxTaskSharedMemorySize,               (16<<10)    );
            chGreaterEq( maxTaskPayloadAndSharedMemorySize,     (16<<10)    );
            chGreaterEq( maxPreferredTaskWorkGroupInvocations,  32          );
            chEqual(     meshShader,                            EFeature::RequireTrue );
            if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
            if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 300 ); }
        }
        else
        {
            chEqual( maxTaskWorkGroupSize,                  0 );
            chEqual( maxTaskPayloadSize,                    0 );
            chEqual( maxTaskSharedMemorySize,               0 );
            chEqual( maxTaskPayloadAndSharedMemorySize,     0 );
            chEqual( maxPreferredTaskWorkGroupInvocations,  0 );
        }

        if ( fEqual( meshShader, EFeature::RequireTrue ))
        {
            chGreaterEq( maxMeshWorkGroupSize,                  32          );
            chGreaterEq( maxMeshOutputVertices,                 128         );
            chGreaterEq( maxMeshOutputPrimitives,               128         );
            chGreaterEq( maxMeshOutputPerVertexGranularity,     32          );
            chGreaterEq( maxMeshOutputPerPrimitiveGranularity,  32          );
            chGreaterEq( maxMeshSharedMemorySize,               (16<<10)    );
            chGreaterEq( maxMeshPayloadAndSharedMemorySize,     (16<<10)    );
            chGreaterEq( maxMeshOutputMemorySize,               (16<<10)    );
            chGreaterEq( maxMeshPayloadAndOutputMemorySize,     (16<<10)    );
            chGreaterEq( maxPreferredMeshWorkGroupInvocations,  32          );
            // maxMeshMultiviewViewCount can be 0
            if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
            if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 300 ); }
        }
        else
        {
            chEqual( maxMeshWorkGroupSize,                  0 );
            chEqual( maxMeshOutputVertices,                 0 );
            chEqual( maxMeshOutputPrimitives,               0 );
            chEqual( maxMeshOutputPerVertexGranularity,     0 );
            chEqual( maxMeshOutputPerPrimitiveGranularity,  0 );
            chEqual( maxMeshSharedMemorySize,               0 );
            chEqual( maxMeshPayloadAndSharedMemorySize,     0 );
            chEqual( maxMeshOutputMemorySize,               0 );
            chEqual( maxMeshPayloadAndOutputMemorySize,     0 );
            chEqual( maxPreferredMeshWorkGroupInvocations,  0 );
            chEqual( maxMeshMultiviewViewCount,             0 );
        }

        chGreaterEq( maxImageArrayLayers,   1 );
        chGreaterEq( maxFramebufferLayers,  1 );

        //CHECK_ERR( surfaceFormats.Any() ); // can be empty
        //CHECK_ERR( attachmentBlendFormats.Any() ); // can be empty
        chNotEqual2( attachmentFormats, PixelFormatSet_t{},     PixelFormatSet_t{}.insert( EPixelFormat::RGBA8_UNorm ));
        chNotEqual2( vertexFormats,     VertexFormatSet_t{},    VertexFormatSet_t{}.insert( EVertexType::Float4 ));

        CHECK_ERR( (VertexTypeBits & vertexFormats) == vertexFormats );

        if ( fNotEq( vulkanMemoryModel, EFeature::RequireTrue ))
        {
            chNotEqual2( vulkanMemoryModelDeviceScope,                  EFeature::RequireTrue, neg_feat );
            chNotEqual2( vulkanMemoryModelAvailabilityVisibilityChains, EFeature::RequireTrue, neg_feat );
        }

        if ( fEqual( variableSampleLocations, EFeature::RequireTrue )) {
             chEqual( sampleLocations, EFeature::RequireTrue );
        }
        if ( fEqual( shaderSampleRateInterpolationFunctions, EFeature::RequireTrue )) {
             chEqual( sampleRateShading, EFeature::RequireTrue );
        }
        if ( maxShaderVersion.metal != 0 and maxShaderVersion.metal < 230 ) {
            chNotEqual( shaderInt64, EFeature::RequireTrue );
        }
        // not supported in Metal
        if ( maxShaderVersion.spirv == 0 and maxShaderVersion.metal != 0 )
        {
            chNotEqual( shaderFloat64,          EFeature::RequireTrue );
            chNotEqual( shaderSubgroupClock,    EFeature::RequireTrue );
            chNotEqual( shaderDeviceClock,      EFeature::RequireTrue );
            chNotEqual( rayTracingPipeline,     EFeature::RequireTrue );
            // TODO
        }

        chNotEqual2( queues.supported, EQueueMask::Unknown, EQueueMask::Graphics );
        chEqual2(    AnyBits( queues.required, ~queues.supported ),  false,  queues.required &= queues.supported );

        return true;

    #undef chEqual
    #undef chNotEqual
    #undef chLessEq
    #undef chGreaterEq
    }

    bool  FeatureSet::IsValid ()    C_NE___ { return const_cast<FeatureSet*>(this)->_Validate<false>(); }
    void  FeatureSet::Validate ()   __NE___ { _Validate<true>(); }

/*
=================================================
    IsSupported (RenderState)
=================================================
*/
    bool  FeatureSet::IsSupported (const RenderState &rs) C_NE___
    {
        if ( rs.multisample.alphaToOne )
            CHECK_ERR( alphaToOne != EFeature::RequireFalse );

        if ( rs.multisample.sampleShading )
            CHECK_ERR( sampleRateShading != EFeature::RequireFalse );

        if ( rs.rasterization.depthBiasClamp != 0.f )
            CHECK_ERR( depthBiasClamp != EFeature::RequireFalse );

        if ( rs.rasterization.depthClamp )
            CHECK_ERR( depthClamp != EFeature::RequireFalse );

        if ( rs.rasterization.polygonMode != EPolygonMode::Fill )
            CHECK_ERR( fillModeNonSolid != EFeature::RequireFalse );

        if ( rs.rasterization.polygonMode == EPolygonMode::Point )
            CHECK_ERR( pointPolygons != EFeature::RequireFalse );

        if ( rs.depth.bounds )
            CHECK_ERR( depthBounds != EFeature::RequireFalse );

        if ( rs.stencil.enabled )
        {
            if ( not (rs.stencil.front == rs.stencil.back) )
                CHECK_ERR( separateStencilMaskRef != EFeature::RequireFalse );
        }

        const auto  CheckBlend = [dual_src      = dualSrcBlend != EFeature::RequireFalse,
                                  const_alpha   = constantAlphaColorBlendFactors != EFeature::RequireFalse] (EBlendFactor factor) -> bool
        {{
            BEGIN_ENUM_CHECKS();
            switch ( factor )
            {
                case EBlendFactor::Zero :
                case EBlendFactor::One :
                case EBlendFactor::SrcColor :
                case EBlendFactor::OneMinusSrcColor :
                case EBlendFactor::DstColor :
                case EBlendFactor::OneMinusDstColor :
                case EBlendFactor::SrcAlpha :
                case EBlendFactor::OneMinusSrcAlpha :
                case EBlendFactor::DstAlpha :
                case EBlendFactor::OneMinusDstAlpha :
                case EBlendFactor::ConstColor :
                case EBlendFactor::OneMinusConstColor :
                case EBlendFactor::SrcAlphaSaturate :
                    return true;

                case EBlendFactor::ConstAlpha :
                case EBlendFactor::OneMinusConstAlpha :
                    return const_alpha;

                case EBlendFactor::Src1Color :
                case EBlendFactor::OneMinusSrc1Color :
                case EBlendFactor::Src1Alpha :
                case EBlendFactor::OneMinusSrc1Alpha :
                    return dual_src;

                case EBlendFactor::_Count :
                case EBlendFactor::Unknown :
                default_unlikely :
                    return false;
            }
            END_ENUM_CHECKS();
        }};

        for (auto& cb : rs.color.buffers)
        {
            if ( cb.blend )
            {
                CHECK_ERR( CheckBlend( cb.srcBlendFactor.color ));
                CHECK_ERR( CheckBlend( cb.srcBlendFactor.alpha ));
                CHECK_ERR( CheckBlend( cb.dstBlendFactor.color ));
                CHECK_ERR( CheckBlend( cb.dstBlendFactor.alpha ));

                if ( cb.srcBlendFactor.color != cb.srcBlendFactor.alpha )
                    CHECK_ERR( independentBlend != EFeature::RequireFalse );

                if ( cb.dstBlendFactor.color != cb.dstBlendFactor.alpha )
                    CHECK_ERR( independentBlend != EFeature::RequireFalse );
            }
        }

        if ( rs.color.logicOp != ELogicOp::None )
            CHECK_ERR( logicOp != EFeature::RequireFalse );

        if ( rs.inputAssembly.topology == EPrimitive::TriangleFan )
            CHECK_ERR( triangleFans != EFeature::RequireFalse );

        return true;
    }

/*
=================================================
    IsSupported (BufferDesc)
=================================================
*/
    bool  FeatureSet::IsSupported (const BufferDesc &desc) C_NE___
    {
        STATIC_ASSERT( uint(EBufferOpt::All) == 0x1F );

        bool    result = true;

        result &= (desc.queues == Default) or AllBits( queues.supported, desc.queues );

        for (auto opt : BitfieldIterate( desc.options ))
        {
            BEGIN_ENUM_CHECKS();
            switch ( opt )
            {
                case EBufferOpt::SparseResidency :          break;
                case EBufferOpt::SparseAliased :            break;
                case EBufferOpt::VertexPplnStore :          result &= (fragmentStoresAndAtomics         == EFeature::RequireTrue);  break;
                case EBufferOpt::FragmentPplnStore :        result &= (vertexPipelineStoresAndAtomics   == EFeature::RequireTrue);  break;
                case EBufferOpt::StorageTexelAtomic :       break;

                case EBufferOpt::_Last :
                case EBufferOpt::SparseResidencyAliased :
                case EBufferOpt::All :
                case EBufferOpt::Unknown :
                default_unlikely :                          DBG_WARNING( "unknown buffer option" ); break;
            }
            END_ENUM_CHECKS();
        }

        return result;
    }

/*
=================================================
    IsSupported (BufferDesc, BufferViewDesc)
=================================================
*/
    bool  FeatureSet::IsSupported (const BufferDesc &desc, const BufferViewDesc &view) C_NE___
    {
        STATIC_ASSERT( uint(EBufferUsage::All) == 0x1FFF );
        STATIC_ASSERT( uint(EBufferOpt::All) == 0x1F );

        bool    result = true;

        if ( AllBits( desc.usage, EBufferUsage::UniformTexel ))
            result &= uniformTexBufferFormats.contains( view.format );

        if ( AllBits( desc.usage, EBufferUsage::StorageTexel ))
            result &= storageTexBufferFormats.contains( view.format );

        if ( AllBits( desc.options, EBufferOpt::StorageTexelAtomic ))
            result &= storageTexBufferAtomicFormats.contains( view.format );

        return result;
    }

/*
=================================================
    IsSupported (ImageDesc)
=================================================
*/
    bool  FeatureSet::IsSupported (const ImageDesc &desc) C_NE___
    {
        STATIC_ASSERT( uint(EImageUsage::All) == 0xFF );
        STATIC_ASSERT( uint(EImageOpt::All) == 0x1FFFF );

        bool    result = true;

        if ( (desc.format >= EPixelFormat::ASTC_RGBA_4x4 and desc.format <= EPixelFormat::ASTC_RGBA_12x12) or
             (desc.format >= EPixelFormat::ASTC_sRGB8_A8_4x4 and desc.format <= EPixelFormat::ASTC_sRGB8_A8_12x12) )
            result &= (textureCompressionASTC_LDR == EFeature::RequireTrue);

        if ( desc.format >= EPixelFormat::ASTC_RGBA16F_4x4 and desc.format <= EPixelFormat::ASTC_RGBA16F_12x12 )
            result &= (textureCompressionASTC_HDR == EFeature::RequireTrue);

        if ( desc.format >= EPixelFormat::BC1_RGB8_UNorm and desc.format <= EPixelFormat::BC7_sRGB8_A8 )
            result &= (textureCompressionBC == EFeature::RequireTrue);

        if ( (desc.format >= EPixelFormat::ETC2_RGB8_UNorm and desc.format <= EPixelFormat::ETC2_sRGB8_A8) or
             (desc.format >= EPixelFormat::EAC_R11_SNorm and desc.format <= EPixelFormat::EAC_RG11_UNorm) )
            result &= (textureCompressionETC2 == EFeature::RequireTrue);

        result &= (desc.arrayLayers.Get() <= maxImageArrayLayers);

        if ( desc.arrayLayers.Get() > 1 and desc.samples.Get() > 1 )
            result &= (multisampleArrayImage == EFeature::RequireTrue);

        //if ( desc.arrayLayers > 6 and AllBits( options, EImageOpt::CubeCompatible ))
        //  result &= (imageCubeArray == EFeature::RequireTrue);

        result &= (desc.queues == Default) or AllBits( queues.supported, desc.queues );

        for (auto usage : BitfieldIterate( desc.usage ))
        {
            BEGIN_ENUM_CHECKS();
            switch ( usage )
            {
                case EImageUsage::TransferSrc :             break;
                case EImageUsage::TransferDst :             break;
                case EImageUsage::Sampled :                 break;
                case EImageUsage::Storage :                 result &= storageImageFormats.contains( desc.format );              break;
                case EImageUsage::ColorAttachment :
                case EImageUsage::DepthStencilAttachment :
                case EImageUsage::InputAttachment :         result &= attachmentFormats.contains( desc.format );                break;
                case EImageUsage::ShadingRate :             result &= (attachmentFragmentShadingRate == EFeature::RequireTrue); break;

                case EImageUsage::_Last :
                case EImageUsage::All :
                case EImageUsage::Transfer :
                case EImageUsage::RWAttachment :
                case EImageUsage::Unknown :
                default_unlikely :                          DBG_WARNING( "unknown image usage" ); break;
            }
            END_ENUM_CHECKS();
        }

        for (auto opt : BitfieldIterate( desc.options ))
        {
            BEGIN_ENUM_CHECKS();
            switch ( opt )
            {
                case EImageOpt::CubeCompatible :            break;
                case EImageOpt::MutableFormat :             break;  // TODO
                case EImageOpt::Array2DCompatible :         break;  // TODO
                case EImageOpt::BlockTexelViewCompatible :  break;  // TODO
                case EImageOpt::SparseResidency :           break;
                case EImageOpt::SparseAliased :             break;
                case EImageOpt::Alias :                     break;
                case EImageOpt::SampleLocationsCompatible : break;  // TODO
                case EImageOpt::StorageAtomic :             result &= storageImageAtomicFormats.contains( desc.format );            break;
                case EImageOpt::ColorAttachmentBlend :      result &= attachmentBlendFormats.contains( desc.format );               break;
                case EImageOpt::SampledLinear :             result &= linearSampledFormats.contains( desc.format );                 break;
                case EImageOpt::SampledMinMax :             break;  // TODO
                case EImageOpt::VertexPplnStore :           result &= (fragmentStoresAndAtomics         == EFeature::RequireTrue);  break;
                case EImageOpt::FragmentPplnStore :         result &= (vertexPipelineStoresAndAtomics   == EFeature::RequireTrue);  break;
                case EImageOpt::LossyRTCompression :        break;  // TODO
                case EImageOpt::BlitSrc :                   break;
                case EImageOpt::BlitDst :                   break;

                case EImageOpt::_Last :
                case EImageOpt::SparseResidencyAliased :
                case EImageOpt::All :
                case EImageOpt::Unknown :
                default_unlikely :                          DBG_WARNING( "unknown image option" ); break;
            }
            END_ENUM_CHECKS();
        }

        return result;
    }

/*
=================================================
    IsSupported (ImageDesc, ImageViewDesc)
=================================================
*/
    bool  FeatureSet::IsSupported (const ImageDesc &desc, const ImageViewDesc &view) C_NE___
    {
        bool    result = true;

        if ( view.viewType == EImage_CubeArray )
            result &= (imageCubeArray == EFeature::RequireTrue);

        if ( desc.format != view.format and view.extUsage != Default )
        {
            for (auto usage : BitfieldIterate( view.extUsage ))
            {
                BEGIN_ENUM_CHECKS();
                switch ( usage )
                {
                    case EImageUsage::TransferSrc :             break;
                    case EImageUsage::TransferDst :             break;
                    case EImageUsage::Sampled :                 break;
                    case EImageUsage::Storage :                 result &= storageImageFormats.contains( view.format );              break;
                    case EImageUsage::ColorAttachment :
                    case EImageUsage::DepthStencilAttachment :
                    case EImageUsage::InputAttachment :         result &= attachmentFormats.contains( view.format );                break;
                    case EImageUsage::ShadingRate :             result &= (attachmentFragmentShadingRate == EFeature::RequireTrue); break;

                    case EImageUsage::_Last :
                    case EImageUsage::All :
                    case EImageUsage::Transfer :
                    case EImageUsage::RWAttachment :
                    case EImageUsage::Unknown :
                    default_unlikely :                          DBG_WARNING( "unknown image usage" ); break;
                }
                END_ENUM_CHECKS();
            }
        }

        return result;
    }

/*
=================================================
    IsCompatible
=================================================
*/
    bool  FeatureSet::IsCompatible (const FeatureSet &rhs) C_NE___
    {
        return
            #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  FS_IsCompatible( _name_, rhs. _name_, AE_TOSTRING(_name_) ) &
            AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
            #undef AE_FEATURE_SET_VISIT
            true;
    }

    bool  FeatureSet::DbgIsCompatible (const FeatureSet &rhs) C_NE___
    {
        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  CHECK_ERR( FS_IsCompatible( _name_, rhs. _name_, AE_TOSTRING(_name_) ));
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT
        return true;
    }

/*
=================================================
    operator ==
=================================================
*/
    bool  FeatureSet::operator == (const FeatureSet &rhs) C_NE___
    {
        return
            #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  FS_Equal( _name_, rhs. _name_, AE_TOSTRING(_name_) ) &
            AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
            #undef AE_FEATURE_SET_VISIT
            true;
    }

/*
=================================================
    operator >=
=================================================
*/
    bool  FeatureSet::operator >= (const FeatureSet &rhs) C_NE___
    {
        return
            #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  FS_GreaterEqual( _name_, rhs. _name_, AE_TOSTRING(_name_) ) &
            AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
            #undef AE_FEATURE_SET_VISIT
            true;
    }

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  FeatureSet::CalcHash () C_NE___
    {
        HashVal res;
        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )      res << HashOf( _name_ );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT
        return res;
    }

/*
=================================================
    SetAll
=================================================
*/
    void  FeatureSet::SetAll (EFeature value) __NE___
    {
        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )      if constexpr( IsSameTypes< _type_, EFeature >)  _name_ = FS_Set<_type_>( value );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT
    }

/*
=================================================
    SetDefault
=================================================
*/
    void  FeatureSet::SetDefault () __NE___
    {
        maxShaderVersion.spirv  = MinSpirvVersion;
        maxShaderVersion.metal  = MinMetalVersion;

        maxViewports    = 1;

        maxUniformBufferSize                = 16 << 10;
        maxDescriptorSets                   = 1;
        maxFragmentOutputAttachments        = 1;
        maxFragmentCombinedOutputResources  = 1;

        maxImageArrayLayers = 1;

        framebufferColorSampleCounts = SampleCountBits( 1 );
        framebufferDepthSampleCounts = SampleCountBits( 1 );
        maxFramebufferLayers         = 1;
    }

/*
=================================================
    MergeMin
=================================================
*/
    void  FeatureSet::MergeMin (const FeatureSet &rhs) __NE___
    {
        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  _name_ = FS_MergeMin( _name_, rhs. _name_, AE_TOSTRING(_name_) );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT
    }

/*
=================================================
    MergeMax
=================================================
*/
    void  FeatureSet::MergeMax (const FeatureSet &rhs) __NE___
    {
        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  _name_ = FS_MergeMax( _name_, rhs. _name_, AE_TOSTRING(_name_) );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT
    }

/*
=================================================
    AddDevice
=================================================
*/
    void  FeatureSet::AddDevice (uint vendorId, uint deviceId, StringView name) __NE___
    {
        EVendorID           vendor  = GetVendorTypeByID( vendorId );
        EGraphicsDeviceID   device  = GetEGraphicsDeviceByID( deviceId );

        if ( device == Default )
            device = GetEGraphicsDeviceByName( name );

        if ( vendor == Default )
            vendor = GetVendorTypeByDevice( device );

        if ( vendor != Default )
            vendorIds.include.insert( vendor );

        if ( device != Default )
            devicesIds.include.insert( device );
    }

/*
=================================================
    GetHashOfFieldNames
=================================================
*/
namespace {
    ND_ static HashVal64  HashOfStr (const char* str, uint shift) { return HashVal64{ ulong(uint(CT_Hash( str, UMax, 0x7453 ))) << (shift & 32) }; }
}
    HashVal64  FeatureSet::GetHashOfFieldNames () __NE___
    {
        HashVal64   result;
        uint        counter = 0;

        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )                                                      \
            result += (HashOfStr( AE_TOSTRING( _type_ ), counter ) + HashOfStr( AE_TOSTRING( _name_ ), counter ));  \
            ++counter;                                                                                              \

        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT

        return result;
    }

/*
=================================================
    GetHashOfDependencies
=================================================
*/
    HashVal64  FeatureSet::GetHashOfDependencies () __NE___
    {
        HashVal64   result;
        uint        counter = 0;

        #define AE_GRAPHICS_DEVICE_VISIT( _name_ )      result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
        AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
        #undef AE_GRAPHICS_DEVICE_VISIT

        #define AE_PIXELFORMAT_VISIT( _name_ )          result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
        AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
        #undef AE_PIXELFORMAT_VISIT

        #define AE_VERTEXTYPE_VISIT( _name_, _value_ )  result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
        AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
        #undef AE_VERTEXTYPE_VISIT

        result += HashVal64{ sizeof(PerDescriptorSet) };
        result += HashVal64{ uint(ESubgroupOperation::_Count) };
        result += HashVal64{ uint(ESurfaceFormat::_Count) };
        result += HashVal64{ uint(EVendorID::_Count) };
        result += HashVal64{ uint(ESubgroupTypes::All) };
        result += HashVal64{ uint(EShaderStages::All) };

        result += HashVal64{ MaxSpirvVersion };
        result += HashVal64{ MaxMetalVersion };

        return result;
    }

/*
=================================================
    GetHashOfFS_Precalculated
=================================================
*/
    HashVal64  FeatureSet::GetHashOfFS_Precalculated () __NE___
    {
        return HashVal64{0x92bc3d49423bcd36ull};
    }

/*
=================================================
    accelerationStructure
=================================================
*/
    EFeature  FeatureSet::accelerationStructure () C_NE___
    {
        if_likely( rayTracingPipeline == EFeature::RequireTrue )
            return EFeature::RequireTrue;

        if_likely( rayQuery == EFeature::RequireTrue )
            return EFeature::RequireTrue;

        if ( rayTracingPipeline == EFeature::Ignore or rayQuery == EFeature::Ignore )
            return EFeature::Ignore;

        return EFeature::RequireFalse;
    }


} // AE::Graphics
