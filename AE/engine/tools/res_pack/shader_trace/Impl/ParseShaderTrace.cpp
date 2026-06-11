// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Common.h"

#ifdef AE_ENABLE_GLSLANG
# include "glslang/Include/BaseTypes.h"
#endif

namespace AE::PipelineCompiler
{
namespace
{
	using namespace glslang;

#ifndef AE_ENABLE_GLSLANG
	enum TBasicType : ubyte {
		EbtVoid,
		EbtFloat,
		EbtDouble,
		EbtFloat16,
		EbtBFloat16,
		EbtFloatE5M2,
		EbtFloatE4M3,
		EbtInt8,
		EbtUint8,
		EbtInt16,
		EbtUint16,
		EbtInt,
		EbtUint,
		EbtInt64,
		EbtUint64,
		EbtBool,
		EbtAtomicUint,
		EbtSampler,
		EbtStruct,
		EbtBlock,
		EbtAccStruct,
		EbtReference,
		EbtRayQuery,
		EbtHitObjectNV,
		EbtHitObjectEXT,
		EbtCoopmat,
		EbtFunction,
		EbtTensorLayoutNV,
		EbtTensorViewNV,
		EbtCoopvecNV,
		EbtTensorARM,
		EbtLongVector,
		EbtSpirvType,
		EbtString,
		EbtNumTypes
	};
#endif
	StaticAssert( uint(TBasicType::EbtNumTypes) == 34 );


	//
	// Trace
	//

	struct Trace
	{
	public:
		using ExprInfo			= ShaderTrace::ExprInfo;
		using VarNames_t		= ShaderTrace::VarNames_t;
		using Sources_t			= ShaderTrace::Sources_t;
		using SourceLocation	= ShaderTrace::SourceLocation;
		using ELogFormat		= ShaderTrace::ELogFormat;
		using Swizzle			= ShaderTrace::Swizzle;
		using ActiveElems		= Bitfield< ushort >;

		struct VariableState
		{
			uint			valueOffset		= UMax;					// in '_storage'
			Swizzle			lastSwizzle;							// only for write access
			TBasicType		type			= TBasicType::EbtVoid;
			ActiveElems		hasValue;								// max 4x4
			bool			modified		= false;

			explicit VariableState (TBasicType t) : type{t} {}

			ND_ Tuple<Swizzle, uint, uint>  GetSwizzleRowsCols () const;
		};

		struct FnExecutionDuration
		{
			ulong			subgroup	= 0;
			ulong			device		= 0;
			uint			count		= 0;
		};

		using VarStates_t	= HashMap< VariableID, VariableState >;
		using Pending_t		= Array< VariableID >;
		using Profiling_t	= HashMap< ExprInfo const*, FnExecutionDuration >;


	public:
		uint				lastPosition	= ~0u;
	private:
		VarStates_t			_states;
		Pending_t			_pending;
		Profiling_t			_profiling;
		SourceLocation		_lastLoc;

		Unique<ulong[]>		_storage;
		uint				_storageSize	= 0;
		const uint			_storageCapacity;


	public:
		Trace ();

		ND_ bool  AddState (const ExprInfo &expr, TBasicType type, uint rows, uint cols, const uint* data,
							const VarNames_t &varNames, const Sources_t &src, ELogFormat format, INOUT String &result);

		ND_ bool  AddTime (const ExprInfo &expr, uint rows, uint cols, const uint* data);

		ND_ bool  Flush (const VarNames_t &varNames, const Sources_t &src, ELogFormat format, INOUT String &result);


	private:
		ND_ bool  _FlushStates (const VarNames_t &varNames, const Sources_t &src, ELogFormat format, INOUT String &result);
		ND_ bool  _FlushProfiling (const Sources_t &src, ELogFormat format, INOUT String &result);

