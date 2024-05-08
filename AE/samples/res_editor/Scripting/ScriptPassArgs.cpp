// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptPassArgs.h"
#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
	InitResources
=================================================
*/
	void  ScriptPassArgs::InitResources (OUT ResourceArray &resources, PipelinePackID packId) C_Th___
	{
		for (auto& arg : _args)
		{
			Visit( arg.res,
				[&] (ScriptBufferPtr buf) {
					auto	res = buf->ToResource();
					CHECK_THROW( res );
					resources.Add( UniformName{arg.name}, res, arg.state );
				},
				[&] (ScriptImagePtr tex) {
					auto	res = tex->ToResource();
					CHECK_THROW( res );
					resources.Add( UniformName{arg.name}, res, arg.state );
				},
				[&] (ScriptVideoImagePtr video) {
					auto	res = video->ToResource( packId );
					CHECK_THROW( not IsNullUnion( res ));
					Visit( res,
						[&] (RC<VideoImage>  vi) { resources.Add( UniformName{arg.name}, vi, arg.state ); },
						[&] (RC<VideoImage2> vi) { resources.Add( UniformName{arg.name}, vi, arg.state ); },
						[] (NullUnion) {}
					);
				},
				[&] (ScriptRTScenePtr scene) {
					auto	res = scene->ToResource();
					CHECK_THROW( res );
					resources.Add( UniformName{arg.name}, res, arg.state );
				},
				[&] (const Array<ScriptImagePtr> &arr)
				{
					Array<RC<Image>>	images;
					images.reserve( arr.size() );

					for (auto& tex : arr) {
						auto	res = tex->ToResource();
						CHECK_THROW( res );
						images.push_back( RVRef(res) );
					}
					resources.Add( UniformName{arg.name}, RVRef(images), arg.state );
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
				[]  (ScriptBufferPtr buf)				{ buf->AddLayoutReflection();  CHECK_THROW_MSG( buf->ToResource() ); },
				[]  (ScriptImagePtr tex)				{ CHECK_THROW_MSG( tex->ToResource() ); },
				[&] (ScriptVideoImagePtr video)			{ video->Validate( arg.samplerName ); },
				[]  (ScriptRTScenePtr scene)			{ CHECK_THROW_MSG( scene->ToResource() ); },
				[]  (const Array<ScriptImagePtr> &arr)	{ for (auto& tex : arr) CHECK_THROW_MSG( tex->ToResource() ); },
				[]  (NullUnion)							{ CHECK_THROW_MSG( false, "unsupported argument type" ); }
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

		Argument&	arg = _args.emplace_back();
		arg.name	= name;
		arg.res		= scene;
		arg.state	= EResourceState::ShaderRTAS;

		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	ArgBufferIn***
=================================================
*/
	void  ScriptPassArgs::ArgBufferIn (const String &name, const ScriptBufferPtr &buf)		__Th___	{ _AddArg( name, buf, EResourceUsage::ComputeRead ); }
	void  ScriptPassArgs::ArgBufferOut (const String &name, const ScriptBufferPtr &buf)		__Th___	{ _AddArg( name, buf, EResourceUsage::ComputeWrite ); }
	void  ScriptPassArgs::ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)	__Th___	{ _AddArg( name, buf, EResourceUsage::ComputeRW ); }

	void  ScriptPassArgs::_AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage) __Th___
	{
		CHECK_THROW_MSG( buf );
		buf->AddUsage( usage );

		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

		Argument&	arg = _args.emplace_back();
		arg.name	= name;
		arg.res		= buf;

		switch ( usage ) {
			case EResourceUsage::ComputeRead :	arg.state |= EResourceState::ShaderStorage_Read;	break;
			case EResourceUsage::ComputeWrite :	arg.state |= EResourceState::ShaderStorage_Write;	break;
			case EResourceUsage::ComputeRW :	arg.state |= EResourceState::ShaderStorage_RW;		break;
			default :							CHECK_MSG( false, "unsupported usage" );
		}
		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	ArgImageIn***
=================================================
*/
	void  ScriptPassArgs::ArgImageIn (const String &name, const ScriptImagePtr &img)	__Th___	{ _AddArg( name, img, EResourceUsage::ComputeRead ); }
	void  ScriptPassArgs::ArgImageOut (const String &name, const ScriptImagePtr &img)	__Th___	{ _AddArg( name, img, EResourceUsage::ComputeWrite ); }
	void  ScriptPassArgs::ArgImageInOut (const String &name, const ScriptImagePtr &img)	__Th___	{ _AddArg( name, img, EResourceUsage::ComputeRW ); }

	void  ScriptPassArgs::_AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage) __Th___
	{
		CHECK_THROW_MSG( img );
		img->AddUsage( usage );

		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

		Argument&	arg = _args.emplace_back();
		arg.name	= name;
		arg.res		= img;

		switch ( usage ) {
			case EResourceUsage::ComputeRead :	arg.state |= EResourceState::ShaderStorage_Read;	break;
			case EResourceUsage::ComputeWrite :	arg.state |= EResourceState::ShaderStorage_Write;	break;
			case EResourceUsage::ComputeRW :	arg.state |= EResourceState::ShaderStorage_RW;		break;
			default :							CHECK_MSG( false, "unsupported usage" );
		}
		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	ArgTextureIn*
=================================================
*/
	void  ScriptPassArgs::ArgTextureIn (const String &name, const ScriptImagePtr &tex) __Th___
	{
		return _AddTexture( name, tex, Default );
	}

	void  ScriptPassArgs::ArgTextureArrIn (const String &name, Array<ScriptImagePtr> arr) __Th___
	{
		return _AddTexture( name, RVRef(arr), Default );
	}

	void  ScriptPassArgs::ArgTextureIn2 (const String &name, const ScriptImagePtr &tex, const String &samplerName) __Th___
	{
		CHECK_THROW_MSG( not samplerName.empty() );
		return _AddTexture( name, tex, samplerName );
	}

	void  ScriptPassArgs::ArgTextureArrIn2 (const String &name, Array<ScriptImagePtr> arr, const String &samplerName) __Th___
	{
		CHECK_THROW_MSG( not samplerName.empty() );
		return _AddTexture( name, RVRef(arr), samplerName );
	}

/*
=================================================
	_AddTexture
=================================================
*/
	void  ScriptPassArgs::_AddTexture (const String &name, const ScriptImagePtr &tex, const String &samplerName) __Th___
	{
		CHECK_THROW_MSG( tex );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

		tex->AddUsage( EResourceUsage::Sampled );

		Argument&	arg = _args.emplace_back();
		arg.name		= name;
		arg.res			= tex;
		arg.state		= EResourceState::ShaderSample;
		arg.samplerName	= samplerName;

		if ( tex->IsDepthAndStencil() )
		{
			ScriptImagePtr	tmp;	tmp.Attach( tex->CreateView7() );
			CHECK_THROW_MSG( tmp, "Failed to create depth view for depth stencil image" );

			tmp->SetAspectMask( EImageAspect::Depth );
			arg.res = tmp;
		}

		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	_AddTexture
=================================================
*/
	void  ScriptPassArgs::_AddTexture (const String &name, Array<ScriptImagePtr> arr, const String &samplerName) __Th___
	{
		CHECK_THROW_MSG( arr.size() > 1 );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );
		CHECK_THROW_MSG( arr[0] );

		const auto	img_type = arr[0]->ImageType();

		for (auto& img : arr)
		{
			CHECK_THROW_MSG( img );
			CHECK_THROW_MSG( img_type == img->ImageType() );

			img->AddUsage( EResourceUsage::Sampled );

			if ( img->IsDepthAndStencil() )
			{
				ScriptImagePtr	tmp;	tmp.Attach( img->CreateView7() );
				CHECK_THROW_MSG( tmp, "Failed to create depth view for depth stencil image" );

				tmp->SetAspectMask( EImageAspect::Depth );
				img = RVRef(tmp);
			}
		}

		Argument&	arg = _args.emplace_back();
		arg.name		= name;
		arg.state		= EResourceState::ShaderSample;
		arg.samplerName	= samplerName;
		arg.res.emplace< Array<ScriptImagePtr> >( RVRef(arr) );

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
		CHECK_THROW_MSG( tex->HasYcbcrSampler() or not samplerName.empty() );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

		tex->AddUsage( EResourceUsage::Sampled );

		Argument&	arg = _args.emplace_back();
		arg.name		= name;
		arg.res			= tex;
		arg.state		= EResourceState::ShaderSample;
		arg.samplerName	= samplerName;

		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	ArgImageArr*
=================================================
*/
	void  ScriptPassArgs::ArgImageArrIn (const String &name, Array<ScriptImagePtr> arr)		__Th___	{ _AddArg( name, RVRef(arr), EResourceUsage::ComputeRead ); }
	void  ScriptPassArgs::ArgImageArrOut (const String &name, Array<ScriptImagePtr> arr)	__Th___	{ _AddArg( name, RVRef(arr), EResourceUsage::ComputeWrite ); }
	void  ScriptPassArgs::ArgImageArrInOut (const String &name, Array<ScriptImagePtr> arr)	__Th___	{ _AddArg( name, RVRef(arr), EResourceUsage::ComputeRW ); }

	void  ScriptPassArgs::_AddArg (const String &name, Array<ScriptImagePtr> arr, EResourceUsage usage) __Th___
	{
		CHECK_THROW_MSG( arr.size() > 1 );
		CHECK_THROW_MSG( arr[0] );

		const auto	img_type	= arr[0]->ImageType();
		const auto	img_fmt		= arr[0]->Description().format;

		for (auto& img : arr)
		{
			CHECK_THROW_MSG( img );
			CHECK_THROW_MSG( img_type == img->ImageType() );
			CHECK_THROW_MSG( img_fmt == img->Description().format );

			img->AddUsage( usage );
		}
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second, "uniform '"s << name << "' is already exists" );

		Argument&	arg = _args.emplace_back();
		arg.name = name;
		arg.res.emplace< Array<ScriptImagePtr> >( RVRef(arr) );

		switch ( usage ) {
			case EResourceUsage::ComputeRead :	arg.state |= EResourceState::ShaderStorage_Read;	break;
			case EResourceUsage::ComputeWrite :	arg.state |= EResourceState::ShaderStorage_Write;	break;
			case EResourceUsage::ComputeRW :	arg.state |= EResourceState::ShaderStorage_RW;		break;
			default :							CHECK_MSG( false, "unsupported usage" );
		}
		if ( _onAddArg )
			_onAddArg( arg );
	}

/*
=================================================
	CopyFrom
=================================================
*/
	void  ScriptPassArgs::CopyFrom (const ScriptPassArgs &other) __Th___
	{
		_args			= other._args;
		_uniqueNames	= other._uniqueNames;

		if ( _onAddArg )
		{
			for (auto& arg : _args) {
				_onAddArg( arg );
			}
		}
	}


} // AE::ResEditor
