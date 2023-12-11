// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"
#include "base/Utils/Version.h"

#include "graphics/Vulkan/VEnumCast.h"
#include "graphics/Private/EnumToString.h"

#include "FeatureSetUtils.h"

#include "vulkan/vulkan_core.h"

namespace AE::Graphics
{
namespace
{

/*
=================================================
    FS_ParseJSON_N
=================================================
*/
    ND_ static Array<StringView>  FS_ParseJSON_N (StringView json, StringView name)
    {
        usize   pos = json.find( name );
        if ( name.empty() or pos == String::npos )
        {
            AE_LOGI( "Not found: "s << name );
            return {};
        }

        usize   begin_pos = pos;

        StringView  line;
        Parser::ReadCurrLine( json, INOUT pos, OUT line );

        Array<StringView>   tokens;
        Parser::DivideString_CPP( line, OUT tokens );

        // ", <name>, ", :, <value>
        CHECK( tokens.size() >= 5 );
        CHECK( tokens[0] == "\"" );
        CHECK( tokens[2] == "\"" );
        CHECK( tokens[3] == ":" );

        if ( tokens[1] == "name" )
        {
            // pattern:
            //  "name": "<name>",
            //  "value": <value>
            begin_pos = pos;
            Parser::ReadCurrLine( json, INOUT pos, OUT line );
            Parser::DivideString_CPP( line, OUT tokens );

            CHECK( tokens.size() > 3 );
            CHECK( tokens[0] == "\"" );
            CHECK( tokens[1] == "value" or tokens[1] == "supported" );
            CHECK( tokens[2] == "\"" );
            CHECK( tokens[3] == ":" );

            if ( tokens[4] == "[" )
            {
                usize   pos1 = json.find( '[', begin_pos );
                usize   pos2 = json.find( ']', pos1 );
                CHECK_ERR( pos2 != String::npos )
                CHECK_ERR( pos1 < pos2 );

                Parser::DivideString_CPP( json.substr( pos1, pos2 - pos1 + 1 ), OUT tokens );

                CHECK( tokens.size() >= 2 );
                CHECK( tokens.front() == "[" );
                CHECK( tokens.back() == "]" );
            }
            else
            {
                tokens.erase( tokens.begin(), tokens.begin()+4 );
            }
        }
        else
        if ( tokens[4] == "[" )
        {
            usize   pos1 = json.find( '[', begin_pos );
            usize   pos2 = json.find( ']', pos1 );
            CHECK_ERR( pos2 != String::npos )
            CHECK_ERR( pos1 < pos2 );

            Parser::DivideString_CPP( json.substr( pos1, pos2 - pos1 + 1 ), OUT tokens );

            CHECK( tokens.size() >= 2 );
            CHECK( tokens.front() == "[" );
            CHECK( tokens.back() == "]" );
        }
        else
        {
            if ( name.front() == '"' )  name = name.substr( 1 );
            if ( name.back()  == '"' )  name = name.substr( 0, name.size()-1 );
            CHECK( tokens[1] == name );
            tokens.erase( tokens.begin(), tokens.begin() + 4 );
        }
        return tokens;
    }

/*
=================================================
    GetToken
=================================================
*/
    ND_ static StringView  GetToken (ArrayView<StringView> tokens)
    {
        // TODO: -value "-value"
        // "<value>"
        if ( tokens.size() == 4 )
        {
            CHECK( tokens[0] == "\"" );
            CHECK( tokens[2] == "\"" );
            CHECK( tokens[3] == "," );
            return tokens[1];
        }
        if ( tokens.size() == 3 )
        {
            CHECK( tokens[0] == "\"" );
            CHECK( tokens[2] == "\"" );
            return tokens[1];
        }
        CHECK( tokens.size() == 1 or (tokens.size() == 2 and tokens[1] == ",") );
        return tokens[0];
    }

    ND_ static StringView  GetToken1Of3 (ArrayView<StringView> tokens)
    {
        CHECK( tokens.size() == 7 );
        CHECK( tokens[0] == "[" );
        CHECK( tokens[2] == "," );
        CHECK( tokens[4] == "," );
        CHECK( tokens[6] == "]" );
        return tokens[1];
    }

    ND_ static int  GetUIntToken (ArrayView<StringView> tokens)
    {
        StringView  value_str = GetToken( tokens );
        int         val;
        Unused( StringToValue( value_str, OUT val ));
        return val;
    }

/*
=================================================
    FS_ParseJSON_1
=================================================
*/
    ND_ static StringView  FS_ParseJSON_1 (StringView json, StringView name)
    {
        Array<StringView>   tokens = FS_ParseJSON_N( json, name );
        if ( tokens.empty() )
            return {};

        if ( name == "\"maxTaskWorkGroupSize\"" or
             name == "\"maxMeshWorkGroupSize\"" )
            return GetToken1Of3( tokens );

        return GetToken( tokens );
    }

/*
=================================================
    FS_ParseJSON_Str
=================================================
*/
    ND_ static StringView  FS_ParseJSON_Str (StringView json, StringView name)
    {
        usize   pos = json.find( name );
        if ( name.empty() or pos == String::npos )
        {
            AE_LOGI( "Not found: "s << name );
            return {};
        }

        usize   pos1 = json.find( ": \"", pos + name.size() );
        CHECK_ERR( pos1 != String::npos );
        pos1 += 3;

        usize   pos2 = json.find( "\",", pos1 );
        CHECK_ERR( pos2 != String::npos );
        CHECK_ERR( pos1 < pos2 );

        return json.substr( pos1, pos2 - pos1 );
    }

/*
=================================================
    RemoveSymbolTokens
=================================================
*/
    static void  RemoveSymbolTokens (INOUT Array<StringView> &tokens)
    {
        for (auto it = tokens.begin(); it != tokens.end();)
        {
            StringView  token = *it;

            if ( token.size() == 1 ) {
                if ( AnyEqual( token[0], ':', ',', '"', '{', '}', '[', ']' )) {
                    it = tokens.erase( it );
                    continue;
                }
            }
            ++it;
        }
    }

/*
=================================================
    FS_ParseJSON (EFeature)
=================================================
*/
    ND_ static EFeature  FS_ParseJSON (EFeature prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        return (value_str == "1" or value_str == "true") ? EFeature::RequireTrue : EFeature::Ignore;
    }

/*
=================================================
    FS_ParseJSON (uint)
=================================================
*/
    ND_ static uint  FS_ParseJSON (uint prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        uint    val;
        Unused( StringToValue( value_str, OUT val ));
        return val;
    }

    ND_ static ubyte  FS_ParseJSON (ubyte prev, StringView json, StringView name)
    {
        return CheckCast<ubyte>( FS_ParseJSON( uint(prev), json, name ));
    }

