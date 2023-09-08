// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:    yes (???)
*/

#pragma once

#include "res_loaders/Public/ModelLoader.h"

#ifdef AE_ENABLE_TINYGLTF

namespace AE::ResLoader
{

    //
    // glTF Model Loader
    //

    class glTFLoader final : public IModelLoader
    {
    // variables
    private:


    // methods
    public:
        glTFLoader ()                                   __NE___;
        ~glTFLoader ()                                  __NE___;

        bool  LoadModel (OUT IntermScene    &scene,
                         RStream            &stream,
                         const Config       &cfg,
                         EModelFormat       format)     __NE_OV;

        bool  LoadModel (OUT IntermScene    &scene,
                         const Path         &scenePath,
                         const Config       &cfg)       __NE_OV;
    };


} // AE::ResLoader

#endif // AE_ENABLE_TINYGLTF
