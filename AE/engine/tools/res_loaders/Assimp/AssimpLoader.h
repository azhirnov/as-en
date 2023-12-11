// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:    yes (locks in allocator)
*/

#pragma once

#ifdef AE_ENABLE_ASSIMP
# include "res_loaders/Public/ModelLoader.h"

namespace AE::ResLoader
{

    //
    // Assimp Model Loader
    //

    class AssimpLoader final : public IModelLoader
    {
    // methods
    public:
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
