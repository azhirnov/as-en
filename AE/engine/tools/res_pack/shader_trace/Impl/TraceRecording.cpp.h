// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Common.h"

namespace AE::PipelineCompiler
{
namespace
{
	using namespace glslang;


	//
	// Debug Info
	//

	struct DebugInfo
	{
	public:
		using SrcLoc		= ShaderTrace::SourceLocation;
		using SrcPoint		= ShaderTrace::SourcePoint;
		using ExprInfos_t	= ShaderTrace::ExprInfos_t;
		using VarNames_t	= ShaderTrace::VarNames_t;

		struct StackFrame
		{
			TIntermNode*		node			= null;
			SrcLoc				loc;
		};

		struct VariableInfo
		{
			VariableID			id;
			String				name;
			Array<TSourceLoc>	locations;
		};

		struct FnCallLocation
		{
			String			fnName;
			TSourceLoc		loc;

			ND_ bool  operator == (const FnCallLocation &) const;
		};

		struct FnCallLocationHash {
			ND_ usize  operator () (const FnCallLocation &) const;
		};

		struct FieldInfo
		{
			int		baseId		= 0;
			int		fieldIndex	= 0;

			ND_ bool  operator == (const FieldInfo &) const;
		};

		struct FieldInfoHash {
			ND_ usize  operator () (const FieldInfo &) const;
		};

		using SymbolLocations_t		= HashMap< int, Array<TIntermSymbol *> >;
		using RequiredFunctions_t	= HashSet< TString >;
		using CachedSymbols_t		= HashMap< TString, TIntermSymbol *>;
		using VariableInfoMap_t		= HashMap< slong, VariableInfo >;
		using FnCallMap_t			= HashMap< FnCallLocation, VariableInfo, FnCallLocationHash >;
		using StructFieldMap_t		= HashMap< FieldInfo, VariableInfo, FieldInfoHash >;
		using CallStack_t			= Array< StackFrame >;
		using FileMap_t				= ShaderTrace::FileMap_t;
		using StartTimeStack_t		= Array< TIntermSymbol* >;
		using StartTimeNodes_t		= Array< TIntermSymbol* >;

		static constexpr int	InvalidSymbolID = -1;


	public:
		CallStack_t				_callStack;

	  #ifdef PROFILER
		StartTimeStack_t		_startTimeStack;
		StartTimeNodes_t		_uniqueStartTimes;
	  #else
		TIntermTyped *			_injection			= null;
	  #endif

		RequiredFunctions_t		_requiredFunctions;
		CachedSymbols_t			_cachedSymbols;

		ExprInfos_t &			_exprLocations;
		VariableInfoMap_t		_varInfos;
		FnCallMap_t				_fnCallMap;
		StructFieldMap_t		_fieldMap;

		slong					_maxSymbolId				= 0;
		bool					_startedUserDefinedSymbols	= false;

		const bool				_shaderSubgroupClock		= false;
		const bool				_shaderDeviceClock			= false;

		TIntermSymbol *			_dbgStorage					= null;

		FileMap_t const&		_includedFilesMap;

		const String			_entryName;

		const EShLanguage		_shLang;


	public:
	  #ifdef PROFILER
		DebugInfo (const TIntermediate &intermediate, OUT ExprInfos_t &exprLoc, const FileMap_t &includedFiles,
				   bool shaderSubgroupClock, bool shaderDeviceClock) :
			_exprLocations{ exprLoc },
			_shaderSubgroupClock{ shaderSubgroupClock },
			_shaderDeviceClock{ shaderDeviceClock },
			_includedFilesMap{ includedFiles },
			_entryName{ intermediate.getEntryPointMangledName() },
			_shLang{ intermediate.getStage() }
		{}
	  #else
		DebugInfo (const TIntermediate &intermediate, OUT ExprInfos_t &exprLoc, const FileMap_t &includedFiles) :
			_exprLocations{ exprLoc },
			_includedFilesMap{ includedFiles },
			_entryName{ intermediate.getEntryPointMangledName() },
			_shLang{ intermediate.getStage() }
		{}
	  #endif

			void				Enter (TIntermNode* node);
			void				Leave (TIntermNode* node);

			bool				PostProcess (OUT VarNames_t &);

	  #ifdef PROFILER
			bool				PushStartTime (TIntermSymbol* node);
			bool				PopStartTime ();
			bool				UpdateSymbolIDs ();

		ND_ TIntermSymbol*		GetStartTime ();
		ND_ TIntermSymbol*		CreateStartTimeSymbolNode ();
	  #else

		ND_ TIntermTyped*		GetInjection ();
			void				InjectNode (TIntermTyped* node);
	  #endif

		ND_ CallStack_t const&	GetCallStack ()										const	{ return _callStack; }
		ND_ TIntermAggregate*	GetCurrentFunction ();


		ND_ uint				GetSourceLocation (TIntermNode* node, const TSourceLoc &curr, bool setColumnToZero = false);
		ND_ uint				GetCustomSourceLocation (TIntermNode* node, const TSourceLoc &curr);
		ND_ uint				GetCustomSourceLocation2 (TIntermNode* node, const TSourceLoc &begin, const TSourceLoc &end);

		ND_ SrcLoc				GetCurrentLocation ()								const	{ return _callStack.back().loc; }
			void				AddLocation (const TSourceLoc &loc);
			void				AddLocation (const SrcLoc &src);

			void				RequestFunc (const TString &fname)							{ _requiredFunctions.insert( fname ); }
		ND_ auto const&			GetRequiredFunctions ()								const	{ return _requiredFunctions; }

			void				AddSymbol (TIntermSymbol* node, Bool isUserDefined = True{});
		ND_ slong				GetUniqueSymbolID ();

		ND_ StringView			GetEntryPoint ()									const	{ return _entryName; }

			void				CacheSymbolNode (TIntermSymbol* node, Bool isUserDefined = True{});
		ND_ TIntermSymbol*		GetCachedSymbolNode (const TString &name);

		ND_ TIntermSymbol*		GetDebugStorage ()									const	{ CHECK( _dbgStorage != null );  return _dbgStorage; }
			bool				SetDebugStorage (TIntermSymbol* symb);
		ND_ TIntermBinary*		GetDebugStorageField (const char* name)				const;

		ND_ EShLanguage			GetShaderType ()									const	{ return _shLang; }


	private:
			void				_GetVariableID (TIntermNode* node, OUT VariableID &id, OUT uint &swizzle);
		ND_ uint				_GetSourceId (const TSourceLoc &) const;
	};


#ifdef PROFILER
	struct FunctionScope
	{
		DebugInfo &						_info;
		TIntermNode *					_node;
		ShaderTrace::SourceLocation		_location;
		bool							_hasLocation = false;

		FunctionScope (TIntermNode* node, DebugInfo &info) : _info{info}, _node{node}
		{
			_info.Enter( _node );
		}

		~FunctionScope ()
		{
			_info.Leave( _node );

			// propagate source location to root
			if ( _hasLocation )
				_info.AddLocation( _location );
		}

		void  SetLoc (const ShaderTrace::SourceLocation &loc)
		{
			_location		= loc;
			_hasLocation	= true;
		}
	};
#endif
//-----------------------------------------------------------------------------