    ND_ static ushort  FS_ParseJSON (ushort prev, StringView json, StringView name)
    {
        return CheckCast<ushort>( FS_ParseJSON( uint(prev), json, name ));
    }


/*
=================================================
    FS_ParseJSON (ulong)
=================================================
*/
    ND_ static ulong  FS_ParseJSON (ulong prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        ulong   val;
        Unused( StringToValue( value_str, OUT val ));
        return val;
    }

/*
=================================================
    FS_ParseJSON (Bytes)
=================================================
*/
    ND_ static Bytes  FS_ParseJSON (Bytes prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        ulong   val;
        Unused( StringToValue( value_str, OUT val ));
        return Bytes{val};
    }

/*
=================================================
    FS_ParseJSON (int)
=================================================
*/
    ND_ static int  FS_ParseJSON (int prev, StringView json, StringView name)
    {
        Array<StringView>   tokens = FS_ParseJSON_N( json, name );
        if ( tokens.empty() )
            return prev;

        StringView  value_str;
        int         sign    = 0;

        if ( tokens.size() > 2 and tokens[0] == "\"" )
        {
            CHECK( tokens[2] == "\"" or (tokens.size() > 3 and tokens[3] == "\"") );

            if ( tokens[1] == "-" ) { value_str = tokens[2];  sign = -1; }  else
            if ( tokens[1] == "+" ) { value_str = tokens[2];             }  else
                                    { value_str = tokens[1];             }
        }
        else
        if ( tokens.size() > 1 )
        {
            if ( tokens[0] == "-" ) { value_str = tokens[1];  sign = -1; }  else
            if ( tokens[0] == "+" ) { value_str = tokens[1];             }  else
                                    { value_str = tokens[0];             }
        }

        int     val;
        Unused( StringToValue( value_str, OUT val ));
        return val * sign;
    }

/*
=================================================
    FS_ParseJSON (float)
=================================================
*/
    ND_ static float  FS_ParseJSON (float prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        float   val;
        Unused( StringToValue( value_str, OUT val ));
        return val;
    }

/*
=================================================
    FS_ParseJSON (SubgroupOperationBits)
=================================================
*/
    ND_ static FeatureSet::SubgroupOperationBits  FS_ParseJSON (FeatureSet::SubgroupOperationBits prev, StringView json, StringView name)
    {
        Array<StringView>   tokens = FS_ParseJSON_N( json, name );
        if ( tokens.empty() )
            return prev;

        CHECK( name == "\"subgroupSupportedOperations\"" );

        if ( tokens.back() == "]" )
        {
            const auto  Contains = [&tokens, &prev] (StringView bit, ESubgroupOperation first, ESubgroupOperation last)
            {{
                if ( std::find( tokens.begin(), tokens.end(), bit ) != tokens.end() )
                    prev.InsertRange( first, last );
            }};

            Contains( "VK_SUBGROUP_FEATURE_BASIC_BIT",              ESubgroupOperation::_Basic_Begin,           ESubgroupOperation::_Basic_End           );
            Contains( "VK_SUBGROUP_FEATURE_VOTE_BIT",               ESubgroupOperation::_Vote_Begin,            ESubgroupOperation::_Vote_End            );
            Contains( "VK_SUBGROUP_FEATURE_ARITHMETIC_BIT",         ESubgroupOperation::_Arithmetic_Begin,      ESubgroupOperation::_Arithmetic_End      );
            Contains( "VK_SUBGROUP_FEATURE_BALLOT_BIT",             ESubgroupOperation::_Ballot_Begin,          ESubgroupOperation::_Ballot_End          );
            Contains( "VK_SUBGROUP_FEATURE_SHUFFLE_BIT",            ESubgroupOperation::_Shuffle_Begin,         ESubgroupOperation::_Shuffle_End         );
            Contains( "VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT",   ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );
            Contains( "VK_SUBGROUP_FEATURE_CLUSTERED_BIT",          ESubgroupOperation::_Clustered_Begin,       ESubgroupOperation::_Clustered_End       );
            Contains( "VK_SUBGROUP_FEATURE_QUAD_BIT",               ESubgroupOperation::_Quad_Begin,            ESubgroupOperation::_Quad_End            );
        }
        else
        {
            const auto  Contains = [available = GetUIntToken(tokens), &prev] (VkSubgroupFeatureFlagBits bit, ESubgroupOperation first, ESubgroupOperation last)
            {{
                if ( AllBits( available, bit ))
                    prev.InsertRange( first, last );
            }};

            Contains( VK_SUBGROUP_FEATURE_BASIC_BIT,            ESubgroupOperation::_Basic_Begin,           ESubgroupOperation::_Basic_End           );
            Contains( VK_SUBGROUP_FEATURE_VOTE_BIT,             ESubgroupOperation::_Vote_Begin,            ESubgroupOperation::_Vote_End            );
            Contains( VK_SUBGROUP_FEATURE_ARITHMETIC_BIT,       ESubgroupOperation::_Arithmetic_Begin,      ESubgroupOperation::_Arithmetic_End      );
            Contains( VK_SUBGROUP_FEATURE_BALLOT_BIT,           ESubgroupOperation::_Ballot_Begin,          ESubgroupOperation::_Ballot_End          );
            Contains( VK_SUBGROUP_FEATURE_SHUFFLE_BIT,          ESubgroupOperation::_Shuffle_Begin,         ESubgroupOperation::_Shuffle_End         );
            Contains( VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT, ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );
            Contains( VK_SUBGROUP_FEATURE_CLUSTERED_BIT,        ESubgroupOperation::_Clustered_Begin,       ESubgroupOperation::_Clustered_End       );
            Contains( VK_SUBGROUP_FEATURE_QUAD_BIT,             ESubgroupOperation::_Quad_Begin,            ESubgroupOperation::_Quad_End            );
        }

        CHECK( prev.Any() );
        return prev;
    }

/*
=================================================
    FS_ParseJSON (PerDescriptorSet)
=================================================
*/
    ND_ static FeatureSet::PerDescriptorSet  FS_ParseJSON (FeatureSet::PerDescriptorSet prev, StringView json, StringView name)
    {
        if ( name == "\"perDescrSet\"" )
        {
            prev.maxInputAttachments= FS_ParseJSON( 0u,     json, "maxDescriptorSetInputAttachments" );
            prev.maxSampledImages   = FS_ParseJSON( 0u,     json, "maxDescriptorSetSampledImages" );
            prev.maxSamplers        = FS_ParseJSON( 0u,     json, "maxDescriptorSetSamplers" );
            prev.maxStorageBuffers  = FS_ParseJSON( 0u,     json, "maxDescriptorSetStorageBuffers" );
            prev.maxStorageImages   = FS_ParseJSON( 0u,     json, "maxDescriptorSetStorageImages" );
            prev.maxUniformBuffers  = FS_ParseJSON( 0u,     json, "maxDescriptorSetUniformBuffers" );
            prev.maxAccelStructures = FS_ParseJSON( 0u,     json, "maxDescriptorSetAccelerationStructures" );
            prev.maxTotalResources  = FS_ParseJSON( 512u,   json, "maxPerSetDescriptors" );
        }
        else
        if ( name == "\"perStage\"" )
        {
            prev.maxInputAttachments= FS_ParseJSON( 0u, json, "maxPerStageDescriptorInputAttachments" );
            prev.maxSampledImages   = FS_ParseJSON( 0u, json, "maxPerStageDescriptorSampledImages" );
            prev.maxSamplers        = FS_ParseJSON( 0u, json, "maxPerStageDescriptorSamplers" );
            prev.maxStorageBuffers  = FS_ParseJSON( 0u, json, "maxPerStageDescriptorStorageBuffers" );
            prev.maxStorageImages   = FS_ParseJSON( 0u, json, "maxPerStageDescriptorStorageImages" );
            prev.maxUniformBuffers  = FS_ParseJSON( 0u, json, "maxPerStageDescriptorUniformBuffers" );
            prev.maxAccelStructures = FS_ParseJSON( 0u, json, "maxPerStageDescriptorAccelerationStructures" );
            prev.maxTotalResources  = FS_ParseJSON( 0u, json, "maxPerStageResources" );
        }
        else
        {
            CHECK(false);
        }

        return prev;
    }

/*
=================================================
    FS_ParseJSON (ESubgroupTypes)
=================================================
*/
    ND_ static ESubgroupTypes  FS_ParseJSON (ESubgroupTypes prev, StringView json, StringView name)
    {
        StringView  value_str = FS_ParseJSON_1( json, name );
        if ( value_str.empty() )
            return prev;

        CHECK( name == "\"shaderSubgroupExtendedTypes\"" );

        prev |= ESubgroupTypes::Float32 | ESubgroupTypes::Int32;

        if ( value_str == "1" or value_str == "true" )
            prev |= ESubgroupTypes::Int8 | ESubgroupTypes::Int16 | ESubgroupTypes::Int64 | ESubgroupTypes::Float16;

        return prev;
    }

/*
=================================================
    FS_ParseJSON (EShaderStages)
=================================================
*/
    ND_ static EShaderStages  FS_ParseJSON (EShaderStages prev, StringView json, StringView name)
    {
        Array<StringView>   tokens = FS_ParseJSON_N( json, name );
        if ( tokens.empty() )
            return prev;

        if ( name == "\"subgroupSupportedStages\""  or
             name == "\"requiredSubgroupSizeStages\"" )
        {
            if ( tokens.back() == "]" )
            {
                const auto  Contains = [&tokens, &prev] (StringView bit, EShaderStages stage)
                {{
                    if ( std::find( tokens.begin(), tokens.end(), bit ) != tokens.end() )
                        prev |= stage;
                }};
                Contains( "VK_SHADER_STAGE_VERTEX_BIT",                     EShaderStages::Vertex );
                Contains( "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT",       EShaderStages::TessControl );
                Contains( "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT",    EShaderStages::TessEvaluation );
                Contains( "VK_SHADER_STAGE_GEOMETRY_BIT",                   EShaderStages::Geometry );
                Contains( "VK_SHADER_STAGE_FRAGMENT_BIT",                   EShaderStages::Fragment );
                Contains( "VK_SHADER_STAGE_COMPUTE_BIT",                    EShaderStages::Compute );
                Contains( "VK_SHADER_STAGE_ALL_GRAPHICS",                   EShaderStages::AllGraphics );
                Contains( "VK_SHADER_STAGE_ALL",                            EShaderStages::All );
                Contains( "VK_SHADER_STAGE_RAYGEN_BIT_KHR",                 EShaderStages::RayGen );
                Contains( "VK_SHADER_STAGE_ANY_HIT_BIT_KHR",                EShaderStages::RayAnyHit );
                Contains( "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR",            EShaderStages::RayClosestHit );
                Contains( "VK_SHADER_STAGE_MISS_BIT_KHR",                   EShaderStages::RayMiss );
                Contains( "VK_SHADER_STAGE_INTERSECTION_BIT_KHR",           EShaderStages::RayIntersection );
                Contains( "VK_SHADER_STAGE_CALLABLE_BIT_KHR",               EShaderStages::RayCallable );
            //  Contains( "VK_SHADER_STAGE_TASK_BIT_NV",                    EShaderStages::MeshTask );
            //  Contains( "VK_SHADER_STAGE_MESH_BIT_NV",                    EShaderStages::Mesh );
                Contains( "VK_SHADER_STAGE_TASK_BIT_EXT",                   EShaderStages::MeshTask );
                Contains( "VK_SHADER_STAGE_MESH_BIT_EXT",                   EShaderStages::Mesh );
                Contains( "VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI",     EShaderStages::Tile );
                CHECK( tokens.size() == 2 or prev != Default );
            }
            else
            {
                const auto  Contains = [available = GetUIntToken(tokens), &prev] (VkShaderStageFlagBits bit, EShaderStages stage)
                {{
                    if ( AllBits( available, bit ))
                        prev |= stage;
                }};
                Contains( VK_SHADER_STAGE_VERTEX_BIT,                   EShaderStages::Vertex );
                Contains( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,     EShaderStages::TessControl );
                Contains( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,  EShaderStages::TessEvaluation );
                Contains( VK_SHADER_STAGE_GEOMETRY_BIT,                 EShaderStages::Geometry );
                Contains( VK_SHADER_STAGE_FRAGMENT_BIT,                 EShaderStages::Fragment );
                Contains( VK_SHADER_STAGE_COMPUTE_BIT,                  EShaderStages::Compute );
                Contains( VK_SHADER_STAGE_ALL_GRAPHICS,                 EShaderStages::AllGraphics );
                Contains( VK_SHADER_STAGE_ALL,                          EShaderStages::All );
                Contains( VK_SHADER_STAGE_RAYGEN_BIT_KHR,               EShaderStages::RayGen );
                Contains( VK_SHADER_STAGE_ANY_HIT_BIT_KHR,              EShaderStages::RayAnyHit );
                Contains( VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,          EShaderStages::RayClosestHit );
                Contains( VK_SHADER_STAGE_MISS_BIT_KHR,                 EShaderStages::RayMiss );
                Contains( VK_SHADER_STAGE_INTERSECTION_BIT_KHR,         EShaderStages::RayIntersection );
                Contains( VK_SHADER_STAGE_CALLABLE_BIT_KHR,             EShaderStages::RayCallable );
                Contains( VK_SHADER_STAGE_TASK_BIT_EXT,                 EShaderStages::MeshTask );
                Contains( VK_SHADER_STAGE_MESH_BIT_EXT,                 EShaderStages::Mesh );
                Contains( VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,   EShaderStages::Tile );
            }
        }
        else
        if ( name == "\"subgroupQuadOperationsInAllStages\"" )
        {
            CHECK( tokens.size() >= 1 );
            if ( tokens[0] == "1" or tokens[0] == "true" )
                prev |= EShaderStages::All;
            else
                prev = EShaderStages::Fragment | EShaderStages::Compute;
        }
        else
            CHECK(false);

        return prev;
    }

/*
=================================================
    FS_ParseJSON (SampleCountBits)
=================================================
*/
    ND_ static FeatureSet::SampleCountBits  FS_ParseJSON (FeatureSet::SampleCountBits prev, StringView json, StringView name)
    {
        Array<StringView>   tokens = FS_ParseJSON_N( json, name );
        if ( tokens.empty() )
            return prev;

        if ( tokens.front() == "[" )
        {
            const auto  Contains = [&tokens, &prev] (StringView bit, uint index)
            {{
                if ( std::find( tokens.begin(), tokens.end(), bit ) != tokens.end() )
                    prev = FeatureSet::SampleCountBits( uint(prev) | (1u << index) );
            }};
            Contains( "VK_SAMPLE_COUNT_1_BIT",  0 );
            Contains( "VK_SAMPLE_COUNT_2_BIT",  1 );
            Contains( "VK_SAMPLE_COUNT_4_BIT",  2 );
            Contains( "VK_SAMPLE_COUNT_8_BIT",  3 );
            Contains( "VK_SAMPLE_COUNT_16_BIT", 4 );
            Contains( "VK_SAMPLE_COUNT_32_BIT", 5 );
            Contains( "VK_SAMPLE_COUNT_64_BIT", 6 );
        }
        else
        {
            const auto  Contains = [available = GetUIntToken(tokens), &prev] (VkSampleCountFlagBits bit, uint index)
            {{
                if ( AllBits( available, bit ))
                    prev = FeatureSet::SampleCountBits( uint(prev) | (1u << index) );
            }};
            Contains( VK_SAMPLE_COUNT_1_BIT,    0 );
            Contains( VK_SAMPLE_COUNT_2_BIT,    1 );
            Contains( VK_SAMPLE_COUNT_4_BIT,    2 );
            Contains( VK_SAMPLE_COUNT_8_BIT,    3 );
            Contains( VK_SAMPLE_COUNT_16_BIT,   4 );
            Contains( VK_SAMPLE_COUNT_32_BIT,   5 );
            Contains( VK_SAMPLE_COUNT_64_BIT,   6 );
        }

        CHECK( prev != Default );
        return prev;
    }

