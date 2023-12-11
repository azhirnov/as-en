// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

    //
    // Pass Group
    //

    class PassGroup final : public IPass
    {
    // types
    public:
        enum class EFlags : uint
        {
            Unknown     = 0,
            RunOnce,
            OnRequest,
            RunOnce_AfterLoading,
            _Count,
        };

    private:
        using PassArr_t     = Array< RC<IPass> >;


    // variables
    private:
        PassArr_t           _passes;
        const EFlags        _flags          = Default;
        Atomic<uint>        _count          {0};
        Atomic<bool>        _requestUpdate  {false};
        DataTransferQueue&  _dtQueue;


    // methods
    public:
        PassGroup (EFlags flags, DataTransferQueue &dtq)                __NE___ : IPass{""}, _flags{flags}, _dtQueue{dtq} {}
        ~PassGroup ()                                                   {}

        void  AddPass (RC<IPass> pass)                                  __Th___;
        void  RequestUpdate ()                                          __NE___ { _requestUpdate.store( true ); }

    // IPass //
        EPassType   GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        bool        Execute (SyncPassData &)                            __Th_OV;
        bool        Update (TransferCtx_t &, const UpdatePassData &)    __Th_OV;
        void        GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;

    private:
        ND_ bool  _CanUpdate () const;
    };


} // AE::ResEditor
