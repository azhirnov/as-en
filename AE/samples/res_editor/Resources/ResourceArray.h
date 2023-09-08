// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Resource Array
    //

    class ResourceArray final
    {
    // types
    public:
        using AnyResource_t     = Union< NullUnion, RC<Buffer>, RC<RTScene>, RC<Image>, RC<VideoImage> >;
        using Resources_t       = Array<Tuple< UniformName, AnyResource_t, EResourceState >>;


    // variables
    public:
        Resources_t     _resources;


    // methods
    public:
        ResourceArray ();
        ~ResourceArray ();

        ND_ bool                Empty ()                                    const   { return _resources.empty(); }
        ND_ Resources_t const&  Get ()                                      const   { return _resources; }

            template <typename CtxType>
            void  SetStates (CtxType &ctx, EResourceState shaderStages)     const;

            void  GetResourcesToResize (INOUT Array<RC<IResource>> &)       const;

        ND_ bool  Bind (FrameUID, DescriptorUpdater &updater)               const;
    };


} // AE::ResEditor