    ND_ static EnumBitSet<EPixelFormat>  FS_ParseJSON (const EnumBitSet<EPixelFormat> &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static EnumBitSet<EVertexType>  FS_ParseJSON (const EnumBitSet<EVertexType> &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static FeatureSet::ShaderVersion  FS_ParseJSON (const FeatureSet::ShaderVersion &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static FeatureSet::Queues  FS_ParseJSON (const FeatureSet::Queues &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static FeatureSet::ShadingRateSet_t  FS_ParseJSON (const FeatureSet::ShadingRateSet_t &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static FeatureSet::VRSTexelSize  FS_ParseJSON (const FeatureSet::VRSTexelSize &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    template <typename E>
    ND_ static FeatureSet::IncludeExcludeBits<E>  FS_ParseJSON (const FeatureSet::IncludeExcludeBits<E> &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

    ND_ static FeatureSet::SurfaceFormatSet_t  FS_ParseJSON (const FeatureSet::SurfaceFormatSet_t &prev, StringView json, StringView name)
    {
        Unused( json, name );
        return prev;
    }

/*
=================================================
    FS_ParseJSON_Extent2D
=================================================
*/
    ND_ static bool  FS_ParseJSON_Extent2D (StringView name, StringView json, OUT VkExtent2D &result)
    {
        result.width  = 0;
        result.height = 0;

        String  name2 = '"' + String{name} + '"';
        usize   pos = json.find( name2 );
        if ( pos == StringView::npos )
            return true;

        pos += name2.size();
        const usize end = json.find( '}', pos );

        Array<StringView>   tokens;
        Parser::DivideString_CPP( json.substr( pos, end - pos ), OUT tokens );
        RemoveSymbolTokens( INOUT tokens );

        if ( tokens.size() == 3 )
        {
            CHECK( tokens[0] == "value" );
            Unused( StringToValue( tokens[1], OUT result.width ));
            Unused( StringToValue( tokens[2], OUT result.height ));

            CHECK_ERR( result.width > 0 and result.height > 0 );
            return true;
        }

        if ( tokens.size() == 4 )
        {
            CHECK( tokens[0] == "width" );
            CHECK( tokens[2] == "height" );

            Unused( StringToValue( tokens[1], OUT result.width ));
            Unused( StringToValue( tokens[3], OUT result.height ));

            CHECK_ERR( result.width > 0 and result.height > 0 );
            return true;
        }

        RETURN_ERR( "failed to parse" );
    }

/*
=================================================
    FS_ParseJSON_ParseFeatures
=================================================
*/
    static void  FS_ParseJSON_ParseFeatures (EPixelFormat fmt, StringView json, INOUT FeatureSet &outFeatureSet)
    {
        if ( HasSubString( json, "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT" ))
        {
            outFeatureSet.storageImageFormats.insert( fmt );

            if ( HasSubString( json, "VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT" ))
                outFeatureSet.storageImageAtomicFormats.insert( fmt );
        }

        if ( HasSubString( json, "VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT" ))
        {
            outFeatureSet.storageTexBufferFormats.insert( fmt );

            if ( HasSubString( json, "VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT" ))
                outFeatureSet.storageTexBufferAtomicFormats.insert( fmt );
        }

        if ( HasSubString( json, "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT" ))
            outFeatureSet.attachmentFormats.insert( fmt );

        if ( HasSubString( json, "VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT" ))
            outFeatureSet.attachmentFormats.insert( fmt );

        if ( HasSubString( json, "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT" ))
            outFeatureSet.attachmentBlendFormats.insert( fmt );

        if ( HasSubString( json, "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT" ))
            outFeatureSet.linearSampledFormats.insert( fmt );

        if ( HasSubString( json, "VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT" ))
            outFeatureSet.uniformTexBufferFormats.insert( fmt );
    }

    static void  FS_ParseJSON_ParseFeatures (EVertexType type, StringView json, INOUT FeatureSet &outFeatureSet)
    {
        if ( HasSubString( json, "VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT" ))
            outFeatureSet.vertexFormats.insert( type );

        if ( HasSubString( json, "VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR" ))
            outFeatureSet.accelStructVertexFormats.insert( type );
    }

/*
=================================================
    FS_ParseJSON_ParseFeatures2
=================================================
*/
    static void  FS_ParseJSON_ParseFeatures2 (EPixelFormat fmt, uint feats, INOUT FeatureSet &outFeatureSet)
    {
        if ( AllBits( feats, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT ))
        {
            outFeatureSet.storageImageFormats.insert( fmt );

            if ( AllBits( feats, VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT ))
                outFeatureSet.storageImageAtomicFormats.insert( fmt );
        }

        if ( AllBits( feats, VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT ))
        {
            outFeatureSet.storageTexBufferFormats.insert( fmt );

            if ( AllBits( feats, VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT ))
                outFeatureSet.storageTexBufferAtomicFormats.insert( fmt );
        }

        if ( AllBits( feats, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ))
            outFeatureSet.attachmentFormats.insert( fmt );

        if ( AllBits( feats, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ))
            outFeatureSet.attachmentFormats.insert( fmt );

        if ( AllBits( feats, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT ))
            outFeatureSet.attachmentBlendFormats.insert( fmt );

        if ( AllBits( feats, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ))
            outFeatureSet.linearSampledFormats.insert( fmt );

        if ( AllBits( feats, VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT ))
            outFeatureSet.uniformTexBufferFormats.insert( fmt );
    }

    static void  FS_ParseJSON_ParseFeatures2 (EVertexType type, uint feats, INOUT FeatureSet &outFeatureSet)
    {
        if ( AllBits( feats, VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT ))
            outFeatureSet.vertexFormats.insert( type );

        if ( AllBits( feats, VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR ))
            outFeatureSet.accelStructVertexFormats.insert( type );
    }

/*
=================================================
    FS_ParseJSON_ImageFormats
=================================================
*/
    static bool  FS_ParseJSON_ImageFormats (StringView json, INOUT FeatureSet &outFeatureSet)
    {
        CHECK( json[0] == '\"' );

        usize   pos = json.find( '\"', 1 );
        CHECK_ERR( pos != StringView::npos );

        StringView  fmt_name = json.substr( 1, pos - 1 );

        const HashMap< StringView, EPixelFormat >   name_to_format =
        {
            #define AE_PRIVATE_VKPIXELFORMATS_VISIT( _engineFmt_, _vkFormat_ )  {AE_TOSTRING(_vkFormat_), EPixelFormat::_engineFmt_},
            AE_PRIVATE_VKPIXELFORMATS( AE_PRIVATE_VKPIXELFORMATS_VISIT )
            #undef AE_PRIVATE_VKPIXELFORMATS_VISIT
        };

        auto    it = name_to_format.find( fmt_name );
        if ( it == name_to_format.end() )
        {
            //AE_LOGI( "Skip image format: "s << fmt_name );
            return true;
        }

        {
            const StringView    prefix = "\"optimalTilingFeatures\": [";

            pos = json.find( prefix );
            CHECK_ERR( pos != StringView::npos );

            usize   pos2 = json.find( ']', pos );
            CHECK_ERR( pos2 != StringView::npos );

            FS_ParseJSON_ParseFeatures( it->second, json.substr( pos, pos2 - pos ), outFeatureSet );
        }
        {
            const StringView    prefix = "\"bufferFeatures\": [";

            pos = json.find( prefix );
            CHECK_ERR( pos != StringView::npos );

            usize   pos2 = json.find( ']', pos );
            CHECK_ERR( pos2 != StringView::npos );

            FS_ParseJSON_ParseFeatures( it->second, json.substr( pos, pos2 - pos ), outFeatureSet );
        }
        return true;
    }

/*
=================================================
    FS_ParseJSON_VertexFormats
=================================================
*/
    static bool  FS_ParseJSON_VertexFormats (StringView json, INOUT FeatureSet &outFeatureSet)
    {
        CHECK( json[0] == '\"' );

        usize   pos = json.find( '\"', 1 );
        CHECK_ERR( pos != StringView::npos );

        StringView  fmt_name = json.substr( 1, pos - 1 );

        const HashMap< StringView, EVertexType >    name_to_format =
        {
            #define AE_PRIVATE_VKPIXELFORMATS_VISIT( _engineFmt_, _vkFormat_ )  {AE_TOSTRING(_vkFormat_), EVertexType::_engineFmt_},
            AE_PRIVATE_VKVERTEXFORMATS( AE_PRIVATE_VKPIXELFORMATS_VISIT )
            #undef AE_PRIVATE_VKPIXELFORMATS_VISIT
        };

        auto    it = name_to_format.find( fmt_name );
        if ( it == name_to_format.end() )
        {
            //AE_LOGI( "Skip vertex format: "s << fmt_name );
            return true;
        }

        const StringView    prefix = "\"bufferFeatures\": [";

        pos = json.find( prefix );
        CHECK_ERR( pos != StringView::npos );

        usize   pos2 = json.find( ']', pos );
        CHECK_ERR( pos2 != StringView::npos );

        FS_ParseJSON_ParseFeatures( it->second, json.substr( pos, pos2 - pos ), outFeatureSet );
        return true;
    }

/*
=================================================
    FS_ParseJSON_Formats_1
=================================================
*/
    static bool  FS_ParseJSON_Formats_1 (StringView json, INOUT FeatureSet &outFeatureSet)
    {
        const StringView    prefix      = "\"VK_FORMAT_";
        usize               fmt_count   = 0;

        for (usize pos = 0;;)
        {
            pos = json.find( prefix, pos );
            if ( pos == String::npos )
                break;

            const usize begin_pos   = pos;
            int         count       = 0;

            for (; pos < json.size(); ++pos)
            {
                const char  c = json[pos];
                if ( c == '{' )
                    ++count;
                else
                if ( c == '}' )
                {
                    if ( --count <= 0 )
                        break;
                }
            }
            CHECK_ERR( count == 0 );

            StringView  view = StringView{json}.substr( begin_pos, pos - begin_pos );

            FS_ParseJSON_ImageFormats( view, INOUT outFeatureSet );
            FS_ParseJSON_VertexFormats( view, INOUT outFeatureSet );
            ++fmt_count;
        }
        return fmt_count > 0;
    }

/*
=================================================
    FS_ParseJSON_Formats_2
=================================================
*/
    static bool  FS_ParseJSON_Formats_2 (StringView json, INOUT FeatureSet &outFeatureSet)
    {
        const usize formats_begin   = json.find( "    \"formats\": [" );
        CHECK_ERR(  formats_begin != StringView::npos );

        usize   formats_end = formats_begin;

        for (;; ++formats_end)
        {
            formats_end = json.find( "    ],", formats_end );
            CHECK_ERR( formats_end != StringView::npos );

            const char  c = json[formats_end-1];
            if ( c == '\n' or c != ' ' )
                break;
        }

        StringView  fmt_str = json.substr( formats_begin, formats_end - formats_begin );

        const auto  ParseFormatFlags1 = [fmt_str, &outFeatureSet] (EPixelFormat engFmt, uint vkFmt)
        {{
            String  name        = "            "s + ToString(vkFmt) + ",";
            usize   begin_pos   = fmt_str.find( name );
            if ( begin_pos == StringView::npos )
                return;

            usize       end_pos = fmt_str.find( '}', begin_pos );
            StringView  range   = fmt_str.substr( begin_pos, end_pos - begin_pos );

            {
                uint    feats = FS_ParseJSON( 0u, range, "\"optimalTilingFeatures\"" );
                FS_ParseJSON_ParseFeatures2( engFmt, feats, INOUT outFeatureSet );
            }
            {
                uint    feats = FS_ParseJSON( 0u, range, "\"bufferFeatures\"" );
                FS_ParseJSON_ParseFeatures2( engFmt, feats, INOUT outFeatureSet );
            }
        }};

        const auto  ParseFormatFlags2 = [fmt_str, &outFeatureSet] (EVertexType engFmt, uint vkFmt)
        {{
            String  name        = "            "s + ToString(vkFmt) + ",";
            usize   begin_pos   = fmt_str.find( name );
            if ( begin_pos == StringView::npos )
                return;

            usize       end_pos = fmt_str.find( '}', begin_pos );
            StringView  range   = fmt_str.substr( begin_pos, end_pos - begin_pos );

            {
                uint    feats = FS_ParseJSON( 0u, range, "\"bufferFeatures\"" );
                FS_ParseJSON_ParseFeatures2( engFmt, feats, INOUT outFeatureSet );
            }
        }};

        #define AE_PRIVATE_VKPIXELFORMATS_VISIT( _engineFmt_, _vkFormat_ )  ParseFormatFlags1( EPixelFormat::_engineFmt_, _vkFormat_ );
        AE_PRIVATE_VKPIXELFORMATS( AE_PRIVATE_VKPIXELFORMATS_VISIT )
        #undef AE_PRIVATE_VKPIXELFORMATS_VISIT

        #define AE_PRIVATE_VKPIXELFORMATS_VISIT( _engineFmt_, _vkFormat_ )  ParseFormatFlags2( EVertexType::_engineFmt_, _vkFormat_ );
        AE_PRIVATE_VKVERTEXFORMATS( AE_PRIVATE_VKPIXELFORMATS_VISIT )
        #undef AE_PRIVATE_VKPIXELFORMATS_VISIT

        return true;
    }

/*
=================================================
    FS_ParseJSON_Formats
=================================================
*/
    static bool  FS_ParseJSON_Formats (StringView json, INOUT FeatureSet &outFeatureSet)
    {
        if ( not FS_ParseJSON_Formats_1( json, INOUT outFeatureSet ))
            CHECK_ERR( FS_ParseJSON_Formats_2( json, INOUT outFeatureSet ));
        return true;
    }

/*
=================================================
    FS_ParseJSON_Queues
=================================================
*/
    static bool  FS_ParseJSON_Queues (StringView json, INOUT FeatureSet::Queues &queues)
    {
        const StringView    prefix = "\"queueFlags\": ";

        for (usize pos = 0;;)
        {
            pos = json.find( prefix, pos );
            if ( pos == String::npos )
                break;

            pos += prefix.size();
            const usize begin = pos;

            uint    val = 0;

            if ( json[pos] == '[' )
            {
                pos = json.find( ']', pos );
                CHECK_ERR( pos != String::npos );

                StringView  part = json.substr( begin, pos - begin );

                Array<StringView>   tokens;
                Parser::DivideString_CPP( part, OUT tokens );

                for (StringView token : tokens)
                {
                    if ( token == "VK_QUEUE_GRAPHICS_BIT" )         val |= VK_QUEUE_GRAPHICS_BIT;
                    if ( token == "VK_QUEUE_COMPUTE_BIT" )          val |= VK_QUEUE_COMPUTE_BIT;
                    if ( token == "VK_QUEUE_TRANSFER_BIT" )         val |= VK_QUEUE_TRANSFER_BIT;
                    if ( token == "VK_QUEUE_SPARSE_BINDING_BIT" )   val |= VK_QUEUE_SPARSE_BINDING_BIT;
                    if ( token == "VK_QUEUE_PROTECTED_BIT" )        val |= VK_QUEUE_PROTECTED_BIT;
                    if ( token == "VK_QUEUE_VIDEO_DECODE_BIT_KHR" ) val |= VK_QUEUE_VIDEO_DECODE_BIT_KHR;
                    if ( token == "VK_QUEUE_VIDEO_ENCODE_BIT_KHR" ) val |= VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
                    if ( token == "VK_QUEUE_OPTICAL_FLOW_BIT_NV" )  val |= VK_QUEUE_OPTICAL_FLOW_BIT_NV;
                }
            }
            else
            {
                Parser::ToEndOfLine( json, INOUT pos );

                StringView  part = json.substr( begin, pos - begin );

                CHECK_ERR( StringToValue( part, OUT val ));
            }

            if ( AllBits( val, VK_QUEUE_GRAPHICS_BIT ))
                queues.supported |= EQueueMask::Graphics;

            if ( AllBits( val, VK_QUEUE_COMPUTE_BIT ) and not AnyBits( val, VK_QUEUE_GRAPHICS_BIT ))
                queues.supported |= EQueueMask::AsyncCompute;

            if ( AllBits( val, VK_QUEUE_TRANSFER_BIT ) and not AnyBits( val, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT ))
                queues.supported |= EQueueMask::AsyncTransfer;
        }
        return true;
    }

} // namespace


/*
=================================================
    FeatureSetFromJSON
=================================================
*/
    bool  FeatureSetFromJSON (const Path &jsonFile, OUT FeatureSetExt &outFeatureSet, OUT String &outName)
    {
        String  json;
        {
            FileRStream     file{ jsonFile };
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( file.Read( file.RemainingSize(), OUT json ));
        }

        const HashMap<StringView, StringView>   replace_names = {
            { "maxUniformBufferSize",                   "maxUniformBufferRange"                 },
            { "maxStorageBufferSize",                   "maxStorageBufferRange"                 },
            { "maxDescriptorSets",                      "maxBoundDescriptorSets"                },
            { "imageViewMinLod",                        "minLod"                                },
            { "subgroupOperations",                     "subgroupSupportedOperations"           },
            { "subgroupStages",                         "subgroupSupportedStages"               },
            { "subgroupTypes",                          "shaderSubgroupExtendedTypes"           },
            { "subgroupQuadStages",                     "subgroupQuadOperationsInAllStages"     },
            { "subgroupSizeStages",                     "requiredSubgroupSizeStages"            },
            { "maxVertexAttributes",                    "maxVertexInputAttributes"              },
            { "maxVertexBuffers",                       "maxVertexInputBindings"                },
            { "maxMeshOutputPerVertexGranularity",      "meshOutputPerVertexGranularity"        },
            { "maxMeshOutputPerPrimitiveGranularity",   "meshOutputPerPrimitiveGranularity"     }
        };

        const auto  ReplaceName = [&replace_names] (StringView key)
        {{
            auto    it = replace_names.find( key );
            if ( it != replace_names.end() )
                return "\""s << it->second << "\"";
            else
                return "\""s << key << "\"";
        }};

        outFeatureSet = Default;
        outFeatureSet.SetAll( EFeature::Ignore );

        outFeatureSet.computeShader = EFeature::RequireTrue;
        outFeatureSet.tileShader    = EFeature::RequireFalse;

        {
            const uint  api_ver = FS_ParseJSON( 0u, json, "\"apiVersion\"" );
            const auto  ver     = Version2{ (api_ver >> 22) & 0x7Fu, (api_ver >> 12) & 0x3FFu };

            if ( ver >= Version2{1,1} )
            {
                outFeatureSet.subgroup          = EFeature::RequireTrue;
                outFeatureSet.subgroupTypes     = ESubgroupTypes::Float32 | ESubgroupTypes::Int32;

                const uint      subgroup_size   = FS_ParseJSON( 0u, json, "\"subgroupSize\"" );
                outFeatureSet.minSubgroupSize   = CheckCast<ushort>(subgroup_size);
                outFeatureSet.maxSubgroupSize   = CheckCast<ushort>(subgroup_size);
            }

            outFeatureSet.maxShaderVersion.spirv    = ver >= Version2{1,2}                      ? 150 :
                                                      HasSubString( json, "VK_KHR_spirv_1_4" )  ? 140 :
                                                      ver >= Version2{1,1}                      ? 130 : 100;

            outName = FS_ParseJSON_Str( json, "\"label\"" );
            if ( outName.empty() )
                outName = jsonFile.filename().string();
        }

        if ( not HasSubString( json, "VK_KHR_portability_subset" ))
        {
            outFeatureSet.constantAlphaColorBlendFactors = EFeature::RequireTrue;
            outFeatureSet.shaderSampleRateInterpolationFunctions = EFeature::RequireTrue;
            outFeatureSet.pointPolygons             = EFeature::RequireTrue;
            outFeatureSet.separateStencilMaskRef    = EFeature::RequireTrue;
            outFeatureSet.tessellationIsolines      = EFeature::RequireTrue;
            outFeatureSet.tessellationPointMode     = EFeature::RequireTrue;
            outFeatureSet.multisampleArrayImage     = EFeature::RequireTrue;
            outFeatureSet.samplerMipLodBias         = EFeature::RequireTrue;
            outFeatureSet.triangleFans              = EFeature::RequireTrue;
        }

        if ( HasSubString( json, "VK_EXT_sample_locations" ))
            outFeatureSet.sampleLocations = EFeature::RequireTrue;

        if ( HasSubString( json, "VK_EXT_vertex_attribute_divisor" ))
            outFeatureSet.vertexDivisor = EFeature::RequireTrue;

        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  outFeatureSet._name_ = FS_ParseJSON( outFeatureSet._name_, json, ReplaceName(AE_TOSTRING(_name_)) );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT

        #define AE_FEATURE_SET_VISIT( _type_, _name_ )  outFeatureSet.ext._name_ = FS_ParseJSON( outFeatureSet.ext._name_, json, "\"" AE_TOSTRING(_name_) "\"" );
        AE_FEATURE_SET_FIELDS2( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT

        // minTexelOffset
        {
            int min_off = 0;
            min_off = FS_ParseJSON( min_off, json, "\"minTexelOffset\"" );
            outFeatureSet.maxTexelOffset = CheckCast<ushort>(Min( outFeatureSet.maxTexelOffset, Abs(min_off)-1 ));
        }

        // minTexelGatherOffset
        {
            int min_off = 0;
            min_off = FS_ParseJSON( min_off, json, "\"minTexelGatherOffset\"" );
            min_off = Abs(min_off);
            min_off += min_off != 0 ? -1 : 0;
            outFeatureSet.maxTexelGatherOffset = CheckCast<ushort>(Min( outFeatureSet.maxTexelGatherOffset, min_off ));
        }

        // compute shader
        {
            Array<StringView>   tokens = FS_ParseJSON_N( json, "\"maxComputeWorkGroupSize\"" );
            if ( tokens.size() == 7 and tokens.front() == "[" )
            {
                outFeatureSet.maxComputeWorkGroupSizeX = StringToUInt( tokens[1] );
                outFeatureSet.maxComputeWorkGroupSizeY = StringToUInt( tokens[3] );
                outFeatureSet.maxComputeWorkGroupSizeZ = StringToUInt( tokens[5] );
            }
            else
            {
                CHECK( tokens.empty() );
                outFeatureSet.maxComputeWorkGroupSizeX = outFeatureSet.maxComputeWorkGroupInvocations;
                outFeatureSet.maxComputeWorkGroupSizeY = outFeatureSet.maxComputeWorkGroupInvocations;
                outFeatureSet.maxComputeWorkGroupSizeZ = outFeatureSet.maxComputeWorkGroupInvocations;
            }
        }

        // non uniform
        {
            #define NONUNIFORM( _name_ ) \
                if ( outFeatureSet._name_ == EFeature::RequireTrue ) \
                    outFeatureSet._name_ = FS_ParseJSON( EFeature::Ignore, json, AE_TOSTRING(_name_) "Native" )

            NONUNIFORM( shaderUniformBufferArrayNonUniformIndexing );
            NONUNIFORM( shaderSampledImageArrayNonUniformIndexing );
            NONUNIFORM( shaderStorageBufferArrayNonUniformIndexing );
            NONUNIFORM( shaderStorageImageArrayNonUniformIndexing );
            NONUNIFORM( shaderInputAttachmentArrayNonUniformIndexing );
            #undef NONUNIFORM
        }

        // validate shader stages
        {
            EShaderStages   all_stages = EShaderStages::All;
            if ( outFeatureSet.computeShader        != EFeature::RequireTrue )      all_stages &= ~EShaderStages::Compute;
            if ( outFeatureSet.geometryShader       != EFeature::RequireTrue )      all_stages &= ~EShaderStages::Geometry;
            if ( outFeatureSet.tessellationShader   != EFeature::RequireTrue )      all_stages &= ~(EShaderStages::TessControl | EShaderStages::TessEvaluation);
            if ( outFeatureSet.tileShader           != EFeature::RequireTrue )      all_stages &= ~EShaderStages::Tile;
            if ( outFeatureSet.taskShader           != EFeature::RequireTrue )      all_stages &= ~EShaderStages::MeshTask;
            if ( outFeatureSet.meshShader           != EFeature::RequireTrue )      all_stages &= ~EShaderStages::Mesh;
            if ( outFeatureSet.rayTracingPipeline   != EFeature::RequireTrue )      all_stages &= ~EShaderStages::AllRayTracing;

            outFeatureSet.subgroupStages             &= all_stages;
            outFeatureSet.subgroupQuadStages         &= all_stages;
            outFeatureSet.requiredSubgroupSizeStages &= all_stages;
            outFeatureSet.rayQueryStages              = (outFeatureSet.rayQuery == EFeature::RequireTrue ? all_stages : Default);
        }

        // fragment shading rate
        if ( outFeatureSet.attachmentFragmentShadingRate == EFeature::RequireTrue )
        {
            VkExtent2D  min, max;
            uint        aspect = FS_ParseJSON( 0u, json, "maxFragmentShadingRateAttachmentTexelSizeAspectRatio" );
            CHECK( FS_ParseJSON_Extent2D( "minFragmentShadingRateAttachmentTexelSize", json, OUT min ));
            CHECK( FS_ParseJSON_Extent2D( "maxFragmentShadingRateAttachmentTexelSize", json, OUT max ));

            CHECK( min.width > 0 and min.height > 0 );
            CHECK( min.width <= max.width and min.height <= max.height );

            outFeatureSet.fragmentShadingRateTexelSize.minX     = POTValue{ min.width }.GetPOT();
            outFeatureSet.fragmentShadingRateTexelSize.minY     = POTValue{ min.height }.GetPOT();
            outFeatureSet.fragmentShadingRateTexelSize.maxX     = POTValue{ max.width }.GetPOT();
            outFeatureSet.fragmentShadingRateTexelSize.maxY     = POTValue{ max.height }.GetPOT();
            outFeatureSet.fragmentShadingRateTexelSize.aspect   = POTValue{ aspect }.GetPOT();
        }

        FS_ParseJSON_Queues( json, INOUT outFeatureSet.queues );
        FS_ParseJSON_Formats( json, INOUT outFeatureSet );

        return true;
    }
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
    FS_ToString (EFeature)
=================================================
*/
    static void  FS_ToString (INOUT String &str, EFeature feat, StringView name)
    {
        if ( feat == EFeature::Ignore )
            return;

        str << "\tfset." << name << " (";

        BEGIN_ENUM_CHECKS();
        switch ( feat )
        {
            case EFeature::RequireFalse :   str << "False"; break;
            case EFeature::RequireTrue :    str << "True";  break;
            case EFeature::Ignore :
            case EFeature::_Count :         CHECK(false);   break;
        }
        END_ENUM_CHECKS();

        str << ");\n";
    }

/*
=================================================
    FS_ToString (uint)
=================================================
*/
    static void  FS_ToString (INOUT String &str, uint value, StringView name)
    {
        if ( value == 0 )
            return;

        str << "\tfset." << name << " (";

        const uint  pot     = CeilIntLog2( value );
        const uint  pot10   = AlignDown( pot, 10 );

        if ( pot10 > 0 and IsPowerOfTwo( value ))
        {
            str << ToString( 1u << (pot - pot10) ) << " << " << ToString( pot10 );
        }
        else
        if ( value >= (1u << 10) and value == (1u << pot)-1 )
        {
            str << "0x" << ToString<16>( value );
        }
        else
            str << ToString<10>( value );

        str << ");\n";
    }

    static void  FS_ToString (INOUT String &str, ubyte value, StringView name)
    {
        FS_ToString( INOUT str, uint(value), name );
    }

    static void  FS_ToString (INOUT String &str, ushort value, StringView name)
    {
        FS_ToString( INOUT str, uint(value), name );
    }

/*
=================================================
    FS_ToString (float)
=================================================
*/
    static void  FS_ToString (INOUT String &str, float value, StringView name)
    {
        if ( value == 0.f )
            return;

        str << "\tfset." << name << " (" << ToString( value, 2 ) << ");\n";
    }

/*
=================================================
    FS_ToString (PerDescriptorSet)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::PerDescriptorSet &val, StringView name)
    {
        const auto  ValToStr = [&str, name] (StringView valName, uint value)
        {{
            if ( value == 0 )
                return;
            str << "\tfset." << name << "_" << valName << " (" << ToString(value) << ");\n";
        }};

        ValToStr( "maxInputAttachments",    val.maxInputAttachments );
        ValToStr( "maxSampledImages",       val.maxSampledImages );
        ValToStr( "maxSamplers",            val.maxSamplers );
        ValToStr( "maxStorageBuffers",      val.maxStorageBuffers );
        ValToStr( "maxStorageImages",       val.maxStorageImages );
        ValToStr( "maxUniformBuffers",      val.maxUniformBuffers );
        ValToStr( "maxAccelStructures",     val.maxAccelStructures );
        ValToStr( "maxTotalResources",      val.maxTotalResources );

        CHECK( name == "perDescrSet" or name == "perStage" );
    }

/*
=================================================
    FS_ToString (SubgroupOperationBits)
=================================================
*/
    static void  FS_ToString (INOUT String &str, FeatureSet::SubgroupOperationBits val, StringView name)
    {
        if ( val.None() )
            return;

        CHECK( name == "subgroupOperations" );

        const auto  basic       = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Basic_Begin,            ESubgroupOperation::_Basic_End );
        const auto  vote        = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Vote_Begin,             ESubgroupOperation::_Vote_End );
        const auto  arithmetic  = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Arithmetic_Begin,       ESubgroupOperation::_Arithmetic_End );
        const auto  ballot      = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Ballot_Begin,           ESubgroupOperation::_Ballot_End );
        const auto  shuffle     = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Shuffle_Begin,          ESubgroupOperation::_Shuffle_End );
        const auto  shuffle_rel = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_ShuffleRelative_Begin,  ESubgroupOperation::_ShuffleRelative_End );
        const auto  clustered   = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Clustered_Begin,        ESubgroupOperation::_Clustered_End );
        const auto  quad        = FeatureSet::SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Quad_Begin,             ESubgroupOperation::_Quad_End );

