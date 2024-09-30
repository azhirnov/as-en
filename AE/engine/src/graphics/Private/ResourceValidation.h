// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/IDevice.h"
#include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
	Buffer_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  Buffer_IsSupported (const ResMngr &resMngr, const BufferDesc &desc) __NE___
	{
		if_unlikely( desc.size == 0 )
			return false;

		if_unlikely( desc.usage == Default or desc.memType == Default )
			return false;

		const auto&		res_flags = resMngr.GetDevice().GetResourceFlags();

		// validate usage
		if_unlikely( not AllBits( res_flags.bufferUsage, desc.usage ))
			return false;

		// validate options
		if_unlikely( desc.options != Default and not AllBits( res_flags.bufferOptions, desc.options ))
			return false;

		// validate memory type
		if_unlikely( not res_flags.memTypes.contains( desc.memType & ~EMemoryType::_External ))
			return false;

		// check supported features in FS
		if_unlikely( not resMngr.GetFeatureSet().IsSupported( desc ))
			return false;

		return true;
	}

/*
=================================================
	BufferView_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  BufferView_IsSupported (const ResMngr &resMngr, const BufferDesc &desc, const BufferViewDesc &view) __NE___
	{
		StaticAssert( uint(EBufferUsage::All) == 0x1FFF );

		constexpr auto	view_usage	= EBufferUsage::UniformTexel | EBufferUsage::StorageTexel;

		if_unlikely( NoBits( desc.usage, view_usage ))
			return false;

		if_unlikely( view.format == Default or view.format >= EPixelFormat::_Count )
			return false;

		// check supported view formats in FS
		if_unlikely( not resMngr.GetFeatureSet().IsSupported( desc, view ))
			return false;

		return true;
	}

/*
=================================================
	Image_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  Image_IsSupported (const ResMngr &resMngr, const ImageDesc &desc, Bool imageFormatListSupported) __NE___
	{
		const auto&		res_flags		= resMngr.GetDevice().GetResourceFlags();
		const usize		fmt_list_size	= desc.ViewFormatListSize();

		if_unlikely( desc.imageDim == Default or desc.usage == Default or desc.format == Default or desc.memType == Default )
			return false;

		// validate options
		{
			if_unlikely( not AllBits( res_flags.imageOptions, desc.options ))
				return false;

			for (auto option : BitfieldIterate( desc.options ))
			{
				switch_enum( option )
				{
					case EImageOpt::BlitSrc :					if_unlikely( NoBits( desc.usage, EImageUsage::TransferSrc )) return false;			break;
					case EImageOpt::BlitDst :					if_unlikely( NoBits( desc.usage, EImageUsage::TransferDst )) return false;			break;
					case EImageOpt::BlockTexelViewCompatible :	if_unlikely( not EPixelFormat_IsCompressed( desc.format )) return false;			break;

					case EImageOpt::StorageAtomic :
					case EImageOpt::VertexPplnStore :
					case EImageOpt::FragmentPplnStore :			if_unlikely( NoBits( desc.usage, EImageUsage::Storage )) return false;				break;

					case EImageOpt::SampledLinear :
					case EImageOpt::SampledMinMax :				if_unlikely( NoBits( desc.usage, EImageUsage::Sampled )) return false;				break;

					case EImageOpt::ColorAttachmentBlend :		if_unlikely( NoBits( desc.usage, EImageUsage::ColorAttachment )) return false;		break;

					case EImageOpt::ExtendedUsage :
					case EImageOpt::LossyRTCompression :
					case EImageOpt::SparseAliased :
					case EImageOpt::SparseResidencyAliased :
					case EImageOpt::SparseResidency :
					case EImageOpt::CubeCompatible :
					case EImageOpt::MutableFormat :
					case EImageOpt::Array2DCompatible :
					case EImageOpt::Alias :
					case EImageOpt::SampleLocationsCompatible :	break;

					case EImageOpt::_Last :
					case EImageOpt::All :
					case EImageOpt::Unknown :
					default_unlikely :			DBG_WARNING( "unknown image option" );	break;
				}
				switch_end
			}
		}

		// validate memory type
		if_unlikely( not res_flags.memTypes.contains( desc.memType & ~EMemoryType::_External ))
			return false;

		// validate format list
		if ( imageFormatListSupported and fmt_list_size > 0 )
		{
			using EFmtType = PixelFormatInfo::EType;

			if_unlikely( NoBits( desc.options, EImageOpt::MutableFormat ) and fmt_list_size > 1 )
				return false;

			const auto&		origin_fmt_info = EPixelFormat_GetInfo( desc.format );
			const bool		uncompress		= AllBits( desc.options, EImageOpt::BlockTexelViewCompatible ) and origin_fmt_info.IsCompressed();

			for (EPixelFormat fmt : desc.viewFormats)
			{
				if ( fmt == Default )
					continue;

				const auto&		fmt_info	= EPixelFormat_GetInfo( fmt );
				bool			compatible	= true;

				if ( uncompress and not fmt_info.IsCompressed() )
				{
					compatible &= origin_fmt_info.IsColor() and fmt_info.IsColor();
					compatible &= fmt_info.bitsPerBlock		== origin_fmt_info.bitsPerBlock;
				}
				else
				{
					compatible &= EPixelFormat_IsCompatible( desc.format, fmt );
				}

				if_unlikely( not compatible )
					return false;
			}
		}
		else
		if_unlikely( fmt_list_size > 0 )
		{
			if ( AllBits( desc.options, EImageOpt::MutableFormat ))
				return true;

			return false;  // extension is not supported
		}

		if_unlikely( not resMngr.GetFeatureSet().IsSupported( desc ))
			return false;

		return true;
	}

/*
=================================================
	ImageView_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  ImageView_IsSupported (const ResMngr &resMngr, const ImageDesc &desc, const ImageViewDesc &view) __NE___
	{
		StaticAssert( uint(EImageUsage::All) == 0xFF );
		StaticAssert( uint(EImageOpt::All) == 0x3FFFF );
		ASSERT( view.format != Default );

		constexpr EImageUsage	view_usage	=
			EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment |
			EImageUsage::InputAttachment | EImageUsage::ShadingRate;

		if_unlikely( NoBits( desc.usage, view_usage ))
			return false;

		if ( view.viewType == EImage_CubeArray )
		{
			if_unlikely( desc.imageDim != EImageDim_2D or (desc.imageDim == EImageDim_3D and AllBits( desc.options, EImageOpt::Array2DCompatible)) )
				return false;

			if_unlikely( NoBits( desc.options, EImageOpt::CubeCompatible ))
				return false;

			if_unlikely( not IsMultipleOf( view.layerCount, 6 ))
				return false;
		}

		if ( view.viewType == EImage_Cube )
		{
			if_unlikely( NoBits( desc.options, EImageOpt::CubeCompatible ))
				return false;

			if_unlikely( view.layerCount != 6 )
				return false;
		}

		if ( desc.imageDim == EImageDim_3D and view.viewType != EImage_3D )
		{
			if_unlikely( NoBits( desc.options, EImageOpt::Array2DCompatible ))
				return false;
		}

		// check view format
		{
			const usize	fmt_list_size = desc.ViewFormatListSize();

			if ( fmt_list_size > 0 )
			{
				if_unlikely( not ArrayContains( ArrayView<EPixelFormat>{desc.viewFormats}, view.format ))
					return false;

				if_unlikely( fmt_list_size > 1 and NoBits( desc.options, EImageOpt::MutableFormat ))
					return false;
			}

			if ( AllBits( desc.options, EImageOpt::BlockTexelViewCompatible ) and not EPixelFormat_IsCompressed( view.format ))
			{
				if_unlikely( not EPixelFormat_IsCopySupportedRelaxed( desc.format, view.format ))
					return false;
			}
			else
			{
				if_unlikely( not EPixelFormat_IsCompatible( desc.format, view.format ))
					return false;

				if_unlikely( NoBits( desc.options, EImageOpt::MutableFormat )	and
							 fmt_list_size == 0									and
							 view.format != desc.format							)
					return false;
			}
		}

		if_unlikely( not resMngr.GetFeatureSet().IsSupported( desc, view ))
			return false;

		return true;
	}

/*
=================================================
	RTGeometry_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  RTGeometry_IsSupported (const ResMngr &resMngr, const RTGeometryDesc &desc) __NE___
	{
		if_unlikely( resMngr.GetFeatureSet().accelerationStructure() != FeatureSet::EFeature::RequireTrue )
			return false;

		if_unlikely( desc.size == 0 )
			return false;

		// TODO: desc.options

		return true;
	}

/*
=================================================
	RTScene_IsSupported
=================================================
*/
	template <typename ResMngr>
	ND_ bool  RTScene_IsSupported (const ResMngr &resMngr, const RTSceneDesc &desc) __NE___
	{
		if_unlikely( resMngr.GetFeatureSet().accelerationStructure() != FeatureSet::EFeature::RequireTrue )
			return false;

		if_unlikely( desc.size == 0 )
			return false;

		// TODO: desc.options

		return true;
	}


} // AE::Graphics
