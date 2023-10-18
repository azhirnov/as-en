// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    from docs:
    Multiple concurrent imports may or may not be beneficial, however.
    For certain file formats in conjunction with little or no post processing IO times tend to be the performance bottleneck.
    Intense post processing together with 'slow' file formats like X or Collada might scale well with multiple concurrent imports.
*/

#ifdef AE_ENABLE_ASSIMP
# include "res_loaders/Assimp/AsssimpUtils.cpp.h"

# include "assimp/Importer.hpp"
# include "assimp/PostProcess.h"
# include "assimp/Scene.h"
# include "assimp/GltfMaterial.h"

# include "res_loaders/Assimp/AssimpLoader.h"
# include "res_loaders/Intermediate/IntermScene.h"
# include "res_loaders/Intermediate/IntermVertexAttribs_Setter.h"

namespace AE::ResLoader
{
namespace
{
    struct AttribHash {
        ND_ usize  operator () (const RC<IntermVertexAttribs> &value) C_NE___ {
            return usize(value->CalcHash());
        }
    };

    struct AttribEq {
        ND_ bool  operator () (const RC<IntermVertexAttribs> &lhs, const RC<IntermVertexAttribs> &rhs) C_NE___ {
            return *lhs == *rhs;
        }
    };

    using VertexAttribsSet_t = HashSet< RC<IntermVertexAttribs>, AttribHash, AttribEq >;


    struct SceneData
    {
        Array<RC<IntermMaterial>>   materials;
        Array<RC<IntermMesh>>       meshes;
        VertexAttribsSet_t          attribsCache;
        Array<RC<IntermLight>>      lights;
        IntermScene::SceneNode      root;
    };

/*
=================================================
    ConvertMatrix
=================================================
*/
    ND_ static TTransformation<float>  ConvertMatrix (const aiMatrix4x4 &src)
    {
        const float4x4  dst {
            float4{ src[0][0], src[0][1], src[0][2], src[0][3] },
            float4{ src[1][0], src[1][1], src[1][2], src[1][3] },
            float4{ src[2][0], src[2][1], src[2][2], src[2][3] },
            float4{ src[3][0], src[3][1], src[3][2], src[3][3] }
        };
        return TTransformation<float>{ dst.Transpose() };
    }

/*
=================================================
    ConvertVec
=================================================
*/
    ND_ static float2  ConvertVec (const aiVector2D &src)
    {
        return float2{ src.x, src.y };
    }

    ND_ static float3  ConvertVec (const aiVector3D &src)
    {
        return float3{ src.x, src.y, src.z };
    }

    ND_ static float3  ConvertVec (const aiColor3D &src)
    {
        return float3{ src.r, src.g, src.b };
    }

    ND_ static float4  ConvertVec (const aiColor4D &src)
    {
        return float4{ src.r, src.g, src.b, src.a };
    }

/*
=================================================
    Str
=================================================
*/
    ND_ static StringView  Str (const aiString &str)
    {
        return StringView{ str.C_Str(), str.length };
    }

/*
=================================================
    ConvertWrapMode
=================================================
*/
    ND_ static EAddressMode  ConvertWrapMode (aiTextureMapMode mode)
    {
        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case aiTextureMapMode_Wrap :        return EAddressMode::Repeat;
            case aiTextureMapMode_Mirror :      return EAddressMode::MirrorRepeat;
            case aiTextureMapMode_Clamp :       return EAddressMode::ClampToEdge;
            case aiTextureMapMode_Decal :       return EAddressMode::ClampToBorder; // TODO: add transparent border color
            case _aiTextureMapMode_Force32Bit : break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown wrap mode", EAddressMode::Repeat );
    }

/*
=================================================
    ConvertBorderColor
=================================================
*/
    ND_ static EBorderColor  ConvertBorderColor (aiTextureMapMode u, aiTextureMapMode v, aiTextureMapMode w)
    {
        if ( u == aiTextureMapMode_Decal or
             v == aiTextureMapMode_Decal or
             w == aiTextureMapMode_Decal )
            return EBorderColor::FloatTransparentBlack;

        return Default;
    }

/*
=================================================
    ConvertMapping
=================================================
*/
    using ETextureMapping = IntermMaterial::ETextureMapping;