        if ( val.All( basic ))
        {
            val &= ~basic;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );\n";
        }
        if ( val.All( vote ))
        {
            val &= ~vote;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Vote_Begin, ESubgroupOperation::_Vote_End );\n";
        }
        if ( val.All( arithmetic ))
        {
            val &= ~arithmetic;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Arithmetic_Begin, ESubgroupOperation::_Arithmetic_End );\n";
        }
        if ( val.All( ballot ))
        {
            val &= ~ballot;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Ballot_Begin, ESubgroupOperation::_Ballot_End );\n";
        }
        if ( val.All( shuffle ))
        {
            val &= ~shuffle;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Shuffle_Begin, ESubgroupOperation::_Shuffle_End );\n";
        }
        if ( val.All( shuffle_rel ))
        {
            val &= ~shuffle_rel;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );\n";
        }
        if ( val.All( clustered ))
        {
            val &= ~clustered;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Clustered_Begin, ESubgroupOperation::_Clustered_End );\n";
        }
        if ( val.All( quad ))
        {
            val &= ~quad;
            str << "\tfset.AddSubgroupOperationRange( ESubgroupOperation::_Quad_Begin, ESubgroupOperation::_Quad_End );\n";
        }

        if ( val.None() )
            return;

        str << "\tfset.AddSubgroupOperations({\n\t\t";

        for (usize i = 0, j = 0; i < val.size(); ++i)
        {
            if ( val.contains( ESubgroupOperation(i) ))
            {
                str << "ESubgroupOperation::" << ToString( ESubgroupOperation(i) ) << ", ";
                if ( ++j > 3 )
                {
                    j = 0;
                    str << "\n\t\t";
                }
            }
        }

        usize   pos = str.rfind( ',' );
        if ( pos < str.size() )
            str.resize( pos );

        str << "\n\t});\n";
    }

