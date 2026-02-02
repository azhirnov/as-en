// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Common.h"

namespace AE::PipelineCompiler
{
namespace
{
	using namespace glslang;


	//
	// Asserts Debug Info
	//

	struct AssertsDebugInfo
	{
	// types
	public:
		using SrcLoc		= ShaderTrace::SourceLocation;
		using Asserts_t		= ShaderTrace::Asserts_t;
		using FileMap_t		= ShaderTrace::FileMap_t;

		struct SrcLocHash {
			ND_ usize  operator () (const SrcLoc &) const;
		};
		using LocToIndex_t	= HashMap< SrcLoc, uint, SrcLocHash >;


	// variables
	private:
		Asserts_t &				_assertLocations;

		FileMap_t const&		_includedFilesMap;

		const String			_entryName;
		const EShLanguage		_shLang;
		const bool				_hasSubgroupBasicOps;

		TIntermSymbol *			_dbgStorage					= null;
		const uint				_maxBitCount				= 32 * 1024;	// 1 KiB

		LocToIndex_t			_locToIndex;

		slong					_maxSymbolId				= 0;
		bool					_startedUserDefinedSymbols	= false;


	// methods
	public:
		AssertsDebugInfo (const TIntermediate &intermediate, OUT Asserts_t &assertLocations, const FileMap_t &includedFiles, bool hasSubgroupBasicOps) :
			_assertLocations{ assertLocations },
			_includedFilesMap{ includedFiles },
			_entryName{ intermediate.getEntryPointMangledName() },
			_shLang{ intermediate.getStage() },
			_hasSubgroupBasicOps{ hasSubgroupBasicOps }
		{}

		ND_ TIntermSymbol*	GetDebugStorage ()			const	{ return _dbgStorage; }
		ND_ EShLanguage		GetShaderType ()			const	{ return _shLang; }
		ND_ bool			SupportsSubgroupBasicOps ()	const	{ return _hasSubgroupBasicOps; }
		ND_ slong			GetUniqueSymbolID ();

			bool  SetDebugStorage (TIntermSymbol* symb);

		ND_ uint  AddAssertLocation (const TSourceLoc &loc);
		ND_ uint  AddAssertLocation (const SrcLoc &src);

			void  AddSymbol (TIntermSymbol* node);

	private:
		ND_ uint  _GetSourceId (const TSourceLoc &) const;
	};


/*
=================================================
	SrcLocHash
=================================================
*/
	usize  AssertsDebugInfo::SrcLocHash::operator () (const SrcLoc &key) const
	{
		return usize{HashOf( key.sourceId ) + HashOf( key.begin._ul )};
	}

/*
=================================================
	SetDebugStorage
=================================================
*/
	bool  AssertsDebugInfo::SetDebugStorage (TIntermSymbol* symb)
	{
		CHECK_ERR( not _dbgStorage );
		CHECK_ERR( symb != null );

		_dbgStorage = symb;
		return true;
	}

/*
=================================================
	AddLocation
=================================================
*/
	uint  AssertsDebugInfo::AddAssertLocation (const TSourceLoc &loc)
	{
		return AddAssertLocation( SrcLoc{ _GetSourceId( loc ), uint(loc.line), uint(loc.column) });
	}

