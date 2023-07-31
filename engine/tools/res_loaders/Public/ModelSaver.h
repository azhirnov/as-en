// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes

    Assimp  - 
    glTF    - 
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
    class IntermScene;


    //
    // Model Saver interface
    //

    class IModelSaver
    {
    // types
    public:
        using Allocator_t   = SharedMem::Allocator_t;


    // methods
    public:
        virtual bool  SaveModel (const IntermScene  &scene,
                                 const WStream      &stream,
                                 EModelFormat       format)     __NE___ = 0;
    };


} // AE::ResLoader