/*
=================================================
    FS_ToString (SampleCountBits)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::SampleCountBits bits, StringView name)
    {
        if ( bits == Default )
            return;

        str << "\tfset." << name << "({ ";

        for (usize i = 0; i < CT_SizeOfInBits< decltype(bits) >; ++i)
        {
            if ( uint(bits) & (1u << i) )
                str << ToString( 1u << i ) << ", ";
        }

        str.pop_back();
        str.pop_back();

        str << " });\n";
    }

/*
=================================================
    FS_ToString (ESubgroupTypes)
=================================================
*/
    static void  FS_ToString (INOUT String &str, ESubgroupTypes val, StringView name)
    {
        if ( val == Default )
            return;

        str << "\tfset." << name << "(ESubgroupTypes(";

        for (auto t : BitfieldIterate( val ))
        {
            str << "\n\t\tESubgroupTypes::";

            BEGIN_ENUM_CHECKS();
            switch ( t )
            {
                case ESubgroupTypes::Float32 :  str << "Float32";   break;
                case ESubgroupTypes::Int32 :    str << "Int32";     break;
                case ESubgroupTypes::Int8 :     str << "Int8";      break;
                case ESubgroupTypes::Int16 :    str << "Int16";     break;
                case ESubgroupTypes::Int64 :    str << "Int64";     break;
                case ESubgroupTypes::Float16 :  str << "Float16";   break;
                case ESubgroupTypes::Unknown :
                case ESubgroupTypes::_Last :
                case ESubgroupTypes::All :
                default :                       CHECK( !"unknown subgroup type" ); break;
            }
            END_ENUM_CHECKS();

            str << " | ";
        }

        str.pop_back();
        str.pop_back();
        str.pop_back();

        str << "\n\t));\n";
    }