	uint  AssertsDebugInfo::AddAssertLocation (const SrcLoc &src)
	{
		if ( src.IsNotDefined() )
			return UMax;

		if ( _locToIndex.size() >= _maxBitCount )
		{
			DBG_WARNING( "overflow, increase 'dbg_ShaderTrace' buffer size" );
			return UMax;
		}

		auto [it, inserted] = _locToIndex.emplace( src, uint(_locToIndex.size()) );

		if ( inserted )
		{
			CHECK( it->second == _assertLocations.size() );  // compatible with indexed access

			auto&	dst		= _assertLocations.emplace_back();
			dst.bitIndex	= it->second;
			dst.sourceId	= src.sourceId;
			dst.point		= src.begin;
		}

		return it->second;
	}

/*
=================================================
	_GetSourceId
=================================================
*/
	uint  AssertsDebugInfo::_GetSourceId (const TSourceLoc &loc) const
	{
		if ( loc.name != null )
		{
			auto	iter = _includedFilesMap.find( loc.name->c_str() );
			CHECK( iter != _includedFilesMap.end() );

			return iter->second;
		}
		else
			return loc.string;
	}

/*
=================================================
	GetUniqueSymbolID
=================================================
*/
	slong  AssertsDebugInfo::GetUniqueSymbolID ()
	{
		_startedUserDefinedSymbols = true;
		return ++_maxSymbolId;
	}

/*
=================================================
	AddSymbol
=================================================
*/
	void  AssertsDebugInfo::AddSymbol (TIntermSymbol* node)
	{
		NonNull( node );
		ASSERT( not _startedUserDefinedSymbols );

		_maxSymbolId = Max( _maxSymbolId, node->getId() );
	}
//-----------------------------------------------------------------------------



