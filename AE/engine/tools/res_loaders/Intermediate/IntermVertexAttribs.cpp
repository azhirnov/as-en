// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermVertexAttribs.h"
#include "graphics_rhi/Private/EnumToString.h"

template <>
struct std::hash< AE::ResLoader::IntermVertexAttribs::VertexInput > final :
	public AE::Base::DefaultHasher_CalcHash< AE::ResLoader::IntermVertexAttribs::VertexInput >
{};

template <>
struct std::hash< AE::ResLoader::IntermVertexAttribs::BufferBinding > final :
	public AE::Base::DefaultHasher_CalcHash< AE::ResLoader::IntermVertexAttribs::BufferBinding >
{};


namespace AE::ResLoader
{
	using namespace AE::Graphics;

/*
=================================================
	VertexInput::operator ==
=================================================
*/
	bool  IntermVertexAttribs::VertexInput::operator == (const VertexInput &rhs) C_NE___
	{
		return	type			== rhs.type			and
				index			== rhs.index		and
				offset			== rhs.offset		and
				bufferBinding	== rhs.bufferBinding;
	}

/*
=================================================
	VertexInput::CalcHash
=================================================
*/
	HashVal  IntermVertexAttribs::VertexInput::CalcHash () C_NE___
	{
		return HashOf(type) + HashOf(index) + HashOf(offset) + HashOf(bufferBinding);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BufferBinding::operator ==
=================================================
*/
	bool  IntermVertexAttribs::BufferBinding::operator == (const BufferBinding &rhs) C_NE___
	{
		return	index	== rhs.index	and
				stride	== rhs.stride	and
				rate	== rhs.rate;
	}

/*
=================================================
	BufferBinding::CalcHash
=================================================
*/
	HashVal  IntermVertexAttribs::BufferBinding::CalcHash () C_NE___
	{
		return HashOf(index) + HashOf(stride) + HashOf(rate);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	FindBuffer
=================================================
*/
	IntermVertexAttribs::BufferBinding const*  IntermVertexAttribs::FindBuffer (StringView name) C_NE___
	{
		auto	it = _bindings.find( name );
		if ( it != _bindings.end() )
			return &it->second;
		else
			return null;
	}

/*
=================================================
	FindVertex
=================================================
*/
	IntermVertexAttribs::VertexInput const*  IntermVertexAttribs::FindVertex (StringView name) C_NE___
	{
		auto	it = _vertices.find( name );
		if ( it != _vertices.end() )
			return &it->second;
		else
			return null;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  IntermVertexAttribs::CalcHash () C_NE___
	{
		return	HashOf( _vertices ) + HashOf( _bindings );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  IntermVertexAttribs::operator == (const IntermVertexAttribs &rhs) C_NE___
	{
		return	_vertices	== rhs._vertices	and
				_bindings	== rhs._bindings;
	}

/*
=================================================
	_GetData
=================================================
*/
	bool  IntermVertexAttribs::_GetData (StringView id, const Bytes stride, const EVertexType type, const Bool optional, OUT Bytes &offset) C_NE___
	{
		auto	it = _vertices.find( id );
		if ( it == _vertices.end() )
		{
			if ( optional )	return false;
			else			RETURN_ERR( "can't find vertex attrib '"s << StringView{id} << "'" );
		}

		const EVertexType	mask		= ~(EVertexType::NormalizedFlag | EVertexType::ScaledFlag);
		const EVertexType	cur_type	= it->second.type & mask;
		const EVertexType	req_type	= type & mask;

		if ( cur_type != req_type )
		{
			if ( optional )	return false;
			else			RETURN_ERR( "vertex attrib type mismatch: current "s << ToString(it->second.type) << ", required " << ToString(type) );
		}

		if ( stride == 0 )
		{
			if ( optional )	return false;
			else			RETURN_ERR( "vertex stride is not defined" );
		}

		offset = it->second.offset;
		return true;
	}
	
/*
=================================================
	GetRawData
=================================================
*/
	bool  IntermVertexAttribs::GetRawData (StringView id, OUT Bytes &elemOffset, OUT Bytes &elemSize) C_NE___
	{
		auto	it = _vertices.find( id );
		if ( it == _vertices.end() )
			return false;
		
		elemOffset	= it->second.offset;
		elemSize	= EVertexType_SizeOf( it->second.type );
		return true;
	}

/*
=================================================
	HasVertex
=================================================
*/
	bool  IntermVertexAttribs::HasVertex (StringView name, EVertexType type) C_NE___
	{
		auto	it = _vertices.find( name );
		if ( it == _vertices.end() )
			return false;

		return EVertexType_ToAttribType( it->second.type ) == EVertexType_ToAttribType( type );
	}


} // AE::ResLoader
