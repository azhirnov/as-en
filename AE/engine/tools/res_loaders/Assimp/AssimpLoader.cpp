// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    from docs:
    https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html#threading
    You can use the Asset-Importer-Library in a separate thread context.
    Just make sure that the resources used by the thread are not shared.
    At this moment, assimp will not make sure that it is safe over different thread contexts.

    https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html#thread-safety-using-assimp-concurrently-from-several-threads
    The library can be accessed by multiple threads simultaneously, as long as the following prerequisites are fulfilled:

    * Users of the C++-API should ensure that they use a dedicated #Assimp::Importer instance for each thread.
      Constructing instances of #Assimp::Importer is expensive, so it might be a good idea to let every thread maintain
      its own thread-local instance (which can be used to load as many files as necessary).
    * The C-API is thread safe.
    * When supplying custom IO logic, one must make sure the underlying implementation is thread-safe.
    * Custom log streams or logger replacements have to be thread-safe, too.

    Multiple concurrent imports may or may not be beneficial, however.
    For certain file formats in conjunction with little or no post processing IO times tend to be the performance bottleneck.
    Intense post processing together with 'slow' file formats like X or Collada might scale well with multiple concurrent imports.
*/

#ifdef AE_ENABLE_ASSIMP
# include "base/Defines/StdInclude.h"

# include "assimp/Importer.hpp"
# include "assimp/PostProcess.h"
# include "assimp/Scene.h"
# include "assimp/DefaultLogger.hpp"
# include "assimp/LogStream.hpp"

# include "res_loaders/Assimp/AssimpLoader.h"
# include "res_loaders/Intermediate/IntermScene.h"
# include "res_loaders/Intermediate/IntermVertexAttribs_Settings.h"

namespace AE::ResLoader
{
namespace {

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
    AssimpInit
=================================================
*/
    static void  AssimpInit ()
    {
        static bool isAssimpInit = false;

        if ( isAssimpInit )
            return;

        isAssimpInit = true;

        // Create Logger
        Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
        Assimp::DefaultLogger::create( "", severity, aiDefaultLogStream_STDOUT );
        //Assimp::DefaultLogger::create( "assimp_log.txt", severity, aiDefaultLogStream_FILE );
    }

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
    ND_ static float3  ConvertVec (const aiVector3D &src)
    {
        return float3{ src.x, src.y, src.z };
    }

    ND_ static float3  ConvertVec (const aiColor3D &src)
    {
        return float3{ src.r, src.g, src.b };
    }

/*
=================================================
    ConvertColor
=================================================
*/
    ND_ static RGBA32f  ConvertColor (const aiColor4D &src)
    {
        return RGBA32f{ src.r, src.g, src.b, src.a };
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
            case aiTextureMapMode_Wrap :    return EAddressMode::Repeat;
            case aiTextureMapMode_Mirror :  return EAddressMode::MirrorRepeat;
            case aiTextureMapMode_Clamp :   return EAddressMode::ClampToEdge;
            case aiTextureMapMode_Decal :   return EAddressMode::ClampToBorder; // TODO: add transparent border color
            #ifndef SWIG
            case _aiTextureMapMode_Force32Bit : break;
            #endif
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown wrap mode", EAddressMode::Repeat );
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
            #ifndef SWIG
            case _aiTextureMapping_Force32Bit : break;
            #endif
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown texture mapping" );
    };

/*
=================================================
    LoadMaterial
=================================================
*/
    ND_ static bool  LoadMaterial (const aiMaterial *src, OUT RC<IntermMaterial> &dst)
    {
        IntermMaterial::Settings    mtr;

        aiColor4D   color;
        float       fvalue      = 0.0f;
        uint        max_size    = 1;

        aiString    mtr_name;
        aiGetMaterialString( src, AI_MATKEY_NAME, OUT &mtr_name );
        mtr.name = mtr_name.C_Str();

        if ( aiGetMaterialColor( src, AI_MATKEY_COLOR_DIFFUSE, OUT &color ) == aiReturn_SUCCESS )
            mtr.albedo = ConvertColor( color );

        if ( aiGetMaterialColor( src, AI_MATKEY_COLOR_SPECULAR, OUT &color ) == aiReturn_SUCCESS )
            mtr.specular = ConvertColor( color );

        if ( aiGetMaterialColor( src, AI_MATKEY_COLOR_AMBIENT, OUT &color ) == aiReturn_SUCCESS )
            mtr.ambient = ConvertColor( color );

        if ( aiGetMaterialColor( src, AI_MATKEY_COLOR_EMISSIVE, OUT &color ) == aiReturn_SUCCESS )
            mtr.emissive = ConvertColor( color );

        if ( aiGetMaterialFloatArray( src, AI_MATKEY_OPACITY, OUT &fvalue, &max_size ) == aiReturn_SUCCESS and fvalue < 1.0f )
            mtr.opacity = fvalue;

        if ( aiGetMaterialFloatArray( src, AI_MATKEY_SHININESS, OUT &fvalue, &max_size ) == aiReturn_SUCCESS )
            mtr.shininess = fvalue;

        aiGetMaterialFloatArray( src, AI_MATKEY_SHININESS_STRENGTH, OUT &mtr.shininessStrength, &max_size );

        int     two_sided = 1;
        aiGetMaterialInteger( src, AI_MATKEY_TWOSIDED, OUT &two_sided );
        mtr.cullMode = two_sided ? ECullMode::None : ECullMode::Back;

        StaticArray< IntermMaterial::Parameter*, aiTextureType_UNKNOWN >    textures =
        {
            null, &mtr.albedo, &mtr.specular, &mtr.ambient, &mtr.emissive, &mtr.heightMap, &mtr.normalMap,
            &mtr.shininess, &mtr.opacity, &mtr.displacementMap, &mtr.lightMap, &mtr.reflectionMap
        };

        for (uint i = 0; i < aiTextureType_UNKNOWN; ++i)
        {
            auto tex = textures[i];
            if ( tex == null ) continue;

            aiString            tex_name;
            aiTextureMapMode    map_mode[3] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };
            aiTextureMapping    mapping     = aiTextureMapping_UV;
            uint                uv_index    = 0;

            if ( src->GetTexture( aiTextureType(i), 0, OUT &tex_name, OUT &mapping, OUT &uv_index, null, null, OUT map_mode ) == AI_SUCCESS )
            {
                IntermMaterial::MtrTexture  mtr_tex;
                mtr_tex.name            = tex_name.C_Str();
                mtr_tex.addressModeU    = ConvertWrapMode( map_mode[0] );
                mtr_tex.addressModeV    = ConvertWrapMode( map_mode[1] );
                mtr_tex.addressModeW    = ConvertWrapMode( map_mode[2] );
                mtr_tex.mapping         = ConvertMapping( mapping );
                mtr_tex.filter          = EFilter::Linear;
                mtr_tex.uvIndex         = uv_index;
                mtr_tex.image           = MakeRC<IntermImage>( StringView{tex_name.C_Str()} );

                *tex = RVRef(mtr_tex);
            }
        }

