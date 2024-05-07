// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/DefaultResources.h"
#include "res_editor/Core/RenderGraph.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	DefaultResources::DefaultResources () :
		_dtQueue{ MakeRC<DataTransferQueue>() }
	{
		auto&	rts = GraphicsScheduler();

		{
			auto	mem_info = rts.GetDevice().GetMemoryInfo();
			_gpuMemSize = mem_info.deviceTotal + mem_info.unifiedTotal;
		}

		_gfxLinearAlloc	= rts.GetResourceManager().CreateLinearGfxMemAllocator( _pageSize );
		_gfxLargeAlloc	= rts.GetResourceManager().CreateLargeSizeGfxMemAllocator();

		_CreateDummyImage2D( OUT _dummyRes.image2D, _gfxLinearAlloc );
		_CreateDummyImage3D( OUT _dummyRes.image3D, _gfxLinearAlloc );
		_CreateDummyImageCube( OUT _dummyRes.imageCube, _gfxLinearAlloc );

		if ( rts.GetFeatureSet().accelerationStructure() == FeatureSet::EFeature::RequireTrue )
		{
			_CreateDummyRTGeometry( OUT _dummyRes.rtGeometry, _gfxLinearAlloc );
			_CreateDummyRTScene( OUT _dummyRes.rtScene, _gfxLinearAlloc );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	DefaultResources::~DefaultResources ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.ReleaseResources( _dummyRes.image2D.image,		_dummyRes.image2D.view );
		res_mngr.ReleaseResources( _dummyRes.image3D.image,		_dummyRes.image3D.view );
		res_mngr.ReleaseResources( _dummyRes.imageCube.image,	_dummyRes.imageCube.view );
		res_mngr.ReleaseResources( _dummyRes.rtGeometry,		_dummyRes.rtScene );
	}

/*
=================================================
	GetDummyImage
=================================================
*/
	StrongImageAndViewID  DefaultResources::GetDummyImage (const ImageDesc &desc) C_NE___
	{
		ASSERT( desc.imageDim != Default );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const bool	is_cube		= (desc.imageDim == EImageDim_2D and AllBits( desc.options, EImageOpt::CubeCompatible ));
		const bool	is_2darr	= (desc.imageDim == EImageDim_2D and desc.arrayLayers.Get() > 1);
		const bool	is_2d		= (desc.imageDim == EImageDim_2D and desc.arrayLayers.Get() == 1);
		const bool	is_3d		= (desc.imageDim == EImageDim_3D);

		StrongImageAndViewID	result;

		if ( is_cube or is_2darr )
		{
			result.image = res_mngr.AcquireResource( _dummyRes.imageCube.image.Get() );
			result.view  = res_mngr.AcquireResource( _dummyRes.imageCube.view.Get() );
		}
		else
		if ( is_2d )
		{
			result.image = res_mngr.AcquireResource( _dummyRes.image2D.image.Get() );
			result.view  = res_mngr.AcquireResource( _dummyRes.image2D.view.Get() );
		}
		else
		if ( is_3d )
		{
			result.image = res_mngr.AcquireResource( _dummyRes.image3D.image.Get() );
			result.view  = res_mngr.AcquireResource( _dummyRes.image3D.view.Get() );
		}

		return result;
	}

/*
=================================================
	GetDummyRTGeometry
=================================================
*/
	Strong<RTGeometryID>  DefaultResources::GetDummyRTGeometry () C_NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		return res_mngr.AcquireResource( _dummyRes.rtGeometry.Get() );
	}

/*
=================================================
	GetDummyRTScene
=================================================
*/
	Strong<RTSceneID>  DefaultResources::GetDummyRTScene () C_NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		return res_mngr.AcquireResource( _dummyRes.rtScene.Get() );
	}

/*
=================================================
	_CreateDummyImage2D
=================================================
*/
	void  DefaultResources::_CreateDummyImage2D (OUT StrongImageAndViewID &dst, GfxMemAllocatorPtr gfxAlloc) const
	{
		auto&		res_mngr = GraphicsScheduler().GetResourceManager();
		ImageDesc	desc;

		desc.SetFormat( EPixelFormat::R8_UNorm );
		desc.SetDimension( uint2{2} );
		desc.SetUsage( EImageUsage::Sampled | EImageUsage::TransferSrc );

		dst.image = res_mngr.CreateImage( desc, "dummy image 2D", gfxAlloc );
		CHECK_ERRV( dst.image );

		ImageViewDesc	view {desc};
		view.swizzle = "RRR1"_swizzle;

		dst.view = res_mngr.CreateImageView( view, dst.image, "dummy image 2D view" );
		CHECK_ERRV( dst.view );

		RenderGraph().GetStateTracker().AddResource( dst.image, Default, EResourceState::ShaderSample | EResourceState::AllShaders );
		GetDataTransferQueue().EnqueueImageTransition( dst.image );
	}

