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
        struct LoadOp
        {
            VFS::FileName   filename;
            Array<ubyte>    data;
            bool            clear   = false;
        };

    private:
        struct LoadOp2 : LoadOp
        {
            RC<AsyncRDataSource>    file;
            AsyncDSRequest          request;
            BufferStream            stream;

            ND_ bool  IsDefined ()  C_NE___;
        };


    // variables
    private:
        StrongAtom<BufferID>        _id;
        //StrongAtom<BufferViewID>  _view;

        const ShaderStructName      _typeName;
        const Bytes                 _elemSize;

        Synchronized< RWSpinLock,
            BufferDesc >            _bufDesc;
        RC<DynamicUInt>             _dynCount;

        Mutex                       _loadOpGuard;
        Atomic<EUploadStatus>       _uploadStatus   {EUploadStatus::Canceled};
        LoadOp2                     _loadOp;

        Atomic<ulong>               _version        {0};

        const String                _dbgName;


    // methods
    public:
        Buffer (Strong<BufferID>    id,
                const BufferDesc &  desc,
                Bytes               elemSize,
                LoadOp              loadOp,
                ShaderStructName    typeName,
                Renderer &          renderer,
                RC<DynamicUInt>     dynCount,
                StringView          dbgName)            __Th___;

        ~Buffer () override;

        ND_ BufferID            GetBufferId ()          const   { return _id.Get(); }
        ND_ BufferDesc          GetBufferDesc ()        const   { return _bufDesc.Read(); }
        ND_ ShaderStructName    GetContentType ()       const   { return _typeName; }

        ND_ Bytes               ElementSize ()          const   { return _elemSize; }
        ND_ ulong               ArraySize ()            const   { return ulong(_bufDesc->size / _elemSize); }

        ND_ ulong               GetVersion ()           const   { return _version.load(); }
            void                UpdateVersion ()                { _version.fetch_add(1); }


    // IResource //
        bool            Resize (TransferCtx_t &ctx)     __Th_OV;
        EUploadStatus   GetStatus ()                    C_NE_OV { return _uploadStatus.load(); }
        EUploadStatus   Upload (TransferCtx_t &)        __Th_OV;
        EUploadStatus   Readback (TransferCtx_t &)      __Th_OV;
        void            Cancel ()                       __Th_OV;
    };


} // AE::ResEditor