    ND_ static ETextureMapping  ConvertMapping (aiTextureMapping mapping)
    {
        BEGIN_ENUM_CHECKS();
        switch ( mapping )
        {
            case aiTextureMapping_UV :          return ETextureMapping::UV;
            case aiTextureMapping_SPHERE :      return ETextureMapping::Sphere;
            case aiTextureMapping_CYLINDER :    return ETextureMapping::Cylinder;
            case aiTextureMapping_BOX :         return ETextureMapping::Box;
            case aiTextureMapping_PLANE :       return ETextureMapping::Plane;
            case aiTextureMapping_OTHER :       break;
            case _aiTextureMapping_Force32Bit : break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown texture mapping" );
    };

/*
=================================================
    TextureTypeToMaterialKey
=================================================
*/
    ND_ static IntermMaterial::EKey  TextureTypeToMaterialKey (aiTextureType type)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case aiTextureType_DIFFUSE :            return IntermMaterial::EKey::Diffuse;
            case aiTextureType_SPECULAR :           return IntermMaterial::EKey::Specular;
            case aiTextureType_AMBIENT :            return IntermMaterial::EKey::Ambient;
            case aiTextureType_EMISSIVE :           return IntermMaterial::EKey::Emissive;
            case aiTextureType_HEIGHT :             return IntermMaterial::EKey::HeightMap;
            case aiTextureType_NORMALS :            return IntermMaterial::EKey::NormalMap;
            case aiTextureType_SHININESS :          return IntermMaterial::EKey::Shininess;
            case aiTextureType_OPACITY :            return IntermMaterial::EKey::Opacity;
            case aiTextureType_DISPLACEMENT :       return IntermMaterial::EKey::DisplacementMap;
            case aiTextureType_LIGHTMAP :           return IntermMaterial::EKey::LightMap;
            case aiTextureType_REFLECTION :         return IntermMaterial::EKey::ReflectionMap;

            case aiTextureType_BASE_COLOR :         return IntermMaterial::EKey::PBR_BaseColor;
            case aiTextureType_NORMAL_CAMERA :      return IntermMaterial::EKey::PBR_NormalMap;
            case aiTextureType_EMISSION_COLOR :     return IntermMaterial::EKey::PBR_Emission;
            case aiTextureType_METALNESS :          return IntermMaterial::EKey::PBR_Metalness;
            case aiTextureType_DIFFUSE_ROUGHNESS :  return IntermMaterial::EKey::PBR_Roughness;
            case aiTextureType_AMBIENT_OCCLUSION :  return IntermMaterial::EKey::PBR_AmbientOcclusionMap;
            case aiTextureType_SHEEN :              return IntermMaterial::EKey::PBR_Sheen;
            case aiTextureType_CLEARCOAT :          return IntermMaterial::EKey::PBR_Clearcoat;
            case aiTextureType_TRANSMISSION :       return IntermMaterial::EKey::PBR_Transmission;

            case aiTextureType_NONE :
            case aiTextureType_UNKNOWN :
            case _aiTextureType_Force32Bit :        break;
        }
        END_ENUM_CHECKS();
        return Default;
    }

/*
=================================================
    TextureTypeToMaterialKey
=================================================
*/
    static bool  ReadColorForTexture (const aiMaterial *src, aiTextureType texType, OUT float4 &value)
    {
        #define GET_COLOR( _key_ )                                  \
        {                                                           \
            aiColor4D   color;                                      \
            if ( src->Get( _key_, OUT color ) == AI_SUCCESS ) {     \
                value = ConvertVec( color );                        \
                return true;                                        \
            }                                                       \
            break;                                                  \
        }
        #define GET_VALUE( _key_ )                                  \
        {                                                           \
            float   fvalue  = 0.0f;                                 \
            if ( src->Get( _key_, OUT fvalue ) == AI_SUCCESS ) {    \
                value = float4{ fvalue };                           \
                return true;                                        \
            }                                                       \
            break;                                                  \
        }

        BEGIN_ENUM_CHECKS();
        switch ( texType )
        {
            case aiTextureType_DIFFUSE :            GET_COLOR( AI_MATKEY_COLOR_DIFFUSE );
            case aiTextureType_SPECULAR :           GET_COLOR( AI_MATKEY_COLOR_SPECULAR );
            case aiTextureType_AMBIENT :            GET_COLOR( AI_MATKEY_COLOR_AMBIENT );
            case aiTextureType_EMISSIVE :           GET_COLOR( AI_MATKEY_COLOR_EMISSIVE );
            case aiTextureType_HEIGHT :             break;
            case aiTextureType_NORMALS :            break;
            case aiTextureType_SHININESS :          GET_VALUE( AI_MATKEY_SHININESS );
            case aiTextureType_DISPLACEMENT :       break;
            case aiTextureType_LIGHTMAP :           break;
            case aiTextureType_REFLECTION :         break;

            case aiTextureType_OPACITY :
            {
                float   fvalue  = 0.0f;
                if ( src->Get( AI_MATKEY_OPACITY, OUT fvalue ) == AI_SUCCESS and fvalue < 1.f ) {
                    value = float4{ fvalue };
                    return true;
                }
                break;
            }

            case aiTextureType_BASE_COLOR :         GET_COLOR( AI_MATKEY_BASE_COLOR );
            case aiTextureType_NORMAL_CAMERA :      break;
            case aiTextureType_EMISSION_COLOR :     break;
            case aiTextureType_METALNESS :          break;
            case aiTextureType_DIFFUSE_ROUGHNESS :  break;
            case aiTextureType_AMBIENT_OCCLUSION :  break;
            case aiTextureType_SHEEN :              GET_COLOR( AI_MATKEY_SHEEN_COLOR_FACTOR );
            case aiTextureType_CLEARCOAT :          break;
            case aiTextureType_TRANSMISSION :       break;

            case aiTextureType_NONE :
            case aiTextureType_UNKNOWN :
            case _aiTextureType_Force32Bit :        break;
        }
        END_ENUM_CHECKS();
        return false;

        #undef GET_COLOR
        #undef GET_VALUE
    }

/*
=================================================
    LoadMaterial
=================================================
*/
    ND_ static bool  LoadMaterial (const aiMaterial *src, OUT RC<IntermMaterial> &dst)
    {
        using EKey = IntermMaterial::EKey;

        dst = MakeRC<IntermMaterial>();

        {
            aiString    mtr_name;
            if ( src->Get( AI_MATKEY_NAME, OUT mtr_name ) == AI_SUCCESS )
                dst->Set( Str( mtr_name ));
        }{
            int     two_sided = 1;
            if ( src->Get( AI_MATKEY_TWOSIDED, OUT two_sided ) == AI_SUCCESS )
                dst->Set( two_sided != 0 ? ECullMode::None : ECullMode::Back );
        }{
            int     wireframe = 0;
            if ( src->Get( AI_MATKEY_ENABLE_WIREFRAME, OUT wireframe ) == AI_SUCCESS )
                dst->EditSettings().wireframe = (wireframe != 0);
        }{
            int     blend_mode = 0;
            if ( src->Get( AI_MATKEY_BLEND_FUNC, OUT blend_mode ) == AI_SUCCESS )
            {
                auto&   blend = dst->EditSettings().blendMode;
                BEGIN_ENUM_CHECKS();
                switch ( aiBlendMode(blend_mode) )
                {
                    case aiBlendMode_Default :
                        blend.src = EBlendFactor::SrcAlpha;
                        blend.dst = EBlendFactor::OneMinusSrcAlpha;
                        blend.op  = EBlendOp::Add;
                        break;

                    case aiBlendMode_Additive :
                        blend.src = EBlendFactor::One;
                        blend.dst = EBlendFactor::One;
                        blend.op  = EBlendOp::Add;
                        break;

                    case _aiBlendMode_Force32Bit :
                    default :                       DBG_WARNING( "unknown blend mode" );
                }
                END_ENUM_CHECKS();
            }
        }

        for (uint i = 0; i <= AI_TEXTURE_TYPE_MAX; ++i)
        {
            const auto  key = TextureTypeToMaterialKey( aiTextureType(i) );
            if ( key == Default )
                continue;

            aiString            tex_name;
            aiTextureMapMode    map_mode[3] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };
            aiTextureMapping    mapping     = aiTextureMapping_UV;
            uint                uv_index    = UMax;

            if ( src->GetTexture( aiTextureType(i), 0, OUT &tex_name, OUT &mapping, OUT &uv_index, null, null, OUT map_mode ) == AI_SUCCESS )
            {
                IntermMaterial::MtrTexture  mtr_tex;
                mtr_tex.image           = MakeRC<IntermImage>( Str( tex_name ));
                mtr_tex.name            = Str( tex_name );
            //  mtr_tex.uvTransform     =
                mtr_tex.mapping         = ConvertMapping( mapping );
                mtr_tex.addressModeU    = ConvertWrapMode( map_mode[0] );
                mtr_tex.addressModeV    = ConvertWrapMode( map_mode[1] );
                mtr_tex.addressModeW    = ConvertWrapMode( map_mode[2] );
                mtr_tex.filter          = EFilter::Linear;
                mtr_tex.borderColor     = ConvertBorderColor( map_mode[0], map_mode[1], map_mode[2] );
                mtr_tex.uvIndex         = ubyte(uv_index);
                ReadColorForTexture( src, aiTextureType(i), OUT mtr_tex.valueScale );

                int     flags = 0;
                if ( src->Get( AI_MATKEY_TEXFLAGS(aiTextureType(i),0), OUT flags ) == AI_SUCCESS ) {
                    flags = 0;
                }

                dst->Set( key, RVRef(mtr_tex) );
            }
            else
            {
                float4  value;
                if ( ReadColorForTexture( src, aiTextureType(i), OUT value ))
                    dst->Set( key, value );
            }
        }

