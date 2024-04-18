// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/DefaultResources.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	DefaultResources::DefaultResources () :
		_dtQueue{ MakeRC<DataTransferQueue>() },
		_gfxLinearAlloc{ GraphicsScheduler().GetResourceManager().CreateLinearGfxMemAllocator( 256_Mb )},
		_gfxDynamicAlloc{}	// TODO
	{
		_CreateDummyImage2D( OUT _dummyRes.image2D, _gfxLinearAlloc );
		_CreateDummyImage3D( OUT _dummyRes.image3D, _gfxLinearAlloc );
		_CreateDummyImageCube( OUT _dummyRes.imageCube, _gfxLinearAlloc );

		if ( GraphicsScheduler().GetFeatureSet().accelerationStructure() == EFeature::RequireTrue )
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


} // AE::ResEditor
