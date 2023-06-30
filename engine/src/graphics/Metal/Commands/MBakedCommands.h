// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

    //
    // Metal Baked Commands
    //

    class MBakedCommands final
    {
    // types
    public:
        using Header    = Graphics::_hidden_::SoftwareCmdBufBase::Header;


    // variables
    protected:
        void *      _root   = null;


    // methods
    public:
        MBakedCommands ()                           __NE___
        {}

        explicit MBakedCommands (void* root)        __NE___ :
            _root{ root }
        {}

        MBakedCommands (MBakedCommands &&other)     __NE___ :
            _root{ other._root }
        {
            other._root = null;
        }

        void  Destroy ()                            __NE___
        {
            Graphics::_hidden_::SoftwareCmdBufBase::Deallocate( _root );
            _root = null;
        }

        bool  Execute (INOUT MCommandBuffer &cmdbuf) C_NE___;

        ND_ bool  IsValid ()                        C_NE___
        {
            return _root != null;
        }
    };


} // AE::Graphics

#endif // AE_ENABLE_METAL