        {
            aiString    mode;
            if ( src->Get( AI_MATKEY_GLTF_ALPHAMODE, OUT mode ) == AI_SUCCESS )
            {
                if ( Str(mode) == "MASK" )
                {
                    float   cutoff = 0.5f;
                    src->Get( AI_MATKEY_GLTF_ALPHACUTOFF, OUT cutoff );
                    dst->EditSettings().alphaCutoff = cutoff;
                }
                else
                if ( Str(mode) == "OPAQUE" )
                {
                    CHECK( not dst->EditSettings().blendMode );
                }
                else
                if ( Str(mode) == "BLEND" )
                {
                    auto&   blend = dst->EditSettings().blendMode;
                    blend.src = EBlendFactor::SrcAlpha;
                    blend.dst = EBlendFactor::OneMinusSrcAlpha;
                    blend.op  = EBlendOp::Add;
                }
                else
                    AE_LOG_DBG( "In material '"s << dst->Name() << "' has unsupported alphaMode='"s << Str(mode) << "'" );
            }
        }

        return true;
    }

/*
=================================================
    CreateVertexAttribs
=================================================
*/
    ND_ static bool  CreateVertexAttribs (const aiMesh *mesh, OUT Bytes &stride, OUT RC<IntermVertexAttribs> &outAttribs)
    {
        const StaticArray<EVertexType, 5>   float_vert_types = {
            EVertexType::Unknown, EVertexType::Float, EVertexType::Float2, EVertexType::Float3, EVertexType::Float4
        };

        outAttribs = MakeRC<IntermVertexAttribs>();

        IntermVertexAttribs::Setter     attribs {*outAttribs};
        Bytes                           offset;

        attribs.AddBuffer( 1_b );

        const auto  AddVertex = [&attribs, &offset] (const VertexAttributeName::Name_t &id, EVertexType type)
        {{
            attribs.AddVertex( id, type, offset );
            offset += EVertexType_SizeOf( type );
        }};

        if ( mesh->HasPositions() )
            AddVertex( VertexAttributeName::Position, EVertexType::Float3 );

        if ( mesh->HasNormals() )
            AddVertex( VertexAttributeName::Normal, EVertexType::Float3 );

        if ( mesh->mTangents != null and mesh->mNumVertices > 0 )
            AddVertex( VertexAttributeName::Tangent, EVertexType::Float3 );

        if ( mesh->mBitangents != null and mesh->mNumVertices > 0 )
            AddVertex( VertexAttributeName::BiTangent, EVertexType::Float3 );

        for (uint t = 0; t < CountOf(VertexAttributeName::TextureUVs); ++t)
        {
            if ( mesh->HasTextureCoords(t) )
                AddVertex( VertexAttributeName::TextureUVs[t], float_vert_types[mesh->mNumUVComponents[t]] );
        }

        if ( mesh->HasBones() )
            AddVertex( VertexAttributeName::BoneWeights, EVertexType::Float );

        stride  = offset;
        return true;
    }

