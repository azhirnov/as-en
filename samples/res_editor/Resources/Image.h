// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Resources/ResourceQueue.h"

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResEditor
{
	using ResLoader::EImageFormat;


	//
	// Image
	//

	class Image final : public IImageResource
	{
	// types
	public:
		enum class ELoadOpFlags : uint
		{
			Unknown		= 0,
			GenMipmaps	= 1 << 0,
			_Last,
			All			= ((_Last - 1) << 1) - 1,
		};

		struct LoadOp
		{
			VFS::FileName	filename;
			EImageFormat	imgFormat	= Default;
			MipmapLevel		mipmap;
			ImageLayer		layer;
			ELoadOpFlags	flags		= Default;

			LoadOp () {}
		};

	private:
		using IntermImageRC = RC< ResLoader::IntermImage >;

		struct LoadOp2 : LoadOp
		{
			RC<AsyncRDataSource>	file;
			Promise<IntermImageRC>	loaded;
			ImageStream				stream;

			LoadOp2 (const LoadOp &other) : LoadOp{other} {}
		};


	// variables
	private:
		StrongAtom<ImageID>			_id;
		StrongAtom<ImageViewID>		_view;
		Atomic<bool>				_isDummy		{false};

		RC<DynamicDim>				_inDynSize;		// used to check if current image must be resized
		RC<DynamicDim>				_outDynSize;	// triggered when current image has been resized
		RC<Image>					_base;

		Synchronized< RWSpinLock,
			FlatHashSet<Image*>	>	_derived;
		
		Mutex						_loadOpGuard;
		Atomic<EUploadStatus>		_uploadStatus	{EUploadStatus::Canceled};
		Array<LoadOp2>				_loadOps;

		Synchronized< RWSpinLock,
			ImageDesc,
			ImageViewDesc >			_imageDesc;

		const String				_dbgName;


	// methods
	private:
		Image (Renderer&	renderer,
			   StringView	dbgName);

		void  _Remove (Image* derived);

	public:
		Image (Strong<ImageID>		id,
			   Strong<ImageViewID>	view,
			   ArrayView<LoadOp>	loadOps,
			   Renderer &			renderer,
			   bool					isDummy,
			   const ImageDesc &	desc,
			   RC<DynamicDim>		inDynSize,
			   RC<DynamicDim>		outDynSize,
			   StringView			dbgName)										__Th___;

		~Image () override;

			bool  Resize (TransferCtx_t &ctx)										__Th_OV;
		
		ND_ ImageID			GetImageId ()											C_NE_OV	{ return _id.Get(); }
		ND_ ImageViewID		GetViewId ()											C_NE_OV	{ return _view.Get(); }
		ND_ ImageDesc		GetImageDesc ()											C_NE___	{ return _imageDesc.Read<0>(); }
		ND_ ImageViewDesc	GetViewDesc ()											C_NE___	{ return _imageDesc.Read<1>(); }
		ND_ StringView		GetName ()												C_NE___	{ return _dbgName; }

		ND_ RC<Image>		CreateView (const ImageViewDesc &, StringView dbgName)	__NE___;


	// IResource //
		EUploadStatus	GetStatus ()												C_NE_OV	{ return _uploadStatus.load(); }
		EUploadStatus	Upload (TransferCtx_t &)									__Th_OV;
		EUploadStatus	Readback (TransferCtx_t &)									__Th_OV;
		void			Cancel ()													__Th_OV;


	private:
		ND_ bool  _OnResize (const ImageViewDesc &);

		ND_ bool  _CreateImage (const ResLoader::IntermImage &);
		ND_ bool  _ResizeImage (TransferCtx_t &ctx, const ImageDesc &, const ImageViewDesc &);
			void  _GenMipmaps (TransferCtx_t &ctx)									const;

		ND_ static IntermImageRC  _Load (const AsyncDSRequestResult &in, EImageFormat fileFormat);
	};


	AE_BIT_OPERATORS( Image::ELoadOpFlags );


} // AE::ResEditor
