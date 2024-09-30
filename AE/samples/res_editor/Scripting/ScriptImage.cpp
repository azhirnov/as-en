// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/PipelineCompiler.inl.h"
#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptImage*  ScriptImage_Ctor1 (EImageType imageType, const String &filename) {
		return ScriptImagePtr{ new ScriptImage{ imageType, filename, Default }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor2 (EPixelFormat format, const packed_uint2 &dim) {
		return ScriptImagePtr{ new ScriptImage{ format, packed_uint3{dim,1} }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor3 (EPixelFormat format, const packed_uint3 &dim) {
		return ScriptImagePtr{ new ScriptImage{ format, dim }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor4 (EPixelFormat format, const packed_uint2 &dim, const ImageLayer &layers) {
		return ScriptImagePtr{ new ScriptImage{ format, packed_uint3{dim,1}, layers, 1_mipmap }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor5 (EPixelFormat format, const packed_uint2 &dim, const MipmapLevel &mipmaps) {
		return ScriptImagePtr{ new ScriptImage{ format, packed_uint3{dim,1}, 1_layer, mipmaps }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor6 (EPixelFormat format, const packed_uint3 &dim, const MipmapLevel &mipmaps) {
		return ScriptImagePtr{ new ScriptImage{ format, dim, 1_layer, mipmaps }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor7 (EPixelFormat format, const packed_uint2 &dim, const ImageLayer &layers, const MipmapLevel &mipmaps) {
		return ScriptImagePtr{ new ScriptImage{ format, packed_uint3{dim,1}, layers, mipmaps }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor8 (EPixelFormat format, const ScriptDynamicDimPtr &ds) {
		return ScriptImagePtr{ new ScriptImage{ format, ds }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor9 (EPixelFormat format, const ScriptDynamicDimPtr &ds, const ImageLayer &layers) {
		return ScriptImagePtr{ new ScriptImage{ format, ds, layers, 1_mipmap }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor10 (EPixelFormat format, const ScriptDynamicDimPtr &ds, const MipmapLevel &mipmaps) {
		return ScriptImagePtr{ new ScriptImage{ format, ds, 1_layer, mipmaps }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor11 (EPixelFormat format, const ScriptDynamicDimPtr &ds, const ImageLayer &layers, const MipmapLevel &mipmaps) {
		return ScriptImagePtr{ new ScriptImage{ format, ds, layers, mipmaps }}.Detach();
	}

	static ScriptImage*  ScriptImage_Ctor12 (EImageType imageType, const String &filename, ScriptImage::ELoadOpFlags flags) {
		return ScriptImagePtr{ new ScriptImage{ imageType, filename, flags }}.Detach();
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	ScriptImage::ScriptImage (EImageType imageType, const String &filename, ELoadOpFlags flags) __Th___ :
		_imageType{imageType}
	{
		_desc.imageDim = EImageDim_2D;
		switch ( imageType & EImageType::_TexMask )
		{
			case EImageType::Img1D :
			case EImageType::Img1DArray :		_desc.imageDim = EImageDim_1D;	break;
			case EImageType::Img2D :
			case EImageType::Img2DMS :			_desc.imageDim = EImageDim_2D;	break;
			case EImageType::Img2DArray :
			case EImageType::Img2DMSArray :		_desc.imageDim = EImageDim_2D;	_desc.arrayLayers = 6_layer;	break;
			case EImageType::ImgCube :
			case EImageType::ImgCubeArray :		_desc.imageDim = EImageDim_2D;	_desc.arrayLayers = 6_layer;	_desc.options = EImageOpt::CubeCompatible;	break;
			case EImageType::Img3D :			_desc.imageDim = EImageDim_3D;	break;
			default :							CHECK_THROW_MSG( false, "unsupported image type" );
		}

		// set similar format
		_desc.format = EPixelFormat::RGBA8_UNorm;
		switch ( imageType & EImageType::_ValMask )
		{
			case EImageType::Float :			_desc.format = EPixelFormat::RGBA32F;		break;
			case EImageType::Half :				_desc.format = EPixelFormat::RGBA16F;		break;
			case EImageType::SNorm :			_desc.format = EPixelFormat::RGBA8_SNorm;	break;
			case EImageType::UNorm :			_desc.format = EPixelFormat::RGBA8_UNorm;	break;
			case EImageType::Int :				_desc.format = EPixelFormat::RGBA8I;		break;
			case EImageType::UInt :				_desc.format = EPixelFormat::RGBA8U;		break;
			case EImageType::sRGB :				_desc.format = EPixelFormat::sRGB8_A8;		break;
			case EImageType::Depth :			_desc.format = ScriptExe::ScriptResourceApi::Supported_DepthFormat();			break;
			case EImageType::DepthStencil :		_desc.format = ScriptExe::ScriptResourceApi::Supported_DepthStencilFormat();	break;
			default :							CHECK_THROW_MSG( false, "unsupported image type" );
		}

		_outDynSize = ScriptDynamicDimPtr{ new ScriptDynamicDim{ MakeRC<DynamicDim>( uint3{}, _desc.imageDim )}};

		_Load( filename, 0_mipmap, 0_layer, flags );
	}

	ScriptImage::ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds) __Th___ :
		_descDefined{true}, _inDynSize{ ds }
	{
		CHECK_THROW_MSG( _inDynSize );

		_desc.format	= format;
		_desc.imageDim	= _inDynSize->Get()->NumDimensions();
		_desc.dimension	= ImageDim_t{uint3{ EPixelFormat_GetInfo( format ).TexBlockDim(), 1u }};
		_imageType		= GetDescriptorImageType( _desc );
	}

	ScriptImage::ScriptImage (EPixelFormat format, const packed_uint3 &dim) __Th___ :
		ScriptImage{ format, dim, 1_layer, 1_mipmap }
	{}

	ScriptImage::ScriptImage (EPixelFormat format, const packed_uint3 &dim, const ImageLayer &layers, const MipmapLevel &mipmaps) __Th___ :
		_descDefined{true}
	{
		CHECK_THROW_MSG( All( dim > packed_uint3{0} ));
		CHECK_THROW_MSG( layers > 0_layer );
		CHECK_THROW_MSG( mipmaps > 0_mipmap );

		_desc.format		= format;
		_desc.arrayLayers	= layers;
		_desc.mipLevels		= mipmaps;
		_desc.imageDim		= dim.z > 1 ? EImageDim_3D : EImageDim_2D;
		_desc.dimension		= CheckCast<ImageDim_t>(dim);
		_desc.Validate();

		_imageType			= GetDescriptorImageType( _desc );
	}

	ScriptImage::ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds, const ImageLayer &layers, const MipmapLevel &mipmaps) __Th___ :
		_descDefined{true}, _inDynSize{ ds }
	{
		CHECK_THROW_MSG( layers > 0_layer );
		CHECK_THROW_MSG( mipmaps > 0_mipmap );
		CHECK_THROW_MSG( _inDynSize );

		_desc.format		= format;
		_desc.imageDim		= _inDynSize->Get()->NumDimensions();
		_desc.dimension		= ImageDim_t{uint3{ EPixelFormat_GetInfo( format ).TexBlockDim(), 1u }};
		_desc.arrayLayers	= layers;
		_desc.mipLevels		= mipmaps;
		_desc.Validate();

		_imageType			= GetDescriptorImageType( _desc );
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptImage::~ScriptImage ()
	{
		if ( not _resource )
			AE_LOGW( "Unused image '"s << _dbgName << "'" );
	}

/*
=================================================
	IsDepthOrStencil / HasDepth / HasStencil
=================================================
*/
	bool  ScriptImage::IsDepthOrStencil () C_NE___
	{
		switch ( _imageType & EImageType::_ValMask )
		{
			case EImageType::Depth :
			case EImageType::Stencil :
			case EImageType::DepthStencil :
				return true;
		}
		return false;
	}

	bool  ScriptImage::HasDepth () C_NE___
	{
		switch ( _imageType & EImageType::_ValMask )
		{
			case EImageType::Depth :
			case EImageType::DepthStencil :
				return true;
		}
		return false;
	}

	bool  ScriptImage::HasStencil () C_NE___
	{
		switch ( _imageType & EImageType::_ValMask )
		{
			case EImageType::Stencil :
			case EImageType::DepthStencil :
				return true;
		}
		return false;
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptImage::Name (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change debug name" );

		_dbgName = name.substr( 0, ResNameMaxLen );
	}

/*
=================================================
	AddUsage
=================================================
*/
	void  ScriptImage::AddUsage (EResourceUsage usage) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change usage or content" );

		if ( _base and
			 (_base->Description().format == _viewDesc.format or
			  GraphicsScheduler().GetFeatureSet().imageViewExtendedUsage != FeatureSet::EFeature::RequireTrue) )
		{
			return _base->AddUsage( usage );
		}

		_resUsage |= usage;

		_ValidateResourceUsage( _resUsage );
	}

/*
=================================================
	_ValidateResourceUsage
=================================================
*/
	void  ScriptImage::_ValidateResourceUsage (const EResourceUsage usage) __Th___
	{
		if ( AllBits( usage, EResourceUsage::UploadedData ))
		{
			CHECK_THROW_MSG( NoBits( usage, EResourceUsage::ColorAttachment ));
			CHECK_THROW_MSG( NoBits( usage, EResourceUsage::DepthStencil ));
			CHECK_THROW_MSG( NoBits( usage, EResourceUsage::ComputeWrite ));
		}

		if ( AllBits( usage, EResourceUsage::WithHistory ))
		{
			CHECK_THROW_MSG( NoBits( usage, EResourceUsage::UploadedData ));
		}

		CHECK_THROW_MSG( not AllBits( usage, EResourceUsage::ColorAttachment | EResourceUsage::DepthStencil ),
			"Can not combine ColorAttachment and DepthStencil usage" );

		// TODO
	}

/*
=================================================
	LoadLayer*
=================================================
*/
	void  ScriptImage::LoadLayer1 (const String &filename, uint layer) __Th___
	{
		LoadLayer2( filename, layer, ELoadOpFlags::Unknown );
	}

	void  ScriptImage::LoadLayer2 (const String &filename, uint layer, ELoadOpFlags flags) __Th___
	{
		_Load( filename, 0_mipmap, ImageLayer{layer}, flags );
	}

	void  ScriptImage::LoadLayer3 (const String &filename, uint layer, uint flags) __Th___
	{
		LoadLayer2( filename, layer, ELoadOpFlags(flags) );
	}

/*
=================================================
	IsMutableDimension
=================================================
*/
	bool  ScriptImage::IsMutableDimension () C_Th___
	{
		return bool{_outDynSize} or bool{_inDynSize};
	}

/*
=================================================
	Dimension3
=================================================
*/
	packed_uint3  ScriptImage::Dimension3 () C_Th___
	{
		if ( _base )
			return _base->Dimension3();

		CHECK_THROW_MSG( not IsMutableDimension() );
		return packed_uint3{_desc.dimension};
	}

/*
=================================================
	Dimension
=================================================
*/
	ScriptDynamicDim*  ScriptImage::Dimension () C_Th___
	{
		return DimensionRC().Detach();
	}

	ScriptDynamicDimPtr  ScriptImage::DimensionRC () C_Th___
	{
		if ( _base )
			return _base->DimensionRC();

		CHECK_THROW_MSG( IsMutableDimension() );

		ScriptDynamicDimPtr	result;
		if ( _outDynSize )	result = _outDynSize;
		if ( _inDynSize )	result = _inDynSize;
		return result;
	}

	RC<DynamicDim>  ScriptImage::DynamicDimension () C_Th___
	{
		if ( _base )
			return _base->DynamicDimension();

		CHECK_ERR( IsMutableDimension() );

		if ( _outDynSize )	return _outDynSize->Get();
		if ( _inDynSize )	return _inDynSize->Get();

		return null;
	}

/*
=================================================
	ArrayLayers
=================================================
*/
	uint  ScriptImage::ArrayLayers () C_Th___
	{
		if ( _base )
			return _base->ArrayLayers();

		return _desc.arrayLayers.Get();
	}

/*
=================================================
	MipmapCount
=================================================
*/
	uint  ScriptImage::MipmapCount () C_Th___
	{
		if ( _base )
			return _base->MipmapCount();

		return _desc.mipLevels.Get();
	}

/*
=================================================
	Description
=================================================
*/
	ImageDesc  ScriptImage::Description () C_NE___
	{
		if ( _base )
			return _base->Description();

		return _desc;
	}

/*
=================================================
	SetSwizzle
=================================================
*/
	void  ScriptImage::SetSwizzle (const String &value) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change swizzle" );

		_viewDesc.swizzle = ImageSwizzle::FromString( value );
	}

/*
=================================================
	SetAspectMask
=================================================
*/
	void  ScriptImage::SetAspectMask (EImageAspect value) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change aspect" );

		_viewDesc.aspectMask = value;
	}

/*
=================================================
	CreateView*
=================================================
*/
	ScriptImage*  ScriptImage::CreateView1 (EImage				viewType,
											EPixelFormat		format,
											const MipmapLevel&	baseMipmap,
											uint				mipmapCount,
											const ImageLayer&	baseLayer,
											uint				layerCount) __Th___
	{
		CHECK_THROW_MSG( not _base,
			"Can not create view of view" );
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not create view" );

		if ( viewType == EImage::Cube or viewType == EImage::CubeArray )
			_desc.options |= EImageOpt::CubeCompatible;

		if ( format != Default and format != _desc.format )
			_desc.options |= EImageOpt::MutableFormat;	// TODO: use format list

		ScriptImagePtr	result {new ScriptImage{0}};

		result->_base		= ScriptImagePtr{this};
		result->_viewDesc	= ImageViewDesc{ viewType, format, baseMipmap, mipmapCount, baseLayer, layerCount };
		result->_viewDesc.Validate( _desc );

		result->_imageType	= GetDescriptorImageType( _desc, result->_viewDesc );

		return result.Detach();
	}

	ScriptImage*  ScriptImage::CreateView2 (EImage				viewType,
											const MipmapLevel&	baseMipmap,
											uint				mipmapCount,
											const ImageLayer&	baseLayer,
											uint				layerCount) __Th___
	{
		return CreateView1( viewType, Default, baseMipmap, mipmapCount, baseLayer, layerCount );
	}

	ScriptImage*  ScriptImage::CreateView3 (EImage				viewType,
											const MipmapLevel&	baseMipmap,
											uint				mipmapCount) __Th___
	{
		return CreateView2( viewType, baseMipmap, mipmapCount, 0_layer, UMax );
	}

	ScriptImage*  ScriptImage::CreateView4 (EImage				viewType,
											const ImageLayer&	baseLayer,
											uint				layerCount) __Th___
	{
		return CreateView2( viewType, 0_mipmap, UMax, baseLayer, layerCount );
	}

	ScriptImage*  ScriptImage::CreateView5 (EImage viewType) __Th___
	{
		return CreateView2( viewType, 0_mipmap, UMax, 0_layer, UMax );
	}

	ScriptImage*  ScriptImage::CreateView6 (EImage			viewType,
											EPixelFormat	format) __Th___
	{
		return CreateView1( viewType, format, 0_mipmap, UMax, 0_layer, UMax );
	}

/*
=================================================
	_GetImageType
=================================================
*/
	using PCImageType = EImageType;

	auto  ScriptImage::_GetImageType ()		C_Th___	{ return PCImageType(ImageType()); }

	bool  ScriptImage::_IsFloatFormat ()	C_Th___ {
		return AnyEqual( _GetImageType() & PCImageType::_ValMask,
						 PCImageType::Float, PCImageType::Half, PCImageType::SNorm, PCImageType::UNorm );
	}

	bool  ScriptImage::_IsIntFormat ()		C_Th___	{
		return AnyEqual( _GetImageType() & PCImageType::_ValMask, PCImageType::Int );
	}

	bool  ScriptImage::_IsUIntFormat ()		C_Th___	{
		return AnyEqual( _GetImageType() & PCImageType::_ValMask, PCImageType::UInt );
	}

	bool  ScriptImage::_Is1D ()				C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::Img1D; }
	bool  ScriptImage::_Is2D ()				C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::Img2D; }
	bool  ScriptImage::_Is3D ()				C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::Img3D; }
	bool  ScriptImage::_IsCube ()			C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::ImgCube; }
	bool  ScriptImage::_Is1DArray ()		C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::Img1DArray; }
	bool  ScriptImage::_Is2DArray ()		C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::Img2DArray; }
	bool  ScriptImage::_IsCubeArray ()		C_Th___	{ return (_GetImageType() & PCImageType::_TexMask) == PCImageType::ImgCubeArray; }

/*
=================================================
	Bind
=================================================
*/
	void  ScriptImage::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;
		{
			EnumBinder<ELoadOpFlags>	binder{ se };
			binder.Create();

			binder.Comment( "Generate mipmaps after loading" );
			binder.AddValue( "GenMipmaps",	ELoadOpFlags::GenMipmaps );

			StaticAssert( uint(ELoadOpFlags::All) == 0x1 );
		}{
			ClassBinder<ScriptImage>	binder{ se };
			binder.CreateRef( 0, False{"no ctor"} );

			binder.Comment( "Create image from file.\n"
							"File will be searched in VFS." );
			binder.AddFactoryCtor( &ScriptImage_Ctor1,	{"imageType", "filenameInVFS"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor12,	{"imageType", "filenameInVFS", "flags"} );

			binder.Comment( "Create image with constant dimension" );
			binder.AddFactoryCtor( &ScriptImage_Ctor2,	{"format", "dimension"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor3,	{"format", "dimension"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor4,	{"format", "dimension", "layers"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor5,	{"format", "dimension", "mipmaps"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor6,	{"format", "dimension", "mipmaps"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor7,	{"format", "dimension", "layers", "mipmaps"} );

			binder.Comment( "Create image with dynamic dimension" );
			binder.AddFactoryCtor( &ScriptImage_Ctor8,	{"format", "dynamicDimension"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor9,	{"format", "dynamicDimension", "layers"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor10,	{"format", "dynamicDimension", "mipmaps"} );
			binder.AddFactoryCtor( &ScriptImage_Ctor11, {"format", "dynamicDimension", "layers", "mipmaps"} );

			binder.Comment( "Set resource name. It is used for debugging." );
			binder.AddMethod( &ScriptImage::Name,				"Name",					{} );

			binder.Comment( "Load specified array layer from file, can be used for 2DArray/CubeMap/CubeMapArray.\n"
							"File will be searched in VFS." );
			binder.AddMethod( &ScriptImage::LoadLayer1,			"LoadLayer",			{"filenameInVFS", "layer"} );
			binder.AddMethod( &ScriptImage::LoadLayer2,			"LoadLayer",			{"filenameInVFS", "layer", "flags"} );
			binder.AddMethod( &ScriptImage::LoadLayer3,			"LoadLayer",			{"filenameInVFS", "layer", "flags"} );

			binder.Comment( "Returns 'true' if used dynamic dimension, methods 'Dimension2()', 'Dimension2_Layers()', 'Dimension3()' can not be used for dynamic dimension, only 'Dimension()' is allowed" );
			binder.AddMethod( &ScriptImage::IsMutableDimension,	"IsMutableDimension",	{} );

			binder.Comment( "Returns constant dimension of the image" );
			binder.AddMethod( &ScriptImage::Dimension2,			"Dimension2",			{} );
			binder.AddMethod( &ScriptImage::Dimension2_Layers,	"Dimension2_Layers",	{} );
			binder.AddMethod( &ScriptImage::Dimension3,			"Dimension3",			{} );
			binder.AddMethod( &ScriptImage::ArrayLayers,		"ArrayLayers",			{} );
			binder.AddMethod( &ScriptImage::MipmapCount,		"MipmapCount",			{} );

			binder.Comment( "Returns dynamic dimension of the image" );
			binder.AddMethod( &ScriptImage::Dimension,			"Dimension",			{} );

			binder.Comment( "Set image swizzle like a 'RGBA', 'R000', ..." );
			binder.AddMethod( &ScriptImage::SetSwizzle,			"SetSwizzle",			{} );

			binder.Comment( "Set image aspect, otherwise it will be auto-detected.\n"
							"DepthStencil images can not be sampled, you must choose depth or stencil aspect." );
			binder.AddMethod( &ScriptImage::SetAspectMask,		"SetAspectMask",		{} );

			binder.Comment( "Returns image description" );
			binder.AddMethod( &ScriptImage::_GetImageType,		"ImageType",			{} );
			binder.AddMethod( &ScriptImage::_IsFloatFormat,		"IsFloatFormat",		{} );
			binder.AddMethod( &ScriptImage::_IsIntFormat,		"IsIntFormat",			{} );
			binder.AddMethod( &ScriptImage::_IsUIntFormat,		"IsUIntFormat",			{} );
			binder.AddMethod( &ScriptImage::_Is1D,				"Is1D",					{} );
			binder.AddMethod( &ScriptImage::_Is2D,				"Is2D",					{} );
			binder.AddMethod( &ScriptImage::_Is3D,				"Is3D",					{} );
			binder.AddMethod( &ScriptImage::_IsCube,			"IsCube",				{} );
			binder.AddMethod( &ScriptImage::_Is1DArray,			"Is1DArray",			{} );
			binder.AddMethod( &ScriptImage::_Is2DArray,			"Is2DArray",			{} );
			binder.AddMethod( &ScriptImage::_IsCubeArray,		"IsCubeArray",			{} );

			binder.Comment( "Create image as view for current image."
							"Can be used to create CubeMap from 2DArray or set different swizzle." );
			binder.AddMethod( &ScriptImage::CreateView1,		"CreateView",			{"viewType", "format", "baseMipmap", "mipmapCount", "baseLayer", "layerCount"} );
			binder.AddMethod( &ScriptImage::CreateView2,		"CreateView",			{"viewType", "baseMipmap", "mipmapCount", "baseLayer", "layerCount"} );
			binder.AddMethod( &ScriptImage::CreateView3,		"CreateView",			{"viewType", "baseMipmap", "mipmapCount"} );
			binder.AddMethod( &ScriptImage::CreateView4,		"CreateView",			{"viewType", "baseLayer", "layerCount"} );
			binder.AddMethod( &ScriptImage::CreateView5,		"CreateView",			{"viewType"} );
			binder.AddMethod( &ScriptImage::CreateView6,		"CreateView",			{"viewType", "format"} );
			binder.AddMethod( &ScriptImage::CreateView7,		"CreateView",			{} );
		}
	}

/*
=================================================
	_Load
=================================================
*/
	void  ScriptImage::_Load (const String &filename, MipmapLevel mipmap, ImageLayer layer, ELoadOpFlags flags) __Th___
	{
		CHECK_THROW_MSG( not _base,
			"Can not load image for image view" );
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change usage or content" );
		CHECK_THROW_MSG( layer < _desc.arrayLayers,
			"Image array layer ("s << ToString(layer.Get()) << ") is out of bounds [0," << ToString(_desc.arrayLayers.Get()) << ")" );
		CHECK_THROW_MSG( mipmap < _desc.mipLevels,
			"Image mipmap level ("s << ToString(mipmap.Get()) << ") is out of bounds [0," << ToString(_desc.mipLevels.Get()) << ")" )

		if ( _dbgName.empty() )
			_dbgName = Path{filename}.stem().string().substr( 0, ResNameMaxLen );

		VFS::FileName	fname{ filename };

		CHECK_THROW_MSG( GetVFS().Exists( fname ),
			"File '"s << filename << "' is not exists" );

		// TODO: add file path to dependencies?

		auto&	dst		= _loadOps.emplace_back();
		dst.filename	= fname;
		dst.imgFormat	= ResLoader::PathToImageFileFormat( filename );
		dst.mipmap		= mipmap;
		dst.layer		= layer;
		dst.flags		= flags;

		AddUsage( EResourceUsage::UploadedData );

		if ( AllBits( flags, ELoadOpFlags::GenMipmaps ))
			AddUsage( EResourceUsage::GenMipmaps );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<Image>  ScriptImage::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		if ( not _base )
			CHECK_THROW_MSG( _resUsage != Default, "failed to create image '"s << _dbgName << "'" );

		for (auto usage : BitfieldIterate( _resUsage ))
		{
			switch_enum( usage )
			{
				case EResourceUsage::ComputeRead :		_desc.usage |= EImageUsage::Storage | EImageUsage::TransferSrc;			break;
				case EResourceUsage::ComputeWrite :		_desc.usage |= EImageUsage::Storage;									break;

				case EResourceUsage::ColorAttachment :	_desc.usage |= EImageUsage::ColorAttachment | EImageUsage::TransferSrc;	break;
				case EResourceUsage::DepthStencil :		_desc.usage |= EImageUsage::DepthStencilAttachment;						break;
				case EResourceUsage::InputAttachment :	_desc.usage |= EImageUsage::InputAttachment;							break;

				case EResourceUsage::UploadedData :		_desc.usage |= EImageUsage::TransferDst;								break;
				case EResourceUsage::WillReadback :		_desc.usage |= EImageUsage::TransferSrc;								break;

				case EResourceUsage::Sampled :			_desc.usage |= EImageUsage::Sampled;									break;
				case EResourceUsage::Transfer :			_desc.usage |= EImageUsage::Transfer;									break;

				case EResourceUsage::GenMipmaps :
					_desc.usage		|= EImageUsage::Transfer;
					_desc.options	|= (EImageOpt::BlitSrc | EImageOpt::BlitDst);
					break;

				case EResourceUsage::Present :
					_desc.usage		|= EImageUsage::TransferSrc;
					_desc.options	|= EImageOpt::BlitSrc;
					break;

				case EResourceUsage::Unknown :
				case EResourceUsage::ShaderAddress :
				case EResourceUsage::ComputeRW :
				case EResourceUsage::VertexInput :
				case EResourceUsage::IndirectBuffer :
				case EResourceUsage::ASBuild :
				case EResourceUsage::WithHistory :
				default :								RETURN_ERR( "unsupported usage" );
			}
			switch_end
		}

		if ( _base )
		{
			_viewDesc.extUsage = _desc.usage & ~_base->_desc.usage;
			_resource = _base->ToResource()->CreateView( _viewDesc, _dbgName );
			return _resource;
		}

		if ( AllBits( _desc.usage, EImageUsage::TransferSrc ) and NoBits( _desc.usage, EImageUsage::DepthStencilAttachment ))
			_desc.options |= EImageOpt::BlitSrc;

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer();  // throw

		const auto	mutable_res_usage =	EImageUsage::Storage | EImageUsage::ColorAttachment |
										EImageUsage::TransferDst | EImageUsage::DepthStencilAttachment;

		StrongImageAndViewID	id;
		const bool				is_mutable	= AnyBits( _desc.usage, mutable_res_usage );
		const bool				is_dummy	= not (_descDefined and is_mutable);
		GfxMemAllocatorPtr		gfx_alloc	= renderer.ChooseAllocator( Bool{_inDynSize}, _desc );

		// validate view desc
		{
			ImageDesc	desc = _desc;
			desc.options &= ~EImageOpt::CubeCompatible;

			_viewDesc.Validate( desc );
		}

		if ( is_dummy )
		{
			id = renderer.GetDummyImage( _desc );
			CHECK_THROW_MSG( id.view, "Can't get dummy image" );
		}
		else
		{
			if ( _inDynSize ) {
				_desc.dimension = ImageDim_t{_inDynSize->Get()->Dimension3_NonZero()};
			}else{
				CHECK_THROW_MSG( All( _desc.dimension > ImageDim_t{0} ), "failed to create image '"s << _dbgName << "'" );
			}

			CHECK_THROW_MSG( res_mngr.IsSupported( _desc ),
				"Image '"s << _dbgName << "' description is not supported by GPU device" );

			id.image = res_mngr.CreateImage( _desc, _dbgName, gfx_alloc );
			CHECK_THROW_MSG( id.image, "failed to create image '"s << _dbgName << "'" );

			id.view = res_mngr.CreateImageView( _viewDesc, id.image, _dbgName );
			CHECK_THROW_MSG( id.view, "failed to create image '"s << _dbgName << "'" );

			renderer.GetDataTransferQueue().EnqueueImageTransition( id.image );
		}

		_resource = MakeRCTh<Image>( RVRef(id.image), RVRef(id.view), RVRef(_loadOps), renderer, is_dummy, _desc, _viewDesc,
									 (_inDynSize ? _inDynSize->Get() : null), (_outDynSize ? _outDynSize->Get() : null),
									 _dbgName );	// throw
		return _resource;
	}


} // AE::ResEditor