/*
=================================================
    LoadMesh
=================================================
*/
    ND_ static bool  LoadMesh (const aiMesh *src, OUT RC<IntermMesh> &dst, INOUT VertexAttribsSet_t &attribsCache)
    {
        CHECK_ERR( src->mPrimitiveTypes == aiPrimitiveType_TRIANGLE );
        CHECK_ERR( not src->HasBones() );
        CHECK_ERR( src->mNumFaces > 0 );
        CHECK_ERR( src->mNumVertices > 0 );
        CHECK_ERR( src->mFaces[0].mNumIndices == 3 );

        Bytes                   vert_stride;
        RC<IntermVertexAttribs> attribs;
        CHECK_ERR( CreateVertexAttribs( src, OUT vert_stride, OUT attribs ));

        attribs = *attribsCache.insert( attribs ).first;

        Array<ubyte>    vertices;
        Array<ubyte>    indices;

        const auto      CopyAttrib = [&vertices, attribs, src, vert_stride] (const auto &data, const VertexAttributeName::Name_t &id, uint vertIdx)
        {{
            using   T       = RemoveCVRef< decltype(data) >;
            auto    at_data = attribs->GetData<T>( id, vertices.data(), src->mNumVertices, vert_stride );
            MemCopy( OUT const_cast<T &>(at_data[vertIdx]), data );
        }};

        vertices.resize( src->mNumVertices * usize(vert_stride) );  // throw
        indices.resize( src->mNumFaces * 3 * sizeof(uint) );        // throw

        for (uint i = 0; i < src->mNumFaces; ++i)
        {
            const aiFace *  face = &src->mFaces[i];
            ASSERT( face->mNumIndices == 3 );

            for (uint j = 0; j < 3; ++j) {
                Cast<uint>(indices.data()) [i*3 + j] = face->mIndices[j];
            }
        }

        if ( src->HasPositions() )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( packed_float3{src->mVertices[i].x, src->mVertices[i].y, src->mVertices[i].z}, VertexAttributeName::Position, i );
            }
        }
        if ( src->HasNormals() )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( Normalize(packed_float3{src->mNormals[i].x, src->mNormals[i].y, src->mNormals[i].z}), VertexAttributeName::Normal, i );
            }
        }
        if ( src->mTangents != null )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( Normalize(packed_float3{src->mTangents[i].x, src->mTangents[i].y, src->mTangents[i].z}), VertexAttributeName::Tangent, i );
            }
        }
        if ( src->mBitangents != null )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( Normalize(packed_float3{src->mBitangents[i].x, src->mBitangents[i].y, src->mBitangents[i].z}), VertexAttributeName::BiTangent, i );
            }
        }

        for (uint t = 0; t < CountOf(VertexAttributeName::TextureUVs); ++t)
        {
            if ( not src->HasTextureCoords(t) )
                continue;

            if ( src->mNumUVComponents[t] == 2 )
            {
                for (uint i = 0; i < src->mNumVertices; ++i) {
                    CopyAttrib( packed_float2{src->mTextureCoords[t][i].x, 1.f - src->mTextureCoords[t][i].y}, VertexAttributeName::TextureUVs[t], i );
                }
            }
            else
            //if ( src->mNumUVComponents[t] == 3 )
            {
                for (uint i = 0; i < src->mNumVertices; ++i) {
                    CopyAttrib( packed_float3{src->mTextureCoords[t][i].x, src->mTextureCoords[t][i].y, src->mTextureCoords[t][i].z}, VertexAttributeName::TextureUVs[t], i );
                }
            }
        }

        EPrimitive  topology = Default;
        BEGIN_ENUM_CHECKS();
        switch ( src->mPrimitiveTypes )
        {
            case aiPrimitiveType_POINT :        topology = EPrimitive::Point;           break;
            case aiPrimitiveType_LINE :         topology = EPrimitive::LineList;        break;
            case aiPrimitiveType_TRIANGLE :     topology = EPrimitive::TriangleList;    break;
            case aiPrimitiveType_POLYGON :
            case aiPrimitiveType_NGONEncodingFlag :
            case _aiPrimitiveType_Force32Bit :
            default :                           RETURN_ERR( "unsupported type" );
        }
        END_ENUM_CHECKS();

        dst = MakeRC<IntermMesh>( RVRef(vertices), RVRef(attribs), vert_stride, topology, RVRef(indices), EIndex::UInt );
        return true;
    }

