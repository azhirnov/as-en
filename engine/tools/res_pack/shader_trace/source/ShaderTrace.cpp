// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Common.h"
#include "serializing/ObjectFactory.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	_AppendSource
=================================================
*/
	void  ShaderTrace::_AppendSource (const char* source, usize length)
	{
		CHECK_ERRV( length > 0 );
		CHECK_ERRV( source != null );

		SourceInfo	info;
		usize		pos = 0;

		info.code.assign( source, length );
		info.lines.reserve( 64 );

		for (usize j = 0, len = info.code.length(); j < len; ++j)
		{
			const char	c = info.code[j];
			const char	n = (j+1) >= len ? 0 : info.code[j+1];

			// windows style "\r\n"
			if ( c == '\r' and n == '\n' )
			{
				info.lines.push_back({ pos, j });
				pos = (++j) + 1;
			}
			else
			// linux style "\n" (or mac style "\r")
			if ( c == '\n' or c == '\r' )
			{
				info.lines.push_back({ pos, j });
				pos = j + 1;
			}
		}

		if ( pos < info.code.length() )
			info.lines.push_back({ pos, info.code.length() });

		_sources.push_back( RVRef(info) );
	}

/*
=================================================
	SetSource
=================================================
*/
	void  ShaderTrace::SetSource (const char* const* sources, const usize *lengths, usize count)
	{
		CHECK_ERRV( count > 0 );
		CHECK_ERRV( sources != null );

		_sources.clear();
		_sources.reserve( count );

		for (usize i = 0; i < count; ++i)
		{
			CHECK_ERRV( sources[i] != null );
			_AppendSource( sources[i], (lengths ? lengths[i] : strlen(sources[i])) );
		}
	}

	void  ShaderTrace::SetSource (const char* source, usize length)
	{
		CHECK_ERRV( length > 0 );
		CHECK_ERRV( source != null );

		const char*	sources[] = { source };
		return SetSource( sources, &length, 1 );
	}

	void  ShaderTrace::IncludeSource (const char* filename, const char* source, usize length)
	{
		CHECK_ERRV( length > 0 );
		CHECK_ERRV( source != null );
		CHECK_ERRV( filename != null );

		_fileMap.insert_or_assign( String(filename), uint(_sources.size()) );
		_AppendSource( source, length );
	}

/*
=================================================
	GetSource
=================================================
*/
	void  ShaderTrace::GetSource (OUT String &result) const
	{
		usize	total_size = _sources.size()*2;

		for (auto& src : _sources) {
			total_size += src.code.length();
		}

		result.clear();
		result.reserve( total_size );

		for (auto& src : _sources) {
			result.append( src.code );
		}
	}
//-----------------------------------------------------------------------------


namespace
{
	using namespace Serializing;

	ND_ static bool  Serialize_SourcePoint (Serializer &ser, const ShaderTrace::SourcePoint &x) {
		return ser( x.value );
	}

	ND_ static bool  Deserialize_SourcePoint (Deserializer &des, OUT ShaderTrace::SourcePoint &x) {
		return des( OUT x.value );
	}

	
	ND_ static bool  Serialize_SourceLocation (Serializer &ser, const ShaderTrace::SourceLocation &x) {
		return	ser( x.sourceId )						and
				Serialize_SourcePoint( ser, x.begin )	and
				Serialize_SourcePoint( ser, x.end );
	}

	ND_ static bool  Deserialize_SourceLocation (Deserializer &des, OUT ShaderTrace::SourceLocation &x) {
		return	des( OUT x.sourceId )						and
				Deserialize_SourcePoint( des, OUT x.begin )	and
				Deserialize_SourcePoint( des, OUT x.end );
	}

	
	ND_ static bool  Serialize_ExprInfo (Serializer &ser, const ShaderTrace::ExprInfo &x) {
		return	ser( x.varID )								and
				ser( x.swizzle )							and
				Serialize_SourceLocation( ser, x.range )	and
				Serialize_SourcePoint( ser, x.point )		and
				ser( x.vars );
	}

	ND_ static bool  Deserialize_ExprInfo (Deserializer &des, OUT ShaderTrace::ExprInfo &x) {
		return	des( OUT x.varID )								and
				des( OUT x.swizzle )							and
				Deserialize_SourceLocation( des, OUT x.range )	and
				Deserialize_SourcePoint( des, OUT x.point )		and
				des( OUT x.vars );
	}

	
	ND_ static bool  Serialize_SourceInfo (Serializer &ser, const ShaderTrace::SourceInfo &x) {
		return	ser( x.code )	and
				ser( x.lines );
	}

	ND_ static bool  Deserialize_SourceInfo (Deserializer &des, OUT ShaderTrace::SourceInfo &x) {
		return	des( OUT x.code )	and
				des( OUT x.lines );
	}
}

/*
=================================================
	Serialize
=================================================
*/
	bool  ShaderTrace::Serialize (Serializer &ser) C_NE___
	{
		bool	result = true;

		result &= ser( uint(_exprLocations.size()) );
		for (auto& item : _exprLocations) {
			result &= Serialize_ExprInfo( ser, item );
		}
		
		result &= ser( _varNames );
		
		result &= ser( uint(_sources.size()) );
		for (auto& item : _sources) {
			result &= Serialize_SourceInfo( ser, item );
		}
		
		//result &= ser( _fileMap );

		result &= ser( _posOffset );
		result &= ser( _dataOffset );
		result &= ser( _initialPosition );

		return result;
	}
	
/*
=================================================
	Deserialize
=================================================
*/
	bool  ShaderTrace::Deserialize (Deserializer &des) __NE___
	{
		try {
			bool	result = true;

			// _exprLocations
			{
				uint	count = 0;
				CHECK_ERR( des( OUT count ));
				CHECK_ERR( count < MaxCount );

				_exprLocations.resize( count );		// throw
				for (auto& item : _exprLocations) {
					result &= Deserialize_ExprInfo( des, OUT item );
				}
				CHECK_ERR( result );
			}
		
			result &= des( OUT _varNames );

			// _sources
			{
				uint	count = 0;
				CHECK_ERR( des( OUT count ));
				CHECK_ERR( count < MaxCount );

				_sources.resize( count );		// throw
				for (auto& item : _sources) {
					result &= Deserialize_SourceInfo( des, OUT item );
				}
				CHECK_ERR( result );
			}
		
			//result &= des( OUT _fileMap );

			result &= des( OUT _posOffset );
			result &= des( OUT _dataOffset );
			result &= des( OUT _initialPosition );
		
			CHECK_ERR( result );
			return true;
		}
		catch (...) {
			return false;
		}
	}
	
/*
=================================================
	operator ==
=================================================
*/
	bool  ShaderTrace::operator == (const ShaderTrace &rhs) const
	{
		return	_exprLocations		== rhs._exprLocations	and
				_varNames			== rhs._varNames		and
				_sources			== rhs._sources			and
				_posOffset			== rhs._posOffset		and
				_dataOffset			== rhs._dataOffset		and
				_initialPosition	== rhs._initialPosition;
	}
	
/*
=================================================
	Clone
=================================================
*/
	Unique<ShaderTrace>  ShaderTrace::Clone () const
	{
		auto	ptr = MakeUnique<ShaderTrace>();
		ptr->_exprLocations		= _exprLocations;
		ptr->_varNames			= _varNames;
		ptr->_sources			= _sources;
		ptr->_posOffset			= _posOffset;
		ptr->_dataOffset		= _dataOffset;
		ptr->_initialPosition	= _initialPosition;
		return ptr;
	}


} // AE::PipelineCompiler
