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

			VFS::FileName	fname;
			_currPath = _filePath;

			CHECK_ERR( GetVFS().CreateUniqueFile( OUT fname, INOUT _currPath, VFS::StorageName{"export"} ));

			Image::StoreOp	store_op;
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

		if ( not _temp )
			return true;  // already complete

		if ( _temp->GetStatus() != IResource::EUploadStatus::InProgress )
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

			_currPath = _filePath;
			CHECK_ERR( GetVFS().CreateUniqueFile( OUT _fname, INOUT _currPath, VFS::StorageName{"export"} ));

			_complete.store( false );

			if ( _parser )
			{
				auto	stream	= GetVFS().Open<WStream>( _fname );
				CHECK_ERR_MSG( stream,
					"Failed to open file for writing '"s << ToString(_currPath) << "'" );

				auto	read_res = ctx.ReadbackBuffer( _src->GetBufferId( ctx.GetFrameId() ), ReadbackBufferDesc{}.AnyHeap() );
				CHECK_MSG( read_res.IsCompleted(), "Buffer is too large" );

				read_res.Then(
					[stream, self = GetRC<ExportBuffer>()] (const BufferMemView &memView)
					{
						self->_parser( memView, *stream );  // throw
						stream->Flush();

						AE_LOGI( "Buffer exported to '"s << ToString(self->_currPath) << "'" );

						self->_complete.store( true );
					},
					"ExportBuffer",
					ETaskQueue::Background );

				read_res.Except(
					[self = GetRC<ExportBuffer>()] () { self->_complete.store( true ); });
			}
			else
			{
				Buffer::StoreOp	store_op;
				store_op.file = GetVFS().Open<AsyncWDataSource>( _fname );

				CHECK_ERR_MSG( store_op.file,
					"Failed to open async file for writing '"s << ToString(_currPath) << "'" );

				_temp = Buffer::CreateAndStore( *_src, RVRef(store_op), "" );
				CHECK_ERR( _temp );
			}
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

		if ( not _temp )
		{
			return _complete.load();	// 'true'  - complete
										// 'false' - in progress
		}

		if ( _temp->GetStatus() != IResource::EUploadStatus::InProgress )
		{
			AE_LOGI( "Buffer exported to '"s << ToString(_currPath) << "'" );

			_temp = null;
			return true;  // complete
		}

		return false;  // in progress
	}


} // AE::ResEditor
