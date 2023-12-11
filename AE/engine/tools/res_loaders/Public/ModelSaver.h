// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes

    Assimp  - locks in allocator
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
    // methods
    public:
        virtual ~IModelSaver ()                                 __NE___ {}

        virtual bool  SaveModel (const IntermScene  &scene,
                                 const WStream      &stream,
                                 EModelFormat       format)     __NE___ = 0;
    };


} // AE::ResLoader
