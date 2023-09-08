// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Resources/ResourceQueue.h"

namespace AE::ResEditor
{

    //
    // Buffer
    //

    class Buffer final : public IResource
    {
    // types
    public:
        enum class EBufferFlags : uint
        {
            Unknown         = 0,
            WithHistory     = 1 << 0,   // unique buffer for frame cycle with content history
        };

        struct LoadOp
        {
            VFS::FileName   filename;
            Array<ubyte>    data;
            bool            clear   = false;
        };

        using IDs_t = StaticArray< Strong<BufferID>, GraphicsConfig::MaxFrames >;

    private:
        struct LoadOp2 : LoadOp
        {
            RC<AsyncRDataSource>    file;
            AsyncDSRequest          request;
            BufferStream            stream;

            LoadOp2 () = default;
            LoadOp2 (LoadOp &&op) : LoadOp{RVRef(op)} {}

            ND_ bool  IsDefined ()  C_NE___;
        };

        using _IDs_t        = StaticArray< StrongAtom<BufferID>, GraphicsConfig::MaxFrames >;
        using _Address_t    = StaticArray< ulong, GraphicsConfig::MaxFrames >;


    // variables
    private:
        _IDs_t                      _ids;
        //StrongAtom<BufferViewID>  _view;
        _Address_t                  _address;

        const ShaderStructName      _typeName;
        const Bytes                 _elemSize;

        Synchronized< RWSpinLock,
            BufferDesc >            _bufDesc;
        RC<DynamicUInt>             _dynCount;

        LoadOp2                     _loadOp;

        const EBufferFlags          _flags;
        const String                _dbgName;

        Array<RC<Buffer>>           _refBuffers;


    // methods
    public:
        Buffer (IDs_t               ids,
                const BufferDesc &  desc,
                Bytes               elemSize,
                LoadOp              loadOp,
                ShaderStructName    typeName,
                Renderer &          renderer,
                RC<DynamicUInt>     dynCount,
                StringView          dbgName,
                EBufferFlags        flags,
                Array<RC<Buffer>>   refBuffers)                     __Th___;

        ~Buffer () override;

        ND_ BufferID                GetBufferId (uint fid)          const   { return _ids[ fid ].Get(); }
        ND_ BufferID                GetBufferId (FrameUID fid)      const   { return _ids[ fid.Index() ].Get(); }

        ND_ BufferDesc              GetBufferDesc ()                const   { return _bufDesc.Read(); }
        ND_ ShaderStructName        GetContentType ()               const   { return _typeName; }

        ND_ ulong                   GetDeviceAddress (uint fid)     const   { return _address[ fid ]; }
        ND_ ulong                   GetDeviceAddress (FrameUID fid) const   { return _address[ fid.Index() ]; }

        ND_ Bytes                   ElementSize ()                  const   { return _elemSize; }
        ND_ ulong                   ArraySize ()                    const   { return ulong(_bufDesc->size / _elemSize); }

        ND_ StringView              Name ()                         const   { return _dbgName; }
        ND_ bool                    HasHistory ()                   const   { return AllBits( _flags, EBufferFlags::WithHistory ); }

        ND_ ArrayView<RC<Buffer>>   GetRefBuffers ()                const   { return _refBuffers; }


    // IResource //
        bool            Resize (TransferCtx_t &ctx)                 __Th_OV;
        bool            RequireResize ()                            C_Th_OV;
        EUploadStatus   Upload (TransferCtx_t &)                    __Th_OV;
        EUploadStatus   Readback (TransferCtx_t &)                  __Th_OV;
    };

    AE_BIT_OPERATORS( Buffer::EBufferFlags );


} // AE::ResEditor
