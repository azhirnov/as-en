// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Common.h"
#include "res_editor/Dynamic/DynamicDimension.h"
#include "res_editor/Dynamic/DynamicVec.h"
#include "res_editor/Dynamic/DynamicScalar.h"

namespace AE::ResEditor
{
	class Buffer;
	class Image;
	class RTGeometry;
	class RTScene;
	class VideoImage;
	class VideoImage2;

	class Renderer;
	class DataTransferQueue;



	//
	// Resource interface
	//
	class IResource : public EnableRC< IResource >
	{
	// types
	public:
		enum class EUploadStatus : uint
		{
			Completed,
			Canceled,		// or failed
			InProgress,
			NoMemory,
			Failed,
		};

		using TransferCtx_t = RG::DirectCtx::Transfer;


	// variables
	private:
		Renderer &					_renderer;
	protected:
		Atomic<EUploadStatus>		_uploadStatus	{EUploadStatus::Canceled};


	// interface
	public:

		// Returns 'true' if need to resize.
		// May be called multiple times, but all in single thread and
		// before 'Upload()', 'Readback()' and other usage.
		//
		ND_ virtual bool			RequireResize ()			C_Th___ = 0;

		// Returns 'false' if failed to resize.
		// Returns 'true' if resized or if not needed to resize.
		// Executed only if 'RequireResize()' returns 'true'.
		// Executed in parallel with 'Upload()', 'Readback()' and other usage.
		//
			virtual bool			Resize (TransferCtx_t &ctx)	__Th___	= 0;

		// GPU <-> CPU
		ND_ virtual EUploadStatus	GetStatus ()				C_NE___	{ return _uploadStatus.load(); }
		ND_ virtual EUploadStatus	Upload (TransferCtx_t &)	__Th___	= 0;	// Executed once per frame
		ND_ virtual EUploadStatus	Readback (TransferCtx_t &)	__Th___	= 0;	// Executed once per frame
			virtual void			Cancel ()					__NE___;


	// methods
	protected:
		explicit IResource (Renderer &r)						__NE___	: _renderer{r} {}

		ND_ Renderer&				_Renderer ()				const	{ return _renderer; }
		ND_ DataTransferQueue&		_DtTrQueue ()				const;

			void  _SetUploadStatus (EUploadStatus)				__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Cancel
=================================================
*/
	inline void  IResource::Cancel () __NE___
	{
		for (auto status = _uploadStatus.load();;)
		{
			if ( AnyEqual( status, EUploadStatus::Completed, EUploadStatus::Canceled ))
				break;

			if_likely( _uploadStatus.CAS( INOUT status, EUploadStatus::Canceled ))
				break;

			ThreadUtils::Pause();
		}
	}

/*
=================================================
	_SetUploadStatus
=================================================
*/
	inline void  IResource::_SetUploadStatus (EUploadStatus newStatus) __NE___
	{
		for (auto status = _uploadStatus.load();;)
		{
			if ( AnyEqual( status, EUploadStatus::Completed, EUploadStatus::Canceled, newStatus ))
				break;

			if_likely( _uploadStatus.CAS( INOUT status, newStatus ))
				break;

			ThreadUtils::Pause();
		}
	}


} // AE::ResEditor
