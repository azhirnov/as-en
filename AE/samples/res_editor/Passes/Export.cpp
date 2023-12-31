// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Export.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

/*
=================================================
    _BeginExport
=================================================
*/
    bool  ExportImage::_BeginExport (uint frameId)
    {
        if_unlikely( _frameId != frameId )
        {
            // restart
            _frameId = frameId;

            VFS::FileName   fname;
            _currPath = _filePath;

            CHECK_ERR( GetVFS().CreateUniqueFile( OUT fname, INOUT _currPath, VFS::StorageName{"export"} ));

            Image::StoreOp  store_op;
            store_op.file = GetVFS().Open<AsyncWDataSource>( fname );
            CHECK_ERR( store_op.file );

            _temp = Image::CreateAndStore( *_src, {store_op}, "" );
            CHECK_ERR( _temp );
        }
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  ExportImage::Update (TransferCtx_t &, const UpdatePassData &pd) __Th___
    {
        if ( not _BeginExport( pd.frameId ))
            return true;  // complete (failed)

        if ( not _temp or _temp->GetStatus() != IResource::EUploadStatus::InProgress )
        {
            AE_LOGI( "Image exported to '"s << ToString(_currPath) << "'" );

            _temp = null;
            return true;  // complete
        }

        return false;  // in progress
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _BeginExport
=================================================
*/
    bool  ExportBuffer::_BeginExport (TransferCtx_t &ctx, uint frameId)
    {
        if_unlikely( _frameId != frameId )
        {
            // restart
            _frameId = frameId;

            VFS::FileName   fname;
            Path            path = _filePath;

            CHECK_ERR( GetVFS().CreateUniqueFile( OUT fname, INOUT path, VFS::StorageName{"export"} ));

            auto    stream  = GetVFS().Open<WStream>( fname );
            CHECK_ERR( stream );

            auto    read_op = ctx.ReadbackBuffer( _src->GetBufferId( ctx.GetFrameId() ), ReadbackBufferDesc{}.DynamicHeap() );
            CHECK_ERR( read_op );

            read_op.Then(
                [stream, self = GetRC<ExportBuffer>(), path] (const BufferMemView &memView)
                {
                    self->_parser( memView, *stream );

                    AE_LOGI( "Buffer exported to '"s << ToString(path) << "'" );

                    self->_complete.store( true );
                });

            read_op.Except( [self = GetRC<ExportBuffer>()] () { self->_complete.store( true ); });
        }
        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  ExportBuffer::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
    {
        if ( not _BeginExport( ctx, pd.frameId ))
            return true;  // complete (failed)

        return _complete.load();    // 'true'  - complete
                                    // 'false' - in progress
    }


} // AE::ResEditor
