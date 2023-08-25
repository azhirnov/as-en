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
        PassArr_t       _passes;
        const EFlags    _flags          = Default;
        Atomic<uint>    _count          {0};
        Atomic<bool>    _requestUpdate  {false};
        ResourceQueue&  _resQueue;
        String          _dbgName;


    // methods
    public:
        PassGroup (EFlags flags, ResourceQueue &rq)                         : _flags{flags}, _resQueue{rq} {}
        ~PassGroup ()                                                       {}

        void  AddPass (RC<IPass> pass)                                      __Th___;
        void  RequestUpdate ()                                              __NE___ { _requestUpdate.store( true ); }

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;

    private:
        ND_ bool  _CanUpdate () const;
    };


} // AE::ResEditor
