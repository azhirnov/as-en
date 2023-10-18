// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Export.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

/*
=================================================
    Update
=================================================
*/
    bool  ExportImage::Update (TransferCtx_t &, const UpdatePassData &pd) __Th___
    {
        if_unlikely( _frameId != pd.frameId )
        {
            // restart
            _frameId = pd.frameId;

            VFS::FileName   fname;
            _currPath = _filePath;

            CHECK_THROW( GetVFS().CreateUniqueFile( VFS::StorageName{"export"}, OUT fname, INOUT _currPath ));

            Image::StoreOp  store_op;
            store_op.file = GetVFS().Open<AsyncWDataSource>( fname );

            _temp = Image::CreateAndStore( *_src, {store_op}, "" );
            CHECK_THROW( _temp );
        }

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
    Update
=================================================
*/
    bool  ExportBuffer::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __Th___
    {
        if_unlikely( _frameId != pd.frameId )
        {
            // restart
            _frameId = pd.frameId;

            VFS::FileName   fname;
            Path            path = _filePath;

            CHECK_THROW( GetVFS().CreateUniqueFile( VFS::StorageName{"export"}, OUT fname, INOUT path ));

            auto    stream  = GetVFS().Open<WStream>( fname );
            CHECK_THROW( stream );

            auto    read_op = ctx.ReadbackBuffer( _src->GetBufferId( ctx.GetFrameId() ), ReadbackBufferDesc{}.DynamicHeap() );
            CHECK_THROW( read_op );

            read_op.Then(   [stream, self = GetRC<ExportBuffer>(), path] (const BufferMemView &memView)
                            {
                                self->_parser( memView, *stream );

                                AE_LOGI( "Buffer exported to '"s << ToString(path) << "'" );

                                self->_complete.store( true );
                            });

            read_op.Except( [self = GetRC<ExportBuffer>()] () { self->_complete.store( true ); });
        }

        return _complete.load();    // 'true'  - complete
                                    // 'false' - in progress
    }


} // AE::ResEditor
