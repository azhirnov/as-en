// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes

    Assimp  - internal mutex for instance
    glTF    - 
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
    class IntermScene;


    //
    // Model Loader interface
    //

    class IModelLoader
    {
    // types
    public:
        using Allocator_t   = SharedMem::Allocator_t;

        struct Config
        {
            uint    maxTrianglesPerMesh = UMax;
            uint    maxVerticesPerMesh  = UMax;

            bool    calculateTBN        = false;
            bool    smoothNormals       = false;
            bool    splitLargeMeshes    = false;
            bool    optimize            = false;
        };


    // methods
    public:
        virtual bool  LoadModel (OUT IntermScene    &scene,
                                 RStream            &stream,
                                 const Config       &cfg,
                                 EModelFormat       format  = Default)  __NE___ = 0;

        virtual bool  LoadModel (OUT IntermScene    &scene,
                                 const Path         &scenePath,
                                 const Config       &cfg)               __NE___ = 0;
    };


} // AE::ResLoader
