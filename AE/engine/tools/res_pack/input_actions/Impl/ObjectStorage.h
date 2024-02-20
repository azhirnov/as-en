// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "InputActionsBinding.pch.h"
#include "platform/Private/SerializableInputActions.h"

namespace AE::InputActions
{
    using namespace AE::Base;


    //
    // Object Storage
    //

    class ObjectStorage
    {
    // variables
    private:
        Unique< App::SerializableInputActions >     _android;
        Unique< App::SerializableInputActions >     _glfw;
        Unique< App::SerializableInputActions >     _winapi;
        Unique< App::SerializableInputActions >     _openvr;
        Unique< App::SerializableInputActions >     _openxr;

        App::SerializableInputActions::Reflection   _refl;


    // methods
    public:
        ObjectStorage ();
        ~ObjectStorage ();

        ND_ bool  AddBindings (const Scripting::ScriptEnginePtr &se,
                                const Path &path, ArrayView<Path> includeDirs);

        ND_ bool  Save (const Path &filename)                                           const;
        ND_ bool  SaveCppTypes (const Path &filename)                                   const;

            static void  Bind (const Scripting::ScriptEnginePtr &se)                    __Th___;

        ND_ static Ptr<ObjectStorage>  Instance ();
            static void  SetInstance (ObjectStorage* inst);
    };


} // AE::InputActions
