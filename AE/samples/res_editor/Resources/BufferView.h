// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Resources/Buffer.h"
#include "Resources/DataTransferQueue.h"

namespace AE::ResEditor
{

	//
	// Buffer View
	//

	class BufferView final : public IResource
	{
		friend class ScriptBufferView;

	// variables
	private:
		StrongAtom<BufferViewID>	_view;
		const RC<Buffer>			_baseBuffer;
		const EPixelFormat			_format			= Default;

		const String				_dbgName;


	// methods
	private:
		BufferView (RC<Buffer>				baseBuf,
					Strong<BufferViewID>	viewId,
					EPixelFormat			format,
					Renderer &				renderer,
					StringView				dbgName)				__NE___;

	public:
		~BufferView ()												__NE_OV;

		ND_ StringView				Name ()							C_NE___	{ return _dbgName; }
		ND_ BufferViewID			GetViewId ()					C_NE___	{ return _view.Get(); }

	// IResource //
		bool			Resize (TransferCtx_t &ctx)					__Th_OV	{ return _baseBuffer->Resize( ctx ); }
		bool			RequireResize ()							C_Th_OV	{ return _baseBuffer->RequireResize(); }
		EUploadStatus	Upload (TransferCtx_t &)					__Th_OV	{ return EUploadStatus::Canceled; }
		EUploadStatus	Readback (TransferCtx_t &)					__Th_OV	{ return EUploadStatus::Canceled; }

	private:
		friend class Buffer;
		void  _OnBufferResized (BufferID, Bytes)					__Th___;
	};


} // AE::ResEditor