/*
=================================================
    LoadLight
=================================================
*/
    ND_ static bool  LoadLight (const aiLight *src, OUT RC<IntermLight> &dst)
    {
        using ELightType = IntermLight::ELightType;

        IntermLight::Settings       light;

        light.position      = ConvertVec( src->mPosition );
        light.direction     = normalize( ConvertVec( src->mDirection ));
        light.upDirection   = normalize( ConvertVec( src->mUp ));

        light.attenuation   = float3{ src->mAttenuationConstant, src->mAttenuationLinear, src->mAttenuationQuadratic };
        light.diffuseColor  = ConvertVec( src->mColorDiffuse );
        light.specularColor = ConvertVec( src->mColorSpecular );
        light.ambientColor  = ConvertVec( src->mColorAmbient );
        light.areaSize      = ConvertVec( src->mSize );

        light.coneAngleInnerOuter = float2{ src->mAngleInnerCone, src->mAngleOuterCone };

        BEGIN_ENUM_CHECKS();
        switch ( src->mType )
        {
            case aiLightSource_DIRECTIONAL :    light.type = ELightType::Directional;   break;
            case aiLightSource_POINT :          light.type = ELightType::Point;         break;
            case aiLightSource_SPOT :           light.type = ELightType::Spot;          break;
            case aiLightSource_AMBIENT :        light.type = ELightType::Ambient;       break;
            case aiLightSource_AREA :           light.type = ELightType::Area;          break;
            case aiLightSource_UNDEFINED :
            case _aiLightSource_Force32Bit :
            default :                           DBG_WARNING( "unknown light type" );    break;
        }
        END_ENUM_CHECKS();

        dst = MakeRC<IntermLight>( RVRef(light) );
        return true;
    }