	ND_ static bool  RecursiveProcessAggregateNode (TIntermAggregate* node, AssertsDebugInfo &dbgInfo);

/*
=================================================
	RecursiveProcessNode
=================================================
*/
	ND_ static bool  RecursiveProcessNode (TIntermNode* node, AssertsDebugInfo &dbgInfo)
	{
		if ( not node )
			return true;

		if ( auto* aggr = node->getAsAggregate() )
		{
			CHECK_ERR( RecursiveProcessAggregateNode( aggr, dbgInfo ));
			return true;
		}

		if ( auto* unary = node->getAsUnaryNode() )
		{
			CHECK_ERR( RecursiveProcessNode( unary->getOperand(), dbgInfo ));
			return true;
		}

		if ( auto* binary = node->getAsBinaryNode() )
		{
			CHECK_ERR( RecursiveProcessNode( binary->getLeft(), dbgInfo ));
			CHECK_ERR( RecursiveProcessNode( binary->getRight(), dbgInfo ));
			return true;
		}

		if ( auto* branch = node->getAsBranchNode() )
		{
			CHECK_ERR( RecursiveProcessNode( branch->getExpression(), dbgInfo ));
			return true;
		}

		if ( auto* sw = node->getAsSwitchNode() )
		{
			CHECK_ERR( RecursiveProcessNode( sw->getCondition(), dbgInfo ));
			CHECK_ERR( RecursiveProcessNode( sw->getBody(), dbgInfo ));
			return true;
		}

		if ( auto* selection = node->getAsSelectionNode() )
		{
			CHECK_ERR( RecursiveProcessNode( selection->getCondition(), dbgInfo ));
			CHECK_ERR( RecursiveProcessNode( selection->getTrueBlock(), dbgInfo ));
			CHECK_ERR( RecursiveProcessNode( selection->getFalseBlock(), dbgInfo ));
			return true;
		}

		if ( auto* loop = node->getAsLoopNode() )
		{
			CHECK_ERR( RecursiveProcessNode( loop->getBody(), dbgInfo ));
			if ( loop->getTerminal() )
				CHECK_ERR( RecursiveProcessNode( loop->getTerminal(), dbgInfo ));
			if ( loop->getTest() )
				CHECK_ERR( RecursiveProcessNode( loop->getTest(), dbgInfo ));
			return true;
		}

		if ( auto* cunion = node->getAsConstantUnion() )
		{
			Unused( cunion );
			return true;
		}

		if ( auto* op = node->getAsOperator() )
		{
			Unused( op );
			return true;
		}

		if ( auto* method = node->getAsMethodNode() )
		{
			Unused( method );
			return true;
		}

		if ( auto* symbol = node->getAsSymbolNode() )
		{
			dbgInfo.AddSymbol( symbol );
			return true;
		}

		if ( auto* typed = node->getAsTyped() )
		{
			Unused( typed );
			return true;
		}

		return false;
	}

/*
=================================================
	RecursiveProcessAggregateNode
=================================================
*/
	ND_ static bool  RecursiveProcessAggregateNode (TIntermAggregate* aggr, AssertsDebugInfo &dbgInfo)
	{
		for (auto& node : aggr->getSequence())
		{
			CHECK_ERR( RecursiveProcessNode( node, dbgInfo ));
		}

		if ( aggr->getOp() == TOperator::EOpFunctionCall and
			 aggr->getName() == "dbg_Assert(u1;" )
		{
			const uint	bit_idx = dbgInfo.AddAssertLocation( aggr->getLoc() );
			if ( bit_idx != UMax )
			{
				CHECK_ERR( aggr->getSequence().size() == 1 );

				auto*	cunion = aggr->getSequence()[0]->getAsConstantUnion();
				CHECK_ERR( cunion != null );
				CHECK( cunion->getType().getBasicType() == TBasicType::EbtUint );

				TConstUnionArray		loc_value(1);	loc_value[0].setUConst( bit_idx );
				TIntermConstantUnion*	loc_const		= new TIntermConstantUnion{ loc_value, cunion->getType() };

				aggr->getSequence()[0] = loc_const;
			}
		}

		return true;
	}

/*
=================================================
	ReplaceAssertFnBody
=================================================
*/
	ND_ static bool  ReplaceAssertFnBody (TIntermAggregate* inNode, AssertsDebugInfo &dbgInfo)
	{
		CHECK_ERR( dbgInfo.GetDebugStorage() != null );
		CHECK_ERR( inNode->getSequence().size() == 1 );

		auto*	fn_params = inNode->getSequence()[0]->getAsAggregate();
		CHECK_ERR( fn_params != null );
		CHECK_ERR( fn_params->getOp() == TOperator::EOpParameters );
		CHECK_ERR( fn_params->getSequence().size() == 1 );

		auto*	bit_index_symb = fn_params->getSequence()[0]->getAsSymbolNode();  // bitIndex
		CHECK_ERR( bit_index_symb != null );

		auto*	fn_body = new TIntermAggregate{ TOperator::EOpSequence };
		inNode->getSequence().push_back( fn_body );

		TPublicType		temp_uint;
		temp_uint.init( Default );
		temp_uint.basicType			= TBasicType::EbtUint;
		temp_uint.qualifier.storage	= TStorageQualifier::EvqTemporary;

		TPublicType		global_uint;
		global_uint.init( Default );
		global_uint.basicType			= TBasicType::EbtUint;
		global_uint.qualifier.storage	= TStorageQualifier::EvqGlobal;

		TPublicType		const_uint;
		const_uint.init( Default );
		const_uint.basicType			= TBasicType::EbtUint;
		const_uint.qualifier.storage	= TStorageQualifier::EvqConst;

		// single atomic op per subgroup
		// if ( subgroupElect() )
		if ( dbgInfo.SupportsSubgroupBasicOps() )
		{
			auto	global_bool = global_uint;
			global_bool.basicType = TBasicType::EbtBool;

			TIntermAggregate*	sg_elect	= new TIntermAggregate{ TOperator::EOpSubgroupElect };
			TIntermAggregate*	true_block	= new TIntermAggregate{ TOperator::EOpSequence };
			TIntermSelection*	selection	= new TIntermSelection{ sg_elect, true_block, null };

			sg_elect->setType( TType{global_bool} );
			selection->setType( TType{EbtVoid} );

			fn_body->getSequence().push_back( selection );
			fn_body = true_block;
		}

		// uint  arr_idx = bitIndex / 32;
		// uint  arr_idx = bitIndex >> 5;  // optimized
		TIntermSymbol*	arr_idx_symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "arr_idx", dbgInfo.GetShaderType(), TType{temp_uint} };
		{
			TIntermBinary*			shift_op		= new TIntermBinary{ TOperator::EOpRightShift };	// bitIndex >> 5
			TIntermBinary*			assign_op		= new TIntermBinary{ TOperator::EOpAssign };		// idx = ...

			TConstUnionArray		shift_value(1);	shift_value[0].setUConst( 5 );
			TIntermConstantUnion*	shift_const		= new TIntermConstantUnion{ shift_value, TType{const_uint} };

			shift_op->setType( TType{temp_uint} );
			shift_op->setLeft( bit_index_symb );
			shift_op->setRight( shift_const );

			assign_op->setType( TType{temp_uint} );
			assign_op->setLeft( arr_idx_symb );
			assign_op->setRight( shift_op );

			fn_body->getSequence().push_back( assign_op );
		}

