// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Common.h"
#include "serializing/ObjectFactory.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	ExprInfo
=================================================
*/
	bool  ShaderTrace::ExprInfo::operator == (const ExprInfo &rhs) const
	{
		return	(varID		== rhs.varID)	&
				(swizzle	== rhs.swizzle)	&
				(range		== rhs.range)	&
				(point		== rhs.point)	&
				(vars		== rhs.vars);
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	SourceInfo
=================================================
*/
	bool  ShaderTrace::SourceInfo::operator == (const SourceInfo &rhs) const
	{
		return	(code		== rhs.code)		&
				(filename	== rhs.filename)	&
				(firstLine	== rhs.firstLine)	&
				(lines		== rhs.lines);
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	SourceLocation
=================================================
*/
	ShaderTrace::SourceLocation::SourceLocation (uint sourceId, uint line, uint column) :
		sourceId{sourceId},
		begin{line, column},
		end{line, column}
	{}

	bool  ShaderTrace::SourceLocation::operator == (const SourceLocation &rhs) const
	{
		return	(sourceId	== rhs.sourceId)	&
				(begin		== rhs.begin)		&
				(end		== rhs.end);
	}

	bool  ShaderTrace::SourceLocation::IsNotDefined () const
	{
		return	(sourceId	== 0)	&
				(begin._ul	== 0)	&
				(end._ul	== 0);
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_AppendSource
=================================================
*/
	void  ShaderTrace::_AppendSource (String filename, uint firstLine, String source)
	{
		SourceInfo	info;
		usize		pos = 0;

		info.filename	= RVRef(filename);
		info.code		= RVRef(source);
		info.firstLine	= firstLine;
		info.lines.reserve( 64 );

		for (usize j = 0, len = info.code.length(); j < len; ++j)
		{
			const char	c = info.code[j];
			const char	n = (j+1) >= len ? 0 : info.code[j+1];

			// windows style "\r\n"
			if ( c == '\r' and n == '\n' )
			{
				info.lines.emplace_back( pos, j );
				pos = (++j) + 1;
			}
			else
			// linux style "\n" (or mac style "\r")
			if ( c == '\n' or c == '\r' )
			{
				info.lines.emplace_back( pos, j );
				pos = j + 1;
			}
		}

		if ( pos < info.code.length() )
			info.lines.emplace_back( pos, info.code.length() );

		_sources.push_back( RVRef(info) );
	}

/*
=================================================
	AddSource
=================================================
*/
	void  ShaderTrace::AddSource (StringView source)
	{
		CHECK_ERRV( not source.empty() );

		_AppendSource( Default, 0, String{source} );
	}

	void  ShaderTrace::AddSource (StringView filename, uint firstLine, StringView source)
	{
		CHECK_ERRV( not source.empty() );

		_AppendSource( String{filename}, firstLine, String{source} );
	}
	
	void  ShaderTrace::AddSource (const Path &filename, uint firstLine, StringView source)
	{
		CHECK_ERRV( not source.empty() );

		return _AppendSource( ToString(filename), firstLine, String{source} );
	}

/*
=================================================
	IncludeSource
=================================================
*/
	void  ShaderTrace::IncludeSource (StringView headerName, const Path &fullPath, StringView source)
	{
		CHECK_ERRV( not source.empty() );
		CHECK_ERRV( not headerName.empty() );

		_fileMap.insert_or_assign( String(headerName), uint(_sources.size()) );
		_AppendSource( ToString(fullPath), 0, String{source} );
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
//-----------------------------------------------------------------------------
	
namespace
{
	using namespace Serializing;

	ND_ inline bool  Serialize_SourcePoint (Serializer &ser, const ShaderTrace::SourcePoint &x) {
		return ser( x._ul );
	}

	ND_ inline bool  Deserialize_SourcePoint (Deserializer &des, OUT ShaderTrace::SourcePoint &x) {
		return des( OUT x._ul );
	}

	
	ND_ inline bool  Serialize_SourceLocation (Serializer &ser, const ShaderTrace::SourceLocation &x) {
		return	ser( x.sourceId )								and
				Serialize_SourcePoint( ser, x.begin )			and
				Serialize_SourcePoint( ser, x.end );
	}

	ND_ inline bool  Deserialize_SourceLocation (Deserializer &des, OUT ShaderTrace::SourceLocation &x) {
		return	des( OUT x.sourceId )							and
				Deserialize_SourcePoint( des, OUT x.begin )		and
				Deserialize_SourcePoint( des, OUT x.end );
	}

	
	ND_ inline bool  Serialize_ExprInfo (Serializer &ser, const ShaderTrace::ExprInfo &x) {
		return	ser( x.varID )									and
				ser( x.swizzle )								and
				Serialize_SourceLocation( ser, x.range )		and
				Serialize_SourcePoint( ser, x.point )			and
				ser( x.vars );
	}

	ND_ inline bool  Deserialize_ExprInfo (Deserializer &des, OUT ShaderTrace::ExprInfo &x) {
		return	des( OUT x.varID )								and
				des( OUT x.swizzle )							and
				Deserialize_SourceLocation( des, OUT x.range )	and
				Deserialize_SourcePoint( des, OUT x.point )		and
				des( OUT x.vars );
	}

	
	ND_ inline bool  Serialize_SourceInfo (Serializer &ser, const ShaderTrace::SourceInfo &x) {
		return	ser( x.filename, x.code, x.firstLine, x.lines );
	}

	ND_ inline bool  Deserialize_SourceInfo (Deserializer &des, OUT ShaderTrace::SourceInfo &x) {
		return	des( OUT x.filename, OUT x.code, OUT x.firstLine, OUT x.lines );
	}

} // namespace

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


} // AE::PipelineCompiler
