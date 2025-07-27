// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Possible problems:

	* '_id' and '_view' are atomics but some processes may happens when '_id' has new image and '_view' has previous view.
	* '_id' must be updated before '_view'.
	* Add imageId to the RG state tracker before updating '_id' and '_view'.
*/

#include "Resources/Image.h"
#include "Core/RenderGraph.h"
#include "Passes/Renderer.h"

#include "res_pack/asset_packer/Packer/ImagePacker.h"

#include "threading/DataSource/FileAsyncDataSource.h"

namespace AE::ResEditor
{
namespace {
#	define AE_BUILD_ASSET_PACKER
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
#	undef AE_BUILD_ASSET_PACKER
}

/*
=================================================
	constructor
=================================================
*/
	Image::Image (const ImageDesc &		desc,
			      const ImageViewDesc&	viewDesc,
				  Renderer&				renderer,
				  StringView			dbgName) :
		IResource{ renderer },
		_requiredImageDesc{ desc },
		_requiredViewDesc{ viewDesc },
		_dbgName{ dbgName }
	{}

	Image::Image (Strong<ImageID>		id,
				  Strong<ImageViewID>	view,
				  ArrayView<LoadOp>		loadOps,
				  Renderer &			renderer,
				  bool					isDummy,
				  const ImageDesc &		desc,
				  const ImageViewDesc&	viewDesc,
				  RC<DynamicDim>		inDynSize,
				  RC<DynamicDim>		outDynSize,
				  EImageFlags			flags,
				  StringView			dbgName) :
		IResource{ renderer },
		_id{ RVRef(id) },
		_view{ RVRef(view) },
		_isDummy{ isDummy },
		_flags{ flags },
		_inDynSize{ RVRef(inDynSize) },
		_outDynSize{ RVRef(outDynSize) },
		_loadOps{ loadOps.begin(), loadOps.end() },
		_requiredImageDesc{ desc },
		_requiredViewDesc{ viewDesc },
		_dbgName{ dbgName }
	{
		if ( isDummy )
		{
			CHECK_THROW( not _inDynSize );

			_uploadStatus.store( EUploadStatus::InProgress );
		}
		else
		{
			_uploadStatus.store( EUploadStatus::Completed );

			CHECK( RenderGraph().GetStateTracker().AddResource( _id.Get() ));
		}

		if ( not _loadOps.empty() )
		{
			_uploadStatus.store( EUploadStatus::InProgress );

			for (auto& op : _loadOps)
			{
				CHECK_THROW( GetVFS().Open( OUT op.file, op.filename ));

				auto	req = op.file->ReadRemaining( 0_b );	// TODO: read by blocks
				CHECK_THROW( req );

				op.loaded = CreateInlineRev(
					req, op.imgFormat,
					[] (AsyncDSRequest req, EImageFormat fmt)
						-> InlinePromise< IntermImageRC, ETaskQueue::Background >
					{
						auto  in = co_await req;
						co_return _Load( in, fmt );
					});
			}

			_DtTrQueue().EnqueueForUpload( GetRC() );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	Image::~Image () __NE___
	{
		Cancel();

		{
			auto	view	= _view.Release();
			auto	id		= _id.Release();
			RenderGraph().GetStateTracker().ReleaseResources( view, id );
		}

		if ( _base )
			_base->_Remove( this );
	}
	
/*
=================================================
	GetImageDesc / GetViewDesc
=================================================
*/
	ImageDesc  Image::GetImageDesc () C_NE___
	{
		return GraphicsScheduler().GetResourceManager().GetDescription( _id.Get() );
	}

	ImageViewDesc  Image::GetViewDesc () C_NE___
	{
		return GraphicsScheduler().GetResourceManager().GetDescription( _view.Get() );
	}

/*
=================================================
	CreateDummy2D
=================================================
*/
	RC<Image>  Image::CreateDummy2D (Renderer &renderer, StringView dbgName) __Th___
	{
		ImageDesc	desc;
		desc.imageDim	= EImageDim_2D;

		auto&		res_mngr	 = GraphicsScheduler().GetResourceManager();
		auto		img_and_view = renderer.GetDummyImage( desc );
		CHECK_THROW( img_and_view );

		desc			= res_mngr.GetDescription( img_and_view.image );
		desc.usage		= EImageUsage::Transfer | EImageUsage::Sampled;
		desc.options	= EImageOpt::BlitSrc | EImageOpt::BlitDst;

		RC<Image>	result	{new Image{ desc, ImageViewDesc{}, renderer, dbgName }};

		result->_isDummy.store( true );

		Unused( result->_id.Attach( RVRef(img_and_view.image) ));
		Unused( result->_view.Attach( RVRef(img_and_view.view) ));

		return result;
	}

/*
=================================================
	CreateAndLoad
=================================================
*/
	RC<Image>  Image::CreateAndLoad (Renderer &renderer, const Path &inPath, StringView dbgName, ELoadOpFlags flags, ArrayView<Path> texSearchDirs) __Th___
	{
		Path	path = inPath;

		if ( not FileSystem::IsFile( path ))
		{
			for (auto& dir : texSearchDirs)
			{
				path = dir / inPath;
				if ( FileSystem::IsFile( path ))
					break;
			}
		}
		CHECK_THROW_MSG( FileSystem::IsFile( path ),
			"Image file '"s << ToString(path) << "' is not exists" );

		RC<Image>	result = CreateDummy2D( renderer, (dbgName.empty() ? StringView{path.stem().string()} : dbgName) );

		// load from filesystem instead of VFS
		{
			result->_uploadStatus.store( EUploadStatus::InProgress );

			auto&	load_op		= result->_loadOps.emplace_back();

			load_op.flags		= flags;
			load_op.file		= MakeRC< Threading::FileAsyncRDataSource >( path );
			load_op.imgFormat	= ResLoader::PathToImageFileFormat( path );
			CHECK_THROW( load_op.file->IsOpen() );

			auto	req			= load_op.file->ReadRemaining( 0_b );	// TODO: read by blocks
			CHECK_THROW( req );

			load_op.loaded	= CreateInlineRev(
				req, load_op.imgFormat,
				[] (AsyncDSRequest req, EImageFormat fmt) -> InlinePromise< IntermImageRC, ETaskQueue::Background >
				{
					auto res = co_await req;
					co_return _Load( res, fmt );
				});

			result->_DtTrQueue().EnqueueForUpload( result );
		}

		return result;
	}

/*
=================================================
	CreateAndLoad
=================================================
*/
	RC<Image>  Image::CreateAndLoad (Renderer &renderer, IntermImageRC imageData, StringView dbgName, ELoadOpFlags flags, ArrayView<Path> texSearchDirs) __Th___
	{
		CHECK_THROW( imageData );

		if ( imageData->IsEmpty() )
			return CreateAndLoad( renderer, imageData->GetPath(), dbgName, flags, texSearchDirs );

		RC<Image>	result = CreateDummy2D( renderer, dbgName );

		// upload from RAM
		{
			result->_uploadStatus.store( EUploadStatus::InProgress );

			auto&	load_op = result->_loadOps.emplace_back();

			load_op.flags	= flags;
			load_op.loaded	= DeferResult< IntermImageRC >( RVRef(imageData) );

			result->_DtTrQueue().EnqueueForUpload( result );
		}

		return result;
	}

/*
=================================================
	RequireResize
=================================================
*/
	bool  Image::RequireResize () C_Th___
	{
		if ( _base )
			return _base->RequireResize();

		if ( not _inDynSize )
			return false;

		ImageDesc	desc = GetImageDesc();

		if ( not _inDynSize->IsChanged_NonZero( INOUT desc.dimension ))
			return false;

		return true;
	}

/*
=================================================
	Resize
=================================================
*/
	bool  Image::Resize (TransferCtx_t &ctx)
	{
		if ( _base )
			return _base->Resize( ctx );

		if ( not _inDynSize )
			return false;

		ImageDesc	desc = GetImageDesc();

		if ( not _inDynSize->IsChanged_NonZero( INOUT desc.dimension ))
			return false;

		ImageViewDesc	view = GetViewDesc();

		if ( AllBits( _flags, EImageFlags::AllMipmaps ))
		{
			desc.mipLevels		= MipmapLevel{UMax};
			view.mipmapCount	= UMax;
			desc.Validate();
			view.Validate( desc );
		}

		return _ResizeImage( ctx, desc, view );
	}

/*
=================================================
	_ResizeImage
=================================================
*/
	bool  Image::_ResizeImage (TransferCtx_t &ctx, const ImageDesc &imageDesc, const ImageViewDesc &viewDesc)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		{
			CHECK_ERR_MSG( res_mngr.IsSupported( imageDesc ),
				"Image '"s << _dbgName << "' description is not supported by GPU device" );

			auto	image = res_mngr.CreateImage( imageDesc, _dbgName, _Renderer().ChooseAllocator( True{"dynamic"}, imageDesc ));
			CHECK_ERR( image );

			CHECK( RenderGraph().GetStateTracker().AddResource( image.Get(),
														EResourceState::_InvalidState,	// current is not used
														EResourceState::General,		// default
														ctx.GetCommandBatchRC() ));
			ctx.ResourceState( image, EResourceState::Invalidate );

			auto	old_img	= _id.Attach( RVRef(image) );
			res_mngr.ReleaseResource( old_img );	// release previous resource
		}{
			auto	view = res_mngr.CreateImageView( viewDesc, _id.Get(), _dbgName );
			CHECK_ERR( view );

			auto	old_view = _view.Attach( RVRef(view) );
			res_mngr.ReleaseResource( old_view );	// release previous resource
		}

		if ( _outDynSize )
		{
			ASSERT( imageDesc.imageDim == _outDynSize->NumDimensions() );
			_outDynSize->Resize( imageDesc.Dimension() );
		}

		auto	derived = _derived.ReadLock();

		for (auto* img : *derived) {
			CHECK( img->_UpdateView() );
		}

		return true;
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  Image::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		ASSERT( not _loadOps.empty() );

		bool	all_complete	= true;
		bool	failed			= false;

		for (auto& op : _loadOps)
		{
			if ( op.IsUploadComplete() )
				continue;

			WithResult(
				op.loaded,
				[this, &ctx, &op, &failed] (const IntermImageRC &imageData)
				{
					if ( not imageData )
					{
						failed = true;
						return;
					}

					if_unlikely( _isDummy.load() )
					{
						CHECK_THROW( _CreateImage( *imageData, op.mipmap, op.layer, AllBits( op.flags, ELoadOpFlags::GenMipmaps ), ctx ));
					}

					for (;;)
					{
						ImageMemView	src_mem = imageData->ToView( op.curMipmap, op.curLayer );
						ImageMemView	dst_mem;

						if_unlikely( not op.stream.IsInitialized() )
						{
							auto&	fmt_info	= EPixelFormat_GetInfo( src_mem.Format() );

							UploadImageDesc		upload;
							upload.imageDim		= ImageDim_t{ImageUtils::MipmapDimension( imageData->Dimension(), op.curMipmap.Get(), fmt_info.TexBlockDim() )};
							upload.arrayLayer	= op.layer + op.curLayer;
							upload.mipLevel		= op.mipmap + op.curMipmap;
							upload.heapType		= EStagingHeapType::Dynamic;
							upload.aspectMask	= src_mem.Aspect();
							op.stream			= ImageStream{ _id, upload };
						}

						ASSERT( op.stream.ImageId() == _id );
						ctx.UploadImage( op.stream, OUT dst_mem );

						if ( dst_mem.Empty() )
							break;

						CHECK( dst_mem.CopyFrom( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));

						if ( op.stream.IsCompleted() )
						{
							// invalidate
							op.stream = Default;

							if ( ++op.curMipmap < MipmapLevel{imageData->MipLevels()} )
								continue;

							if ( ++op.curLayer < ImageLayer{imageData->ArrayLayers()} )
							{
								op.curMipmap = MipmapLevel{};
								continue;
							}

							op.complete = true;
							break;
						}
					}
				});

			all_complete &= op.IsUploadComplete();
		}

		if ( failed )
		{
			Reconstruct( _loadOps );
			_SetUploadStatus( EUploadStatus::Canceled );
		}

		if ( all_complete )
		{
			_GenMipmaps( ctx );

			Reconstruct( _loadOps );
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}

/*
=================================================
	_GenMipmaps
=================================================
*/
	void  Image::_GenMipmaps (TransferCtx_t &ctx) const
	{
		const auto	desc		= GetImageDesc();
		uint		num_layers	= 0;
		bool		gen_mipmaps	= false;

		for (auto& op : _loadOps) {
			gen_mipmaps	|= AllBits( op.flags, ELoadOpFlags::GenMipmaps );
			num_layers	+= uint(AllBits( op.flags, ELoadOpFlags::GenMipmaps ) and op.mipmap == 0_mipmap);
		}

		if ( not gen_mipmaps )
			return;

		if ( desc.mipLevels.Get() <= 1 )
		{
			AE_LOGW( "Image '"s << _dbgName << "' has no mipmap levels to generate them" );
			return;
		}

		if ( num_layers == desc.arrayLayers.Get() and
			 num_layers == _loadOps.size() )
		{
			ctx.GenerateMipmaps( _id );
			return;
		}

		// RenderGraph doesn't track specific image layers, so we need to put explicit barriers
		ctx.ResourceState( _id, EResourceState::BlitSrc );
		ctx.CommitBarriers();

		for (auto& op : _loadOps)
		{
			ASSERT( op.IsUploadComplete() );

			if ( AllBits( op.flags, ELoadOpFlags::GenMipmaps ))
			{
				ImageSubresourceRange	range;
				range.aspectMask	= EImageAspect::Color;
				range.baseLayer		= op.layer;
				range.layerCount	= 1;
				range.baseMipLevel	= op.mipmap;
				range.mipmapCount	= MipmapCount_t(desc.mipLevels.Get() - op.mipmap.Get());

				if ( range.mipmapCount > 1 )
					ctx.GetBaseContext().GenerateMipmaps( _id, {range}, EResourceState::BlitSrc );
			}
		}
	}

/*
=================================================
	CreateAndStore
=================================================
*/
	RC<Image>  Image::CreateAndStore (const Image			&src,
									  ArrayView<StoreOp>	storeOps,
									  StringView			dbgName) __Th___
	{
		CHECK_ERR( not src._isDummy.load() );
		CHECK_ERR( not storeOps.empty() );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		RC<Image>	result		{new Image{ ImageDesc{}, ImageViewDesc{}, src._Renderer(), dbgName }};

		Unused( result->_id.Attach( res_mngr.AcquireResource( src._id.Get() )));
		Unused( result->_view.Attach( res_mngr.AcquireResource( src._view.Get() )));

		result->_uploadStatus.store( EUploadStatus::InProgress );

		for (auto& op : storeOps) {
			result->_storeOps.emplace_back( StoreOp2{ op });
		}
		
		const auto	view_desc = res_mngr.GetDescription( result->_view.Get() );

		for (auto& op : result->_storeOps)
		{
			CHECK_ERR( op.file and op.file->IsOpen() );

			using Header = AssetPacker::ImagePacker::FileHeader;

			auto	mem = op.file->Alloc( SizeAndAlignOf<Header> );
			CHECK_ERR( mem );

			auto&	hdr		= PlacementNew<Header>( mem->Data() )->imageHeader;
			hdr.dimension	= view_desc.dimension;
			hdr.arrayLayers	= view_desc.layerCount;
			hdr.mipmaps		= view_desc.mipmapCount;
			hdr.viewType	= view_desc.viewType;
			hdr.format		= view_desc.format;

			CHECK_ERR( ImagePacker_IsValid( hdr ));

			auto	req = op.file->WriteBlock( 0_b, SizeOf<Header>, RVRef(mem) );
			CHECK_ERR( req );
		}

		result->_DtTrQueue().EnqueueForReadback( result );
		return result;
	}

/*
=================================================
	Readback
=================================================
*/
	IResource::EUploadStatus  Image::Readback (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		ASSERT( not _storeOps.empty() );

		bool	all_complete	= true;

		for (auto& op : _storeOps)
		{
			if ( op.IsCompleted() )
				continue;

			if_unlikely( not op.stream.IsInitialized() )
			{
				const auto	img_desc	= GetImageDesc();
				const auto	view_desc	= GetViewDesc();
				const auto&	fmt_info	= EPixelFormat_GetInfo( view_desc.format );
				const auto	mipmap		= view_desc.baseMipmap + op.curMipmap;
				const auto	layer		= view_desc.baseLayer + op.curLayer;

				ReadbackImageDesc	read;
				read.imageDim	= ImageDim_t{ImageUtils::MipmapDimension( img_desc.Dimension(), mipmap.Get(), fmt_info.TexBlockDim() )};
				read.arrayLayer	= layer;
				read.mipLevel	= mipmap;
				read.heapType	= EStagingHeapType::Dynamic;
				read.aspectMask	= EImageAspect::Color;
				op.stream		= ImageStream{ _id, read };
			}

			ctx.ReadbackImage( INOUT op.stream )
				.Then(	GetRC<Image>(), op.curLayer, op.curMipmap, op.file,
						[] (Promise<ImageMemView> readOp, RC<Image> self, ImageLayer cur_layer, MipmapLevel cur_mipmap, RC<AsyncWDataSource> file)
							-> InlineCoro< ETaskQueue::PerFrame >
						{
							ImageMemView	mem_view	= co_await readOp;

							const auto		img_desc	= self->GetImageDesc();
							const auto		view_desc	= self->GetViewDesc();
							const auto		mipmap		= view_desc.baseMipmap + cur_mipmap;
							const auto		layer		= view_desc.baseLayer + cur_layer;
							const auto&		fmt_info	= EPixelFormat_GetInfo( view_desc.format );
							const auto		mip_dim		= ImageDim_t{ImageUtils::MipmapDimension( img_desc.Dimension(), mipmap.Get(), fmt_info.TexBlockDim() )};

							AssetPacker::ImagePacker::Header	header;
							header.dimension	= view_desc.dimension;
							header.arrayLayers	= view_desc.layerCount;
							header.mipmaps		= view_desc.mipmapCount;
							header.viewType		= view_desc.viewType;
							header.format		= view_desc.format;

							ImageDim_t	dim;
							Bytes		off, slice_size;
							Bytes32u	row_size;
							ImagePacker_GetOffset( header, layer, mipmap, mem_view.OffsetRef(),
												   OUT dim, OUT off, OUT row_size, OUT slice_size );

							CHECK( All( dim == mip_dim ));
							CHECK_Eq( row_size, mem_view.RowPitch() );

							if ( All( uint2{mem_view.Offset()} == uint2{0} ))
								CHECK_Eq( slice_size, mem_view.SlicePitch() );

							auto	mem = file->Alloc( mem_view.ContentSize() );
							CHECK_THROW( mem );

							CHECK( mem_view.CopyTo( OUT mem->Data(), mem_view.ContentSize() ));

							Unused( file->WriteBlock( off + SizeOf<AssetPacker::ImagePacker::FileHeader>, mem_view.ContentSize(), RVRef(mem) ));
						});

			if ( op.stream.IsCompleted() )
			{
				const auto	view_desc = GetViewDesc();

				// invalidate
				op.stream = Default;

				if ( (++op.curLayer).Get() < view_desc.layerCount )
				{
					all_complete	= false;
					continue;
				}

				if ( (++op.curMipmap).Get() < view_desc.mipmapCount )
				{
					all_complete	= false;
					op.curLayer		= ImageLayer{};
					continue;
				}

				op.complete = true;
			}

			all_complete &= op.IsCompleted();
		}

		if ( all_complete )
		{
			Reconstruct( _storeOps );
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}

/*
=================================================
	Cancel
=================================================
*/
	void  Image::Cancel () __NE___
	{
		IResource::Cancel();

		for (auto& op : _loadOps)
		{
			if ( op.file )		op.file->CancelAllRequests();
			if ( op.loaded )	Scheduler().Cancel( AsyncTask{op.loaded} );
		}
		Reconstruct( _loadOps );

		for (auto& op : _storeOps)
		{
			if ( op.file )		CHECK( not op.file->CancelAllRequests() );	// all write requests must complete
		}
		Reconstruct( _storeOps );
	}

/*
=================================================
	CreateView
=================================================
*/
	RC<Image>  Image::CreateView (const ImageViewDesc &viewDesc, StringView dbgName) __NE___
	{
		//CHECK_ERR( _uploadStatus == EUploadStatus::Completed );
		//CHECK_ERR( not _base );

		if ( _base )
		{
			ImageViewDesc	new_view = viewDesc;

			new_view.baseMipmap = new_view.baseMipmap + _requiredViewDesc.baseMipmap;
			new_view.baseLayer	= new_view.baseLayer + _requiredViewDesc.baseLayer;

			return _base->CreateView( new_view, dbgName );
		}

		RC<Image>	result	{new Image{ ImageDesc{}, viewDesc, _Renderer(), dbgName }};

		result->_flags		= _flags;
		result->_inDynSize	= _inDynSize;
		result->_base		= GetRC<Image>();
		result->_uploadStatus.store( EUploadStatus::Completed );

		_derived->insert( result.get() );

		CHECK_ERR( result->_UpdateView() );
		return result;
	}

/*
=================================================
	_UpdateView
=================================================
*/
	bool  Image::_UpdateView () __NE___
	{
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	base		= *_base;

		auto	image		= res_mngr.AcquireResource( base._id.Get() );
		CHECK_ERR( image );

		auto	old_img		= _id.Attach( RVRef(image) );
		res_mngr.ReleaseResource( old_img );

		auto	view		= res_mngr.CreateImageView( _requiredViewDesc, _id.Get(), _dbgName );
		CHECK_ERR( view );

		auto	old_view	= _view.Attach( RVRef(view) );
		res_mngr.ReleaseResource( old_view );

		return true;
	}

/*
=================================================
	_Remove
=================================================
*/
	void  Image::_Remove (Image* derived)
	{
		_derived->erase( derived );
	}

/*
=================================================
	_CreateImage
=================================================
*/
	template <typename CtxType>
	bool  Image::_CreateImage (const ResLoader::IntermImage &intermImg, MipmapLevel baseMipmap, ImageLayer baseLayer, bool allMipmaps, CtxType &ctx)
	{
		ASSERT( _isDummy.load() );
		ASSERT( not intermImg.IsEmpty() );

		auto&		res_mngr = GraphicsScheduler().GetResourceManager();

		const auto	Create	 = [&] () -> bool
		{{
			ImageDesc		desc		= _requiredImageDesc;
			ImageViewDesc	view_desc	= _requiredViewDesc;
			CHECK_ERR( CompareImageTypes( desc, intermImg ));

			desc.dimension		= CheckCast<ImageDim_t>( intermImg.Dimension() << baseMipmap.Get() );
			desc.arrayLayers	= ImageLayer{ intermImg.ArrayLayers() + baseLayer.Get() };
			desc.mipLevels		= MipmapLevel{ intermImg.MipLevels() + baseMipmap.Get() };
			desc.imageDim		= intermImg.GetImageDim();
			// keep: desc.options
			// keep: desc.usage
			desc.format			= intermImg.PixelFormat();
			// keep: desc.samples

			if ( allMipmaps )
			{
				desc.mipLevels			= MipmapLevel::Max();
				view_desc.mipmapCount	= UMax;
			}
			if ( _isDummy.load() )
				view_desc.format = desc.format;

			desc.Validate();
			view_desc.Validate( desc );

			// create image
			{
				CHECK_ERR_MSG( res_mngr.IsSupported( desc ),
					"Image '"s << _dbgName << "' description is not supported by GPU device" );

				auto	image	= res_mngr.CreateImage( desc, _dbgName, _Renderer().ChooseAllocator( False{"static"}, desc ));
				CHECK_ERR( image );

				CHECK( RenderGraph().GetStateTracker().AddResource( image,
															 EResourceState::_InvalidState,								// current is not used
															 EResourceState::ShaderSample | EResourceState::AllShaders,	// default
															 ctx.GetCommandBatchRC() ));
				ctx.ResourceState( image, EResourceState::Invalidate );

				auto	old_img	= _id.Attach( RVRef(image) );
				res_mngr.ReleaseResource( old_img );	// release dummy resource
			}

			// create image view
			{
				CHECK_ERR_MSG( res_mngr.IsSupported( _id.Get(), view_desc ),
					"Image view '"s << _dbgName << "' description is not supported by GPU device" );

				auto	view = res_mngr.CreateImageView( view_desc, _id.Get(), _dbgName );
				CHECK_ERR( view );

				auto	old_view = _view.Attach( RVRef(view) );
				res_mngr.ReleaseResource( old_view );	// release dummy resource
			}
			return true;
		}};

		bool	res = Create();

		if ( res )
		{
			_isDummy.store( false );

			if ( _outDynSize )
			{
				ASSERT( intermImg.GetImageDim() == _outDynSize->NumDimensions() );
				_outDynSize->Resize( intermImg.Dimension() );
			}

			auto	derived = _derived.ReadLock();

			for (auto* img : *derived) {
				CHECK( img->_UpdateView() );
			}
		}
		else
		{
			auto	dummy		= _Renderer().GetDummyImage( _requiredImageDesc );
			auto	old_img		= _id.Attach( RVRef(dummy.image) );
			auto	old_view	= _view.Attach( RVRef(dummy.view) );
			res_mngr.ImmediatelyReleaseResources( old_view, old_img );
		}

		return res;
	}

/*
=================================================
	_Load
=================================================
*/
	Image::IntermImageRC  Image::_Load (const AsyncDSRequestResult &in, EImageFormat fileFormat)
	{
		using namespace ResLoader;

		CHECK_ERR( in.data != null );
		CHECK_ERR( fileFormat != Default );

		IntermImageRC	result = MakeRC<IntermImage>();
		MemRefRStream	stream	{ in.data, in.dataSize };
		AllImageLoaders	loader;

		if ( loader.LoadImage( INOUT *result, stream, False{"no flipY"}, null, fileFormat ))
			return result;

		return null;
	}


} // AE::ResEditor