		// uint  bit = 1u << (bitIndex & 31);
		TIntermSymbol*	bit_symb = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "bit", dbgInfo.GetShaderType(), TType{temp_uint} };
		{
			TIntermBinary*			and_op			= new TIntermBinary{ TOperator::EOpAnd };			// bitIndex & 31
			TIntermBinary*			shift_op		= new TIntermBinary{ TOperator::EOpLeftShift };		// 1u << (bitIndex & 31)
			TIntermBinary*			assign_op		= new TIntermBinary{ TOperator::EOpAssign };		// bit = ...

			TConstUnionArray		and_mask_value(1);	and_mask_value[0].setUConst( 31 );
			TIntermConstantUnion*	and_mask_const	= new TIntermConstantUnion{ and_mask_value, TType{const_uint} };

			TConstUnionArray		shift_value(1);	shift_value[0].setUConst( 1 );
			TIntermConstantUnion*	shift_const		= new TIntermConstantUnion{ shift_value, TType{const_uint} };

			and_op->setType( TType{temp_uint} );
			and_op->setLeft( bit_index_symb );
			and_op->setRight( and_mask_const );

			shift_op->setType( TType{temp_uint} );
			shift_op->setLeft( shift_const );
			shift_op->setRight( and_op );

			assign_op->setType( TType{temp_uint} );
			assign_op->setLeft( bit_symb );
			assign_op->setRight( shift_op );

			fn_body->getSequence().push_back( assign_op );
		}

		// atomicOr( dbg_ShaderTrace.data[arr_idx], bit );
		{
			TIntermAggregate*		atomic_or		= new TIntermAggregate{ TOperator::EOpAtomicOr };
			TIntermBinary*			get_arr			= new TIntermBinary{ TOperator::EOpIndexIndirect };
			TIntermBinary*			get_field		= new TIntermBinary{ TOperator::EOpIndexDirectStruct };
			TIntermSymbol*			storage			= dbgInfo.GetDebugStorage();

			TConstUnionArray		field_idx_value(1);	field_idx_value[0].setUConst( 0 );
			TIntermConstantUnion*	field_idx_const	= new TIntermConstantUnion{ field_idx_value, TType{const_uint} };

			TPublicType				buf_uint = temp_uint;
			buf_uint.qualifier.storage = TStorageQualifier::EvqBuffer;

			get_field->setType( TType{buf_uint} );
			get_field->setLeft( storage );
			get_field->setRight( field_idx_const );

			get_arr->setType( TType{temp_uint} );
			get_arr->setLeft( get_field );
			get_arr->setRight( arr_idx_symb );

			atomic_or->setType( TType{global_uint} );

			atomic_or->getSequence().push_back( get_arr );
			atomic_or->getQualifierList().push_back( TStorageQualifier::EvqInOut );

			atomic_or->getSequence().push_back( bit_symb );
			atomic_or->getQualifierList().push_back( TStorageQualifier::EvqIn );

			fn_body->getSequence().push_back( atomic_or );
		}

		return true;
	}

