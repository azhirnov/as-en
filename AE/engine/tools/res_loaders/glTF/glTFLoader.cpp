// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_TINYGLTF
# include "base/Defines/StdInclude.h"

# undef null

# ifdef AE_COMPILER_MSVC
#   pragma warning (push, 0)
#   pragma warning (disable: 4018)
#   pragma warning (disable: 4267)
#   pragma warning (disable: 4101)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-variable"
# endif

# define TINYGLTF_IMPLEMENTATION
# define TINYGLTF_NOEXCEPTION
# define JSON_NOEXCEPTION
# define TINYGLTF_USE_CPP14
# include "tiny_gltf.h"

#ifdef AE_COMPILER_MSVC
#   pragma warning (pop)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic pop
#endif

# define null   nullptr

# include "res_loaders/glTF/glTFLoader.h"

namespace AE::ResLoader
{

/*
=================================================
    constructor
=================================================
*/
    glTFLoader::glTFLoader () __NE___
    {}

/*
=================================================
    destructor
=================================================
*/
    glTFLoader::~glTFLoader () __NE___
    {}

/*
=================================================
    LoadModel
=================================================
*
    bool  glTFLoader::LoadModel (OUT IntermScene    &scene,
                                 RStream            &stream,
                                 const Config       &cfg,
                                 EModelFormat       format) __NE___
    {
        tinygltf::TinyGLTF  gltf_ctx;
        tinygltf::Model     gltf_model;
        String              error, warning;

        CHECK_ERR( gltf_ctx.LoadBinaryFromMemory( OUT &gltf_model, OUT &error, OUT &warning, );

        return true;
    }

/*
=================================================
    LoadModel
=================================================
*
    bool  glTFLoader::LoadModel (OUT IntermScene    &scene,
                                 const Path         &scenePath,
                                 const Config       &cfg) __NE___
    {

    //  CHECK_ERR( gltf_ctx.LoadBinaryFromFile( OUT &gltf_model, OUT &error, OUT &warning, filename.c_str() ));
        CHECK_ERR( gltf_ctx.LoadASCIIFromFile( OUT &gltf_model, OUT &error, OUT &warning, filename.c_str() ));

        return true;
    }
*/

} // AE::ResLoader

#endif // AE_ENABLE_TINYGLTF