		ND_ bool  _AllocValue (OUT void* &ptr, INOUT VariableState &var, usize elemCount);
	};

/*
=================================================
	Trace ctor
=================================================
*/
	Trace::Trace () : _storageCapacity{4 << 20}
	{
		_storage.reset( new ulong[_storageCapacity] );
	}

/*
=================================================
	VariableState::GetSwizzleRowsCols
=================================================
*/
	Tuple<Trace::Swizzle, uint, uint>  Trace::VariableState::GetSwizzleRowsCols () const
	{
		if ( modified )
		{
			uint	rows	= lastSwizzle.DstOrOriginRows();
			uint	cols	= lastSwizzle.OriginCols();

			if ( not lastSwizzle.IsIdentity() )
				cols = 1;

			ASSERT( rows > 0 and cols > 0 );
			return Tuple{ lastSwizzle, rows, cols };
		}
		else
		{
			Swizzle	sw;
			uint	rows	= lastSwizzle.OriginRows();
			uint	cols	= lastSwizzle.OriginCols();

			ASSERT( rows > 0 and cols > 0 );
			sw.SetIdentityMatrix( cols, rows );

			return Tuple{ sw, rows, cols };
		}
	}

/*
=================================================
	BasicTypeSizeOf
=================================================
*/
	inline uint  BasicTypeSizeOf (TBasicType type)
	{
		switch ( type )
		{
			case TBasicType::EbtBool :		return sizeof(bool);

			case TBasicType::EbtInt :
			case TBasicType::EbtUint :
			case TBasicType::EbtFloat16 :
			case TBasicType::EbtFloat :		return sizeof(int);

			case TBasicType::EbtInt64 :
			case TBasicType::EbtUint64 :
			case TBasicType::EbtDouble :	return sizeof(ulong);

			default :
				CHECK_MSG( false, "not supported" );
				return 0;
		}
	}

/*
=================================================
	CopyValue
=================================================
*/
	inline void  CopyValue (TBasicType type, INOUT void* ptr, uint valueIndex, const uint* data, INOUT uint &dataIndex)
	{
		switch ( type )
		{
			case TBasicType::EbtInt :
			case TBasicType::EbtUint :
			case TBasicType::EbtFloat :
			case TBasicType::EbtFloat16 :
				std::memcpy( OUT ptr + Bytes{sizeof(int) * valueIndex}, &data[dataIndex++], sizeof(int) );
				break;

			case TBasicType::EbtBool :
				Cast<bool>(ptr)[valueIndex] = (data[dataIndex++] != 0);
				break;

			case TBasicType::EbtInt64 :
			case TBasicType::EbtUint64 :
			case TBasicType::EbtDouble :
				std::memcpy( OUT ptr + Bytes{sizeof(int64_t) * valueIndex}, &data[dataIndex], sizeof(int64_t) );
				dataIndex += 2;
				break;

			default :
				CHECK_MSG( false, "not supported" );
				break;
		}
	}

/*
=================================================
	Trace::_AllocValue
=================================================
*/
	bool  Trace::_AllocValue (OUT void* &ptr, INOUT VariableState &var, const usize elemCount)
	{
		const uint	sz = BasicTypeSizeOf( var.type );

		if ( var.valueOffset == UMax )
		{
			var.valueOffset = AlignUp( _storageSize, sz );
			_storageSize	+= uint(sz * elemCount);

			if ( _storageSize > _storageCapacity )
			{
				var.valueOffset = UMax;
				return false;
			}
		}

		ptr = _storage.get() + Bytes{var.valueOffset};
		return true;
	}

/*
=================================================
	Trace::AddState
=================================================
*/
	bool  Trace::AddState (const ExprInfo &expr, TBasicType type, uint rows, uint cols, const uint* data,
						   const VarNames_t &varNames, const Sources_t &sources, ELogFormat format,
						   INOUT String &result)
	{
		if ( not (_lastLoc == expr.range) )
			CHECK_ERR( _FlushStates( varNames, sources, format, INOUT result ));

		const auto	AppendID = [this] (VariableID newID)
		{{
			for (auto& id : _pending) {
				if ( id == newID )
					return;
			}
			_pending.push_back( newID );
		}};

		auto	id   = expr.varID;
		auto	iter = _states.find( id );

		if ( iter == _states.end() or id == VariableID::Unknown )
			iter = _states.insert_or_assign( id, VariableState{ type }).first;

		VariableState&	var = iter->second;

		CHECK( id == VariableID::Unknown or var.type == type );
		var.type		= type;
		var.modified	= true;
		var.lastSwizzle	= Default;

		if ( var.type == TBasicType::EbtVoid )
		{
			ASSERT( var.valueOffset == UMax );
		}
		else
		if ( expr.swizzle.IsArray() )
		{
			// TODO
			ASSERT( false );
		}
		else
		if ( not expr.swizzle.IsUndefined() )	// update part of variable
		{
			void*	ptr;
			CHECK_ERR( _AllocValue( OUT ptr, INOUT var, expr.swizzle.OriginRows() * expr.swizzle.OriginCols() ));
			var.lastSwizzle = expr.swizzle;

			const uint	dst_rows = expr.swizzle.DstOrOriginRows();
			ASSERT( dst_rows == rows );
			ASSERT( cols == 1 );
			ASSERT( not expr.swizzle.IsIdentity() );

			if ( expr.swizzle.IsMatrix() )
			{
				uint	c = expr.swizzle.DstColumn();
				ASSERT( c < expr.swizzle.OriginCols() );

				for (uint i = 0, j = 0; i < rows; ++i)
				{
					uint	r = expr.swizzle[i];
					var.hasValue.Set( c * rows + r );
					CopyValue( type, INOUT ptr, c * rows + r, data, INOUT j );
				}
			}
			else
			{
				ASSERT( expr.swizzle.IsVector() );
				for (uint i = 0, j = 0; i < rows; ++i)
				{
					uint	r = expr.swizzle[i];
					var.hasValue.Set( r );
					CopyValue( type, INOUT ptr, r, data, INOUT j );
				}
			}
		}
		else	// update whole data of variable
		{
			void*	ptr;
			CHECK_ERR( _AllocValue( OUT ptr, INOUT var, cols * rows ));

			if ( cols > 1 )
			{
				var.lastSwizzle.SetIdentityMatrix( cols, rows );

				for (uint c = 0, j = 0; c < cols; ++c)
				{
					for (uint r = 0; r < rows; ++r)
					{
						var.hasValue.Set( c * rows + r );
						CopyValue( type, INOUT ptr, c * rows + r, data, INOUT j );
					}
				}
			}
			else
			{
				var.lastSwizzle.SetIdentityVector( rows );

				for (uint r = 0, j = 0; r < rows; ++r)
				{
					var.hasValue.Set( r );
					CopyValue( type, INOUT ptr, r, data, INOUT j );
				}
			}
		}

		AppendID( id );

		for (auto& var_id : expr.vars)
		{
			AppendID( var_id );
		}

		_lastLoc = expr.range;
		return true;
	}

/*
=================================================
	AddTime
=================================================
*/
	bool  Trace::AddTime (const ExprInfo &expr, uint, uint, const uint* data)
	{
		auto&	fn = _profiling[ &expr ];

		ulong	subgroup_begin;		std::memcpy( OUT &subgroup_begin, data + 0, sizeof(subgroup_begin) );
		ulong	subgroup_end;		std::memcpy( OUT &subgroup_end,   data + 4, sizeof(subgroup_end) );
		ulong	device_begin;		std::memcpy( OUT &device_begin,   data + 2, sizeof(device_begin) );
		ulong	device_end;			std::memcpy( OUT &device_end,     data + 6, sizeof(device_end) );

		if ( subgroup_end < subgroup_begin )
		{
			DBG_WARNING( "incorrect subgroup time" );
			subgroup_end = subgroup_begin;
		}

		if ( device_end < device_begin )
		{
			DBG_WARNING( "incorrect device time" );
			device_end = device_begin;
		}

		fn.count++;
		fn.subgroup += (subgroup_end - subgroup_begin);
		fn.device   += (device_end - device_begin);

		return true;
	}

/*
=================================================
	TypeToString
=================================================
*/
	Nd__In String  TypeToString (bool value)
	{
		return value ? "true" : "false";
	}