        dst = MakeRC<IntermMaterial>( RVRef(mtr) );
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

        vertices.resize( src->mNumVertices * usize(vert_stride) );
        indices.resize( src->mNumFaces * 3 * sizeof(uint) );

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
                CopyAttrib( float3{src->mVertices[i].x, src->mVertices[i].y, src->mVertices[i].z}, VertexAttributeName::Position, i );
            }
        }
        if ( src->HasNormals() )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( float3{src->mNormals[i].x, src->mNormals[i].y, src->mNormals[i].z}, VertexAttributeName::Normal, i );
            }
        }
        if ( src->mTangents != null )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( float3{src->mTangents[i].x, src->mTangents[i].y, src->mTangents[i].z}, VertexAttributeName::Tangent, i );
            }
        }
        if ( src->mBitangents != null )
        {
            for (uint i = 0; i < src->mNumVertices; ++i) {
                CopyAttrib( float3{src->mBitangents[i].x, src->mBitangents[i].y, src->mBitangents[i].z}, VertexAttributeName::BiTangent, i );
            }
        }

        for (uint t = 0; t < CountOf(VertexAttributeName::TextureUVs); ++t)
        {
            if ( not src->HasTextureCoords(t) )
                continue;

            if ( src->mNumUVComponents[t] == 2 )
            {
                for (uint i = 0; i < src->mNumVertices; ++i) {
                    CopyAttrib( float2{src->mTextureCoords[t][i].x, src->mTextureCoords[t][i].y}, VertexAttributeName::TextureUVs[t], i );
                }
            }
            else
            //if ( src->mNumUVComponents[t] == 3 )
            {
                for (uint i = 0; i < src->mNumVertices; ++i) {
                    CopyAttrib( float3{src->mTextureCoords[t][i].x, src->mTextureCoords[t][i].y, src->mTextureCoords[t][i].z}, VertexAttributeName::TextureUVs[t], i );
                }
            }
        }

        EPrimitive  topology = Default;
        switch ( src->mPrimitiveTypes )
        {
            case aiPrimitiveType_POINT :        topology = EPrimitive::Point;           break;
            case aiPrimitiveType_LINE :         topology = EPrimitive::LineList;        break;
            case aiPrimitiveType_TRIANGLE :     topology = EPrimitive::TriangleList;    break;
            default :                           RETURN_ERR( "unsupported type" );
        }

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

        light.coneAngleInnerOuter = float2{ src->mAngleInnerCone, src->mAngleOuterCone };

        switch ( src->mType )
        {
            case aiLightSource_DIRECTIONAL :    light.type = ELightType::Directional;   break;
            case aiLightSource_POINT :          light.type = ELightType::Point;         break;
            case aiLightSource_SPOT :           light.type = ELightType::Spot;          break;
            case aiLightSource_AMBIENT :        light.type = ELightType::Ambient;       break;
            case aiLightSource_AREA :           light.type = ELightType::Area;          break;
            default :                           ASSERT( !"unknown light type" );        break;
        }

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
        outMaterials.resize( scene->mNumMaterials );

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
        outMeshes.resize( scene->mNumMeshes );

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
        outLights.resize( scene->mNumLights );

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

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    AssimpLoader::AssimpLoader () __NE___
    {
        EXLOCK( _guard );

        _importerPtr.reset( new Assimp::Importer{} );
        AssimpInit();
    }

