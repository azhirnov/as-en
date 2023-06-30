// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/PassGroup.h"

namespace AE::ResEditor
{

/*
=================================================
    AddPass
=================================================
*/
    void  PassGroup::AddPass (RC<IPass> pass) __Th___
    {
        CHECK_THROW( pass );
        CHECK_THROW( not AnyBits( pass->GetType(), ~(EPassType::Sync | EPassType::Update) ));

        _passes.push_back( pass );
    }

/*
=================================================
    Execute
=================================================
*/
    bool  PassGroup::Execute (SyncPassData &pd) __NE___
    {
        if ( AllBits( _flags, EFlags::RunOnce ))
        {
            if ( _count.Inc() > 1 )
                return true;
        }

        if ( AllBits( _flags, EFlags::OnRequest ))
        {
            if ( not _requestUpdate.exchange( false ))
                return true;
        }

        for (auto& pass : _passes)
        {
            if ( AllBits( pass->GetType(), EPassType::Sync ))
                CHECK_ERR( pass->Execute( pd ));
        }
        return true;
    }

/*
=================================================
    Execute
=================================================
*/
    bool  PassGroup::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __NE___
    {
        const uint  idx = _count.load();
        if ( idx > 1 and AllBits( _flags, EFlags::RunOnce ))
            return true;

        if ( AllBits( _flags, EFlags::OnRequest ))
        {
            if ( not _requestUpdate.load() )
                return true;
        }

        for (auto& pass : _passes)
        {
            if ( AllBits( pass->GetType(), EPassType::Update ))
                CHECK_ERR( pass->Update( ctx, pd ));
        }
        return true;
    }


} // AE::ResEditor
