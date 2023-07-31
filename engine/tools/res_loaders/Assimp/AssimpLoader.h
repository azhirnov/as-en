// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:    yes (mutex)
*/

#pragma once

#include "res_loaders/Public/ModelLoader.h"

#ifdef AE_ENABLE_ASSIMP

namespace Assimp {
    class Importer;
}

namespace AE::ResLoader
{

    //
    // Assimp Model Loader
    //

    class AssimpLoader final : public IModelLoader
    {
    // variables
    private:
        Mutex                       _guard;
        Unique< Assimp::Importer >  _importerPtr;


    // methods
    public:
        AssimpLoader ()                                 __NE___;
        ~AssimpLoader ()                                __NE___;

        bool  LoadModel (OUT IntermScene    &scene,
                         RStream            &stream,
                         const Config       &cfg,
                         EModelFormat       format)     __NE_OV;

        bool  LoadModel (OUT IntermScene    &scene,
                         const Path         &scenePath,
                         const Config       &cfg)       __NE_OV;
    };


} // AE::ResLoader

#endif // AE_ENABLE_ASSIMP
