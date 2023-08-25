// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptPassArgs.h"

namespace AE::ResEditor
{

/*
=================================================
    InitResources
=================================================
*/
    void  ScriptPassArgs::InitResources (OUT ResourceArray &resources) C_Th___
    {
        for (auto& arg : _args)
        {
            Visit( arg.res,
                [&] (ScriptBufferPtr buf) {
                    auto    res = buf->ToResource();
                    CHECK_THROW( res );
                    resources._resources.emplace_back( UniformName{arg.name}, res, arg.state );
                },
                [&] (ScriptImagePtr tex) {
                    auto    res = tex->ToResource();
                    CHECK_THROW( res );
                    resources._resources.emplace_back( UniformName{arg.name}, res, arg.state );
                },
                [&] (ScriptVideoImagePtr video) {
                    auto    res = video->ToResource();
                    CHECK_THROW( res );
                    resources._resources.emplace_back( UniformName{arg.name}, res, arg.state );
                },
                [&] (ScriptRTScenePtr scene) {
                    auto    res = scene->ToResource();
                    CHECK_THROW( res );
                    resources._resources.emplace_back( UniformName{arg.name}, res, arg.state );
                },
                [] (NullUnion) {
                    CHECK_THROW_MSG( false, "unsupported argument type" );
                }
            );
        }
    }

/*
=================================================
    ValidateArgs
=================================================
*/
    void  ScriptPassArgs::ValidateArgs () C_Th___
    {
        for (auto& arg : _args)
        {
            Visit( arg.res,
                [] (ScriptBufferPtr buf)        { buf->AddLayoutReflection();  CHECK_THROW_MSG( buf->ToResource() ); },
                [] (ScriptImagePtr tex)         { CHECK_THROW_MSG( tex->ToResource() ); },
                [] (ScriptVideoImagePtr video)  { CHECK_THROW_MSG( video->ToResource() ); },
                [] (ScriptRTScenePtr scene)     { CHECK_THROW_MSG( scene->ToResource() ); },
                [] (NullUnion)                  { CHECK_THROW_MSG( false, "unsupported argument type" ); }
            );
        }
    }

/*
=================================================
    AddLayoutReflection
=================================================
*/
    void  ScriptPassArgs::AddLayoutReflection () C_Th___
    {
        for (auto& arg : _args)
        {
            if ( auto* buf = UnionGet<ScriptBufferPtr>( arg.res )) {
                (*buf)->AddLayoutReflection();
            }
        }
    }

/*
=================================================
    ArgSceneIn
=================================================
*/
    void  ScriptPassArgs::ArgSceneIn (const String &name, const ScriptRTScenePtr &scene) __Th___
    {
        CHECK_THROW_MSG( scene );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = scene;
        arg.state   = EResourceState::ShaderRTAS;

        if ( _onAddArg )
            _onAddArg( arg );
    }

/*
=================================================
    ArgBufferIn***
=================================================
*/
    void  ScriptPassArgs::ArgBufferIn (const String &name, const ScriptBufferPtr &buf)      __Th___ { _AddArg( name, buf, EResourceUsage::ComputeRead ); }
    void  ScriptPassArgs::ArgBufferOut (const String &name, const ScriptBufferPtr &buf)     __Th___ { _AddArg( name, buf, EResourceUsage::ComputeWrite ); }
    void  ScriptPassArgs::ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)   __Th___ { _AddArg( name, buf, EResourceUsage::ComputeRW ); }

    void  ScriptPassArgs::_AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage) __Th___
    {
        CHECK_THROW_MSG( buf );
        buf->AddUsage( usage );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = buf;

        switch ( usage ) {
            case EResourceUsage::ComputeRead :  arg.state |= EResourceState::ShaderStorage_Read;    break;
            case EResourceUsage::ComputeWrite : arg.state |= EResourceState::ShaderAddress_Write;   break;
            case EResourceUsage::ComputeRW :    arg.state |= EResourceState::ShaderStorage_RW;      break;
            default :                           CHECK_THROW_MSG( false, "unsupported usage" );
        }
        if ( _onAddArg )
            _onAddArg( arg );
    }

/*
=================================================
    ArgImageIn***
=================================================
*/
    void  ScriptPassArgs::ArgImageIn (const String &name, const ScriptImagePtr &img)    __Th___ { _AddArg( name, img, EResourceUsage::ComputeRead ); }
    void  ScriptPassArgs::ArgImageOut (const String &name, const ScriptImagePtr &img)   __Th___ { _AddArg( name, img, EResourceUsage::ComputeWrite ); }
    void  ScriptPassArgs::ArgImageInOut (const String &name, const ScriptImagePtr &img) __Th___ { _AddArg( name, img, EResourceUsage::ComputeRW ); }

    void  ScriptPassArgs::_AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage) __Th___
    {
        CHECK_THROW_MSG( img );
        img->AddUsage( usage );

        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

        Argument&   arg = _args.emplace_back();
        arg.name    = name;
        arg.res     = img;

        switch ( usage ) {
            case EResourceUsage::ComputeRead :  arg.state |= EResourceState::ShaderStorage_Read;    break;
            case EResourceUsage::ComputeWrite : arg.state |= EResourceState::ShaderAddress_Write;   break;
            case EResourceUsage::ComputeRW :    arg.state |= EResourceState::ShaderStorage_RW;      break;
            default :                           CHECK_THROW_MSG( false, "unsupported usage" );
        }
        if ( _onAddArg )
            _onAddArg( arg );
    }

/*
=================================================
    ArgTextureIn
=================================================
*/
    void  ScriptPassArgs::ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName) __Th___
    {
        CHECK_THROW_MSG( tex );
        CHECK_THROW_MSG( not samplerName.empty() );
        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

        tex->AddUsage( EResourceUsage::Sampled );

        Argument&   arg = _args.emplace_back();
        arg.name        = name;
        arg.res         = tex;
        arg.state       = EResourceState::ShaderSample;
        arg.samplerName = samplerName;

        if ( _onAddArg )
            _onAddArg( arg );
    }

/*
=================================================
    ArgVideoIn
=================================================
*/
    void  ScriptPassArgs::ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName) __Th___
    {
        CHECK_THROW_MSG( tex );
        CHECK_THROW_MSG( not samplerName.empty() );
        CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

        tex->AddUsage( EResourceUsage::Sampled );

        Argument&   arg = _args.emplace_back();
        arg.name        = name;
        arg.res         = tex;
        arg.state       = EResourceState::ShaderSample;
        arg.samplerName = samplerName;

        if ( _onAddArg )
            _onAddArg( arg );
    }


} // AE::ResEditor
