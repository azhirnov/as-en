// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/PassCommon.inl.h"
#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Scripting/ScriptBuffer.h"

#include "scripting/Impl/ClassBinder.h"

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


/*
=================================================
    constructor
=================================================
*/
    ScriptVideoImage::ScriptVideoImage (EPixelFormat format, const String &filename) __Th___ :
        _format{ format },
        _imageType{uint( PipelineCompiler::EImageType::Img2D | PipelineCompiler::EImageType::Float )},
        _videoFile{ FileSystem::ToAbsolute( filename )}
    {
        if ( _dbgName.empty() )
            _dbgName = Path{filename}.filename().replace_extension("").string().substr( 0, ResNameMaxLen );

        CHECK_THROW_MSG( FileSystem::IsFile( _videoFile ),
            "File '"s << filename << "' is not exists" );

        _resUsage |= EResourceUsage::UploadedData;

        _outDynSize = ScriptDynamicDimPtr{ new ScriptDynamicDim{ new DynamicDim{ uint3{}, EImageDim_2D } }};
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
        ScriptDynamicDimPtr result = _outDynSize;
        return result.Detach();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptVideoImage::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptVideoImage>    binder{ se };
        binder.CreateRef();
        binder.AddFactoryCtor( &ScriptVideoImage_Ctor1 );
        binder.AddFactoryCtor( &ScriptVideoImage_Ctor2 );

        binder.AddMethod( &ScriptVideoImage::Name,      "Name"      );
        binder.AddMethod( &ScriptVideoImage::Dimension, "Dimension" );
    }

/*
=================================================
    ToResource
=================================================
*/
    RC<VideoImage>  ScriptVideoImage::ToResource () __Th___
    {
        if ( _resource )
            return _resource;

        ImageDesc   desc;
        desc.imageDim   = EImageDim_2D;
        desc.format     = _format;

        CHECK_ERR( _resUsage != Default );
        for (auto usage = _resUsage; usage != Default;)
        {
            BEGIN_ENUM_CHECKS();
            switch ( ExtractBit( INOUT usage ))
            {
                case EResourceUsage::ComputeRead :
                case EResourceUsage::ComputeWrite :     desc.usage |= EImageUsage::Storage;     break;

                case EResourceUsage::UploadedData :     desc.usage |= EImageUsage::TransferDst; break;

                case EResourceUsage::Sampled :          desc.usage |= EImageUsage::Sampled;     break;
                case EResourceUsage::GenMipmaps :       desc.usage |= EImageUsage::Transfer;    desc.options |= (EImageOpt::BlitSrc | EImageOpt::BlitDst);  break;
                case EResourceUsage::Present :          desc.usage |= EImageUsage::TransferSrc; desc.options |= EImageOpt::BlitSrc;                         break;

                case EResourceUsage::Unknown :
                case EResourceUsage::WillReadback :
                case EResourceUsage::ColorAttachment :
                case EResourceUsage::DepthStencil :
                case EResourceUsage::ComputeRW :
                case EResourceUsage::VertexInput :
                case EResourceUsage::IndirectBuffer :
                case EResourceUsage::ASBuild :
                default :                               RETURN_ERR( "unsupported usage" );
            }
            END_ENUM_CHECKS();
        }

        Renderer&   renderer = ScriptExe::ScriptResourceApi::GetRenderer();  // throw

        _resource = MakeRC<VideoImage>( renderer, desc, _videoFile, _outDynSize->Get(), _dbgName );  // throw
        return _resource;
    }


} // AE::ResEditor