/*
=================================================
    LoadMaterials
=================================================
*/
    ND_ static bool  LoadMaterials (const aiScene *scene, OUT Array<RC<IntermMaterial>> &outMaterials)
    {
        outMaterials.resize( scene->mNumMaterials );  // throw

        for (uint i = 0; i < scene->mNumMaterials; ++i)
        {
            CHECK_ERR( LoadMaterial( scene->mMaterials[i], OUT outMaterials[i] ));
        }
        return true;
    }

/*
=================================================
    LoadMeshes
=================================================
*/
    ND_ static bool  LoadMeshes (const aiScene *scene, OUT Array<RC<IntermMesh>> &outMeshes, INOUT VertexAttribsSet_t &attribsCache)
    {
        outMeshes.resize( scene->mNumMeshes );  // throw

        for (uint i = 0; i < scene->mNumMeshes; ++i)
        {
            CHECK_ERR( LoadMesh( scene->mMeshes[i], OUT outMeshes[i], INOUT attribsCache ));
        }
        return true;
    }

/*
=================================================
    LoadLights
=================================================
*/
    ND_ static bool  LoadLights (const aiScene *scene, OUT Array<RC<IntermLight>> &outLights)
    {
        outLights.resize( scene->mNumLights );  // throw

        for (uint i = 0; i < scene->mNumLights; ++i)
        {
            CHECK_ERR( LoadLight( scene->mLights[i], OUT outLights[i] ));
        }
        return true;
    }

/*
=================================================
    RecursiveLoadHierarchy
=================================================
*/
    ND_ static bool  RecursiveLoadHierarchy (const aiScene *aiScene, const aiNode *node, const SceneData &scene, INOUT IntermScene::SceneNode &parent)
    {
        IntermScene::SceneNode  snode;
        snode.localTransform    = ConvertMatrix( node->mTransformation );
        snode.name              = node->mName.C_Str();

        for (uint i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh const*   ai_mesh = aiScene->mMeshes[ node->mMeshes[i] ];
            CHECK( not ai_mesh->HasBones() );

            IntermScene::ModelData  mnode;

            for (auto& level : mnode.levels)
            {
                level.mesh  = scene.meshes[ node->mMeshes[i] ];
                level.mtr   = scene.materials[ ai_mesh->mMaterialIndex ];
            }

            snode.data.push_back( mnode );
        }

        for (uint i = 0; i < node->mNumChildren; ++i)
        {
            CHECK_ERR( RecursiveLoadHierarchy( aiScene, node->mChildren[i], scene, INOUT snode ));
        }

        parent.nodes.push_back( snode );
        return true;
    }