/*
=================================================
    FS_ToString (EShaderStages)
=================================================
*/
    static void  FS_ToString (INOUT String &str, EShaderStages val, StringView name)
    {
        if ( val == Default )
            return;

        str << "\tfset." << name << "(";

        if ( val == EShaderStages::All )
        {
            str << "EShaderStages::All);\n";
            return;
        }

        str << "EShaderStages(";

        if ( AllBits( val, EShaderStages::AllGraphics ))
        {
            val &= ~EShaderStages::AllGraphics;
            str << "\n\t\tEShaderStages::AllGraphics | ";
        }

        if ( AllBits( val, EShaderStages::AllRayTracing ))
        {
            val &= ~EShaderStages::AllRayTracing;
            str << "\n\t\tEShaderStages::AllRayTracing | ";
        }

        for (auto t : BitIndexIterate<EShader>( val ))
        {
            str << "\n\t\tEShaderStages::" << ToString( t ) << " | ";
        }

        str.pop_back();
        str.pop_back();
        str.pop_back();

        str << "\n\t));\n";
    }

/*
=================================================
    FS_ToString (PixelFormatSet_t)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::PixelFormatSet_t &val, StringView name)
    {
        if ( val.None() )
            return;

        str << "\tfset.AddTexelFormats( EFormatFeature::";

        if ( name == "storageImageAtomicFormats" )          str << "StorageImageAtomic";        else
        if ( name == "storageImageFormats" )                str << "StorageImage";              else
        if ( name == "attachmentBlendFormats" )             str << "AttachmentBlend";           else
        if ( name == "attachmentFormats" )                  str << "Attachment";                else
        if ( name == "linearSampledFormats" )               str << "LinearSampled";             else
        if ( name == "uniformTexBufferFormats" )            str << "UniformTexelBuffer";        else
        if ( name == "storageTexBufferFormats" )            str << "StorageTexelBuffer";        else
        if ( name == "storageTexBufferAtomicFormats" )      str << "StorageTexelBufferAtomic";  else
        if ( name == "hwCompressedAttachmentFormats" )      str << "HWCompressedAttachment";    else
        if ( name == "lossyCompressedAttachmentFormats" )   str << "LossyCompressedAttachment"; else
                                                            CHECK_MSG( false, "unknown pixel format set" );
        str << ", {\n\t\t";

        for (usize i = 0, j = 0; i < val.size(); ++i)
        {
            if ( val.contains( EPixelFormat(i) ))
            {
                str << "EPixelFormat::" << ToString( EPixelFormat(i) ) << ", ";
                if ( ++j > 3 )
                {
                    j = 0;
                    str << "\n\t\t";
                }
            }
        }

        usize   pos = str.rfind( ',' );
        if ( pos < str.size() )
            str.resize( pos );

        str << "\n\t});\n";
    }

/*
=================================================
    FS_ToString (SurfaceFormatSet_t)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::SurfaceFormatSet_t &val, StringView name)
    {
        Unused( str, val, name );
    }

/*
=================================================
    FS_ToString (VertexFormatSet_t)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::VertexFormatSet_t &val, StringView name)
    {
        Unused( name );

        if ( val.None() )
            return;

        str << "\tfset.";
        if ( name == "vertexFormats" )              str << "AddVertexFormats";              else
        if ( name == "accelStructVertexFormats" )   str << "AddAccelStructVertexFormats";   else
                                                    CHECK_MSG( false, "unknown vertex format set" );
        str << "({\n\t\t";

        for (usize i = 0, j = 0; i < val.size(); ++i)
        {
            if ( val.contains( EVertexType(i) ))
            {
                str << "EVertexType::" << ToString( EVertexType(i) ) << ", ";
                if ( ++j > 3 )
                {
                    j = 0;
                    str << "\n\t\t";
                }
            }
        }

        usize   pos = str.rfind( ',' );
        if ( pos < str.size() )
            str.resize( pos );

        str << "\n\t});\n";
    }

/*
=================================================
    FS_ToString (VendorIDs_t)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::VendorIDs_t &val, StringView name)
    {
        Unused( str, val, name );
        // TODO
    }

/*
=================================================
    FS_ToString (GraphicsDevices_t)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::GraphicsDevices_t &val, StringView name)
    {
        Unused( str, val, name );
        // TODO
    }

/*
=================================================
    FS_ToString (ShaderVersion)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::ShaderVersion &val, StringView)
    {
        if ( val.spirv != 0 )   str << "\tfset.minSpirvVersion (" << ToString( val.spirv ) << ");\n";
        if ( val.metal != 0 )   str << "\tfset.minMetalVersion (" << ToString( val.metal ) << ");\n";
    }

/*
=================================================
    FS_ToString (VRSTexelSize)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::VRSTexelSize &val, StringView)
    {
        const auto  ToStr = [] (uint val)
        {{
            return ToString( 1u << val );
        }};

        if ( val ) {
            str << "\tfset.fragmentShadingRateTexelSize ( {" << ToStr( val.minX ) << ", " << ToStr( val.minY )
                << "}, {" << ToStr( val.maxX ) << ", " << ToStr( val.maxY ) << "}, " << ToStr( val.aspect ) << " );\n";
        }
    }

/*
=================================================
    FS_ToString (ShadingRateSet_t)
=================================================
*/
    static void  FS_ToString (INOUT String &, const FeatureSet::ShadingRateSet_t &, StringView)
    {
    }

