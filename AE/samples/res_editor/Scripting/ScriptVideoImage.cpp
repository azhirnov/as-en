// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/PipelineCompiler.inl.h"
#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Scripting/ScriptBuffer.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptVideoImage*  ScriptVideoImage_Ctor1 (const String &filename) {
		return ScriptVideoImagePtr{ new ScriptVideoImage{ EPixelFormat::RGBA8_UNorm, filename }}.Detach();
	}

	static ScriptVideoImage*  ScriptVideoImage_Ctor2 (EPixelFormat format, const String &filename) {
		return ScriptVideoImagePtr{ new ScriptVideoImage{ format, filename }}.Detach();
	}

} // namespace

	using namespace AE::Threading;

/*
=================================================
	constructor
=================================================
*/
	ScriptVideoImage::ScriptVideoImage (EPixelFormat format, const String &filename) __Th___ :
		_format{ format },
		_imageType{uint( EImageType::Img2D | EImageType::Float )},
		_videoFile{ filename }
	{
		CHECK_THROW_MSG( GetVFS().Exists( _videoFile ),
			"File '"s << filename << "' is not exists" );

		if ( _dbgName.empty() )
			_dbgName = Path{filename}.stem().string().substr( 0, ResNameMaxLen );

		_resUsage |= EResourceUsage::UploadedData;

		_outDynSize = ScriptDynamicDimPtr{ new ScriptDynamicDim{ new DynamicDim{ uint3{}, EImageDim_2D } }};

		_videoInfo = MakePromise(
			[fname = _videoFile] () -> PromiseResult< VideoStreamInfo >
			{
				auto	rstream = GetVFS().Open<RStream>( fname );
				CHECK_PE( rstream );

				auto	decoder = Video::VideoFactory::CreateFFmpegDecoder();
				CHECK_PE( decoder );

				return decoder->GetFileProperties( RVRef(rstream), VideoImage::GetHwConfig() ).videoStream;
			},
			Tuple{},
			"async read video props",
			ETaskQueue::Background );
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptVideoImage::~ScriptVideoImage ()
	{
		if ( IsNullUnion( _resource ))
			AE_LOGW( "Unused VideoImage '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptVideoImage::Name (const String &name) __Th___
	{
		_dbgName = name.substr( 0, ResNameMaxLen );
	}

/*
=================================================
	AddUsage
=================================================
*/
	void  ScriptVideoImage::AddUsage (EResourceUsage usage) __Th___
	{
		_resUsage |= usage;

		ScriptImage::_ValidateResourceUsage( _resUsage );
	}

/*
=================================================
	Dimension
=================================================
*/
	ScriptDynamicDim*  ScriptVideoImage::Dimension () __Th___
	{
		ScriptDynamicDimPtr	result = _outDynSize;
		return result.Detach();
	}

/*
=================================================
	Ycbcr_*
=================================================
*/
	void  ScriptVideoImage::Ycbcr_SetFormat (EPixelFormat value) __Th___
	{
		_ycbcrDesc.format = value;
	}

	void  ScriptVideoImage::Ycbcr_SetModel (ESamplerYcbcrModelConversion value) __Th___
	{
		_ycbcrDesc.ycbcrModel = value;
	}

	void  ScriptVideoImage::Ycbcr_SetRange (ESamplerYcbcrRange value) __Th___
	{
		_ycbcrDesc.ycbcrRange = value;
	}

	void  ScriptVideoImage::Ycbcr_SetComponents (const String &value) __Th___
	{
		CHECK_THROW_MSG( not value.empty() );

		_ycbcrDesc.components = ImageSwizzle::FromString( value.data(), value.size() );
	}

	void  ScriptVideoImage::Ycbcr_SetXChromaOffset (ESamplerChromaLocation value) __Th___
	{
		_ycbcrDesc.xChromaOffset = value;
	}

	void  ScriptVideoImage::Ycbcr_SetYChromaOffset (ESamplerChromaLocation value) __Th___
	{
		_ycbcrDesc.yChromaOffset = value;
	}

	void  ScriptVideoImage::Ycbcr_SetChromaFilter (EFilter value) __Th___
	{
		_ycbcrDesc.chromaFilter = value;
	}

	void  ScriptVideoImage::Ycbcr_ForceExplicitReconstruction (bool value) __Th___
	{
		_ycbcrDesc.forceExplicitReconstruction = value;
	}

	void  ScriptVideoImage::SetSampler (const String &name) __Th___
	{
		_sampName = name;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptVideoImage::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptVideoImage>	binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptVideoImage_Ctor1,		{"videoFilePath"} );
		binder.AddFactoryCtor( &ScriptVideoImage_Ctor2,		{"format", "videoFilePath"} );

		binder.Comment( "Set resource name. It is used for debugging." );
		binder.AddMethod( &ScriptVideoImage::Name,								"Name",								{} );

		binder.Comment( "Returns dynamic dimension of the image" );
		binder.AddMethod( &ScriptVideoImage::Dimension,							"Dimension",						{} );

		binder.AddMethod( &ScriptVideoImage::SetSampler,						"Sampler",							{} );

		binder.Comment( "Set Ycbcr format. Requires multiplanar format." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetFormat,					"Ycbcr_Format",						{} );

		binder.Comment( "Set Ycbcr model conversion." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetModel,					"Ycbcr_Model",						{} );

		binder.Comment( "Set Ycbcr range." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetRange,					"Ycbcr_Range",						{} );

		binder.Comment( "Set Ycbcr component swizzle.\n"
						"Format: 'ARGB', 'R001'." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetComponents,				"Ycbcr_Components",					{} );

		binder.Comment( "Set Ycbcr X chroma location." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetXChromaOffset,			"Ycbcr_XChromaOffset",				{} );

		binder.Comment( "Set Ycbcr Y chroma location." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetYChromaOffset,			"Ycbcr_YChromaOffset",				{} );

		binder.Comment( "Set Ycbcr Y chroma filter." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_SetChromaFilter,				"Ycbcr_ChromaFilter",				{} );

		binder.Comment( "Set Ycbcr force explicit reconstruction." );
		binder.AddMethod( &ScriptVideoImage::Ycbcr_ForceExplicitReconstruction,	"Ycbcr_ForceExplicitReconstruction", {} );
	}

/*
=================================================
	ToResource
=================================================
*/
	ScriptVideoImage::VideImage_t  ScriptVideoImage::ToResource (PipelinePackID packId) __Th___
	{
		if ( not IsNullUnion( _resource ))
			return _resource;

		ImageDesc	desc;
		desc.imageDim	= EImageDim_2D;
		desc.format		= _format;
		desc.dimension	= ImageDim_t{uint3{ _dim, 1u }};

		CHECK_ERR( _resUsage != Default );
		for (auto usage : BitfieldIterate( _resUsage ))
		{
			switch_enum( usage )
			{
				case EResourceUsage::ComputeRead :
				case EResourceUsage::ComputeWrite :		desc.usage |= EImageUsage::Storage;		break;

				case EResourceUsage::UploadedData :		desc.usage |= EImageUsage::TransferDst;	break;

				case EResourceUsage::Sampled :			desc.usage |= EImageUsage::Sampled;		break;
				case EResourceUsage::GenMipmaps :		desc.usage |= EImageUsage::Transfer;	desc.options |= (EImageOpt::BlitSrc | EImageOpt::BlitDst);	break;
				case EResourceUsage::Present :			desc.usage |= EImageUsage::TransferSrc;	desc.options |= EImageOpt::BlitSrc;							break;
				case EResourceUsage::Transfer :			desc.usage |= EImageUsage::Transfer;	break;

				case EResourceUsage::Unknown :
				case EResourceUsage::WillReadback :
				case EResourceUsage::ColorAttachment :
				case EResourceUsage::DepthStencil :
				case EResourceUsage::ShaderAddress :
				case EResourceUsage::ComputeRW :
				case EResourceUsage::VertexInput :
				case EResourceUsage::IndirectBuffer :
				case EResourceUsage::ASBuild :
				case EResourceUsage::WithHistory :
				case EResourceUsage::InputAttachment :
				default :								RETURN_ERR( "unsupported usage" );
			}
			switch_end
		}

		Renderer&	renderer = ScriptExe::ScriptResourceApi::GetRenderer();  // throw

		VideoStreamInfo		video_stream_info;
		CHECK_ERR( _videoInfo.WithResult( [&video_stream_info] (const VideoStreamInfo &info){ video_stream_info = info; }));

		_outDynSize->Get()->Resize( video_stream_info.dimension );

		if ( HasYcbcrSampler() )
		{
			CHECK_THROW_MSG( not _ycbcrSampName.empty() );
			_resource = MakeRCTh<VideoImage2>( renderer, desc, _videoFile, _outDynSize->Get(),
											   SamplerName{_ycbcrSampName}, video_stream_info, packId, _dbgName );
		}
		else{
			_resource = MakeRCTh<VideoImage>( renderer, desc, _videoFile, _outDynSize->Get(), video_stream_info, _dbgName );
		}
		return _resource;
	}

/*
=================================================
	Validate
=================================================
*/
	void  ScriptVideoImage::Validate (String sampName) __Th___
	{
		CHECK_THROW( Scheduler().Wait( {AsyncTask{_videoInfo}}, EThreadArray{EThread::Background}, seconds{1} ));

		if ( HasYcbcrSampler() )
		{
			_videoInfo.WithResult(
				[this] (const VideoStreamInfo &info)
				{
					CHECK_THROW( info.IsValid() );

					_format	= info.pixFormat;
					_dim	= info.dimension;

					_ycbcrDesc.format			= info.pixFormat;
					_ycbcrDesc.xChromaOffset	= (_ycbcrDesc.xChromaOffset != Default ? _ycbcrDesc.xChromaOffset : info.xChromaOffset);
					_ycbcrDesc.yChromaOffset	= (_ycbcrDesc.yChromaOffset != Default ? _ycbcrDesc.yChromaOffset : info.yChromaOffset);
					_ycbcrDesc.ycbcrRange		= (_ycbcrDesc.ycbcrRange != Default ? _ycbcrDesc.ycbcrRange : info.ycbcrRange);
				});

			_CreateYcbcrSampler( RVRef(sampName) );
		}
		else
		{
			_videoInfo.WithResult(
				[this] (const VideoStreamInfo &info)
				{
					_dim = info.dimension;
				});

			CHECK_THROW_MSG( not IsNullUnion( ToResource( Default )));
		}
	}

/*
=================================================
	_CreateYcbcrSampler
=================================================
*/
	void  ScriptVideoImage::_CreateYcbcrSampler (String sampName) __Th___
	{
		using namespace AE::PipelineCompiler;

		if ( sampName.empty() )
			sampName = _sampName;

		auto	obj_storage = ObjectStorage::Instance();

		CHECK_THROW_MSG( obj_storage != null );
		CHECK_THROW_MSG( HasYcbcrSampler() );

		auto	it = obj_storage->samplerMap.find( sampName );
		CHECK_THROW_MSG( it != obj_storage->samplerMap.end(),
			"origin sampler '"s << sampName << "' is not exists" );

		ScriptSamplerPtr	src_samp = obj_storage->samplerRefs[ it->second ];
		CHECK_THROW_MSG( not src_samp->HasYcbcr(),
			"origin sampler must not be ycbcr sampler" );

		const auto	GetUniqueName = [obj_storage] () -> String
		{{
			String	name;
			for (usize i = 0; i < 100'000; i += 100)
			{
				name = "ycbcr-"s << ToString(i);

				if ( not obj_storage->samplerMap.contains( name ))
				{
					usize		j	= (i == 0 ? 0 : i-100);
					const usize	max	= (i == 0 ? 100 : i);

					for (; j < max; ++j)
					{
						name = "ycbcr-"s << ToString(i);

						if ( not obj_storage->samplerMap.contains( name ))
							return name;
					}
				}
			}
			CHECK_THROW_MSG( false, "can't find unique name for sampler" );
		}};
		const String	name = GetUniqueName();

		ScriptSamplerPtr	sampler {new ScriptSampler{ name }};

		sampler->SetDesc( src_samp->Desc() );
		sampler->SetYcbcrDesc( GetYcbcrDesc() );

		_ycbcrSampName = name;
	}


} // AE::ResEditor