/*
=================================================
	_CreateDummyImage3D
=================================================
*/
	void  DefaultResources::_CreateDummyImage3D (OUT StrongImageAndViewID &dst, GfxMemAllocatorPtr gfxAlloc) const
	{
		auto&		res_mngr = GraphicsScheduler().GetResourceManager();
		ImageDesc	desc;

		desc.SetFormat( EPixelFormat::R8_UNorm );
		desc.SetDimension( uint3{2} );
		desc.SetUsage( EImageUsage::Sampled | EImageUsage::TransferSrc );

		dst.image = res_mngr.CreateImage( desc, "dummy image 3D", gfxAlloc );
		CHECK_ERRV( dst.image );

		ImageViewDesc	view {desc};
		view.swizzle = "RRR1"_swizzle;

		dst.view = res_mngr.CreateImageView( view, dst.image, "dummy image 3D view" );
		CHECK_ERRV( dst.view );

		RenderGraph().GetStateTracker().AddResource( dst.image, Default, EResourceState::ShaderSample | EResourceState::AllShaders );
		GetDataTransferQueue().EnqueueImageTransition( dst.image );
	}

/*
=================================================
	_CreateDummyImageCube
=================================================
*/
	void  DefaultResources::_CreateDummyImageCube (OUT StrongImageAndViewID &dst, GfxMemAllocatorPtr gfxAlloc) const
	{
		auto&		res_mngr = GraphicsScheduler().GetResourceManager();
		ImageDesc	desc;

		desc.SetFormat( EPixelFormat::R8_UNorm );
		desc.SetDimension( uint2{2} );
		desc.SetUsage( EImageUsage::Sampled | EImageUsage::TransferSrc );
		desc.SetArrayLayers( 6 );
		desc.SetOptions( EImageOpt::CubeCompatible );

		dst.image = res_mngr.CreateImage( desc, "dummy image cube", gfxAlloc );
		CHECK_ERRV( dst.image );

		ImageViewDesc	view {desc};
		view.swizzle = "RRR1"_swizzle;

		dst.view = res_mngr.CreateImageView( view, dst.image, "dummy image cube view" );
		CHECK_ERRV( dst.view );

		RenderGraph().GetStateTracker().AddResource( dst.image, Default, EResourceState::ShaderSample | EResourceState::AllShaders );
		GetDataTransferQueue().EnqueueImageTransition( dst.image );
	}

/*
=================================================
	_CreateDummyRTGeometry
=================================================
*/
	void  DefaultResources::_CreateDummyRTGeometry (OUT Strong<RTGeometryID> &dst, GfxMemAllocatorPtr gfxAlloc) const
	{
		auto&			res_mngr = GraphicsScheduler().GetResourceManager();
		RTGeometryDesc	desc;

		desc.options	= Default;
		desc.size		= 16_b;

		dst = res_mngr.CreateRTGeometry( desc, "dummy RTGeometry", gfxAlloc );
		CHECK_ERRV( dst );
	}

/*
=================================================
	_CreateDummyRTScene
=================================================
*/
	void  DefaultResources::_CreateDummyRTScene (OUT Strong<RTSceneID> &dst, GfxMemAllocatorPtr gfxAlloc) const
	{
		auto&		res_mngr = GraphicsScheduler().GetResourceManager();
		RTSceneDesc	desc;

		desc.options	= Default;
		desc.size		= 16_b;

		dst = res_mngr.CreateRTScene( desc, "dummy RTScene", gfxAlloc );
		CHECK_ERRV( dst );
	}

/*
=================================================
	ChooseAllocator
=================================================
*/
	GfxMemAllocatorPtr  DefaultResources::ChooseAllocator (Bool isDynamic, Bytes size) C_NE___
	{
		if ( size > _pageSize )
			return _gfxLargeAlloc;

		return isDynamic ? _gfxDynamicAlloc : _gfxLinearAlloc;
	}

	GfxMemAllocatorPtr  DefaultResources::ChooseAllocator (Bool isDynamic, const ImageDesc &desc) C_NE___
	{
		auto&	fmt_info = EPixelFormat_GetInfo( desc.format );
		return ChooseAllocator( isDynamic,
					ImageUtils::ImageSize( desc.dimension, desc.arrayLayers, desc.maxLevel, desc.samples, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() ));
	}

	GfxMemAllocatorPtr  DefaultResources::ChooseAllocator (Bool isDynamic, const VideoImageDesc &desc) C_NE___
	{
		const auto	CalcSize = [&desc] (OUT Bytes &size)
		{{
			auto&		fmt_info	= EPixelFormat_GetInfo( desc.format );
			const uint	plane_count	= Max( 1u, fmt_info.PlaneCount() );
			const bool	multiplanar	= fmt_info.IsMultiPlanar();

			for (uint plane = 0; plane < plane_count; ++plane)
			{
				const auto		aspect		= multiplanar ? EImageAspect_Plane( plane ) : EImageAspect::Color;
				EPixelFormat	plane_fmt	= desc.format;
				uint2			plane_scale	{1,1};

				if ( aspect != EImageAspect::Color )
				{
					CHECK_ERRV( EPixelFormat_GetPlaneInfo( desc.format, aspect, OUT plane_fmt, OUT plane_scale ));
					CHECK_ERRV( All( IsMultipleOf( desc.dimension, plane_scale )));
				}

				const uint2		dim			= desc.dimension / plane_scale;
				auto&			plane_info	= EPixelFormat_GetInfo( plane_fmt );

				size += ImageUtils::ImageSize( uint3{dim,1}, plane_info.bitsPerBlock, plane_info.TexBlockDim() );
			}
		}};

		Bytes	size;
		CalcSize( OUT size );
		return ChooseAllocator( isDynamic, size );
	}


} // AE::ResEditor