/*
=================================================
    FS_ToString (EQueueMask)
=================================================
*/
    static void  FS_ToString (INOUT String &str, const FeatureSet::Queues &queues, StringView)
    {
        if ( queues.supported != Default )
        {
            str << "\tfset.supportedQueues(EQueueMask( ";

            for (EQueueMask mask = queues.supported; mask != Default;)
            {
                auto    q = ExtractBitLog2<EQueueType>( INOUT mask );

                str << "EQueueMask::" << ToString(q);

                if ( mask != Default )
                    str << " | ";
            }
            str << " ));\n";
        }
        if ( queues.required != Default )
        {
            str << "\tfset.requiredQueues(EQueueMask( ";

            for (EQueueMask mask = queues.required; mask != Default;)
            {
                auto    q = ExtractBitLog2<EQueueType>( INOUT mask );

                str << "EQueueMask::" << ToString(q);

                if ( mask != Default )
                    str << " | ";
            }
            str << " ));\n";
        }
    }

} // namespace


/*
=================================================
    FeatureSetToScript
=================================================
*/
    bool  FeatureSetToScript (const Path &outFile, StringView fsName, const FeatureSet &fs, StringView comment)
    {
        String  str;
        str << "#include <pipeline_compiler.as>\n\n"
            << "\nvoid ASmain ()\n{\n"
            << comment
            << "\tconst EFeature  True = EFeature::RequireTrue;\n\n"
            << "\tRC<FeatureSet>  fset = FeatureSet( \"" << fsName << "\" );\n\n";

        #define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )  FS_ToString( INOUT str, fs. _name_, AE_TOSTRING(_name_) );
        AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
        #undef AE_FEATURE_SET_VISIT

        str << "}\n";

      #if not AE_PRIVATE_USE_TABS
        str = Parser::TabsToSpaces( str );
      #endif

        FileWStream     file{ outFile };
        CHECK_ERR( file.IsOpen() );
        CHECK_ERR( file.Write( StringView{str} ));

        return true;
    }

} // AE::Graphics