/*
=================================================
    destructor
=================================================
*/
    AssimpLoader::~AssimpLoader () __NE___
    {
        EXLOCK( _guard );
        _importerPtr.reset( null );
    }

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
        CATCH_ERR( data.resize( usize(stream.RemainingSize()) ));

        CHECK_ERR( stream.ReadSeq( OUT data.data(), ArraySizeOf(data) ) == ArraySizeOf(data) );


        EXLOCK( _guard );

        const auto  fmt_hint        = ModelFormatToExt( format );
        const uint  sceneLoadFlags  = 0
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

        _importerPtr->SetPropertyInteger( AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,  config.maxTrianglesPerMesh );
        _importerPtr->SetPropertyInteger( AI_CONFIG_PP_SLM_VERTEX_LIMIT,    config.maxVerticesPerMesh );

        const aiScene * scene   = _importerPtr->ReadFileFromMemory( data.data(), data.size(), sceneLoadFlags, fmt_hint.data() );
        const char*     err_str = _importerPtr->GetErrorString();

        CHECK_ERR( scene != null );
        Unused( err_str );

        SceneData   scene_data;
        CHECK_ERR( LoadMaterials( scene, OUT scene_data.materials ));
        CHECK_ERR( LoadMeshes( scene, OUT scene_data.meshes, INOUT scene_data.attribsCache ));
        //CHECK_ERR( LoadAnimations( scene, OUT scene_data.animations ));   // TODO
        CHECK_ERR( LoadLights( scene, OUT scene_data.lights ));
        CHECK_ERR( LoadHierarchy( scene, INOUT scene_data ));

        outScene.Set( scene_data.materials, scene_data.meshes,
                      RVRef(scene_data.lights), RVRef(scene_data.root) );
        return true;
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
        EXLOCK( _guard );

        const auto  path            = scenePath.string();
        const uint  sceneLoadFlags  = 0
                                    | (config.calculateTBN      ? aiProcess_CalcTangentSpace : 0)
                                    | aiProcess_Triangulate
                                    | (config.smoothNormals     ? aiProcess_GenSmoothNormals : aiProcess_GenNormals)
                                    | aiProcess_RemoveRedundantMaterials
                                //  | aiProcess_GenUVCoords 
                                //  | aiProcess_TransformUVCoords
                                    | (config.splitLargeMeshes  ? aiProcess_SplitLargeMeshes : 0)
                                    | (not config.optimize      ? 0 :
                                        (aiProcess_JoinIdenticalVertices | aiProcess_FindInstances |
                                         aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality /*| aiProcess_OptimizeGraph*/));

        _importerPtr->SetPropertyInteger( AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,  config.maxTrianglesPerMesh );
        _importerPtr->SetPropertyInteger( AI_CONFIG_PP_SLM_VERTEX_LIMIT,    config.maxVerticesPerMesh );

        const aiScene * scene   = _importerPtr->ReadFile( path, sceneLoadFlags );
        const char*     err_str = _importerPtr->GetErrorString();

        CHECK_ERR( scene != null );
        Unused( err_str );

        SceneData   scene_data;
        CHECK_ERR( LoadMaterials( scene, OUT scene_data.materials ));
        CHECK_ERR( LoadMeshes( scene, OUT scene_data.meshes, INOUT scene_data.attribsCache ));
        //CHECK_ERR( LoadAnimations( scene, OUT scene_data.animations ));   // TODO
        CHECK_ERR( LoadLights( scene, OUT scene_data.lights ));
        CHECK_ERR( LoadHierarchy( scene, INOUT scene_data ));

        outScene.Set( scene_data.materials, scene_data.meshes,
                      RVRef(scene_data.lights), RVRef(scene_data.root) );
        return true;
    }


} // AE::ResLoader

#endif // AE_ENABLE_ASSIMP
