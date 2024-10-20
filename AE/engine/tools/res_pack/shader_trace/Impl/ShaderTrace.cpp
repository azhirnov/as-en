// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Common.h"
#include "serializing/Public/ObjectFactory.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	ExprInfo
=================================================
*/
	bool  ShaderTrace::ExprInfo::operator == (const ExprInfo &rhs) C_NE___
	{
		return	(varID		== rhs.varID)	and
				(swizzle	== rhs.swizzle)	and
				(range		== rhs.range)	and
				(point		== rhs.point)	and
				(vars		== rhs.vars);
	}
//-----------------------------------------------------------------------------


/*
=================================================
	SourceInfo
=================================================
*/
	bool  ShaderTrace::SourceInfo::operator == (const SourceInfo &rhs) C_NE___
	{
		return	(code		== rhs.code)		and
				(filename	== rhs.filename)	and
				(firstLine	== rhs.firstLine)	and
				(lines		== rhs.lines);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SourceLocation
=================================================
*/
	ShaderTrace::SourceLocation::SourceLocation (uint sourceId, uint line, uint column) __NE___ :
		sourceId{sourceId},
		begin{line, column},
		end{line, column}
	{}

	bool  ShaderTrace::SourceLocation::operator == (const SourceLocation &rhs) C_NE___
	{
		return	(sourceId	== rhs.sourceId)	and
				(begin		== rhs.begin)		and
				(end		== rhs.end);
	}

	bool  ShaderTrace::SourceLocation::IsNotDefined () C_NE___
	{
		return	(sourceId	== 0)	and
				(begin._ul	== 0)	and
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

		FindAndReplace( INOUT filename, '\\', '/' );

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
				info.lines.emplace_back( uint(pos), uint(j) );
				pos = (++j) + 1;
			}
			else
			// linux style "\n" (or mac style "\r")
			if ( c == '\n' or c == '\r' )
			{
				info.lines.emplace_back( uint(pos), uint(j) );
				pos = j + 1;
			}
		}

		if ( pos < info.code.length() )
			info.lines.emplace_back( uint(pos), uint(info.code.length()) );

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

		result &= ser( _posOffset, _dataOffset, _initialPosition );
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
			_exprLocations.clear();
			_varNames.clear();
			_sources.clear();

			// _exprLocations
			{
				uint	count = 0;
				if_unlikely( not (des( OUT count ) and count < MaxCount) )
					return false;

				_exprLocations.resize( count );		// throw
				for (auto& item : _exprLocations) {
					if_unlikely( not Deserialize_ExprInfo( des, OUT item ))
						return false;
				}
			}

			if_unlikely( not des( OUT _varNames ))
				return false;

			// _sources
			{
				uint	count = 0;
				if_unlikely( not (des( OUT count ) and count < MaxCount) )
					return false;

				_sources.resize( count );		// throw
				for (auto& item : _sources) {
					if_unlikely( not Deserialize_SourceInfo( des, OUT item ))
						return false;
				}
			}

			return des( OUT _posOffset, OUT _dataOffset, OUT _initialPosition );
		}
		catch (...) {
			return false;
		}
	}


} // AE::PipelineCompiler
