// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MBuffer.h"

namespace AE::Graphics
{

    //
    // Metal Buffer view immutable data
    //

    class MBufferView final
    {
    // variables
    private:
        MetalImageRC            _bufferView;
        BufferViewDesc          _desc;
        Strong<BufferID>        _bufferId;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        MBufferView ()                                  __NE___ {}
        ~MBufferView ()                                 __NE___;

        ND_ bool  Create (MResourceManager &, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___;
            void  Destroy (MResourceManager &)          __NE___;

        ND_ MetalImage              Handle ()           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _bufferView; }
        ND_ BufferViewDesc const&   Description ()      C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc; }
        ND_ BufferID                Buffer ()           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _bufferId; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