	Nd__In String  TypeToString (float value)
	{
		float	f		 = std::abs(value);
		bool	exp		 = f != 0.0f and (f < 1.0e-4f or f > 1.0e+5f);
		char	buf[128] = {};
		std::snprintf( buf, sizeof(buf), (exp ? "%1.6e" : "%0.6f"), double(value) );
		return buf;
	}

	Nd__In String  TypeToString (double value)
	{
		double	f		 = std::abs(value);
		bool	exp		 = f != 0.0 and (f < 1.0e-4 or f > 1.0e+5);
		char	buf[128] = {};
		std::snprintf( buf, sizeof(buf), (exp ? "%1.8e" : "%0.8f"), value );
		return buf;
	}

	Nd__In String  TypeToString (half value)
	{
		return TypeToString( value.Get() );
	}

	template <typename T>
	Nd__In String  TypeToString (T value)
	{
		StaticAssert( IsInteger<T> );
		return ToString( value );
	}

/*
=================================================
	SwizzleToString
=================================================
*/
	Nd__In String  SwizzleToString (Trace::Swizzle swizzle)
	{
		String		str;
		const char	sw[]	= "xyzw";
		const uint	rows	= swizzle.DstOrOriginRows();
		CHECK_ERR( rows > 0 and rows <= 4 );

		for (uint i = 0; i < rows; ++i)
		{
			str << sw[ swizzle[i] ];
		}
		return str;
	}

/*
=================================================
	TypeToString
=================================================
*/
	template <typename T>
	Nd__In String  TypeToString (Trace::Swizzle swizzle, uint rowOffset, Trace::ActiveElems hasValue, const T* values)
	{
		String		str;
		const uint	rows = swizzle.DstOrOriginRows();

		CHECK_ERR( rows > 0 and rows <= 4 );
		if ( rows > 1 )
			str << TypeToString( rows );

		str << " {";
		for (uint i = 0; i < rows; ++i)
		{
			uint	r = swizzle[i] + rowOffset;
			ASSERT( hasValue.Has( r ));

			str << (i ? ", " : "");

			if ( hasValue.Has( r ))
				str << TypeToString( values[r] );
			else
				str << "undefined";
		}
		str << '}';

		if constexpr( IsInteger<T> and not IsSame<T,bool> )
		{
			str << " | {";
			for (uint i = 0; i < rows; ++i)
			{
				uint	r = swizzle[i] + rowOffset;
				ASSERT( hasValue.Has( r ));

				str << (i ? ", " : "");

				if ( hasValue.Has( r ))
					str << "0x" << ToString<16>( values[r] );
				else
					str << "undefined";
			}
			str << '}';
		}
		str << '\n';
		return str;
	}