	static void  CreateShaderDebugStorage (uint descSetIndex, DebugInfo &dbgInfo, OUT ulong &posOffset, OUT ulong &dataOffset);
	static void  CreateShaderBuiltinSymbols (TIntermNode* root, DebugInfo &dbgInfo);
ND_ static bool  CreateDebugTraceFunctions (TIntermNode* root, uint initialPosition, DebugInfo &dbgInfo);
ND_ static TIntermAggregate*  RecordShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo);


/*
=================================================
	CacheSymbolNode / GetCachedSymbolNode
=================================================
*/
void  DebugInfo::CacheSymbolNode (TIntermSymbol* node, Bool isUserDefined)
{
	_cachedSymbols.emplace( node->getName(), node );
	AddSymbol( node, isUserDefined );
}

ND_ TIntermSymbol*  DebugInfo::GetCachedSymbolNode (const TString &name)
{
	auto	iter = _cachedSymbols.find( name );
	return	iter != _cachedSymbols.end() ? iter->second : null;
}

/*
=================================================
	Enter
=================================================
*/
void  DebugInfo::Enter (TIntermNode* node)
{
	CHECK_ERRV( node != null );

	StackFrame	frame;
	frame.node			= node;
	frame.loc.sourceId	= _GetSourceId( node->getLoc() );
	frame.loc.begin		= SrcPoint{ node->getLoc() };
	frame.loc.end		= frame.loc.begin;

	_callStack.push_back( frame );
}

/*
=================================================
	Leave
=================================================
*/
void  DebugInfo::Leave (TIntermNode* node)
{
	CHECK( _callStack.back().node == node );
	_callStack.pop_back();
}

/*
=================================================
	GetCustomSourceLocation
=================================================
*/
uint  DebugInfo::GetCustomSourceLocation (TIntermNode* node, const TSourceLoc &curr)
{
	VariableID	id;
	uint		swizzle;
	_GetVariableID( node, OUT id, OUT swizzle );

	SrcLoc	range{ 0, uint(curr.line), uint(curr.column) };
	range.sourceId = _GetSourceId( curr );

	_exprLocations.emplace_back( id, swizzle, range, range.begin );
	return uint(_exprLocations.size()-1);
}

/*
=================================================
	GetCustomSourceLocation2
=================================================
*/
uint  DebugInfo::GetCustomSourceLocation2 (TIntermNode* node, const TSourceLoc &begin, const TSourceLoc &end)
{
	VariableID	id;
	uint		swizzle;
	_GetVariableID( node, OUT id, OUT swizzle );

	SrcLoc	range { 0, uint(begin.line), uint(begin.column) };
	SrcLoc	range2{ 0, uint(end.line),   uint(end.column)   };
	range.sourceId  = _GetSourceId( begin );
	range2.sourceId = _GetSourceId( end );

	ASSERT( range.sourceId == range2.sourceId );

	range.begin.SetMin( range2.begin );
	range.end  .SetMax( range2.end );

	_exprLocations.emplace_back( id, swizzle, range, range.begin );
	return uint(_exprLocations.size()-1);
}

/*
=================================================
	AddLocation
=================================================
*/
void  DebugInfo::AddLocation (const TSourceLoc &loc)
{
	return AddLocation({ _GetSourceId( loc ), uint(loc.line), uint(loc.column) });
}

void  DebugInfo::AddLocation (const SrcLoc &src)
{
	if ( _callStack.empty() or src.IsNotDefined() )
		return;

	auto&	dst = _callStack.back().loc;

	if ( dst.IsNotDefined() )
	{
		dst = src;
		return;
	}

	CHECK( src.sourceId == dst.sourceId );
	dst.begin.SetMin( src.begin );
	dst.end  .SetMax( src.end );
}

/*
=================================================
	SetDebugStorage
=================================================
*/
bool  DebugInfo::SetDebugStorage (TIntermSymbol* symb)
{
	if ( _dbgStorage )
		return true;

	CHECK_ERR( symb and symb->getType().isStruct() );

	_dbgStorage = symb;
	return true;
}

/*
=================================================
	GetSourceLocation
=================================================
*/
uint  DebugInfo::GetSourceLocation (TIntermNode* node, const TSourceLoc &curr, bool setColumnToZero)
{
	VariableID	id;
	uint		swizzle;
	_GetVariableID( node, OUT id, OUT swizzle );

	SrcPoint	point	{ curr };
	SrcLoc		range	= _callStack.back().loc;
	uint		src_id	= _GetSourceId( curr );

	ASSERT( range.sourceId != ~0u );
	ASSERT( range.sourceId == src_id );
	Unused( src_id );

	range.begin.SetMin( point );
	range.end  .SetMax( point );

	if ( setColumnToZero )
		range.begin._packed.column = 0;

	_exprLocations.emplace_back( id, swizzle, range, point );
	return uint(_exprLocations.size()-1);
}

/*
=================================================
	GetCurrentFunction
=================================================
*/
TIntermAggregate*  DebugInfo::GetCurrentFunction ()
{
	for (auto iter = _callStack.rbegin(), end = _callStack.rend(); iter != end; ++iter)
	{
		TIntermAggregate*	aggr = iter->node->getAsAggregate();

		if ( aggr and aggr->getOp() == TOperator::EOpFunction )
			return aggr;
	}
	return null;
}

/*
=================================================
	GetDebugStorageField
=================================================
*/
TIntermBinary*  DebugInfo::GetDebugStorageField (const char* name) const
{
	CHECK_ERR( _dbgStorage != null );

	TPublicType		index_type;		index_type.init( Default );
	index_type.basicType			= TBasicType::EbtInt;
	index_type.qualifier.storage	= TStorageQualifier::EvqConst;

	for (auto& field : *_dbgStorage->getType().getStruct())
	{
		if ( field.type->getFieldName() == name )
		{
			const auto				index			= Distance( _dbgStorage->getType().getStruct()->data(), &field );
			TConstUnionArray		index_Value(1);	index_Value[0].setIConst( int(index) );
			TIntermConstantUnion*	field_index		= new TIntermConstantUnion{ index_Value, TType{index_type} };
			TIntermBinary*			field_access	= new TIntermBinary{ TOperator::EOpIndexDirectStruct };
			field_access->setType( *field.type );
			field_access->setLeft( _dbgStorage );
			field_access->setRight( field_index );
			return field_access;
		}
	}
	return null;
}

/*
=================================================
	AddSymbol
=================================================
*/
void  DebugInfo::AddSymbol (TIntermSymbol* node, Bool isUserDefined)
{
	NonNull( node );
	ASSERT( isUserDefined or not _startedUserDefinedSymbols );
	Unused( isUserDefined );

	_maxSymbolId = Max( _maxSymbolId, node->getId() );

  #ifndef PROFILER
	// register symbol
	VariableID	id;
	uint		sw;
	_GetVariableID( node, OUT id, OUT sw );
  #endif
}

/*
=================================================
	GetUniqueSymbolID
=================================================
*/
slong  DebugInfo::GetUniqueSymbolID ()
{
	_startedUserDefinedSymbols = true;
	return ++_maxSymbolId;
}

/*
=================================================
	IsBuiltinFunction
=================================================
*/
ND_ inline bool  IsBuiltinFunction (TOperator op)
{
  #if 0 // not PROFILER and HIGH_DETAIL_TRACE
	if ( op > TOperator::EOpPreDecrement and op < TOperator::EOpAdd )
		return true;
  #endif

	return	(op > TOperator::EOpRadians			and op < TOperator::EOpKill)	or
			(op > TOperator::EOpArrayLength		and op < TOperator::EOpClip);
}

/*
=================================================
	IsDebugFunction
=================================================
*/
ND_ inline bool  IsDebugFunction (TIntermOperator* node)
{
	auto*	aggr = node->getAsAggregate();

	if ( not (aggr and aggr->getOp() == TOperator::EOpFunctionCall) )
		return false;

  #ifdef PROFILER
	return aggr->getName().rfind( "dbg_EnableProfiling", 0 ) == 0;
  #else
	return aggr->getName().rfind( "dbg_EnableTraceRecording", 0 ) == 0;
  #endif
}

/*
=================================================
	FnCallLocation::operator ==
=================================================
*/
inline bool  DebugInfo::FnCallLocation::operator == (const FnCallLocation &rhs) const
{
	return loc == rhs.loc and fnName == rhs.fnName;
}

/*
=================================================
	FnCallLocationHash::operator ()
=================================================
*/
inline usize  DebugInfo::FnCallLocationHash::operator () (const FnCallLocation &value) const
{
	HashVal		fn_hash		= HashOf( value.fnName );
	HashVal		name_hash	= value.loc.name ? HashOf( *value.loc.name ) : HashVal{};

	return	usize( fn_hash + name_hash +
				HashVal{ usize(value.loc.string) << 24 } +
				HashVal{ usize(value.loc.line) << 8 } +
				HashVal{ usize(value.loc.column) << 18 });
}

/*
=================================================
	FieldInfo::operator ==
=================================================
*/
inline bool  DebugInfo::FieldInfo::operator == (const FieldInfo &rhs) const
{
	return	baseId		== rhs.baseId	and
			fieldIndex	== rhs.fieldIndex;
}

/*
=================================================
	FieldInfoHash::operator ()
=================================================
*/
inline usize  DebugInfo::FieldInfoHash::operator () (const FieldInfo &value) const
{
	return	usize(value.baseId) ^
			(usize(value.fieldIndex) << (CT_SizeOfInBits<usize>-16));
}

/*
=================================================
	GetVectorSwizzleMask
=================================================
*/
ND_ static uint  GetVectorSwizzleMask (TIntermBinary* binary)
{
	Array<uint>				sw_mask;
	Array<TIntermBinary*>	swizzle_op;		swizzle_op.push_back( binary );

	CHECK_ERR( binary and (binary->getOp() == TOperator::EOpVectorSwizzle or binary->getOp() == TOperator::EOpIndexDirect) );

	// extract swizzle mask
	for (TIntermTyped* node = binary->getLeft();
		 node->getAsBinaryNode() and node->getAsBinaryNode()->getOp() == TOperator::EOpVectorSwizzle;)
	{
		swizzle_op.push_back( node->getAsBinaryNode() );

		node = swizzle_op.back()->getLeft();
	}

	binary = swizzle_op.back();

	const auto ProcessUnion = [&sw_mask] (TIntermConstantUnion* cu, const Array<uint> &mask) -> bool
	{
		TConstUnionArray const&	cu_arr = cu->getConstArray();
		CHECK_ERR( cu_arr.size() == 1 and cu->getType().getBasicType() == EbtInt );
		CHECK_ERR( cu_arr[0].getType() == EbtInt and cu_arr[0].getIConst() >= 0 and cu_arr[0].getIConst() < 4 );

		if ( mask.empty() )
			sw_mask.push_back( cu_arr[0].getIConst() );
		else
			sw_mask.push_back( mask[ cu_arr[0].getIConst() ]);
		return true;
	};

	// optimize swizzle
	for (auto iter = swizzle_op.rbegin(); iter != swizzle_op.rend(); ++iter)
	{
		TIntermBinary*		bin		= (*iter);
		const Array<uint>	mask	= sw_mask;

		sw_mask.clear();

		if ( TIntermAggregate* aggr = bin->getRight()->getAsAggregate() )
		{
			CHECK_ERR( aggr->getOp() == TOperator::EOpSequence );

			for (auto& node : aggr->getSequence())
			{
				if ( auto* cu = node->getAsConstantUnion() )
					CHECK_ERR( ProcessUnion( cu, mask ));
			}
		}
		else
		if ( auto* cu = bin->getRight()->getAsConstantUnion() )
		{
			CHECK_ERR( ProcessUnion( cu, mask ));
		}
		else
			RETURN_ERR( "not supported!" );
	}

	uint	result	= 0;
	uint	shift	= 0;
	for (auto& idx : sw_mask)
	{
		result |= ((idx + 1) << shift);
		shift  += 3;
	}
	return result;
}

/*
=================================================
	_GetVariableID
=================================================
*/
void  DebugInfo::_GetVariableID (TIntermNode* node, OUT VariableID &id, OUT uint &swizzle)
{
	if ( node == null )
	{
		CHECK( false );
		return;
	}

	id = VariableID(~0u);
	swizzle = 0;

	const VariableID	new_id = VariableID(uint(_varInfos.size() + _fnCallMap.size() + _fieldMap.size()));

	if ( TIntermSymbol* symb = node->getAsSymbolNode() )
	{
		if ( symb->getId() == InvalidSymbolID )
			return;

		auto	iter = _varInfos.find( symb->getId() );
		if ( iter == _varInfos.end() )
			iter = _varInfos.emplace( symb->getId(), VariableInfo{ new_id, symb->getName().c_str(), Default }).first;

		auto&	locations = iter->second.locations;

		if ( locations.empty() or locations.back() != symb->getLoc() )
			locations.push_back( symb->getLoc() );

		id = iter->second.id;
		return;
	}

	if ( TIntermBinary* binary = node->getAsBinaryNode() )
	{
		// vector swizzle
		if ( binary->getOp() == TOperator::EOpVectorSwizzle or
			(binary->getOp() == TOperator::EOpIndexDirect and not binary->getLeft()->isArray() and
			(binary->getLeft()->isScalar() or binary->getLeft()->isVector())) )
		{
			swizzle = GetVectorSwizzleMask( binary );

			uint	temp;
			return _GetVariableID( binary->getLeft(), OUT id, OUT temp );
		}
		else
		// matrix swizzle
		if ( binary->getOp() == TOperator::EOpIndexDirect and
			 binary->getLeft()->isMatrix() )
		{
			swizzle = GetVectorSwizzleMask( binary );

			uint	temp;
			return _GetVariableID( binary->getLeft(), OUT id, OUT temp );
		}
		else
		// array element
		if ( binary->getOp() == TOperator::EOpIndexDirect and
			 binary->getLeft()->isArray() )
		{
			swizzle = 0;	// TODO

			uint	temp;
			return _GetVariableID( binary->getLeft(), OUT id, OUT temp );
		}
		else
		if ( binary->getOp() == TOperator::EOpIndexIndirect )
		{}
		else
		if ( binary->getOp() == TOperator::EOpIndexDirectStruct )
		{
			// TODO
			/*FieldInfo	field;
			TString		name;
			if ( GetStructFieldInfo( binary, OUT field, OUT name ))
			{
				auto	iter = _fieldMap.find( field );
				if ( iter == _fieldMap.end() )
					iter = _fieldMap.emplace( field, VariableInfo{ new_id, name.c_str(), Default }).first;

				auto&	locations = iter->second.locations;

				if ( locations.empty() or locations.back() != node->getLoc() )
					locations.push_back( node->getLoc() );

				id = iter->second.id;
				return;
			}*/
			return; // temp
		}
		else
			return;	// it hasn't any ID
	}

	if ( TIntermOperator* op = node->getAsOperator() )
	{
		// temporary variable returned by function
		if ( IsBuiltinFunction( op->getOp() ) or op->getOp() == TOperator::EOpFunctionCall )
		{
			auto	name = GetFunctionName( op ) + "()";
			auto	iter = _fnCallMap.find({ name, op->getLoc() });

			if ( iter == _fnCallMap.end() )
				iter = _fnCallMap.emplace( FnCallLocation{ name, op->getLoc() }, VariableInfo{ new_id, name, Default }).first;

			auto&	locations = iter->second.locations;

			if ( locations.empty() or locations.back() != op->getLoc() )
				locations.push_back( op->getLoc() );

			id = iter->second.id;
			return;
		}
		return;	// it hasn't any ID
	}

	if ( node->getAsBranchNode() or node->getAsSelectionNode() )
		return;	// no ID for branches

	CHECK(false);
}

/*
=================================================
	_GetSourceId
=================================================
*/
uint  DebugInfo::_GetSourceId (const TSourceLoc &loc) const
{
	if ( loc.name != null )
	{
		auto	iter = _includedFilesMap.find( loc.name->c_str() );
		CHECK( iter != _includedFilesMap.end() );
		//CHECK( loc.string == 0 );

		return iter->second;
	}
	else
		return loc.string;
}

/*
=================================================
	PostProcess
=================================================
*/
bool  DebugInfo::PostProcess (OUT VarNames_t &varNames)
{
	const auto	SearchInExpressions = [&] (VariableInfo &info)
	{{
		std::sort(	info.locations.begin(), info.locations.end(),
					[] (auto& lhs, auto& rhs) { return lhs < rhs; });

		bool	is_unused = true;

		for (auto& expr : _exprLocations)
		{
			if ( expr.varID == info.id ) {
				is_unused = false;
				continue;
			}

			for (auto& loc : info.locations)
			{
				uint	loc_line	= uint(loc.line);
				uint	loc_column	= uint(loc.column);
				uint	source_id	= _GetSourceId( loc );

				if ( source_id == 0 and loc_line == 0 and loc_column == 0 )
					continue;	// skip undefined location

				// check intersection
				if ( source_id != expr.range.sourceId )
					continue;

				if ( loc_line < expr.range.begin.Line() or loc_line > expr.range.end.Line() )
					continue;

				if ( loc_line == expr.range.begin.Line() and loc_column < expr.range.begin.Column() )
					continue;

				if ( loc_line == expr.range.end.Line() and loc_column > expr.range.end.Column() )
					continue;

				bool	exist = false;
				for (auto& v : expr.vars) {
					exist |= (v == info.id);
				}

				if ( not exist )
					expr.vars.push_back( info.id );

				is_unused = false;
			}
		}

		if ( not is_unused )
			varNames.insert_or_assign( info.id, RVRef(info.name) );
	}};

	for (auto& info : _varInfos) {
		SearchInExpressions( info.second );
	}
	for (auto& info : _fnCallMap) {
		SearchInExpressions( info.second );
	}
	for (auto& info : _fieldMap) {
		SearchInExpressions( info.second );
	}
	return true;
}
//-----------------------------------------------------------------------------


#ifdef PROFILER
/*
=================================================
	UpdateSymbolIDs
=================================================
*/
bool  DebugInfo::UpdateSymbolIDs ()
{
	for (auto* symb : _uniqueStartTimes)
	{
		if ( symb->getId() == InvalidSymbolID )
		{
			symb->changeId( GetUniqueSymbolID() );
		}
	}
	return true;
}

/*
=================================================
	PushStartTime
=================================================
*/
bool  DebugInfo::PushStartTime (TIntermSymbol* node)
{
	CHECK_ERR( node != null );

	_startTimeStack.push_back( node );
	return true;
}

/*
=================================================
	PopStartTime
=================================================
*/
bool  DebugInfo::PopStartTime ()
{
	CHECK_ERR( not _startTimeStack.empty() );

	_startTimeStack.pop_back();
	return true;
}

/*
=================================================
	GetStartTime
=================================================
*/
TIntermSymbol*  DebugInfo::GetStartTime ()
{
	CHECK_ERR( not _startTimeStack.empty() );

	return _startTimeStack.back();
}

/*
=================================================
	CreateStartTimeSymbolNode
=================================================
*/
TIntermSymbol*	DebugInfo::CreateStartTimeSymbolNode ()
{
	TPublicType			uint_type;	uint_type.init( Default );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.vectorSize			= 4;
	uint_type.qualifier.storage		= TStorageQualifier::EvqTemporary;
	uint_type.qualifier.precision	= TPrecisionQualifier::EpqHigh;

	TIntermSymbol*	node = new TIntermSymbol{ InvalidSymbolID, TString{"dbg_StartTime_"} + ToString(_uniqueStartTimes.size()).c_str(), TType{uint_type} };

	_uniqueStartTimes.push_back( node );

	return node;
}

//-----------------------------------------------------------------------------
#else // not PROFILER

/*
=================================================
	GetInjection
=================================================
*/
TIntermTyped*  DebugInfo::GetInjection ()
{
	auto	temp = _injection;
	_injection = null;
	return temp;
}

/*
=================================================
	InjectNode
=================================================
*/
void  DebugInfo::InjectNode (TIntermTyped* node)
{
	if ( node == null )
		return;

	CHECK( not _injection );
	_injection = node;
}

#endif // PROFILER
//-----------------------------------------------------------------------------



ND_ static TIntermAggregate*  CreateAppendToTrace (TIntermTyped* exprNode, uint sourceLoc, DebugInfo &dbgInfo);

/*
=================================================
	CreateGraphicsShaderDebugStorage
=================================================
*/
static void  CreateGraphicsShaderDebugStorage (TTypeList* typeList, INOUT TPublicType &type)
{
	type.basicType = TBasicType::EbtInt;

	TType*	fragcoord_x	= new TType{type};		fragcoord_x->setFieldName( "fragCoordX" );
	type.qualifier.layoutOffset += sizeof(int);

	TType*	fragcoord_y	= new TType{type};		fragcoord_y->setFieldName( "fragCoordY" );
	type.qualifier.layoutOffset += sizeof(int);

	TType*	padding	= new TType{type};			padding->setFieldName( "padding1" );
	type.qualifier.layoutOffset += sizeof(int);

	typeList->push_back( TTypeLoc{ fragcoord_x,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ fragcoord_y,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ padding,		TSourceLoc{} });
}

/*
=================================================
	CreateComputeShaderDebugStorage
=================================================
*/
static void  CreateComputeShaderDebugStorage (TTypeList* typeList, INOUT TPublicType &type)
{
	type.basicType = TBasicType::EbtUint;

	TType*	thread_id_x	= new TType{type};		thread_id_x->setFieldName( "globalInvocationX" );
	type.qualifier.layoutOffset += sizeof(uint);

	TType*	thread_id_y	= new TType{type};		thread_id_y->setFieldName( "globalInvocationY" );
	type.qualifier.layoutOffset += sizeof(uint);

	TType*	thread_id_z	= new TType{type};		thread_id_z->setFieldName( "globalInvocationZ" );
	type.qualifier.layoutOffset += sizeof(uint);

	typeList->push_back( TTypeLoc{ thread_id_x,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ thread_id_y,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ thread_id_z,	TSourceLoc{} });
}

/*
=================================================
	CreateRayTracingShaderDebugStorage
=================================================
*/
static void  CreateRayTracingShaderDebugStorage (TTypeList* typeList, INOUT TPublicType &type)
{
	type.basicType = TBasicType::EbtUint;

	TType*	thread_id_x	= new TType{type};		thread_id_x->setFieldName( "launchID_x" );
	type.qualifier.layoutOffset += sizeof(uint);

	TType*	thread_id_y	= new TType{type};		thread_id_y->setFieldName( "launchID_y" );
	type.qualifier.layoutOffset += sizeof(uint);

	TType*	thread_id_z	= new TType{type};		thread_id_z->setFieldName( "launchID_z" );
	type.qualifier.layoutOffset += sizeof(uint);

	typeList->push_back( TTypeLoc{ thread_id_x,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ thread_id_y,	TSourceLoc{} });
	typeList->push_back( TTypeLoc{ thread_id_z,	TSourceLoc{} });
}

/*
=================================================
	CreateShaderDebugStorage
=================================================
*/
static void  CreateShaderDebugStorage (uint descSetIndex, DebugInfo &dbgInfo, OUT ulong &posOffset, OUT ulong &dataOffset)
{
	//	staticSize: 16, arrayStride: 4
	//	layout(binding=x, std430) buffer dbg_ShaderTraceStorage {
	//		union {
	//		  // graphics
	//			readonly int  fragCoordX;
	//			readonly int  fragCoordY;
	//			readonly int  padding1;
	//		  // compute
	//			readonly uint  globalInvocationX;
	//			readonly uint  globalInvocationY;
	//			readonly uint  globalInvocationZ;
	//		  // ray tracing
	//			readonly uint  launchID_x;
	//			readonly uint  launchID_y;
	//			readonly uint  launchID_z;
	//		};
	//		coherent uint  position;
	//		restrict uint  outData [];
	//	} dbg_ShaderTrace;

	TPublicType		uint_type;			uint_type.init( Default );
	uint_type.basicType					= TBasicType::EbtUint;
	uint_type.qualifier.storage			= TStorageQualifier::EvqBuffer;
	uint_type.qualifier.layoutMatrix	= TLayoutMatrix::ElmColumnMajor;
	uint_type.qualifier.layoutPacking	= TLayoutPacking::ElpStd430;
	uint_type.qualifier.precision		= TPrecisionQualifier::EpqHigh;
	uint_type.qualifier.layoutOffset	= 0;

	TTypeList*		type_list	= new TTypeList{};
	TPublicType		temp		= uint_type;

	switch_enum( dbgInfo.GetShaderType() )
	{
		case EShLangVertex :
		case EShLangTessControl :
		case EShLangTessEvaluation :
		case EShLangGeometry :
		case EShLangFragment :
		case EShLangTask :
		case EShLangMesh :				CreateGraphicsShaderDebugStorage( type_list, INOUT temp );		break;

		case EShLangCompute :			CreateComputeShaderDebugStorage( type_list, INOUT temp );		break;

		case EShLangRayGen :
		case EShLangIntersect :
		case EShLangAnyHit :
		case EShLangClosestHit :
		case EShLangMiss :
		case EShLangCallable :			CreateRayTracingShaderDebugStorage( type_list, INOUT temp );	break;

		case EShLangCount :
		default :						CHECK(false); return;
	}
	switch_end

#if USE_STORAGE_QUALIFIERS
	uint_type.qualifier.coherent = true;
#endif
	uint_type.qualifier.layoutOffset = temp.qualifier.layoutOffset;

	TType*			position	= new TType{uint_type};		position->setFieldName( "position" );

	uint_type.qualifier.layoutOffset += sizeof(uint);
	uint_type.arraySizes			 = new TArraySizes{};
	uint_type.arraySizes->addInnerSize();

#if USE_STORAGE_QUALIFIERS
	uint_type.qualifier.coherent	= false;
	uint_type.qualifier.restrict	= true;
	uint_type.qualifier.writeonly	= true;
#endif

	TType*			data_arr	= new TType{uint_type};		data_arr->setFieldName( "outData" );

	type_list->push_back( TTypeLoc{ position,	TSourceLoc{} });
	type_list->push_back( TTypeLoc{ data_arr,	TSourceLoc{} });

	TQualifier		block_qual;	block_qual.clear();
	block_qual.storage			= TStorageQualifier::EvqBuffer;
	block_qual.layoutMatrix		= TLayoutMatrix::ElmColumnMajor;
	block_qual.layoutPacking	= TLayoutPacking::ElpStd430;
	block_qual.layoutBinding	= 0;
	block_qual.layoutSet		= descSetIndex;

	TIntermSymbol*	storage_buf	= new TIntermSymbol{ 0x10000001, "dbg_ShaderTrace", TType{type_list, "dbg_ShaderTraceStorage", block_qual} };

	posOffset  = position->getQualifier().layoutOffset;
	dataOffset = data_arr->getQualifier().layoutOffset;

	dbgInfo.SetDebugStorage( storage_buf );
}

/*
=================================================
	CreateShaderBuiltinSymbols
=================================================
*/
static void  CreateShaderBuiltinSymbols (TIntermNode*, DebugInfo &dbgInfo)
{
	const auto	shader				= dbgInfo.GetShaderType();
	const bool	is_compute			= (shader == EShLangCompute or shader == EShLangTask or shader == EShLangMesh);
	const bool	need_invocation_id	= (shader == EShLangGeometry or shader == EShLangTessControl);
	const bool	need_primitive_id	= (shader == EShLangTessControl or shader == EShLangTessEvaluation);
	const bool	need_launch_id		= (shader == EShLangRayGen or shader == EShLangIntersect or shader == EShLangAnyHit or
									   shader == EShLangClosestHit or shader == EShLangMiss or shader == EShLangCallable);
	TSourceLoc	loc		{};

	// find default source location
	/*if ( auto* aggr = root->getAsAggregate() )
	{
		for (auto& node : aggr->getSequence())
		{
			auto*	fn = node->getAsAggregate();

			if ( fn and fn->getOp() == TOperator::EOpFunction and fn->getName() == dbgInfo.GetEntryPoint() )
			{
				loc = fn->getLoc();
				break;
			}
		}
	}*/

	if ( shader == EShLangFragment and not dbgInfo.GetCachedSymbolNode( "gl_FragCoord" ))
	{
		TPublicType		vec4_type;	vec4_type.init( Default );
		vec4_type.basicType			= TBasicType::EbtFloat;
		vec4_type.vectorSize		= 4;
		vec4_type.qualifier.storage	= TStorageQualifier::EvqFragCoord;
		vec4_type.qualifier.builtIn	= TBuiltInVariable::EbvFragCoord;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_FragCoord", TType{vec4_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	// "Any static use of this variable in a fragment shader causes the entire shader to be evaluated per-sample rather than per-fragment."
	// so don't add 'gl_SampleID' and 'gl_SamplePosition' if it doesn't exists.

	if ( need_primitive_id and not dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		int_type.qualifier.builtIn	= TBuiltInVariable::EbvPrimitiveId;
		int_type.qualifier.flat		= true;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_PrimitiveID", TType{int_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( is_compute and not dbgInfo.GetCachedSymbolNode( "gl_GlobalInvocationID" ))
	{
		TPublicType		uint_type;	uint_type.init( Default );
		uint_type.basicType			= TBasicType::EbtUint;
		uint_type.vectorSize		= 3;
		uint_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		uint_type.qualifier.builtIn	= TBuiltInVariable::EbvGlobalInvocationId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_GlobalInvocationID", TType{uint_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( is_compute and not dbgInfo.GetCachedSymbolNode( "gl_LocalInvocationID" ))
	{
		TPublicType		uint_type;	uint_type.init( Default );
		uint_type.basicType			= TBasicType::EbtUint;
		uint_type.vectorSize		= 3;
		uint_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		uint_type.qualifier.builtIn	= TBuiltInVariable::EbvLocalInvocationId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_LocalInvocationID", TType{uint_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( is_compute and not dbgInfo.GetCachedSymbolNode( "gl_WorkGroupID" ))
	{
		TPublicType		uint_type;	uint_type.init( Default );
		uint_type.basicType			= TBasicType::EbtUint;
		uint_type.vectorSize		= 3;
		uint_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		uint_type.qualifier.builtIn	= TBuiltInVariable::EbvWorkGroupId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_WorkGroupID", TType{uint_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( need_invocation_id and not dbgInfo.GetCachedSymbolNode( "gl_InvocationID" ))
	{
		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		int_type.qualifier.builtIn	= TBuiltInVariable::EbvInvocationId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_InvocationID", TType{int_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangGeometry and not dbgInfo.GetCachedSymbolNode( "gl_PrimitiveIDIn" ))
	{
		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		int_type.qualifier.builtIn	= TBuiltInVariable::EbvPrimitiveId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_PrimitiveIDIn", TType{int_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangTessEvaluation and not dbgInfo.GetCachedSymbolNode( "gl_TessCoord" ))
	{
		TPublicType		float_type;		float_type.init( Default );
		float_type.basicType			= TBasicType::EbtFloat;
		float_type.vectorSize			= 3;
		float_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvTessCoord;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_TessCoord", TType{float_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangTessEvaluation and not dbgInfo.GetCachedSymbolNode( "gl_TessLevelInner" ))
	{
		TPublicType		float_type;		float_type.init( Default );
		float_type.basicType			= TBasicType::EbtFloat;
		float_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvTessLevelInner;
		float_type.arraySizes			= new TArraySizes{};
		float_type.arraySizes->addInnerSize( 2 );

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_TessLevelInner", TType{float_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangTessEvaluation and not dbgInfo.GetCachedSymbolNode( "gl_TessLevelOuter" ))
	{
		TPublicType		float_type;		float_type.init( Default );
		float_type.basicType			= TBasicType::EbtFloat;
		float_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvTessLevelOuter;
		float_type.arraySizes			= new TArraySizes{};
		float_type.arraySizes->addInnerSize( 4 );

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_TessLevelOuter", TType{float_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( need_launch_id and not dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" ))
	{
		TPublicType		uint_type;	uint_type.init( Default );
		uint_type.basicType			= TBasicType::EbtUint;
		uint_type.vectorSize		= 3;
		uint_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		uint_type.qualifier.builtIn	= TBuiltInVariable::EbvLaunchId;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_LaunchIDEXT", TType{uint_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangVertex and not dbgInfo.GetCachedSymbolNode( "gl_VertexIndex" ))
	{
		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		int_type.qualifier.builtIn	= TBuiltInVariable::EbvVertexIndex;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_VertexIndex", TType{int_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}

	if ( shader == EShLangVertex and not dbgInfo.GetCachedSymbolNode( "gl_InstanceIndex" ))
	{
		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.qualifier.storage	= TStorageQualifier::EvqVaryingIn;
		int_type.qualifier.builtIn	= TBuiltInVariable::EbvInstanceIndex;

		TIntermSymbol*	symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "gl_InstanceIndex", TType{int_type} };
		symb->setLoc( loc );
		dbgInfo.CacheSymbolNode( symb );
	}
}

/*
=================================================
	CreateAppendToTraceBody2
----
	also see 'CreateAppendToTrace2()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAppendToTraceBody2 (DebugInfo &dbgInfo)
{
	TPublicType		uint_type;	uint_type.init( Default );
	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;

	// last_pos, location, size, value
	const uint			dbg_data_size = 3;

	TIntermAggregate*	fn_node		= new TIntermAggregate{ TOperator::EOpFunction };
	TIntermAggregate*	fn_args		= new TIntermAggregate{ TOperator::EOpParameters };
	TIntermAggregate*	fn_body		= new TIntermAggregate{ TOperator::EOpSequence };
	TIntermAggregate*	branch_body = new TIntermAggregate{ TOperator::EOpSequence };

	// build function body
	{
		fn_node->setType( TType{ TBasicType::EbtVoid, TStorageQualifier::EvqGlobal } );
		fn_body->setType( TType{ TBasicType::EbtVoid } );
		fn_node->setName( "dbg_AppendToTrace(u1;" );
		fn_node->getSequence().push_back( fn_args );
		fn_node->getSequence().push_back( fn_body );
	}

	// build function argument sequence
	{
		TIntermSymbol*	arg0 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "sourceLocation", TType{uint_type} };
		fn_args->setType( TType{EbtVoid} );
		fn_args->getSequence().push_back( arg0 );
	}

	// "pos" variable
	uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
	TIntermSymbol*	var_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "pos", TType{uint_type} };

	// "uint pos = atomicAdd( dbg_ShaderTrace.position, x );"
	{
		TIntermAggregate*	move_pos	= new TIntermAggregate{ TOperator::EOpSequence };
		TIntermBinary*		assign_op	= new TIntermBinary{ TOperator::EOpAssign };			// pos = ...

		branch_body->setType( TType{EbtVoid} );
		branch_body->getSequence().push_back( move_pos );

		move_pos->setType( TType{EbtVoid} );
		move_pos->getSequence().push_back( assign_op );

		uint_type.qualifier.storage = TStorageQualifier::EvqConst;
		TConstUnionArray		data_size_value(1);	data_size_value[0].setUConst( dbg_data_size );
		TIntermConstantUnion*	data_size	= new TIntermConstantUnion{ data_size_value, TType{uint_type} };

		TIntermBinary*			pos_field	= dbgInfo.GetDebugStorageField( "position" );
		CHECK_ERR( pos_field != null );

		TIntermAggregate*		add_op		= new TIntermAggregate{ TOperator::EOpAtomicAdd };		// atomicAdd
		uint_type.qualifier.storage = TStorageQualifier::EvqGlobal;
		add_op->setType( TType{uint_type} );
		add_op->setOperationPrecision( TPrecisionQualifier::EpqHigh );
		add_op->getQualifierList().push_back( TStorageQualifier::EvqInOut );
		add_op->getQualifierList().push_back( TStorageQualifier::EvqIn );
		add_op->getSequence().push_back( pos_field );
		add_op->getSequence().push_back( data_size );

		assign_op->setType( TType{uint_type} );
		assign_op->setLeft( var_pos );
		assign_op->setRight( add_op );
	}

	// "dbg_ShaderTrace.outData[pos++] = ..."
	{
		uint_type.qualifier.storage = TStorageQualifier::EvqConst;
		TConstUnionArray		type_value(1);	type_value[0].setUConst( (uint(TBasicType::EbtVoid) & 0xFF) );
		TIntermConstantUnion*	type_id			= new TIntermConstantUnion{ type_value, TType{uint_type} };
		TConstUnionArray		const_value(1);	const_value[0].setUConst( 1 );
		TIntermConstantUnion*	const_one		= new TIntermConstantUnion{ const_value, TType{uint_type} };

		uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
		TIntermBinary*			assign_data0	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			assign_data1	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			assign_data2	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			indexed_access	= new TIntermBinary{ TOperator::EOpIndexIndirect };
		TIntermUnary*			inc_pos			= new TIntermUnary{ TOperator::EOpPostIncrement };
		TIntermSymbol*			last_pos		= dbgInfo.GetCachedSymbolNode( "dbg_LastPosition" );
		TIntermBinary*			new_last_pos	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			prev_pos		= new TIntermBinary{ TOperator::EOpSub };
		TIntermBinary*			out_data_field	= dbgInfo.GetDebugStorageField( "outData" );

		CHECK_ERR( last_pos and out_data_field );

		// "pos++"
		inc_pos->setType( TType{uint_type} );
		inc_pos->setOperand( var_pos );

		// "dbg_ShaderTrace.outData[pos++]"
		indexed_access->setType( TType{uint_type} );
		indexed_access->getWritableType().setFieldName( "outData" );
		indexed_access->setLeft( out_data_field );
		indexed_access->setRight( inc_pos );

		// "dbg_ShaderTrace.outData[pos++] = dbg_LastPosition"
		assign_data0->setType( TType{uint_type} );
		assign_data0->setLeft( indexed_access );
		assign_data0->setRight( last_pos );
		branch_body->getSequence().push_back( assign_data0 );

		// "pos - 1"
		prev_pos->setType( TType{uint_type} );
		prev_pos->setLeft( var_pos );
		prev_pos->setRight( const_one );

		// "dbg_LastPosition = pos - 1"
		new_last_pos->setType( TType{uint_type} );
		new_last_pos->setLeft( last_pos );
		new_last_pos->setRight( prev_pos );
		branch_body->getSequence().push_back( new_last_pos );

		// "dbg_ShaderTrace.outData[pos++] = sourceLocation"
		assign_data1->setType( TType{uint_type} );
		assign_data1->setLeft( indexed_access );
		assign_data1->setRight( fn_args->getSequence()[0]->getAsTyped() );
		branch_body->getSequence().push_back( assign_data1 );

		// "dbg_ShaderTrace.outData[pos++] = typeid"
		assign_data2->setType( TType{uint_type} );
		assign_data2->setLeft( indexed_access );
		assign_data2->setRight( type_id );
		branch_body->getSequence().push_back( assign_data2 );
	}

	// "if ( dbg_IsEnabled )"
	{
		TIntermSymbol*		condition	= dbgInfo.GetCachedSymbolNode( "dbg_IsEnabled" );
		CHECK_ERR( condition != null );

		TIntermSelection*	selection	= new TIntermSelection{ condition, branch_body, null };
		selection->setType( TType{EbtVoid} );

		fn_body->getSequence().push_back( selection );
	}

	return fn_node;
}

/*
=================================================
	CreateAppendToTraceBody
----
	also see 'CreateAppendToTrace()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAppendToTraceBody (const TString &fnName, DebugInfo &dbgInfo)
{
	TPublicType		value_type;	value_type.init( Default );
	TPublicType		uint_type;	uint_type.init( Default );
	TPublicType		index_type;	index_type.init( Default );

	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;

	index_type.basicType		= TBasicType::EbtInt;
	index_type.qualifier.storage= TStorageQualifier::EvqConst;

	// extract type
	uint	scale = 1;
	{
		usize	pos  = fnName.find( '(' );
		usize	pos1 = fnName.find( ';', pos );
		CHECK_ERR( pos != TString::npos and pos1 != TString::npos and pos < pos1 );
		++pos;

		const bool	is_vector	= (fnName[pos] == 'v');
		const bool	is_matrix	= (fnName[pos] == 'm');

		if ( is_vector or is_matrix )	++pos;

		const bool	is_i64	= (pos+2 < fnName.size() and fnName[pos+1] == '6' and fnName[pos+2] == '4');

		switch ( fnName[pos] )
		{
			case 'h' :	value_type.basicType = TBasicType::EbtFloat16;	++pos;	CHECK( fnName[pos] == 'f' );	break;
			case 'f' :	value_type.basicType = TBasicType::EbtFloat;	break;
			case 'd' :	value_type.basicType = TBasicType::EbtDouble;	scale = 2;	break;
			case 'b' :	value_type.basicType = TBasicType::EbtBool;		break;
			case 'i' :	if ( is_i64 ) { value_type.basicType = TBasicType::EbtInt64;  scale = 2; } else value_type.basicType = TBasicType::EbtInt;	break;
			case 'u' :	if ( is_i64 ) { value_type.basicType = TBasicType::EbtUint64; scale = 2; } else value_type.basicType = TBasicType::EbtUint;	break;
			default  :	RETURN_ERR( "unknown type" );
		}
		++pos;

		if ( is_matrix ) {
			value_type.setMatrix( fnName[pos] - '0', fnName[pos+1] - '0' );
			CHECK_ERR(	value_type.matrixCols > 0 and value_type.matrixCols <= 4 and
						value_type.matrixRows > 0 and value_type.matrixRows <= 4 );
		}
		else
		if ( is_vector ) {
			value_type.setVector( fnName[pos] - '0' );
			CHECK_ERR( value_type.vectorSize > 0 and value_type.vectorSize <= 4 );
		}
		else {
			// scalar
			value_type.vectorSize = 1;
		}
	}

	// last_pos, location, size, value
	const uint			dbg_data_size = (value_type.matrixCols * value_type.matrixRows + value_type.vectorSize) * scale + 3;

	TIntermAggregate*	fn_node		= new TIntermAggregate{ TOperator::EOpFunction };
	TIntermAggregate*	fn_args		= new TIntermAggregate{ TOperator::EOpParameters };
	TIntermAggregate*	fn_body		= new TIntermAggregate{ TOperator::EOpSequence };
	TIntermAggregate*	branch_body = new TIntermAggregate{ TOperator::EOpSequence };

	// build function body
	{
		value_type.qualifier.storage = TStorageQualifier::EvqGlobal;
		fn_node->setType( TType{value_type} );
		fn_node->setName( fnName );
		fn_node->getSequence().push_back( fn_args );
		fn_node->getSequence().push_back( fn_body );
	}

	// build function argument sequence
	{
		value_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;
		TIntermSymbol*		arg0	 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "value", TType{value_type} };
		TIntermSymbol*		arg1	 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "sourceLocation", TType{uint_type} };

		fn_args->setType( TType{EbtVoid} );
		fn_args->getSequence().push_back( arg0 );
		fn_args->getSequence().push_back( arg1 );
	}

	// build function body
	{
		value_type.qualifier.storage = TStorageQualifier::EvqTemporary;
		fn_body->setType( TType{value_type} );
	}

	// "pos" variable
	uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
	TIntermSymbol*	var_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "pos", TType{uint_type} };

	// "uint pos = atomicAdd( dbg_ShaderTrace.position, x );"
	{
		TIntermAggregate*	move_pos	= new TIntermAggregate{ TOperator::EOpSequence };
		TIntermBinary*		assign_op	= new TIntermBinary{ TOperator::EOpAssign };			// pos = ...

		branch_body->setType( TType{EbtVoid} );
		branch_body->getSequence().push_back( move_pos );

		move_pos->setType( TType{EbtVoid} );
		move_pos->getSequence().push_back( assign_op );

		uint_type.qualifier.storage = TStorageQualifier::EvqConst;
		TConstUnionArray		data_size_value(1);	data_size_value[0].setUConst( dbg_data_size );
		TIntermConstantUnion*	data_size	= new TIntermConstantUnion{ data_size_value, TType{uint_type} };

		TIntermBinary*			pos_field	= dbgInfo.GetDebugStorageField( "position" );
		CHECK_ERR( pos_field != null );

		TIntermAggregate*		add_op		= new TIntermAggregate{ TOperator::EOpAtomicAdd };		// atomicAdd
		uint_type.qualifier.storage = TStorageQualifier::EvqGlobal;
		add_op->setType( TType{uint_type} );
		add_op->setOperationPrecision( TPrecisionQualifier::EpqHigh );
		add_op->getQualifierList().push_back( TStorageQualifier::EvqInOut );
		add_op->getQualifierList().push_back( TStorageQualifier::EvqIn );
		add_op->getSequence().push_back( pos_field );
		add_op->getSequence().push_back( data_size );

		assign_op->setType( TType{uint_type} );
		assign_op->setLeft( var_pos );
		assign_op->setRight( add_op );
	}

	// "dbg_ShaderTrace.outData[pos++] = ..."
	{
		uint_type.qualifier.storage = TStorageQualifier::EvqConst;
		TConstUnionArray		type_value(1);	type_value[0].setUConst( (uint(value_type.basicType) & 0xFF) | ((value_type.vectorSize & 0xF) << 8) |
																		 ((value_type.matrixRows & 0xF) << 8) | ((value_type.matrixCols & 0xF) << 12) );
		TIntermConstantUnion*	type_id			= new TIntermConstantUnion{ type_value, TType{uint_type} };
		TConstUnionArray		const_value(1);	const_value[0].setUConst( 1 );
		TIntermConstantUnion*	const_one		= new TIntermConstantUnion{ const_value, TType{uint_type} };

		uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
		TIntermBinary*			assign_data0	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			assign_data1	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			assign_data2	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			indexed_access	= new TIntermBinary{ TOperator::EOpIndexIndirect };
		TIntermUnary*			inc_pos			= new TIntermUnary{ TOperator::EOpPostIncrement };
		TIntermSymbol*			last_pos		= dbgInfo.GetCachedSymbolNode( "dbg_LastPosition" );
		TIntermBinary*			new_last_pos	= new TIntermBinary{ TOperator::EOpAssign };
		TIntermBinary*			prev_pos		= new TIntermBinary{ TOperator::EOpSub };
		TIntermBinary*			out_data_field	= dbgInfo.GetDebugStorageField( "outData" );

		CHECK_ERR( last_pos and out_data_field );

		// "pos++"
		inc_pos->setType( TType{uint_type} );
		inc_pos->setOperand( var_pos );

		// "dbg_ShaderTrace.outData[pos++]"
		indexed_access->setType( TType{uint_type} );
		indexed_access->getWritableType().setFieldName( "outData" );
		indexed_access->setLeft( out_data_field );
		indexed_access->setRight( inc_pos );

		// "dbg_ShaderTrace.outData[pos++] = dbg_LastPosition"
		assign_data0->setType( TType{uint_type} );
		assign_data0->setLeft( indexed_access );
		assign_data0->setRight( last_pos );
		branch_body->getSequence().push_back( assign_data0 );

		// "pos - 1"
		prev_pos->setType( TType{uint_type} );
		prev_pos->setLeft( var_pos );
		prev_pos->setRight( const_one );

		// "dbg_LastPosition = pos - 1"
		new_last_pos->setType( TType{uint_type} );
		new_last_pos->setLeft( last_pos );
		new_last_pos->setRight( prev_pos );
		branch_body->getSequence().push_back( new_last_pos );

		// "dbg_ShaderTrace.outData[pos++] = sourceLocation"
		assign_data1->setType( TType{uint_type} );
		assign_data1->setLeft( indexed_access );
		assign_data1->setRight( fn_args->getSequence()[1]->getAsTyped() );
		branch_body->getSequence().push_back( assign_data1 );

		// "dbg_ShaderTrace.outData[pos++] = typeid"
		assign_data2->setType( TType{uint_type} );
		assign_data2->setLeft( indexed_access );
		assign_data2->setRight( type_id );
		branch_body->getSequence().push_back( assign_data2 );

		// "ToUint(...)"
		const auto	TypeToUint	= [] (TIntermTyped* operand, int index) -> TIntermTyped*
		{{
			TPublicType		utype;	utype.init( Default );
			utype.basicType			= TBasicType::EbtUint;
			utype.qualifier.storage = TStorageQualifier::EvqGlobal;

			switch_enum( operand->getType().getBasicType() )
			{
				case TBasicType::EbtFloat : {
					TIntermUnary*	as_uint = new TIntermUnary{ TOperator::EOpFloatBitsToUint };
					as_uint->setType( TType{utype} );
					as_uint->setOperand( operand );
					as_uint->setOperationPrecision( TPrecisionQualifier::EpqHigh );
					return as_uint;
				}
				case TBasicType::EbtFloat16 : {
					TIntermUnary*	as_uint16 = new TIntermUnary{ TOperator::EOpFloat16BitsToUint16 };
					utype.basicType = TBasicType::EbtUint16;
					as_uint16->setType( TType{utype} );
					as_uint16->setOperand( operand );
					as_uint16->setOperationPrecision( TPrecisionQualifier::EpqHigh );

					TIntermUnary*	to_uint = new TIntermUnary{ TOperator::EOpConvUint16ToUint };
					utype.basicType	= TBasicType::EbtUint;
					utype.qualifier.storage	= TStorageQualifier::EvqGlobal;
					to_uint->setType( TType{utype} );
					to_uint->setOperand( as_uint16 );
					return to_uint;
				}
				case TBasicType::EbtInt : {
					TIntermUnary*	to_uint = new TIntermUnary{ TOperator::EOpConvIntToUint };
					to_uint->setType( TType{utype} );
					to_uint->setOperand( operand );
					return to_uint;
				}
				case TBasicType::EbtUint : {
					return operand;
				}
				case TBasicType::EbtBool : {
					TIntermUnary*	to_uint = new TIntermUnary{ TOperator::EOpConvBoolToUint };
					to_uint->setType( TType{utype} );
					to_uint->setOperand( operand );
					return to_uint;
				}
				case TBasicType::EbtDouble : {
					// "doubleBitsToUint64(value)"
					TIntermUnary*	as_uint64 = new TIntermUnary{ TOperator::EOpDoubleBitsToUint64 };
					utype.basicType = TBasicType::EbtUint64;
					as_uint64->setType( TType{utype} );
					as_uint64->setOperand( operand );
					as_uint64->setOperationPrecision( TPrecisionQualifier::EpqHigh );

					// "doubleBitsToUint64(value) >> x"
					utype.qualifier.storage = TStorageQualifier::EvqConst;
					TConstUnionArray		shift_value(1);	shift_value[0].setU64Const( (index&1)*32 );
					TIntermConstantUnion*	const_shift		= new TIntermConstantUnion{ shift_value, TType{utype} };
					TIntermBinary*			shift			= new TIntermBinary{ TOperator::EOpRightShift };
					utype.qualifier.storage	= TStorageQualifier::EvqTemporary;
					shift->setType( TType{utype} );
					shift->setLeft( as_uint64 );
					shift->setRight( const_shift );

					// "uint(doubleBitsToUint64(value) >> x)"
					TIntermUnary*			to_uint = new TIntermUnary{ TOperator::EOpConvUint64ToUint };
					utype.basicType			= TBasicType::EbtUint;
					utype.qualifier.storage	= TStorageQualifier::EvqGlobal;
					to_uint->setType( TType{utype} );
					to_uint->setOperand( shift );
					return to_uint;
				}
				case TBasicType::EbtInt64 : {
					// "value >> x"
					utype.basicType			= TBasicType::EbtInt64;
					utype.qualifier.storage = TStorageQualifier::EvqConst;
					TConstUnionArray		shift_value(1);	shift_value[0].setI64Const( (index&1)*32 );
					TIntermConstantUnion*	const_shift		= new TIntermConstantUnion{ shift_value, TType{utype} };
					TIntermBinary*			shift			= new TIntermBinary{ TOperator::EOpRightShift };
					utype.qualifier.storage	= TStorageQualifier::EvqTemporary;
					shift->setType( TType{utype} );
					shift->setLeft( operand );
					shift->setRight( const_shift );

					// "uint(value >> x)"
					TIntermUnary*			to_uint = new TIntermUnary{ TOperator::EOpConvInt64ToUint };
					utype.basicType			= TBasicType::EbtUint;
					utype.qualifier.storage	= TStorageQualifier::EvqGlobal;
					to_uint->setType( TType{utype} );
					to_uint->setOperand( shift );
					return to_uint;
				}
				case TBasicType::EbtUint64 : {
					// "value >> x"
					utype.basicType			= TBasicType::EbtUint64;
					utype.qualifier.storage = TStorageQualifier::EvqConst;
					TConstUnionArray		shift_value(1);	shift_value[0].setU64Const( (index&1)*32 );
					TIntermConstantUnion*	const_shift		= new TIntermConstantUnion{ shift_value, TType{utype} };
					TIntermBinary*			shift			= new TIntermBinary{ TOperator::EOpRightShift };
					utype.qualifier.storage	= TStorageQualifier::EvqTemporary;
					shift->setType( TType{utype} );
					shift->setLeft( operand );
					shift->setRight( const_shift );

					// "uint(value >> x)"
					TIntermUnary*			to_uint = new TIntermUnary{ TOperator::EOpConvUint64ToUint };
					utype.basicType			= TBasicType::EbtUint;
					utype.qualifier.storage	= TStorageQualifier::EvqGlobal;
					to_uint->setType( TType{utype} );
					to_uint->setOperand( shift );
					return to_uint;
				}
				case TBasicType::EbtVoid :
				case TBasicType::EbtInt8 :
				case TBasicType::EbtUint8 :
				case TBasicType::EbtInt16 :
				case TBasicType::EbtUint16 :
				case TBasicType::EbtAtomicUint :
				case TBasicType::EbtSampler :
				case TBasicType::EbtStruct :
				case TBasicType::EbtBlock :
				case TBasicType::EbtReference :
				case TBasicType::EbtString :
				case TBasicType::EbtNumTypes :
				case TBasicType::EbtAccStruct :
				case TBasicType::EbtRayQuery :
				case TBasicType::EbtSpirvType :
				case TBasicType::EbtHitObjectNV :
				case TBasicType::EbtCoopmat :
					break;
			}
			switch_end
			RETURN_ERR( "not supported" );
		}};

		// "dbg_ShaderTrace.outData[pos++] = ToUint(value)"
		if ( value_type.isScalar() )
		{
			for (uint i = 0; i < scale; ++i)
			{
				TIntermBinary*	assign_data3	= new TIntermBinary{ TOperator::EOpAssign };
				TIntermTyped*	scalar			= fn_args->getSequence()[0]->getAsTyped();

				CHECK_ERR( scalar != null );

				assign_data3->setType( TType{uint_type} );
				assign_data3->setLeft( indexed_access );
				assign_data3->setRight( TypeToUint( scalar, i ));
				branch_body->getSequence().push_back( assign_data3 );
			}
		}
		else
		if ( value_type.matrixCols and value_type.matrixRows )
		{
			TIntermTyped*	mat			= fn_args->getSequence()[0]->getAsTyped();
			TPublicType		pub_type;	pub_type.init( Default );

			pub_type.basicType			= mat->getType().getBasicType();
			pub_type.qualifier.storage	= mat->getType().getQualifier().storage;

			for (uint c = 0; c < value_type.matrixCols; ++c)
			{
				TConstUnionArray		col_index(1);	col_index[0].setIConst( c );
				TIntermConstantUnion*	col_field		= new TIntermConstantUnion{ col_index, TType{index_type} };
				TIntermBinary*			col_access		= new TIntermBinary{ TOperator::EOpIndexDirect };

				// "matrix[c]"
				pub_type.setVector( value_type.matrixRows );
				col_access->setType( TType{pub_type} );
				col_access->setLeft( mat );
				col_access->setRight( col_field );

				for (uint r = 0; r < value_type.matrixRows * scale; ++r)
				{
					TConstUnionArray		row_index(1);	row_index[0].setIConst( r / scale );
					TIntermConstantUnion*	row_field		= new TIntermConstantUnion{ row_index, TType{index_type} };
					TIntermBinary*			row_access		= new TIntermBinary{ TOperator::EOpIndexDirect };
					TIntermBinary*			assign_data3	= new TIntermBinary{ TOperator::EOpAssign };

					// "matrix[c][r]"
					pub_type.setVector( 1 );
					row_access->setType( TType{pub_type} );
					row_access->setLeft( col_access );
					row_access->setRight( row_field );

					// "dbg_ShaderTrace.outData[pos++] = ToUint(value.x)"
					assign_data3->setType( TType{uint_type} );
					assign_data3->setLeft( indexed_access );
					assign_data3->setRight( TypeToUint( row_access, r ));
					branch_body->getSequence().push_back( assign_data3 );
				}
			}
		}
		else
		for (uint i = 0; i < value_type.vectorSize * scale; ++i)
		{
			TIntermBinary*			assign_data3	= new TIntermBinary{ TOperator::EOpAssign };
			TConstUnionArray		field_index(1);	field_index[0].setIConst( i / scale );
			TIntermConstantUnion*	vec_field		= new TIntermConstantUnion{ field_index, TType{index_type} };
			TIntermBinary*			field_access	= new TIntermBinary{ TOperator::EOpIndexDirect };
			TIntermTyped*			vec				= fn_args->getSequence()[0]->getAsTyped();

			CHECK_ERR( vec != null );

			TPublicType		pub_type;	pub_type.init( Default );
			pub_type.basicType			= vec->getType().getBasicType();
			pub_type.qualifier.storage	= vec->getType().getQualifier().storage;

			// "value.x"
			field_access->setType( TType{pub_type} );
			field_access->setLeft( vec );
			field_access->setRight( vec_field );

			// "dbg_ShaderTrace.outData[pos++] = ToUint(value.x)"
			assign_data3->setType( TType{uint_type} );
			assign_data3->setLeft( indexed_access );
			assign_data3->setRight( TypeToUint( field_access, i ));
			branch_body->getSequence().push_back( assign_data3 );
		}
	}

	// "if ( dbg_IsEnabled )"
	{
		TIntermSymbol*		condition	= dbgInfo.GetCachedSymbolNode( "dbg_IsEnabled" );
		CHECK_ERR( condition != null );

		TIntermSelection*	selection	= new TIntermSelection{ condition, branch_body, null };
		selection->setType( TType{EbtVoid} );

		fn_body->getSequence().push_back( selection );
	}

	// "return value"
	{
		TIntermBranch*		fn_return	= new TIntermBranch{ TOperator::EOpReturn, fn_args->getSequence()[0]->getAsTyped() };
		fn_body->getSequence().push_back( fn_return );
	}

	return fn_node;
}

/*
=================================================
	AppendShaderInputVaryings
=================================================
*/
ND_ static bool  AppendShaderInputVaryings (TIntermAggregate* body, DebugInfo &dbgInfo)
{
	CHECK_ERR( not dbgInfo.GetCallStack().empty() );

	TIntermAggregate*	root		= dbgInfo.GetCallStack().front().node->getAsAggregate();
	TIntermAggregate*	linker_objs	= null;
	CHECK_ERR( root != null );

	for (auto* node : root->getSequence()) {
		if ( auto* aggr = node->getAsAggregate(); aggr and aggr->getOp() == TOperator::EOpLinkerObjects ) {
			linker_objs = aggr;
			break;
		}
	}
	CHECK_ERR( linker_objs != null );

	for (auto* node : linker_objs->getSequence())
	{
		TIntermSymbol*	symb = node->getAsSymbolNode();

		if ( symb == null or
			 not AnyEqual( symb->getQualifier().storage, TStorageQualifier::EvqVaryingIn, TStorageQualifier::EvqHitAttr ))
			continue;

		if ( symb->isStruct() )
		{
			auto&	struct_fields = *symb->getType().getStruct();

			TPublicType		index_type;		index_type.init( Default );
			index_type.basicType			= TBasicType::EbtInt;
			index_type.qualifier.storage	= TStorageQualifier::EvqConst;

			for (auto& field : struct_fields)
			{
				ASSERT( not field.type->isStruct() );	// TODO

				const usize				index			= Distance( struct_fields.data(), &field );
				TConstUnionArray		index_Value(1);	index_Value[0].setIConst( int(index) );
				TIntermConstantUnion*	field_index		= new TIntermConstantUnion{ index_Value, TType{index_type} };
				TIntermBinary*			field_access	= new TIntermBinary{ TOperator::EOpIndexDirectStruct };
				field_access->setType( *field.type );
				field_access->setLeft( symb );
				field_access->setRight( field_index );

				const uint	loc_id = dbgInfo.GetCustomSourceLocation( field_access, symb->getLoc()/*field.loc*/ );
				if ( auto* fncall = CreateAppendToTrace( field_access, loc_id, dbgInfo ))
					body->getSequence().push_back( fncall );
			}
		}
		else
		if ( symb->isScalar() or symb->isVector() or symb->isMatrix() )
		{
			const uint	loc_id = dbgInfo.GetCustomSourceLocation( symb, symb->getLoc() );
			if ( auto* fncall = CreateAppendToTrace( symb, loc_id, dbgInfo ))
				body->getSequence().push_back( fncall );
		}
	}

	return true;
}

/*
=================================================
	RecordBasicShaderInfo
=================================================
*/
static void  RecordBasicShaderInfo (TIntermAggregate* body, const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	// "dbg_AppendToTrace( gl_SubgroupInvocationID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_SubgroupInvocationID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}
}

/*
=================================================
	RecordVertexShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordVertexShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_VertexIndex, location )"
	if ( auto*  vert_index = dbgInfo.GetCachedSymbolNode( "gl_VertexIndex" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( vert_index, loc );
		if ( auto* fncall = CreateAppendToTrace( vert_index, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_InstanceIndex, location )"
	if ( auto*  instance = dbgInfo.GetCachedSymbolNode( "gl_InstanceIndex" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( instance, loc );
		if ( auto* fncall = CreateAppendToTrace( instance, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_DrawID, location )"
	if ( auto*  draw_id = dbgInfo.GetCachedSymbolNode( "gl_DrawID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( draw_id, loc );
		if ( auto* fncall = CreateAppendToTrace( draw_id, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordTessControlShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordTessControlShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_InvocationID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_InvocationID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_PrimitiveID, location )"
	if ( auto*  primitive = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordTessEvaluationShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordTessEvaluationShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_PrimitiveID, location )"
	if ( auto*  primitive = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_TessCoord, location )"
	if ( auto*  tess_coord = dbgInfo.GetCachedSymbolNode( "gl_TessCoord" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( tess_coord, loc );
		if ( auto* fncall = CreateAppendToTrace( tess_coord, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	TPublicType		index_type;		index_type.init( Default );
	index_type.basicType			= TBasicType::EbtInt;
	index_type.qualifier.storage	= TStorageQualifier::EvqConst;

	// "dbg_AppendToTrace( gl_TessLevelInner, location )"
	{
		TIntermSymbol*		inner_level	= dbgInfo.GetCachedSymbolNode( "gl_TessLevelInner" );
		CHECK_ERR( inner_level != null );

		const uint			loc_id		= dbgInfo.GetCustomSourceLocation( inner_level, loc );
		TIntermAggregate*	vec2_ctor	= new TIntermAggregate{ TOperator::EOpConstructVec2 };
		TPublicType			float_type;	float_type.init( Default );
		float_type.basicType			= TBasicType::EbtFloat;
		float_type.qualifier.storage	= TStorageQualifier::EvqTemporary;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvTessLevelInner;
		float_type.qualifier.precision	= TPrecisionQualifier::EpqHigh;

		for (int i = 0; i < 2; ++i) {
			TIntermBinary*			elem_access		= new TIntermBinary{ TOperator::EOpIndexDirect };
			TConstUnionArray		elem_Value(1);	elem_Value[0].setIConst( i );
			TIntermConstantUnion*	elem_index		= new TIntermConstantUnion{ elem_Value, TType{index_type} };
			elem_access->setType( TType{float_type} );
			elem_access->setLeft( inner_level );
			elem_access->setRight( elem_index );
			vec2_ctor->getSequence().push_back( elem_access );
		}

		float_type.vectorSize			= 2;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvNone;
		float_type.qualifier.precision	= TPrecisionQualifier::EpqNone;

		vec2_ctor->setType( TType{float_type} );

		if ( auto* fncall = CreateAppendToTrace( vec2_ctor, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_TessLevelOuter, location )"
	{
		TIntermSymbol*		outer_level	= dbgInfo.GetCachedSymbolNode( "gl_TessLevelOuter" );
		CHECK_ERR( outer_level != null );

		const uint			loc_id		= dbgInfo.GetCustomSourceLocation( outer_level, loc );
		TIntermAggregate*	vec4_ctor	= new TIntermAggregate{ TOperator::EOpConstructVec4 };
		TPublicType			float_type;	float_type.init( Default );
		float_type.basicType			= TBasicType::EbtFloat;
		float_type.qualifier.storage	= TStorageQualifier::EvqTemporary;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvTessLevelOuter;
		float_type.qualifier.precision	= TPrecisionQualifier::EpqHigh;

		for (int i = 0; i < 4; ++i) {
			TIntermBinary*			elem_access		= new TIntermBinary{ TOperator::EOpIndexDirect };
			TConstUnionArray		elem_Value(1);	elem_Value[0].setIConst( i );
			TIntermConstantUnion*	elem_index		= new TIntermConstantUnion{ elem_Value, TType{index_type} };
			elem_access->setType( TType{float_type} );
			elem_access->setLeft( outer_level );
			elem_access->setRight( elem_index );
			vec4_ctor->getSequence().push_back( elem_access );
		}

		float_type.vectorSize			= 4;
		float_type.qualifier.builtIn	= TBuiltInVariable::EbvNone;
		float_type.qualifier.precision	= TPrecisionQualifier::EpqNone;
		vec4_ctor->setType( TType{float_type} );

		if ( auto* fncall = CreateAppendToTrace( vec4_ctor, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordGeometryShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordGeometryShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_InvocationID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_InvocationID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_PrimitiveIDIn, location )"
	if ( auto*  primitive = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveIDIn" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordFragmentShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordFragmentShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_FragCoord, location )"
	{
		TIntermSymbol*	fragcoord = dbgInfo.GetCachedSymbolNode( "gl_FragCoord" );
		CHECK_ERR( fragcoord != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( fragcoord, loc );
		if ( auto* fncall = CreateAppendToTrace( fragcoord, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_SampleID, location )"
	if ( auto*  sample_id = dbgInfo.GetCachedSymbolNode( "gl_SampleID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( sample_id, loc );
		if ( auto* fncall = CreateAppendToTrace( sample_id, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_SamplePosition, location )"
	if ( auto*  sample_pos = dbgInfo.GetCachedSymbolNode( "gl_SamplePosition" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( sample_pos, loc );
		if ( auto* fncall = CreateAppendToTrace( sample_pos, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_PrimitiveID, location )"
	if ( auto*  primitive_id = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive_id, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive_id, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_SamplePosition, location )"
	if ( auto*  layer = dbgInfo.GetCachedSymbolNode( "gl_Layer" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( layer, loc );
		if ( auto* fncall = CreateAppendToTrace( layer, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ViewportIndex, location )"
	if ( auto*  viewport_idx = dbgInfo.GetCachedSymbolNode( "gl_ViewportIndex" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( viewport_idx, loc );
		if ( auto* fncall = CreateAppendToTrace( viewport_idx, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );

	CHECK_ERR( AppendShaderInputVaryings( body, dbgInfo ));

	return body;
}

/*
=================================================
	RecordComputeShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordComputeShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_GlobalInvocationID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_GlobalInvocationID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_LocalInvocationID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_LocalInvocationID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorkGroupID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_WorkGroupID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_SubgroupID, location )"
	if ( auto*  invocation = dbgInfo.GetCachedSymbolNode( "gl_SubgroupID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordRayGenShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordRayGenShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_LaunchID, location )"
	{
		TIntermSymbol*	invocation = dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
		CHECK_ERR( invocation != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordHitShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordHitShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_LaunchID, location )"
	{
		TIntermSymbol*	invocation = dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
		CHECK_ERR( invocation != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_PrimitiveID, location )"
	if ( auto*  primitive = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_InstanceID, location )"
	if ( auto*  instance = dbgInfo.GetCachedSymbolNode( "gl_InstanceID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( instance, loc );
		if ( auto* fncall = CreateAppendToTrace( instance, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_InstanceCustomIndex, location )"
	if ( auto*  instance_index = dbgInfo.GetCachedSymbolNode( "gl_InstanceCustomIndexEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( instance_index, loc );
		if ( auto* fncall = CreateAppendToTrace( instance_index, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayOrigin, location )"
	if ( auto*  world_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_WorldRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayDirection, location )"
	if ( auto*  world_ray_direction = dbgInfo.GetCachedSymbolNode( "gl_WorldRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_direction, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_direction, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayOrigin, location )"
	if ( auto*  object_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( object_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayDirection, location )"
	if ( auto*  object_ray_direction = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_ray_direction, loc );
		if ( auto* fncall = CreateAppendToTrace( object_ray_direction, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_RayTmin, location )"
	if ( auto*  ray_tmin = dbgInfo.GetCachedSymbolNode( "gl_RayTminEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( ray_tmin, loc );
		if ( auto* fncall = CreateAppendToTrace( ray_tmin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_RayTmax, location )"
	if ( auto*  ray_tmax = dbgInfo.GetCachedSymbolNode( "gl_RayTmaxEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( ray_tmax, loc );
		if ( auto* fncall = CreateAppendToTrace( ray_tmax, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_IncomingRayFlags, location )"
	if ( auto*  incoming_ray_flags = dbgInfo.GetCachedSymbolNode( "gl_IncomingRayFlagsEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( incoming_ray_flags, loc );
		if ( auto* fncall = CreateAppendToTrace( incoming_ray_flags, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_HitT, location )"
	if ( auto*  hit_t = dbgInfo.GetCachedSymbolNode( "gl_HitTEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( hit_t, loc );
		if ( auto* fncall = CreateAppendToTrace( hit_t, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_HitKind, location )"
	if ( auto*  hit_kind = dbgInfo.GetCachedSymbolNode( "gl_HitKindEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( hit_kind, loc );
		if ( auto* fncall = CreateAppendToTrace( hit_kind, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectToWorld, location )"
	if ( auto*  object_to_world = dbgInfo.GetCachedSymbolNode( "gl_ObjectToWorldEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_to_world, loc );
		if ( auto* fncall = CreateAppendToTrace( object_to_world, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldToObject, location )"
	if ( auto*  world_to_object = dbgInfo.GetCachedSymbolNode( "gl_WorldToObjectEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_to_object, loc );
		if ( auto* fncall = CreateAppendToTrace( world_to_object, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );

	// hitAttributeEXT
	CHECK_ERR( AppendShaderInputVaryings( body, dbgInfo ));

	return body;
}

/*
=================================================
	RecordIntersectionShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordIntersectionShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_LaunchID, location )"
	{
		TIntermSymbol*	invocation = dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
		CHECK_ERR( invocation != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_PrimitiveID, location )"
	if ( auto*  primitive = dbgInfo.GetCachedSymbolNode( "gl_PrimitiveID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( primitive, loc );
		if ( auto* fncall = CreateAppendToTrace( primitive, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_InstanceID, location )"
	if ( auto*  instance = dbgInfo.GetCachedSymbolNode( "gl_InstanceID" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( instance, loc );
		if ( auto* fncall = CreateAppendToTrace( instance, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_InstanceCustomIndex, location )"
	if ( auto*  instance_index = dbgInfo.GetCachedSymbolNode( "gl_InstanceCustomIndexEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( instance_index, loc );
		if ( auto* fncall = CreateAppendToTrace( instance_index, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayOrigin, location )"
	if ( auto*  world_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_WorldRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayDirection, location )"
	if ( auto*  world_ray_dir = dbgInfo.GetCachedSymbolNode( "gl_WorldRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_dir, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_dir, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayOrigin, location )"
	if ( auto*  object_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( object_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayDirection, location )"
	if ( auto*  object_ray_dir = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_ray_dir, loc );
		if ( auto* fncall = CreateAppendToTrace( object_ray_dir, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_RayTmin, location )"
	if ( auto*  ray_tmin = dbgInfo.GetCachedSymbolNode( "gl_RayTminEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( ray_tmin, loc );
		if ( auto* fncall = CreateAppendToTrace( ray_tmin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_RayTmax, location )"
	if ( auto*  ray_tmax = dbgInfo.GetCachedSymbolNode( "gl_RayTmaxEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( ray_tmax, loc );
		if ( auto* fncall = CreateAppendToTrace( ray_tmax, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_IncomingRayFlags, location )"
	if ( auto*  incoming_ray_flags = dbgInfo.GetCachedSymbolNode( "gl_IncomingRayFlagsEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( incoming_ray_flags, loc );
		if ( auto* fncall = CreateAppendToTrace( incoming_ray_flags, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectToWorld, location )"
	if ( auto*  object_to_world = dbgInfo.GetCachedSymbolNode( "gl_ObjectToWorldEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( object_to_world, loc );
		if ( auto* fncall = CreateAppendToTrace( object_to_world, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldToObject, location )"
	if ( auto*  world_to_object = dbgInfo.GetCachedSymbolNode( "gl_WorldToObjectEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_to_object, loc );
		if ( auto* fncall = CreateAppendToTrace( world_to_object, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordMissShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordMissShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_LaunchID, location )"
	{
		TIntermSymbol*	invocation = dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
		CHECK_ERR( invocation != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayOrigin, location )"
	if ( auto*  world_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_WorldRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_WorldRayDirection, location )"
	if ( auto*  world_ray_direction = dbgInfo.GetCachedSymbolNode( "gl_WorldRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( world_ray_direction, loc );
		if ( auto* fncall = CreateAppendToTrace( world_ray_direction, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayOrigin, location )"
	if ( auto*  obj_ray_origin = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayOriginEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( obj_ray_origin, loc );
		if ( auto* fncall = CreateAppendToTrace( obj_ray_origin, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_ObjectRayDirection, location )"
	if ( auto*  obj_ray_direction = dbgInfo.GetCachedSymbolNode( "gl_ObjectRayDirectionEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( obj_ray_direction, loc );
		if ( auto* fncall = CreateAppendToTrace( obj_ray_direction, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordCallableShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordCallableShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	body = new TIntermAggregate{ TOperator::EOpSequence };
	body->setType( TType{EbtVoid} );

	// "dbg_AppendToTrace( gl_LaunchID, location )"
	{
		TIntermSymbol*	invocation = dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
		CHECK_ERR( invocation != null );

		const uint	loc_id = dbgInfo.GetCustomSourceLocation( invocation, loc );
		if ( auto* fncall = CreateAppendToTrace( invocation, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	// "dbg_AppendToTrace( gl_IncomingRayFlags, location )"
	if ( auto*  incoming_ray_flags = dbgInfo.GetCachedSymbolNode( "gl_IncomingRayFlagsEXT" ))
	{
		const uint	loc_id = dbgInfo.GetCustomSourceLocation( incoming_ray_flags, loc );
		if ( auto* fncall = CreateAppendToTrace( incoming_ray_flags, loc_id, dbgInfo ))
			body->getSequence().push_back( fncall );
	}

	RecordBasicShaderInfo( body, loc, dbgInfo );
	return body;
}

/*
=================================================
	RecordShaderInfo
=================================================
*/
ND_ static TIntermAggregate*  RecordShaderInfo (const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	switch_enum( dbgInfo.GetShaderType() )
	{
		case EShLangVertex :			return RecordVertexShaderInfo( loc, dbgInfo );
		case EShLangTessControl :		return RecordTessControlShaderInfo( loc, dbgInfo );
		case EShLangTessEvaluation :	return RecordTessEvaluationShaderInfo( loc, dbgInfo );
		case EShLangGeometry :			return RecordGeometryShaderInfo( loc, dbgInfo );
		case EShLangFragment :			return RecordFragmentShaderInfo( loc, dbgInfo );
		case EShLangTask :
		case EShLangMesh :
		case EShLangCompute :			return RecordComputeShaderInfo( loc, dbgInfo );
		case EShLangRayGen :			return RecordRayGenShaderInfo( loc, dbgInfo );
		case EShLangAnyHit :
		case EShLangClosestHit :		return RecordHitShaderInfo( loc, dbgInfo );
		case EShLangIntersect :			return RecordIntersectionShaderInfo( loc, dbgInfo );
		case EShLangMiss :				return RecordMissShaderInfo( loc, dbgInfo );
		case EShLangCallable :			return RecordCallableShaderInfo( loc, dbgInfo );
		case EShLangCount :				break;
	}
	switch_end
	return null;
}

/*
=================================================
	CreateFragmentShaderIsDebugInvocation
=================================================
*/
ND_ static TIntermOperator*  CreateFragmentShaderIsDebugInvocation (DebugInfo &dbgInfo)
{
	TPublicType		bool_type;	bool_type.init( Default );
	bool_type.basicType			= TBasicType::EbtBool;
	bool_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		int_type;	int_type.init( Default );
	int_type.basicType			= TBasicType::EbtInt;
	int_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		float_type;	 float_type.init( Default );
	float_type.basicType		 = TBasicType::EbtFloat;
	float_type.qualifier.storage = TStorageQualifier::EvqTemporary;

	TPublicType		index_type;	 index_type.init( Default );
	index_type.basicType		 = TBasicType::EbtInt;
	index_type.qualifier.storage = TStorageQualifier::EvqConst;

	TIntermSymbol*	frag_coord	= dbgInfo.GetCachedSymbolNode( "gl_FragCoord" );
	CHECK_ERR( frag_coord != null );

	TIntermBinary*	eq1 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.fragCoordX
		TIntermBinary*			fscoord_x	= dbgInfo.GetDebugStorageField( "fragCoordX" );
		CHECK_ERR( fscoord_x != null );

		// gl_FragCoord.x
		TConstUnionArray		x_index(1);	x_index[0].setIConst( 0 );
		TIntermConstantUnion*	x_field		= new TIntermConstantUnion{ x_index, TType{index_type} };
		TIntermBinary*			frag_x		= new TIntermBinary{ TOperator::EOpIndexDirect };
		frag_x->setType( TType{float_type} );
		frag_x->setLeft( frag_coord );
		frag_x->setRight( x_field );

		// int(gl_FragCoord.x)
		TIntermUnary*			uint_fc_x	= new TIntermUnary{ TOperator::EOpConvFloatToInt };
		uint_fc_x->setType( TType{int_type} );
		uint_fc_x->setOperand( frag_x );

		// ... == ...
		eq1->setType( TType{bool_type} );
		eq1->setLeft( uint_fc_x );
		eq1->setRight( fscoord_x );
	}

	TIntermBinary*	eq2 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.fragCoordY
		TIntermBinary*			fscoord_y	= dbgInfo.GetDebugStorageField( "fragCoordY" );
		CHECK_ERR( fscoord_y != null );

		// gl_FragCoord.y
		TConstUnionArray		y_index(1);	y_index[0].setIConst( 1 );
		TIntermConstantUnion*	y_field		= new TIntermConstantUnion{ y_index, TType{index_type} };
		TIntermBinary*			frag_y		= new TIntermBinary{ TOperator::EOpIndexDirect };
		frag_y->setType( TType{float_type} );
		frag_y->setLeft( frag_coord );
		frag_y->setRight( y_field );

		// int(gl_FragCoord.y)
		TIntermUnary*			uint_fc_y	= new TIntermUnary{ TOperator::EOpConvFloatToInt };
		uint_fc_y->setType( TType{int_type} );
		uint_fc_y->setOperand( frag_y );

		// ... == ...
		eq2->setType( TType{bool_type} );
		eq2->setLeft( uint_fc_y );
		eq2->setRight( fscoord_y );
	}

	// ... && ...
	TIntermBinary*	cmp1		= new TIntermBinary{ TOperator::EOpLogicalAnd };
	cmp1->setType( TType{bool_type} );
	cmp1->setLeft( eq1 );
	cmp1->setRight( eq2 );

	return cmp1;
}

/*
=================================================
	CreateComputeShaderIsDebugInvocation
=================================================
*/
ND_ static TIntermOperator*  CreateComputeShaderIsDebugInvocation (DebugInfo &dbgInfo)
{
	TPublicType		bool_type;	bool_type.init( Default );
	bool_type.basicType			= TBasicType::EbtBool;
	bool_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		uint_type;	uint_type.init( Default );
	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		index_type;	 index_type.init( Default );
	index_type.basicType		 = TBasicType::EbtInt;
	index_type.qualifier.storage = TStorageQualifier::EvqConst;

	TIntermSymbol*	global_inv	= dbgInfo.GetCachedSymbolNode( "gl_GlobalInvocationID" );
	CHECK_ERR( global_inv != null );

	TIntermBinary*	eq1 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.globalInvocationX
		TIntermBinary*			thread_id_x	= dbgInfo.GetDebugStorageField( "globalInvocationX" );
		CHECK_ERR( thread_id_x != null );

		// gl_GlobalInvocationID.x
		TConstUnionArray		x_index(1);	x_index[0].setIConst( 0 );
		TIntermConstantUnion*	x_field		= new TIntermConstantUnion{ x_index, TType{index_type} };
		TIntermBinary*			ginvoc_x	= new TIntermBinary{ TOperator::EOpIndexDirect };
		ginvoc_x->setType( TType{uint_type} );
		ginvoc_x->setLeft( global_inv );
		ginvoc_x->setRight( x_field );

		// ... == ...
		eq1->setType( TType{bool_type} );
		eq1->setLeft( thread_id_x );
		eq1->setRight( ginvoc_x );
	}

	TIntermBinary*	eq2 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.globalInvocationY
		TIntermBinary*			thread_id_y	= dbgInfo.GetDebugStorageField( "globalInvocationY" );
		CHECK_ERR( thread_id_y != null );

		// gl_GlobalInvocationID.y
		TConstUnionArray		y_index(1);	y_index[0].setIConst( 1 );
		TIntermConstantUnion*	y_field		= new TIntermConstantUnion{ y_index, TType{index_type} };
		TIntermBinary*			ginvoc_y	= new TIntermBinary{ TOperator::EOpIndexDirect };
		ginvoc_y->setType( TType{uint_type} );
		ginvoc_y->setLeft( global_inv );
		ginvoc_y->setRight( y_field );

		// ... == ...
		eq2->setType( TType{bool_type} );
		eq2->setLeft( thread_id_y );
		eq2->setRight( ginvoc_y );
	}

	TIntermBinary*	eq3 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.globalInvocationZ
		TIntermBinary*			thread_id_z	= dbgInfo.GetDebugStorageField( "globalInvocationZ" );
		CHECK_ERR( thread_id_z != null );

		// gl_GlobalInvocationID.z
		TConstUnionArray		z_index(1);	z_index[0].setIConst( 2 );
		TIntermConstantUnion*	z_field		= new TIntermConstantUnion{ z_index, TType{index_type} };
		TIntermBinary*			ginvoc_z	= new TIntermBinary{ TOperator::EOpIndexDirect };
		ginvoc_z->setType( TType{uint_type} );
		ginvoc_z->setLeft( global_inv );
		ginvoc_z->setRight( z_field );

		// ... == ...
		eq3->setType( TType{bool_type} );
		eq3->setLeft( thread_id_z );
		eq3->setRight( ginvoc_z );
	}

	// ... && ...
	TIntermBinary*		cmp1		= new TIntermBinary{ TOperator::EOpLogicalAnd };
	cmp1->setType( TType{bool_type} );
	cmp1->setLeft( eq1 );
	cmp1->setRight( eq2 );

	// ... && ...
	TIntermBinary*		cmp2		= new TIntermBinary{ TOperator::EOpLogicalAnd };
	cmp2->setType( TType{bool_type} );
	cmp2->setLeft( cmp1 );
	cmp2->setRight( eq3 );

	return cmp2;
}

/*
=================================================
	CreateRayTracingShaderIsDebugInvocation
=================================================
*/
ND_ static TIntermOperator*  CreateRayTracingShaderIsDebugInvocation (DebugInfo &dbgInfo)
{
	TPublicType		bool_type;	bool_type.init( Default );
	bool_type.basicType			= TBasicType::EbtBool;
	bool_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		uint_type;	uint_type.init( Default );
	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	TPublicType		index_type;	 index_type.init( Default );
	index_type.basicType		 = TBasicType::EbtInt;
	index_type.qualifier.storage = TStorageQualifier::EvqConst;

	TIntermSymbol*	launch_id	= dbgInfo.GetCachedSymbolNode( "gl_LaunchIDEXT" );
	CHECK_ERR( launch_id != null );

	TIntermBinary*	eq1 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.launchID_x
		TIntermBinary*			thread_id_x	= dbgInfo.GetDebugStorageField( "launchID_x" );
		CHECK_ERR( thread_id_x != null );

		// gl_LaunchID.x
		TConstUnionArray		x_index(1);	x_index[0].setIConst( 0 );
		TIntermConstantUnion*	x_field		= new TIntermConstantUnion{ x_index, TType{index_type} };
		TIntermBinary*			launch_x	= new TIntermBinary{ TOperator::EOpIndexDirect };
		launch_x->setType( TType{uint_type} );
		launch_x->setLeft( launch_id );
		launch_x->setRight( x_field );

		// ... == ...
		eq1->setType( TType{bool_type} );
		eq1->setLeft( thread_id_x );
		eq1->setRight( launch_x );
	}

	TIntermBinary*	eq2 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.launchID_y
		TIntermBinary*			thread_id_y	= dbgInfo.GetDebugStorageField( "launchID_y" );
		CHECK_ERR( thread_id_y != null );

		// gl_LaunchID.y
		TConstUnionArray		y_index(1);	y_index[0].setIConst( 1 );
		TIntermConstantUnion*	y_field		= new TIntermConstantUnion{ y_index, TType{index_type} };
		TIntermBinary*			launch_y	= new TIntermBinary{ TOperator::EOpIndexDirect };
		launch_y->setType( TType{uint_type} );
		launch_y->setLeft( launch_id );
		launch_y->setRight( y_field );

		// ... == ...
		eq2->setType( TType{bool_type} );
		eq2->setLeft( thread_id_y );
		eq2->setRight( launch_y );
	}

	TIntermBinary*	eq3 = new TIntermBinary{ TOperator::EOpEqual };
	{
		// dbg_ShaderTrace.launchID_z
		TIntermBinary*			thread_id_z	= dbgInfo.GetDebugStorageField( "launchID_z" );
		CHECK_ERR( thread_id_z != null );

		// gl_LaunchID.z
		TConstUnionArray		z_index(1);	z_index[0].setIConst( 2 );
		TIntermConstantUnion*	z_field		= new TIntermConstantUnion{ z_index, TType{index_type} };
		TIntermBinary*			launch_z	= new TIntermBinary{ TOperator::EOpIndexDirect };
		launch_z->setType( TType{uint_type} );
		launch_z->setLeft( launch_id );
		launch_z->setRight( z_field );

		// ... == ...
		eq3->setType( TType{bool_type} );
		eq3->setLeft( thread_id_z );
		eq3->setRight( launch_z );
	}

	// ... && ...
	TIntermBinary*		cmp1		= new TIntermBinary{ TOperator::EOpLogicalAnd };
	cmp1->setType( TType{bool_type} );
	cmp1->setLeft( eq1 );
	cmp1->setRight( eq2 );

	// ... && ...
	TIntermBinary*		cmp2		= new TIntermBinary{ TOperator::EOpLogicalAnd };
	cmp2->setType( TType{bool_type} );
	cmp2->setLeft( cmp1 );
	cmp2->setRight( eq3 );

	return cmp2;
}

/*
=================================================
	CreateAppendToTrace
----
	returns new node instead of 'exprNode'.
	'exprNode' - any operator.
	'sourceLoc' - location index returned by DebugInfo::GetSourceLocation or ::GetCustomSourceLocation.
	also see 'CreateAppendToTraceBody()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAppendToTrace (TIntermTyped* exprNode, uint sourceLoc, DebugInfo &dbgInfo)
{
	CHECK_ERR( exprNode != null );

	TIntermAggregate*	fcall		= new TIntermAggregate( TOperator::EOpFunctionCall );
	String				type_name;
	const TType &		type		= exprNode->getType();

	switch_enum( type.getBasicType() )
	{
		case TBasicType::EbtFloat :		type_name = "f";	break;
		case TBasicType::EbtInt :		type_name = "i";	break;
		case TBasicType::EbtUint :		type_name = "u";	break;
		case TBasicType::EbtBool :		type_name = "b";	break;
		case TBasicType::EbtDouble :	type_name = "d";	break;
		case TBasicType::EbtInt64 :		type_name = "i64";	break;
		case TBasicType::EbtUint64 :	type_name = "u64";	break;
		case TBasicType::EbtFloat16 :	type_name = "hf";	break;

		case TBasicType::EbtSampler :	return null;
		case TBasicType::EbtStruct :	return null;
		case TBasicType::EbtReference :	return null;

		case TBasicType::EbtVoid :
		case TBasicType::EbtInt8 :
		case TBasicType::EbtUint8 :
		case TBasicType::EbtInt16 :
		case TBasicType::EbtUint16 :
		case TBasicType::EbtAtomicUint :
		case TBasicType::EbtBlock :
		case TBasicType::EbtString :
		case TBasicType::EbtNumTypes :
		case TBasicType::EbtAccStruct :
		case TBasicType::EbtRayQuery :
		case TBasicType::EbtSpirvType :
		case TBasicType::EbtHitObjectNV :
		case TBasicType::EbtCoopmat :
		default :						RETURN_ERR( "not supported" );
	}
	switch_end

	if ( type.isArray() )
		return null;	//RETURN_ERR( "arrays is not supported yet" )
	else
	if ( type.isMatrix() )
		type_name = "m"s << type_name << ToString(type.getMatrixCols()) << ToString(type.getMatrixRows());
	else
	if ( type.isVector() )
		type_name = "v"s << type_name << ToString(type.getVectorSize());
	else
	if ( type.isScalarOrVec1() )
		type_name << "1";
	else
		RETURN_ERR( "unknown type" );


	fcall->setLoc( exprNode->getLoc() );
	fcall->setUserDefined();
	fcall->setName( TString{"dbg_AppendToTrace("} + TString{type_name.c_str()} + ";u1;" );
	fcall->setType( exprNode->getType() );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getSequence().push_back( exprNode );

	TPublicType		uint_type;		uint_type.init( exprNode->getLoc() );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.qualifier.storage		= TStorageQualifier::EvqConst;

	TConstUnionArray		loc_value(1);	loc_value[0].setUConst( sourceLoc );
	TIntermConstantUnion*	loc_const		= new TIntermConstantUnion{ loc_value, TType{uint_type} };

	loc_const->setLoc( exprNode->getLoc() );
	fcall->getSequence().push_back( loc_const );

	dbgInfo.RequestFunc( fcall->getName() );

	return fcall;
}
//-----------------------------------------------------------------------------



/*
=================================================
	ProcessSymbolNode
=================================================
*/
ND_ static bool  ProcessSymbolNode (TIntermSymbol* node, DebugInfo &dbgInfo)
{
  #ifndef PROFILER
	dbgInfo.AddSymbol( node, False{"not a user-defined"} );
  #endif

	if ( // fragment shader
		 node->getName() == "gl_FragCoord"				or
		 node->getName() == "gl_SampleID"				or
		 node->getName() == "gl_PrimitiveID"			or
		 node->getName() == "gl_SamplePosition"			or
		 node->getName() == "gl_Layer"					or
		 node->getName() == "gl_ViewportIndex"			or
		 node->getName() == "gl_FrontFacing"			or
		 node->getName() == "gl_HelperInvocation"		or

		 // vertex shader
		 node->getName() == "gl_VertexIndex"			or
		 node->getName() == "gl_InstanceIndex"			or
		 node->getName() == "gl_DrawIDARB"				or		// requires GL_ARB_shader_draw_parameters
		 node->getName() == "gl_DrawID"					or		// requires version 460

		// geometry shader
		 node->getName() == "gl_InvocationID"			or
		 node->getName() == "gl_PrimitiveIDIn"			or

		 // tessellation
		 //		reuse 'gl_InvocationID'
		//		reuse 'gl_PrimitiveID'
		 node->getName() == "gl_PatchVerticesIn"		or
		 node->getName() == "gl_TessCoord"				or
		 node->getName() == "gl_TessLevelInner"			or
		 node->getName() == "gl_TessLevelOuter"			or

		 // compute shader
		 node->getName() == "gl_GlobalInvocationID"		or
		 node->getName() == "gl_LocalInvocationID"		or
		 node->getName() == "gl_LocalInvocationIndex"	or
		 node->getName() == "gl_WorkGroupID"			or
		 node->getName() == "gl_NumWorkGroups"			or
		 node->getName() == "gl_WorkGroupSize"			or
		 node->getName() == "gl_SubgroupID"				or
		 node->getName() == "gl_NumSubgroups"			or

		 // task
		 //		reuse 'gl_NumWorkGroups'
		 //		reuse 'gl_WorkGroupSize'
		 //		reuse 'gl_WorkGroupID'
		 //		reuse 'gl_LocalInvocationID'
		 //		reuse 'gl_GlobalInvocationID'
		 //		reuse 'gl_LocalInvocationIndex'

		 // mesh shader
		 //		reuse 'gl_NumWorkGroups'
		 //		reuse 'gl_WorkGroupSize'
		 //		reuse 'gl_WorkGroupID'
		 //		reuse 'gl_LocalInvocationID'
		 //		reuse 'gl_GlobalInvocationID'
		 //		reuse 'gl_LocalInvocationIndex'
		 //		skip 'gl_PrimitivePointIndicesEXT'
		 //		skip 'gl_PrimitiveLineIndicesEXT'
		 //		skip 'gl_PrimitiveTriangleIndicesEXT'

		 // NV mesh shader
		 node->getName() == "gl_MeshViewCountNV"		or	// TODO: remove

		 // ray generation shader
		 node->getName() == "gl_LaunchIDEXT"			or
		 node->getName() == "gl_LaunchSizeEXT"			or

		 // ray intersection & any-hit & closest-hit & miss shaders
		 //		reuse 'gl_LaunchID'
		 //		reuse 'gl_LaunchSize'
		 //		reuse 'gl_PrimitiveID'
		 node->getName() == "gl_InstanceCustomIndexEXT"	or
		 node->getName() == "gl_WorldRayOriginEXT"		or
		 node->getName() == "gl_WorldRayDirectionEXT"	or
		 node->getName() == "gl_ObjectRayOriginEXT"		or
		 node->getName() == "gl_ObjectRayDirectionEXT"	or
		 node->getName() == "gl_RayTminEXT"				or
		 node->getName() == "gl_RayTmaxEXT"				or
		 node->getName() == "gl_IncomingRayFlagsEXT"	or
		 node->getName() == "gl_ObjectToWorldEXT"		or
		 node->getName() == "gl_WorldToObjectEXT"		or

		 // ray intersection & any-hit & closest-hit shaders
		 node->getName() == "gl_HitTEXT"				or
		 node->getName() == "gl_HitKindEXT"				or
		 node->getName() == "gl_InstanceID"				or

		 // all shaders
		 node->getName() == "gl_SubgroupInvocationID"	or
		 node->getName() == "gl_SubgroupSize"			)
	{
		dbgInfo.CacheSymbolNode( node, False{"not a user-defined"} );
		return true;
	}

	// do nothing
	return true;
}

} // namespace
} // AE::PipelineCompiler
