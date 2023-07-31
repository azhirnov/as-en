// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/PassGroup.h"
#include "res_editor/Resources/ResourceQueue.h"

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
        BEGIN_ENUM_CHECKS();
        switch ( _flags )
        {
            case EFlags::RunOnce :
            {
                if ( _count.Inc() > 1 ) return true;
                break;
            }
            case EFlags::OnRequest :
            {
                if ( not _requestUpdate.exchange( false )) return true;
                break;
            }
            case EFlags::RunOnce_AfterLoading :
            {
                if ( _resQueue.UploadFramesWithoutWork() > 10 ) {
                    if ( _count.Inc() < 1 ) return true;
                }
                break;
            }

            case EFlags::Unknown :
            case EFlags::_Count :
            default :
                DBG_WARNING( "unsupported flags" );
                break;
        }
        END_ENUM_CHECKS();


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
        BEGIN_ENUM_CHECKS();
        switch ( _flags )
        {
            case EFlags::RunOnce :
            {
                if ( _count.load() > 1 ) return true;
                break;
            }
            case EFlags::OnRequest :
            {
                if ( not _requestUpdate.load() ) return true;
                break;
            }
            case EFlags::RunOnce_AfterLoading :
            {
                if ( _count.load() > 1 ) return true;
                break;
            }

            case EFlags::Unknown :
            case EFlags::_Count :
            default :
                DBG_WARNING( "unsupported flags" );
                break;
        }
        END_ENUM_CHECKS();


        for (auto& pass : _passes)
        {
            if ( AllBits( pass->GetType(), EPassType::Update ))
                CHECK_ERR( pass->Update( ctx, pd ));
        }
        return true;
    }


} // AE::ResEditor