	template <typename Dst, typename Src>
	Nd__In String  TypeToString2 (Trace::Swizzle swizzle, uint rowOffset, Trace::ActiveElems hasValue, const Src* values)
	{
		String		str;
		const uint	rows = swizzle.DstOrOriginRows();

		CHECK_ERR( rows > 0 and rows <= 4 );
		if ( rows > 1 )
			str << TypeToString( rows );

		str << " {";
		for (uint i = 0; i < rows; ++i)
		{
			uint	r = swizzle[i] + rowOffset;
			ASSERT( hasValue.Has( r ));

			str << (i ? ", " : "");

			if ( hasValue.Has( r ))
				str << TypeToString( BitCastRlx<Dst>( values[r] ));
			else
				str << "undefined";
		}
		str << "}\n";

		return str;
	}

/*
=================================================
	AppendSourceRange
=================================================
*/
	ND_ static bool  AppendSourceRange (const Trace::SourceLocation &loc, const Trace::Sources_t &sources, Trace::ELogFormat format, INOUT String &result)
	{
		CHECK_ERR( loc.sourceId < sources.size() );

		const auto&		src			= sources[ loc.sourceId ];
		const uint		start_line	= Max( 1u, loc.begin.Line() ) - 1;	// because first line is 1
		const uint		end_line	= Max( 1u, loc.end  .Line() ) - 1;
		const uint		start_col	= Max( 1u, loc.begin.Column() ) - 1;
		const uint		end_col		= Max( 1u, loc.end  .Column() ) - 1;
		const uint		file_line	= loc.begin.Line() >= src.firstLine ? loc.begin.Line() - src.firstLine : 0;

		CHECK_ERR( start_line < src.lines.size() );
		CHECK_ERR( end_line < src.lines.size() );

		if ( loc.sourceId == 0 and end_line == 0 and end_col == 0 )
		{
			result << "no source\n";
			return true;
		}

		switch_enum( format )
		{
			// pattern: 'file (line): ...'
			case Trace::ELogFormat::VS_Console :
				result << "//  " << src.filename << " (" << ToString(file_line) << "):\n";
				break;

			// pattern: 'url (line)'
			case Trace::ELogFormat::FileURL :
				result << "//  file:///" << src.filename << " (" << ToString(file_line) << ")\n";
				break;

			// pattern: 'url#line'
			case Trace::ELogFormat::VSCode :
				result << "//  file:///" << src.filename << "#" << ToString(file_line) << "\n";
				break;

			case Trace::ELogFormat::Text :
			case Trace::ELogFormat::Unknown :
			case Trace::ELogFormat::_Count :	break;
		}
		switch_end

		for (uint i = start_line; i <= end_line; ++i)
		{
			result << ToString(i+1) + ". ";

			usize	start	= src.lines[i].first;
			usize	end		= src.lines[i].second;

			// TODO
			/*if ( i == end_line ) {
				CHECK_ERR( start + end_col < end );
				end = start + end_col;
			}*/
			if ( i == start_line ) {
				//CHECK_ERR( start + start_col < end );
				start += start_col;
			}

			if ( start < end )
			{
				result << SubStringBE( src.code, start, end );
				result << '\n';
			}
			else
				result << "invalid source location\n";
		}

		return true;
	}

/*
=================================================
	Trace::_FlushStates
=================================================
*/
	bool  Trace::_FlushStates (const VarNames_t &varNames, const Sources_t &sources, ELogFormat format, INOUT String &result)
	{
		const auto	Convert = [this, &varNames, INOUT &result] (VariableID id) -> bool
		{{
			auto	iter = _states.find( id );

			if ( iter == _states.end() )
				return false;

			auto&		var					= iter->second;
			auto		name				= varNames.find( id );
			const auto	[sw, rows, cols]	= var.GetSwizzleRowsCols();
			void*		ptr					= null;
			Swizzle		row_sw				= sw;	// same swizzle for each column
			const uint	dst_col				= sw.DstColumn();
			const bool	has_col_idx			= dst_col != UMax or cols > 1;
			const uint	active_rows			= has_col_idx ? sw.DstOrOriginRows() : 1;

			ASSERT_MSG( not ((dst_col != UMax) and (cols > 1)), "can not use both: whole matrix and dst column" );

			if ( has_col_idx )
			{
				ASSERT( rows == active_rows );
				row_sw.SetIdentityVector( active_rows );
			}

			if ( var.type != TBasicType::EbtVoid )
			{
				CHECK_ERR( var.valueOffset != UMax );
				ptr = _storage.get() + Bytes{var.valueOffset};
			}

			for (uint c = 0; c < cols; ++c)
			{
				uint	row = 0;

				if ( name != varNames.end() )
				{
					result << "//" << (var.modified ? "> " : "  ") << name->second;

					if ( has_col_idx )
					{
						row = dst_col != UMax ? dst_col : c;
						result << '[' << ToString( row ) << ']';
						row *= active_rows;
					}

					if ( not sw.IsIdentityRows() )
						result << '.' << SwizzleToString( sw );

					result << ": ";
				}
				else
					result << "//" << (var.modified ? "> (out): " : "  (temp): ");

				switch ( var.type )
				{
					case TBasicType::EbtVoid : {
						result << "void\n";
						break;
					}
					case TBasicType::EbtFloat : {
						result << "float";
						result << TypeToString( row_sw, row, var.hasValue, Cast<float>(ptr) );
						break;
					}
					case TBasicType::EbtDouble : {
						result << "double";
						result << TypeToString( row_sw, row, var.hasValue, Cast<double>(ptr) );
						break;
					}
					case TBasicType::EbtInt : {
						result << "int";
						result << TypeToString( row_sw, row, var.hasValue, Cast<int>(ptr) );
						break;
					}
					case TBasicType::EbtBool : {
						result << "bool";
						result << TypeToString( row_sw, row, var.hasValue, Cast<bool>(ptr) );
						break;
					}
					case TBasicType::EbtUint : {
						result << "uint";
						result << TypeToString( row_sw, row, var.hasValue, Cast<uint>(ptr) );
						break;
					}
					case TBasicType::EbtInt64 : {
						result << "long";
						result << TypeToString( row_sw, row, var.hasValue, Cast<slong>(ptr) );
						break;
					}
					case TBasicType::EbtUint64 : {
						result << "ulong";
						result << TypeToString( row_sw, row, var.hasValue, Cast<ulong>(ptr) );
						break;
					}
					case TBasicType::EbtFloat16 : {
						result << "half";
						result << TypeToString2<half>( row_sw, row, var.hasValue, Cast<uint>(ptr) );		// TODO: use half
						break;
					}
					default :
						RETURN_ERR( "not supported" );
				}
			}

			var.modified = false;
			return true;
		}};

		if ( _pending.empty() )
			return true;

		for (auto& id : _pending) {
			Convert( id );
		}
		_pending.clear();

		CHECK_ERR( AppendSourceRange( _lastLoc, sources, format, INOUT result ));

		result << '\n';

		return true;
	}

/*
=================================================
	Trace::_FlushProfiling
=================================================
*/
	bool  Trace::_FlushProfiling (const Sources_t &sources, ELogFormat format, INOUT String &result)
	{
		if ( _profiling.empty() )
			return true;

		// sort by device time
		Array< Pair<ExprInfo const* const, FnExecutionDuration>* >	sorted;
		sorted.reserve( _profiling.size() );

		for (auto& item : _profiling) {
			sorted.push_back( &item );
		}

		std::sort( sorted.begin(), sorted.end(), [](auto* lhs, auto *rhs) { return lhs->second.device > rhs->second.device; });

		// print
		const double	max_subgroup_time	= sorted.front()->second.subgroup ? 100.0 / double(sorted.front()->second.subgroup) : 1.0;
		const double	max_device_time		= sorted.front()->second.device ? 100.0 / double(sorted.front()->second.device) : 1.0;

		const auto		DtoStr = [] (double value) -> String
		{{
			char		buf[32] = {};
			const int	len = std::snprintf( buf, sizeof(buf), "%0.2f", value );

			if ( len <= 0 )
				buf[0] = '\0';

			return buf;
		}};

		for (auto* item : sorted)
		{
			auto&	expr	= *item->first;
			auto&	time	= item->second;

			result << "// subgroup total: " << DtoStr( double(time.subgroup) * max_subgroup_time ) << "%,  avr: " <<
						DtoStr( (double(time.subgroup) * max_subgroup_time) / double(time.count) ) << "%,  (" <<
						DtoStr( double(time.subgroup) / double(time.count) ) << ")\n";

			result << "// device   total: " << DtoStr( double(time.device) * max_device_time ) << "%,  avr: " <<
						DtoStr( (double(time.device) * max_device_time) / double(time.count) ) << "%,  (" <<
						DtoStr( double(time.device) / double(time.count) ) + ")\n";

			result << "// invocations:    " << ToString( time.count ) << "\n";

			CHECK_ERR( AppendSourceRange( expr.range, sources, format, INOUT result ));
			result << "\n";
		}

		return true;
	}

/*
=================================================
	Trace::Flush
=================================================
*/
	bool  Trace::Flush (const VarNames_t &varNames, const Sources_t &sources, ELogFormat format, INOUT String &result)
	{
		CHECK_ERR( _FlushStates( varNames, sources, format, INOUT result ));
		CHECK_ERR( _FlushProfiling( sources, format, INOUT result ));

		return true;
	}

/*
=================================================
	TypeSizeOf
=================================================
*/
	Nd__In uint  TypeSizeOf (uint type)
	{
		switch ( type )
		{
			case TBasicType::EbtVoid :
			case TBasicType::EbtFloat :
			case TBasicType::EbtInt :
			case TBasicType::EbtBool :
			case TBasicType::EbtUint :
				return 1;

			case TBasicType::EbtDouble :
			case TBasicType::EbtInt64 :
			case TBasicType::EbtUint64 :
				return 2;

			case TBasicType::EbtFloat16 :
				return 1;	// as uint

			case ShaderTrace::TBasicType_Clock :
				return 2;
		}
		RETURN_ERR( "not supported" );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  ShaderTrace::ParseShaderTrace (const void* ptr, const Bytes inMaxSize, ELogFormat format, OUT Array<String> &result) const
	{
		if ( format == Default )
			format = ELogFormat::Text;

		result.clear();

		if ( _exprLocations.empty() )
		{
			ASSERT_MSG( _assertLocations.empty(),
				"ShaderTrace module initialized to record asserts, but used to get trace."
				"You should call 'ParseAsserts()' method instead." );
			return true;
		}

		const ulong		count = *(static_cast<uint const*>(ptr) + _posOffset / sizeof(uint));
		if ( count == 0 )
			return true;

		const ulong		max_size	= ulong(inMaxSize);
		uint const*		start_ptr	= static_cast<uint const*>(ptr) + _dataOffset / sizeof(uint);
		uint const*		end_ptr		= start_ptr + Min( count, (max_size - _dataOffset) / sizeof(uint) );
		Array<Trace>	shaders;	// buffer may contain traces from multiple invocations and shaders

		ASSERT( (count * sizeof(uint)) <= max_size );

		for (auto data_ptr = start_ptr; data_ptr < end_ptr;)
		{
			uint		pos			= uint(PtrDistance( start_ptr, data_ptr ));
			uint		prev_pos	= *(data_ptr++);
			uint		expr_id		= *(data_ptr++);
			uint		type		= *(data_ptr++);
			uint		t_basic		= (type & 0xFF);
			uint		row_size	= (type >> 8) & 0xF;					// for scalar, vector and matrix
			uint		col_size	= Max( 1u, (type >> 12) & 0xF );		// only for matrix
			uint const*	data		= data_ptr;
			Trace*		trace		= null;

			CHECK_ERR( (t_basic == uint(TBasicType::EbtVoid) and row_size == 0) or (row_size > 0 and row_size <= 4) );
			CHECK_ERR( col_size > 0 and col_size <= 4 );

			data_ptr += (row_size * col_size) * TypeSizeOf( t_basic );
			ASSERT( data_ptr <= end_ptr );

			// find existing shader trace
			for (auto& sh : shaders)
			{
				if ( sh.lastPosition == prev_pos ) {
					trace = &sh;
					break;
				}
			}

			if ( not trace )
			{
				if ( prev_pos == _initialPosition )
				{
					// create new shader trace
					shaders.push_back( Trace{} );
					result.resize( shaders.size() );
					trace = &shaders.back();
				}
				else
				{
					// this entry from another shader, skip it
					continue;
				}
			}

			CHECK_ERR( expr_id < _exprLocations.size() );

			auto&	expr = _exprLocations[ expr_id ];
			auto&	str  = result[ PtrDistance( shaders.data(), trace )];

			if ( t_basic == ShaderTrace::TBasicType_Clock )
				CHECK_ERR( trace->AddTime( expr, row_size, col_size, data ))
			else
				CHECK_ERR( trace->AddState( expr, TBasicType(t_basic), row_size, col_size, data, _varNames, _sources, format, INOUT str ));

			trace->lastPosition = pos;
		}

		for (usize i = 0; i < shaders.size(); ++i)
		{
			CHECK_ERR( shaders[i].Flush( _varNames, _sources, format, INOUT result[i] ));
		}
		return true;
	}

} // AE::PipelineCompiler
