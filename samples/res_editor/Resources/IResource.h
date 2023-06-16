// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"
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
	class Renderer;
	class ResourceQueue;



	//
	// Resource interface
	//

	class IResource : public EnableRC< IResource >
	{
	// types
	public:
		enum class EUploadStatus : uint
		{
			Complete,
			Canceled,		// or failed
			InProgress,
			NoMemory,
		};

		using TransferCtx_t = RG::DirectCtx::Transfer;


	// variables
	private:
		Renderer &		_renderer;


	// interface
	public:
			virtual bool			Resize (TransferCtx_t &ctx)	__Th___	= 0;

		// GPU <-> CPU
		ND_ virtual EUploadStatus	GetStatus ()				C_NE___	= 0;
		ND_ virtual EUploadStatus	Upload (TransferCtx_t &)	__Th___	= 0;
		ND_ virtual EUploadStatus	Readback (TransferCtx_t &)	__Th___	= 0;
		ND_ virtual void			Cancel ()							= 0;


	// 
	protected:
		explicit IResource (Renderer &r) : _renderer{r} {}

		ND_ Renderer&			_Renderer ()			const	{ return _renderer; }
		ND_ ResourceQueue&		_ResQueue ()			const;
		ND_ GfxMemAllocatorPtr	_GfxAllocator ()		const;
		ND_ GfxMemAllocatorPtr	_GfxDynamicAllocator ()	const;
	};


	//
	// Image Resource interface
	//

	class IImageResource : public IResource
	{
	// interface
	public:
		ND_ virtual ImageID			GetImageId ()		C_NE___ = 0;
		ND_ virtual ImageViewID		GetViewId ()		C_NE___ = 0;


	protected:
		explicit IImageResource (Renderer &r) : IResource{r} {}
	};


} // AE::ResEditor