/*
=================================================
    LoadHierarchy
=================================================
*/
    ND_ static bool  LoadHierarchy (const aiScene *aiScene, INOUT SceneData &scene)
    {
        scene.root.localTransform   = ConvertMatrix( aiScene->mRootNode->mTransformation );
        scene.root.name             = aiScene->mRootNode->mName.C_Str();

        CHECK_ERR( RecursiveLoadHierarchy( aiScene, aiScene->mRootNode, scene, INOUT scene.root ));
        return true;
    }

/*
=================================================
    SetupImporterAndGetSceneLoadFlags
=================================================
*/
    ND_ static uint  SetupImporterAndGetSceneLoadFlags (Assimp::Importer &importer, const IModelLoader::Config &config)
    {
        importer.SetPropertyInteger( AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,   config.maxTrianglesPerMesh );
        importer.SetPropertyInteger( AI_CONFIG_PP_SLM_VERTEX_LIMIT,     config.maxVerticesPerMesh );

        return  0
                | (config.calculateTBN      ? aiProcess_CalcTangentSpace : 0)
                | aiProcess_Triangulate
                | (config.smoothNormals     ? aiProcess_GenSmoothNormals : aiProcess_GenNormals)
                | aiProcess_RemoveRedundantMaterials
                | aiProcess_GenUVCoords 
                | aiProcess_TransformUVCoords
                | (config.splitLargeMeshes  ? aiProcess_SplitLargeMeshes : 0)
                | (not config.optimize      ? 0 :
                    (aiProcess_JoinIdenticalVertices | aiProcess_FindInstances |
                     aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality /*| aiProcess_OptimizeGraph*/));
    }

/*
=================================================
    LoadScene
=================================================
*/
    ND_ static bool  LoadScene (OUT IntermScene &outScene,
                                const aiScene*  scene)
    {
        SceneData   scene_data;
        CHECK_ERR( LoadMaterials( scene, OUT scene_data.materials ));
        CHECK_ERR( LoadMeshes( scene, OUT scene_data.meshes, INOUT scene_data.attribsCache ));
        //CHECK_ERR( LoadAnimations( scene, OUT scene_data.animations ));   // TODO
        CHECK_ERR( LoadLights( scene, OUT scene_data.lights ));
        CHECK_ERR( LoadHierarchy( scene, INOUT scene_data ));

        outScene.Set( scene_data.materials, scene_data.meshes,
                        RVRef(scene_data.lights), RVRef(scene_data.root) );
        ASSERT( outScene.IsValid() );
        return true;
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    LoadModel
=================================================
*/
    bool  AssimpLoader::LoadModel (OUT IntermScene  &outScene,
                                   RStream          &stream,
                                   const Config     &config,
                                   EModelFormat     format) __NE___
    {
        CHECK_ERR( stream.IsOpen() );

        Array<ubyte>    data;
        CATCH_ERR( data.resize( usize(stream.RemainingSize()) ));  // throw

        CHECK_ERR( stream.ReadSeq( OUT data.data(), ArraySizeOf(data) ) == ArraySizeOf(data) );

        try {
            AssimpInit();
            Assimp::Importer    importer;

            const auto      fmt_hint    = ModelFormatToExt( format );
            const auto      load_flags  = SetupImporterAndGetSceneLoadFlags( importer, config );

            const aiScene * scene       = importer.ReadFileFromMemory( data.data(), data.size(), load_flags, fmt_hint.data() );
            const char*     err_str     = importer.GetErrorString();

            CHECK_ERR( scene != null );
            Unused( err_str );

            return LoadScene( OUT outScene, scene );
        }
        catch (...) {
            return false;
        }
    }

/*
=================================================
    LoadModel
=================================================
*/
    bool  AssimpLoader::LoadModel (OUT IntermScene  &outScene,
                                   const Path       &scenePath,
                                   const Config     &config) __NE___
    {
        try {
            AssimpInit();
            Assimp::Importer    importer;

            const auto      path        = scenePath.string();
            const auto      load_flags  = SetupImporterAndGetSceneLoadFlags( importer, config );

            const aiScene * scene       = importer.ReadFile( path, load_flags );
            const char*     err_str     = importer.GetErrorString();

            CHECK_ERR( scene != null );
            Unused( err_str );

            return LoadScene( OUT outScene, scene );
        }
        catch (...) {
            return false;
        }
    }


} // AE::ResLoader

#endif // AE_ENABLE_ASSIMP