/*
=================================================
	FindAndReplaceAssertFnBody
=================================================
*/
	ND_ static bool  FindAndReplaceAssertFnBody (TIntermNode* root, AssertsDebugInfo &dbgInfo)
	{
		TIntermAggregate* aggr1 = root->getAsAggregate();
		CHECK_ERR( aggr1 != null );
		CHECK_ERR( dbgInfo.GetDebugStorage() != null );

		bool	assert_found		= false;
		bool	storage_attached	= false;

		for (TIntermNode* node : aggr1->getSequence())
		{
			TIntermAggregate* aggr = node->getAsAggregate();

			if ( aggr->getOp() == TOperator::EOpFunction and
				 aggr->getName() == "dbg_Assert(u1;" )
			{
				CHECK_ERR( ReplaceAssertFnBody( aggr, dbgInfo ));
				assert_found = true;
			}

			if ( aggr->getOp() == TOperator::EOpLinkerObjects )
			{
				aggr->getSequence().push_back( dbgInfo.GetDebugStorage() );
				storage_attached = true;
			}
		}

		if ( not storage_attached )
		{
			auto*	linker_objs = new TIntermAggregate{ TOperator::EOpLinkerObjects };
			linker_objs->getSequence().push_back( dbgInfo.GetDebugStorage() );
			aggr1->getSequence().push_back( linker_objs );
		}

		//CHECK_ERR_MSG( assert_found, "'dbg_Assert' function is not found" );
		return true;
	}

/*
=================================================
	CreateShaderDebugStorage
=================================================
*/
	static void  CreateShaderDebugStorage (uint descSetIndex, AssertsDebugInfo &dbgInfo)
	{
		//	layout(binding=x, std430) coherent buffer dbg_ShaderTraceStorage {
		//		uint  data [];
		//	} dbg_ShaderTrace;

		TPublicType		uint_type;			uint_type.init( Default );
		uint_type.basicType					= TBasicType::EbtUint;
		uint_type.qualifier.storage			= TStorageQualifier::EvqBuffer;
		uint_type.qualifier.layoutMatrix	= TLayoutMatrix::ElmColumnMajor;
		uint_type.qualifier.layoutPacking	= TLayoutPacking::ElpStd430;
		uint_type.qualifier.precision		= TPrecisionQualifier::EpqHigh;
		uint_type.qualifier.layoutOffset	= 0;
		uint_type.qualifier.coherent		= true;
		uint_type.arraySizes				= new TArraySizes{};
		uint_type.arraySizes->addInnerSize();

		TTypeList*		type_list	= new TTypeList{};
		TType*			data_arr	= new TType{uint_type};		data_arr->setFieldName( "data" );

		type_list->push_back( TTypeLoc{ data_arr,	TSourceLoc{} });

		TQualifier		block_qual;	block_qual.clear();
		block_qual.storage			= TStorageQualifier::EvqBuffer;
		block_qual.layoutMatrix		= TLayoutMatrix::ElmColumnMajor;
		block_qual.layoutPacking	= TLayoutPacking::ElpStd430;
		block_qual.layoutBinding	= 0;
		block_qual.layoutSet		= descSetIndex;

		TIntermSymbol*	storage_buf	= new TIntermSymbol{ 0x10000001, "dbg_ShaderTrace", dbgInfo.GetShaderType(), TType{type_list, "dbg_ShaderTraceStorage", block_qual} };

		dbgInfo.SetDebugStorage( storage_buf );
	}

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	InsertAsserts
=================================================
*/
	bool  ShaderTrace::InsertAsserts (TIntermediate &intermediate, uint descSetIndex, bool hasSubgroupBasicOps)
	{
		CHECK_ERR( intermediate.getNumEntryPoints() == 1 );

		if ( hasSubgroupBasicOps )
			intermediate.addRequestedExtension( "GL_KHR_shader_subgroup_basic" );

		AssertsDebugInfo	dbg_info{ intermediate, OUT _assertLocations, _fileMap, hasSubgroupBasicOps };

		TIntermNode*	root = intermediate.getTreeRoot();
		CHECK_ERR( root != null );

		CreateShaderDebugStorage( descSetIndex, dbg_info );

		CHECK_ERR( RecursiveProcessNode( root, dbg_info ));

		CHECK_ERR( FindAndReplaceAssertFnBody( root, dbg_info ));

		CHECK_ERR( ValidateInterm( intermediate ));
		return true;
	}

} // AE::PipelineCompiler
