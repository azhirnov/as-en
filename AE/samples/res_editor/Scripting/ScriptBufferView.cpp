// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptExe.h"
#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	ScriptBufferView::ScriptBufferView (ScriptBufferPtr	buf,
										EPixelFormat	format,
										StringView		dbgName) __Th___ :
		_baseBuffer{ RVRef(buf) },
		_format{ format },
		_dbgName{ dbgName }
	{}

/*
=================================================
	destructor
=================================================
*/
	ScriptBufferView::~ScriptBufferView ()
	{
		if ( not _resource )
			AE_LOGW( "Unused buffer '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptBufferView::Name (const String &name) __Th___
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
	void  ScriptBufferView::AddUsage (EResourceUsage usage) __Th___
	{
		if ( not AllBits( _resUsage, usage ))
		{
			CHECK_THROW_MSG( not _resource,
				"Resource is already created, can not change usage or content" );
		}

		switch ( usage )
		{
			case EResourceUsage::Sampled :
			case EResourceUsage::TexelStorage :
				break;

			case EResourceUsage::ComputeRead :
			case EResourceUsage::ComputeWrite :
			case EResourceUsage::ComputeRW :
			{
				_resUsage |= EResourceUsage::TexelStorage;
				_baseBuffer->AddUsage( EResourceUsage::TexelStorage );
				break;
			}

			default :
				CHECK_THROW_MSG( false, "unsupported usage" );
		}

		_resUsage |= usage;
		_baseBuffer->AddUsage( usage );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<BufferView>  ScriptBufferView::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		// duplicate
		CHECK_THROW_MSG( not _baseBuffer->HasLayout() );

		RC<Buffer>	buffer = _baseBuffer->ToResource();
		CHECK_THROW( buffer );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer(); // throw
		auto		desc		= buffer->GetBufferDesc();
		BufferID	buf_id		= buffer->GetBufferId(0);

		BufferViewDesc	view_desc;
		view_desc.size		= desc.size;
		view_desc.format	= _format;

		CHECK_THROW_MSG( res_mngr.IsSupported( buf_id, view_desc ),
			"BufferView '"s << _dbgName << "' description is not supported by GPU device" );

		Strong<BufferViewID>	view_id = res_mngr.CreateBufferView( view_desc, buf_id, _dbgName );
		CHECK_THROW_MSG( view_id );

		_resource.reset( new BufferView{ RVRef(buffer), RVRef(view_id), _format, renderer, _dbgName });
		return _resource;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptBufferView::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptBufferView>	binder{ se };
		binder.CreateRef( 0, False{"without ctor"} );

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptBufferView::Name,					"Name",				{} );

		AS_METHOD( binder, ScriptBufferView::Format,				"Format",			{} );
	}


} // AE::ResEditor
