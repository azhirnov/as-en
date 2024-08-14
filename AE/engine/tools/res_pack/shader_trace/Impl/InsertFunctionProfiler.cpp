// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_shader_realtime_clock.txt
	https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shader_clock.txt
	https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VK_KHR_shader_clock
*/

#define PROFILER
#include "TraceRecording.cpp.h"

namespace AE::PipelineCompiler
{
namespace
{

ND_ static bool  ProcessFunctionDefinition (TIntermAggregate* node, DebugInfo &dbgInfo);
ND_ static bool  ProcessFunctionDefinition2 (TIntermAggregate* node, DebugInfo &dbgInfo);
ND_ static bool  ProcessSymbolNode (TIntermSymbol* node, DebugInfo &dbgInfo);
ND_ static TIntermBinary*     AssignClock (TIntermSymbol* dst, const TSourceLoc &loc, DebugInfo &dbgInfo);
ND_ static TIntermAggregate*  CreateAddTimeToTrace2 (TIntermSymbol* startTimeNode, DebugInfo &dbgInfo);
ND_ static TIntermAggregate*  CreateAddTimeToTrace (TIntermTyped* exprNode, TIntermSymbol* startTimeNode, DebugInfo &dbgInfo);

/*
=================================================
	RecursiveProcessNode
=================================================
*/
static bool  RecursiveProcessNode (TIntermNode* node, DebugInfo &dbgInfo)
{
	if ( not node )
		return true;

	FunctionScope	fn_scope{ node, dbgInfo };

	if ( auto* aggr = node->getAsAggregate() )
	{
		bool	is_func = (aggr->getOp() == TOperator::EOpFunction);

		if ( is_func )
			is_func = ProcessFunctionDefinition( aggr, dbgInfo );

		for (auto& n : aggr->getSequence())
		{
			CHECK_ERR( RecursiveProcessNode( n, dbgInfo ));
		}

		if ( is_func )
			CHECK_ERR( ProcessFunctionDefinition2( aggr, dbgInfo ));

		if ( aggr->getOp() >= TOperator::EOpNegative	 or
			 aggr->getOp() == TOperator::EOpFunctionCall or
			 aggr->getOp() == TOperator::EOpParameters	 )
		{
			fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		}
		return true;
	}

	if ( auto* unary = node->getAsUnaryNode() )
	{
		CHECK_ERR( RecursiveProcessNode( unary->getOperand(), dbgInfo ));
		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		return true;
	}

	if ( auto* binary = node->getAsBinaryNode() )
	{
		CHECK_ERR( RecursiveProcessNode( binary->getLeft(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( binary->getRight(), dbgInfo ));
		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		return true;
	}

	if ( auto* op = node->getAsOperator() )
	{
		return true;
	}

	if ( auto* branch = node->getAsBranchNode() )
	{
		// record function time at the end
		if ( branch->getFlowOp() == TOperator::EOpReturn )
		{
			if ( auto* start_time_node = dbgInfo.GetStartTime() )
			{
				if ( not branch->getExpression() or
					 branch->getExpression()->getType().getBasicType() == TBasicType::EbtVoid )
				{
					branch->~TIntermBranch();
					new(branch) TIntermBranch{ TOperator::EOpReturn, CreateAddTimeToTrace2( start_time_node, dbgInfo )};
				}
				else
				{
					TIntermAggregate*	time_trace = CreateAddTimeToTrace( branch->getExpression(), start_time_node, dbgInfo );
					CHECK_ERR( time_trace != null );

					branch->~TIntermBranch();
					new(branch) TIntermBranch{ TOperator::EOpReturn, time_trace };
				}
			}
		}

		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		CHECK_ERR( RecursiveProcessNode( branch->getExpression(), dbgInfo ));
		return true;
	}

	if ( auto* sw = node->getAsSwitchNode() )
	{
		CHECK_ERR( RecursiveProcessNode( sw->getCondition(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( sw->getBody(), dbgInfo ));
		return true;
	}

	if ( auto* cunion = node->getAsConstantUnion() )
	{
		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		return true;
	}

	if ( auto* selection = node->getAsSelectionNode() )
	{
		CHECK_ERR( RecursiveProcessNode( selection->getCondition(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( selection->getTrueBlock(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( selection->getFalseBlock(), dbgInfo ));
		return true;
	}

	if ( auto* method = node->getAsMethodNode() )
	{
		return true;
	}

	if ( auto* symbol = node->getAsSymbolNode() )
	{
		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		CHECK_ERR( ProcessSymbolNode( symbol, dbgInfo ));
		return true;
	}

	if ( auto* typed = node->getAsTyped() )
	{
		fn_scope.SetLoc( dbgInfo.GetCurrentLocation() );
		return true;
	}

	if ( auto* loop = node->getAsLoopNode() )
	{
		CHECK_ERR( RecursiveProcessNode( loop->getBody(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( loop->getTerminal(), dbgInfo ));
		CHECK_ERR( RecursiveProcessNode( loop->getTest(), dbgInfo ));
		return true;
	}

	return false;
}

/*
=================================================
	CreateAddTimeToTraceBody2
----
	also see 'CreateAddTimeToTrace2()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAddTimeToTraceBody2 (DebugInfo &dbgInfo)
{
	TPublicType		uint_type;	uint_type.init( Default );
	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;

	// last_pos, location, size, startTime, endTime
	const uint			dbg_data_size = 3 + 4 + 4;

	TIntermAggregate*	fn_node		= new TIntermAggregate{ TOperator::EOpFunction };
	TIntermAggregate*	fn_args		= new TIntermAggregate{ TOperator::EOpParameters };
	TIntermAggregate*	fn_body		= new TIntermAggregate{ TOperator::EOpSequence };
	TIntermAggregate*	branch_body = new TIntermAggregate{ TOperator::EOpSequence };

	// build function body
	{
		fn_node->setType( TType{ TBasicType::EbtVoid, TStorageQualifier::EvqGlobal } );
		fn_body->setType( TType{ TBasicType::EbtVoid } );
		fn_node->setName( "dbg_AddTimeToTrace(vu4;u1;" );
		fn_node->getSequence().push_back( fn_args );
		fn_node->getSequence().push_back( fn_body );
	}

	// build function argument sequence
	{
		uint_type.vectorSize	= 4;
		TIntermSymbol*	arg0	= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "startTime", TType{uint_type} };

		uint_type.vectorSize	= 1;
		TIntermSymbol*	arg1	= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "sourceLocation", TType{uint_type} };

		fn_args->setType( TType{EbtVoid} );
		fn_args->getSequence().push_back( arg0 );
		fn_args->getSequence().push_back( arg1 );
	}

	// build branch body
	branch_body->setType( TType{EbtVoid} );

	// "endTime = vec4( clock(), clock() );"
	uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
	uint_type.vectorSize		= 4;
	TIntermSymbol*	end_time	= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "endTime", TType{uint_type} };

	TIntermBinary*  end_time_assign = AssignClock( end_time, Default, dbgInfo );
	CHECK_ERR( end_time_assign != null );
	branch_body->getSequence().push_back( end_time_assign );

	// "pos" variable
	uint_type.vectorSize		= 1;
	TIntermSymbol*	var_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "pos", TType{uint_type} };

	// "uint pos = atomicAdd( dbg_ShaderTrace.position, x );"
	{
		TIntermAggregate*	move_pos	= new TIntermAggregate{ TOperator::EOpSequence };
		TIntermBinary*		assign_op	= new TIntermBinary{ TOperator::EOpAssign };			// pos = ...

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
		TConstUnionArray		type_value(1);	type_value[0].setUConst( (ShaderTrace::TBasicType_Clock & 0xFF) | (4 << 8) );
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

		const auto	WriteUVec4 = [&branch_body, &indexed_access, &uint_type] (TIntermTyped* vec)
		{{
			TPublicType		index_type;	index_type.init( Default );
			index_type.basicType		= TBasicType::EbtInt;
			index_type.qualifier.storage= TStorageQualifier::EvqConst;

			for (int i = 0; i < vec->getType().getVectorSize(); ++i)
			{
				TIntermBinary*			assign_data		= new TIntermBinary{ TOperator::EOpAssign };
				TConstUnionArray		field_index(1);	field_index[0].setIConst( i );
				TIntermConstantUnion*	vec_field		= new TIntermConstantUnion{ field_index, TType{index_type} };
				TIntermBinary*			field_access	= new TIntermBinary{ TOperator::EOpIndexDirect };

				TPublicType		pub_type;	pub_type.init( Default );
				pub_type.basicType			= vec->getType().getBasicType();
				pub_type.qualifier.storage	= vec->getType().getQualifier().storage;

				// "value.x"
				field_access->setType( TType{pub_type} );
				field_access->setLeft( vec );
				field_access->setRight( vec_field );

				// "dbg_ShaderTrace.outData[pos++] = value.x"
				assign_data->setType( TType{uint_type} );
				assign_data->setLeft( indexed_access );
				assign_data->setRight( field_access );
				branch_body->getSequence().push_back( assign_data );
			}
		}};

		// "dbg_ShaderTrace.outData[pos++] = startTime"
		WriteUVec4( fn_args->getSequence()[0]->getAsTyped() );

		// "dbg_ShaderTrace.outData[pos++] = endTime"
		WriteUVec4( end_time );
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
	CreateAddTimeToTraceBody
----
	also see 'CreateAddTimeToTrace()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAddTimeToTraceBody (const TString &fnName, DebugInfo &dbgInfo)
{
	TPublicType		value_type;	value_type.init( Default );
	TPublicType		uint_type;	uint_type.init( Default );
	TPublicType		index_type;	index_type.init( Default );

	uint_type.basicType			= TBasicType::EbtUint;
	uint_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;

	index_type.basicType		= TBasicType::EbtInt;
	index_type.qualifier.storage= TStorageQualifier::EvqConst;

	// extract type
	{
		usize	pos  = fnName.find( ';' );
		usize	pos1 = fnName.find( ';', pos+1 );
		CHECK_ERR( pos != TString::npos and pos1 != TString::npos and pos < pos1 );
		++pos;

		const bool	is_vector	= (fnName[pos] == 'v');
		const bool	is_matrix	= (fnName[pos] == 'm');

		if ( is_vector or is_matrix )	++pos;

		const bool	is_64	= (pos+2 < fnName.size() and fnName[pos+1] == '6' and fnName[pos+2] == '4');
		//const bool	is_16	= (pos+2 < fnName.size() and fnName[pos+1] == '1' and fnName[pos+2] == '6');

		switch ( fnName[pos] )
		{
			case 'f' :	value_type.basicType = TBasicType::EbtFloat;	break;
			case 'd' :	value_type.basicType = TBasicType::EbtDouble;	break;
			case 'b' :	value_type.basicType = TBasicType::EbtBool;		break;
			case 'i' :	if ( is_64 ) { value_type.basicType = TBasicType::EbtInt64;  } else value_type.basicType = TBasicType::EbtInt;	break;
			case 'u' :	if ( is_64 ) { value_type.basicType = TBasicType::EbtUint64; } else value_type.basicType = TBasicType::EbtUint;	break;
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

	// last_pos, location, size, startTime, endTime
	const uint			dbg_data_size = 3 + 4 + 4;

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
		uint_type.vectorSize		= 4;
		TIntermSymbol*		arg0	 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "startTime", TType{uint_type} };

		value_type.qualifier.storage = TStorageQualifier::EvqConstReadOnly;
		TIntermSymbol*		arg1	 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "value", TType{value_type} };

		uint_type.vectorSize		= 1;
		TIntermSymbol*		arg2	 = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "sourceLocation", TType{uint_type} };

		fn_args->setType( TType{EbtVoid} );
		fn_args->getSequence().push_back( arg0 );
		fn_args->getSequence().push_back( arg1 );
		fn_args->getSequence().push_back( arg2 );
	}

	// build function body
	{
		value_type.qualifier.storage = TStorageQualifier::EvqTemporary;
		fn_body->setType( TType{value_type} );
	}

	// build branch body
	branch_body->setType( TType{EbtVoid} );

	// "endTime = vec4( clock(), clock() );"
	uint_type.qualifier.storage = TStorageQualifier::EvqTemporary;
	uint_type.vectorSize		= 4;
	TIntermSymbol*	end_time	= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "endTime", TType{uint_type} };

	TIntermBinary*  end_time_assign = AssignClock( end_time, Default, dbgInfo );
	CHECK_ERR( end_time_assign != null );
	branch_body->getSequence().push_back( end_time_assign );

	// "pos" variable
	uint_type.vectorSize		= 1;
	TIntermSymbol*	var_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "pos", TType{uint_type} };

	// "uint pos = atomicAdd( dbg_ShaderTrace.position, x );"
	{
		TIntermAggregate*	move_pos	= new TIntermAggregate{ TOperator::EOpSequence };
		TIntermBinary*		assign_op	= new TIntermBinary{ TOperator::EOpAssign };			// pos = ...

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
		TConstUnionArray		type_value(1);	type_value[0].setUConst( (ShaderTrace::TBasicType_Clock & 0xFF) | (4 << 8) );
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
		assign_data1->setRight( fn_args->getSequence()[2]->getAsTyped() );
		branch_body->getSequence().push_back( assign_data1 );

		// "dbg_ShaderTrace.outData[pos++] = typeid"
		assign_data2->setType( TType{uint_type} );
		assign_data2->setLeft( indexed_access );
		assign_data2->setRight( type_id );
		branch_body->getSequence().push_back( assign_data2 );

		const auto	WriteUVec4 = [&branch_body, &indexed_access, &uint_type, &index_type] (TIntermTyped* vec)
		{{
			for (int i = 0; i < vec->getType().getVectorSize(); ++i)
			{
				TIntermBinary*			assign_data		= new TIntermBinary{ TOperator::EOpAssign };
				TConstUnionArray		field_index(1);	field_index[0].setIConst( i );
				TIntermConstantUnion*	vec_field		= new TIntermConstantUnion{ field_index, TType{index_type} };
				TIntermBinary*			field_access	= new TIntermBinary{ TOperator::EOpIndexDirect };

				TPublicType		pub_type;	pub_type.init( Default );
				pub_type.basicType			= vec->getType().getBasicType();
				pub_type.qualifier.storage	= vec->getType().getQualifier().storage;

				// "value.x"
				field_access->setType( TType{pub_type} );
				field_access->setLeft( vec );
				field_access->setRight( vec_field );

				// "dbg_ShaderTrace.outData[pos++] = value.x"
				assign_data->setType( TType{uint_type} );
				assign_data->setLeft( indexed_access );
				assign_data->setRight( field_access );
				branch_body->getSequence().push_back( assign_data );
			}
		}};

		// "dbg_ShaderTrace.outData[pos++] = startTime"
		WriteUVec4( fn_args->getSequence()[0]->getAsTyped() );

		// "dbg_ShaderTrace.outData[pos++] = endTime"
		WriteUVec4( end_time );
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
		TIntermBranch*		fn_return	= new TIntermBranch{ TOperator::EOpReturn, fn_args->getSequence()[1]->getAsTyped() };
		fn_body->getSequence().push_back( fn_return );
	}

	return fn_node;
}

/*
=================================================
	CreateGetCurrentTimeBody
----
	also see 'AssignClock()'
=================================================
*/
ND_ static TIntermAggregate*  CreateGetCurrentTimeBody (DebugInfo &dbgInfo)
{
	TIntermAggregate*	fn_node		= new TIntermAggregate{ TOperator::EOpFunction };
	TIntermAggregate*	fn_args		= new TIntermAggregate{ TOperator::EOpParameters };
	TIntermAggregate*	fn_body		= new TIntermAggregate{ TOperator::EOpSequence };
	TIntermAggregate*	branch_true = new TIntermAggregate{ TOperator::EOpSequence };
	TIntermSymbol*		curr_time	= null;
	TPublicType			uint_type;	uint_type.init( Default );

	// build function body
	{
		uint_type.basicType				= TBasicType::EbtUint;
		uint_type.vectorSize			= 4;
		uint_type.qualifier.storage		= TStorageQualifier::EvqGlobal;
		uint_type.qualifier.precision	= TPrecisionQualifier::EpqHigh;

		fn_node->setType( TType{uint_type} );
		fn_node->setName( "dbg_GetCurrentTime(" );
		fn_node->getSequence().push_back( fn_args );
		fn_node->getSequence().push_back( fn_body );

		uint_type.qualifier.storage	= TStorageQualifier::EvqTemporary;
		curr_time					= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "curr_time", TType{uint_type} };

		TConstUnionArray		zero_value{ 4 };
		zero_value[0].setUConst( 0 );
		zero_value[1].setUConst( 0 );
		zero_value[2].setUConst( 0 );
		zero_value[3].setUConst( 0 );
		uint_type.qualifier.storage			= TStorageQualifier::EvqConst;
		TIntermConstantUnion*	zero_const	= new TIntermConstantUnion{ zero_value, TType{uint_type} };

		TIntermBinary*		init_time	= new TIntermBinary{ TOperator::EOpAssign };
		uint_type.qualifier.storage		= TStorageQualifier::EvqTemporary;
		init_time->setType( TType{uint_type} );
		init_time->setLeft( curr_time );
		init_time->setRight( zero_const );

		fn_body->getSequence().push_back( init_time );
	}

	// build 'true' branch body
	{
		branch_true->setType( TType{EbtVoid} );

		TPublicType		int_type;	int_type.init( Default );
		int_type.basicType			= TBasicType::EbtInt;
		int_type.vectorSize			= 1;
		int_type.qualifier.storage	= TStorageQualifier::EvqConst;

		uint_type.vectorSize		= 2;
		uint_type.qualifier.storage	= TStorageQualifier::EvqTemporary;

		if ( dbgInfo._shaderSubgroupClock )
		{
			TConstUnionArray		x_value{1};	x_value[0].setIConst( 0 );
			TConstUnionArray		y_value{1};	y_value[0].setIConst( 1 );
			TIntermConstantUnion*	x_index		= new TIntermConstantUnion{ x_value, TType{int_type} };
			TIntermConstantUnion*	y_index		= new TIntermConstantUnion{ y_value, TType{int_type} };
			TIntermAggregate*		xy_index	= new TIntermAggregate{ TOperator::EOpSequence };

			xy_index->getSequence().push_back( x_index );
			xy_index->getSequence().push_back( y_index );

			TIntermBinary*			assign		= new TIntermBinary{ TOperator::EOpAssign };
			TIntermBinary*			swizzle		= new TIntermBinary{ TOperator::EOpVectorSwizzle };
			TIntermAggregate*		time_call	= new TIntermAggregate( TOperator::EOpReadClockSubgroupKHR );

			swizzle->setType( TType{uint_type} );
			swizzle->setLeft( curr_time );
			swizzle->setRight( xy_index );

			assign->setType( TType{uint_type} );
			assign->setLeft( swizzle );
			assign->setRight( time_call );

			time_call->setType( TType{uint_type} );

			branch_true->getSequence().push_back( assign );
		}

		if ( dbgInfo._shaderDeviceClock )
		{
			TConstUnionArray		z_value{1};	z_value[0].setIConst( 2 );
			TConstUnionArray		w_value{1};	w_value[0].setIConst( 3 );
			TIntermConstantUnion*	z_index		= new TIntermConstantUnion{ z_value, TType{int_type} };
			TIntermConstantUnion*	w_index		= new TIntermConstantUnion{ w_value, TType{int_type} };
			TIntermAggregate*		zw_index	= new TIntermAggregate{ TOperator::EOpSequence };

			zw_index->getSequence().push_back( z_index );
			zw_index->getSequence().push_back( w_index );

			TIntermBinary*			assign		= new TIntermBinary{ TOperator::EOpAssign };
			TIntermBinary*			swizzle		= new TIntermBinary{ TOperator::EOpVectorSwizzle };
			TIntermAggregate*		time_call	= new TIntermAggregate( TOperator::EOpReadClockDeviceKHR );

			swizzle->setType( TType{uint_type} );
			swizzle->setLeft( curr_time );
			swizzle->setRight( zw_index );

			assign->setType( TType{uint_type} );
			assign->setLeft( swizzle );
			assign->setRight( time_call );

			time_call->setType( TType{uint_type} );

			branch_true->getSequence().push_back( assign );
		}
	}

	// "if ( dbg_IsEnabled )"
	{
		TIntermSymbol*		condition	= dbgInfo.GetCachedSymbolNode( "dbg_IsEnabled" );
		CHECK_ERR( condition != null );

		TIntermSelection*	selection	= new TIntermSelection{ condition, branch_true, null };
		selection->setType( TType{EbtVoid} );

		fn_body->getSequence().push_back( selection );
	}

	// "return curr_time;"
	{
		TIntermBranch*		fn_return	= new TIntermBranch{ TOperator::EOpReturn, curr_time };
		fn_body->getSequence().push_back( fn_return );
	}

	return fn_node;
}

/*
=================================================
	InsertGlobalVariablesAndBuffers
=================================================
*/
ND_ static bool  InsertGlobalVariablesAndBuffers (TIntermAggregate* linkerObjs, TIntermAggregate* globalVars, uint initialPosition, DebugInfo &dbgInfo)
{
	// "bool dbg_IsEnabled"
	TPublicType		type;	type.init( Default );
	type.basicType			= TBasicType::EbtBool;
	type.qualifier.storage	= TStorageQualifier::EvqGlobal;

	TIntermSymbol*			is_debug_enabled = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "dbg_IsEnabled", TType{type} };
	dbgInfo.CacheSymbolNode( is_debug_enabled );
	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), is_debug_enabled );

	// "dbg_IsEnabled = ..."
	TIntermBinary*			init_debug_enabled = new TIntermBinary{ TOperator::EOpAssign };
	type.qualifier.storage = TStorageQualifier::EvqTemporary;
	init_debug_enabled->setType( TType{type} );
	init_debug_enabled->setLeft( is_debug_enabled );
	globalVars->getSequence().insert( globalVars->getSequence().begin(), init_debug_enabled );

	switch_enum( dbgInfo.GetShaderType() )
	{
		case EShLangVertex :
		case EShLangTessControl :
		case EShLangTessEvaluation :
		case EShLangGeometry :
		case EShLangTask :
		case EShLangMesh :
		{
			type.qualifier.storage	= TStorageQualifier::EvqConst;
			TConstUnionArray		false_value(1);	false_value[0].setBConst( false );
			TIntermConstantUnion*	const_false		= new TIntermConstantUnion{ false_value, TType{type} };

			init_debug_enabled->setRight( const_false );
			break;
		}

		case EShLangFragment :
		{
			auto*	op = CreateFragmentShaderIsDebugInvocation( dbgInfo );
			CHECK_ERR( op != null );
			init_debug_enabled->setRight( op );
			break;
		}

		case EShLangCompute :
		{
			auto*	op = CreateComputeShaderIsDebugInvocation( dbgInfo );
			CHECK_ERR( op != null );
			init_debug_enabled->setRight( op );
			break;
		}

		case EShLangRayGen :
		case EShLangIntersect :
		case EShLangAnyHit :
		case EShLangClosestHit :
		case EShLangMiss :
		case EShLangCallable :
		{
			auto*	op = CreateRayTracingShaderIsDebugInvocation( dbgInfo );
			CHECK_ERR( op != null );
			init_debug_enabled->setRight( op );
			break;
		}

		case EShLangCount :
		default :					RETURN_ERR( "not supported" );
	}
	switch_end


	// "uint dbg_LastPosition"
	type.basicType			= TBasicType::EbtUint;
	type.qualifier.storage	= TStorageQualifier::EvqGlobal;

	TIntermSymbol*			last_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "dbg_LastPosition", TType{type} };
	dbgInfo.CacheSymbolNode( last_pos );
	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), last_pos );

	// "dbg_LastPosition = ~0u"
	type.qualifier.storage = TStorageQualifier::EvqConst;
	TConstUnionArray		init_value(1);	init_value[0].setUConst( initialPosition );
	TIntermConstantUnion*	init_const		= new TIntermConstantUnion{ init_value, TType{type} };
	TIntermBinary*			init_pos		= new TIntermBinary{ TOperator::EOpAssign };

	type.qualifier.storage = TStorageQualifier::EvqTemporary;
	init_pos->setType( TType{type} );
	init_pos->setLeft( last_pos );
	init_pos->setRight( init_const );
	globalVars->getSequence().insert( globalVars->getSequence().begin(), init_pos );


	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), dbgInfo.GetDebugStorage() );
	return true;
}

/*
=================================================
	CreateEnableIfBody
=================================================
*/
ND_ static bool  CreateEnableIfBody (TIntermAggregate* fnDecl, DebugInfo &dbgInfo)
{
	CHECK_ERR( fnDecl->getName() == "dbg_EnableProfiling(b1;" );
	CHECK_ERR( fnDecl->getSequence().size() >= 1 );

	auto*	params = fnDecl->getSequence()[0]->getAsAggregate();
	CHECK_ERR( params and params->getOp() == TOperator::EOpParameters );

	TIntermTyped*		arg		= params->getSequence()[0]->getAsTyped();
	CHECK_ERR( arg != null );

	TIntermAggregate*	body	= null;

	if ( fnDecl->getSequence().size() == 1 )
	{
		body = new TIntermAggregate{ TOperator::EOpSequence };
		fnDecl->getSequence().push_back( body );
	}
	else
	{
		body = fnDecl->getSequence()[1]->getAsAggregate();
		CHECK_ERR( body != null );
	}

	TIntermSymbol*		is_debug_enabled = dbgInfo.GetCachedSymbolNode( "dbg_IsEnabled" );
	TIntermAggregate*	branch_body		 = RecordShaderInfo( TSourceLoc{}, dbgInfo );
	CHECK_ERR( is_debug_enabled and branch_body );

	TPublicType		type;	type.init( Default );
	type.basicType			= TBasicType::EbtBool;
	type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	// "not dbg_IsEnabled"
	TIntermUnary*		condition	= new TIntermUnary{ TOperator::EOpLogicalNot };
	condition->setType( TType{type} );
	condition->setOperand( is_debug_enabled );

	// "if ( not dbg_IsEnabled ) {...}"
	TIntermSelection*	selection	= new TIntermSelection{ arg, branch_body, null };
	selection->setType( TType{EbtVoid} );
	body->getSequence().push_back( selection );

	// "dbg_IsEnabled = arg"
	TIntermBinary*	assign = new TIntermBinary{ TOperator::EOpAssign };
	assign->setType( TType{type} );
	assign->setLeft( is_debug_enabled );
	assign->setRight( arg );
	branch_body->getSequence().insert( branch_body->getSequence().begin(), assign );

	return true;
}

/*
=================================================
	CreateDebugTraceFunctions
=================================================
*/
ND_ static bool  CreateDebugTraceFunctions (TIntermNode* root, uint initialPosition, DebugInfo &dbgInfo)
{
	TIntermAggregate*	aggr = root->getAsAggregate();
	CHECK_ERR( aggr != null );

	TIntermAggregate*	linker_objs	= null;
	TIntermAggregate*	global_vars	= null;

	for (auto& entry : aggr->getSequence())
	{
		if ( auto*  aggr2 = entry->getAsAggregate() )
		{
			if ( aggr2->getOp() == TOperator::EOpLinkerObjects )
				linker_objs = aggr2;

			if ( aggr2->getOp() == TOperator::EOpSequence )
				global_vars = aggr2;
		}
	}

	if ( not linker_objs ) {
		linker_objs = new TIntermAggregate{ TOperator::EOpLinkerObjects };
		aggr->getSequence().push_back( linker_objs );
	}

	if ( not global_vars ) {
		global_vars = new TIntermAggregate{ TOperator::EOpSequence };
		aggr->getSequence().push_back( global_vars );
	}

	CHECK_ERR( InsertGlobalVariablesAndBuffers( linker_objs, global_vars, initialPosition, dbgInfo ));

	for (auto& entry : aggr->getSequence())
	{
		auto*	aggr2 = entry->getAsAggregate();
		if ( not (aggr2 and aggr2->getOp() == TOperator::EOpFunction) )
			continue;

		if ( aggr2->getName().c_str() == dbgInfo.GetEntryPoint()	and
			 aggr2->getSequence().size() >= 2 )
		{
			auto*	body = aggr2->getSequence()[1]->getAsAggregate();
			CHECK_ERR( body != null );

			auto*	shader_info = RecordShaderInfo( TSourceLoc{}, dbgInfo );
			CHECK_ERR( shader_info != null );

			body->getSequence().insert( body->getSequence().begin(), shader_info );
		}
		else
		if ( aggr2->getName().rfind( "dbg_EnableProfiling(", 0 ) == 0 )
		{
			CHECK_ERR( CreateEnableIfBody( INOUT aggr2, dbgInfo ));
		}
	}

	for (auto& fn : dbgInfo.GetRequiredFunctions())
	{
		if ( fn == "dbg_AppendToTrace(u1;" )
		{
			auto*	body = CreateAppendToTraceBody2( dbgInfo );
			CHECK_ERR( body != null );

			aggr->getSequence().push_back( body );
		}
		else
		if ( fn.rfind( "dbg_AppendToTrace(", 0 ) == 0 )
		{
			auto*	body = CreateAppendToTraceBody( fn, dbgInfo );
			CHECK_ERR( body != null );

			aggr->getSequence().push_back( body );
		}
		else
		if ( fn == "dbg_AddTimeToTrace(vu4;u1;" )
		{
			auto*	body = CreateAddTimeToTraceBody2( dbgInfo );
			CHECK_ERR( body != null );

			aggr->getSequence().push_back( body );
		}
		else
		if ( fn.rfind( "dbg_AddTimeToTrace(", 0 ) == 0 )
		{
			auto*	body = CreateAddTimeToTraceBody( fn, dbgInfo );
			CHECK_ERR( body != null );

			aggr->getSequence().push_back( body );
		}
		else
		if ( fn == "dbg_GetCurrentTime(" )
		{
			auto*	body = CreateGetCurrentTimeBody( dbgInfo );
			CHECK_ERR( body != null );

			aggr->getSequence().push_back( body );
		}
		else
			RETURN_ERR( "unknown function" );
	}
	return true;
}

/*
=================================================
	CreateAddTimeToTrace2
----
	also see 'CreateAddTimeToTraceBody2()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAddTimeToTrace2 (TIntermSymbol* startTimeNode, DebugInfo &dbgInfo)
{
	TIntermAggregate*	fcall = new TIntermAggregate( TOperator::EOpFunctionCall );

	fcall->setUserDefined();
	fcall->setName( TString{"dbg_AddTimeToTrace(vu4;u1;"} );
	fcall->setType( TType{ TBasicType::EbtVoid, TStorageQualifier::EvqGlobal });
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );

	TPublicType		uint_type;		uint_type.init( Default );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.qualifier.storage		= TStorageQualifier::EvqConst;

	auto	end_loc = startTimeNode->getLoc();
	end_loc.column = 0;

	const uint				source_loc		= dbgInfo.GetCustomSourceLocation2( startTimeNode, startTimeNode->getLoc(), end_loc );
	TConstUnionArray		loc_value(1);	loc_value[0].setUConst( source_loc );
	TIntermConstantUnion*	loc_const		= new TIntermConstantUnion{ loc_value, TType{uint_type} };

	loc_const->setLoc( Default );
	fcall->getSequence().push_back( startTimeNode );
	fcall->getSequence().push_back( loc_const );

	dbgInfo.RequestFunc( fcall->getName() );

	return fcall;
}

/*
=================================================
	CreateAddTimeToTrace
----
	returns new node instead of 'exprNode'.
	'exprNode' - any operator.
	also see 'CreateAddTimeToTraceBody()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAddTimeToTrace (TIntermTyped* exprNode, TIntermSymbol* startTimeNode, DebugInfo &dbgInfo)
{
	CHECK_ERR( exprNode and startTimeNode );

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

		case TBasicType::EbtSampler :	return null;
		case TBasicType::EbtStruct :	return null;

		case TBasicType::EbtVoid :
		case TBasicType::EbtFloat16 :
		case TBasicType::EbtInt8 :
		case TBasicType::EbtUint8 :
		case TBasicType::EbtInt16 :
		case TBasicType::EbtUint16 :
		case TBasicType::EbtAtomicUint :
		case TBasicType::EbtBlock :
		case TBasicType::EbtString :
		case TBasicType::EbtReference :
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
		RETURN_ERR( "arrays is not supported yet" )
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
	fcall->setName( TString{"dbg_AddTimeToTrace(vu4;"} + TString{type_name.c_str()} + ";u1;" );
	fcall->setType( exprNode->getType() );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );
	fcall->getSequence().push_back( startTimeNode );
	fcall->getSequence().push_back( exprNode );

	TPublicType		uint_type;		uint_type.init( exprNode->getLoc() );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.qualifier.storage		= TStorageQualifier::EvqConst;

	auto	end_loc = startTimeNode->getLoc();
	end_loc.column = 0;

	const uint				source_loc		= dbgInfo.GetCustomSourceLocation2( startTimeNode, startTimeNode->getLoc(), end_loc );
	TConstUnionArray		loc_value(1);	loc_value[0].setUConst( source_loc );
	TIntermConstantUnion*	loc_const		= new TIntermConstantUnion{ loc_value, TType{uint_type} };

	loc_const->setLoc( exprNode->getLoc() );
	fcall->getSequence().push_back( loc_const );

	dbgInfo.RequestFunc( fcall->getName() );

	return fcall;
}

/*
=================================================
	AssignClock
----
	also see 'CreateGetCurrentTimeBody'
=================================================
*/
ND_ static TIntermBinary*  AssignClock (TIntermSymbol* dst, const TSourceLoc &loc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	fcall		= new TIntermAggregate( TOperator::EOpFunctionCall );
	TPublicType			uint_type;	uint_type.init( Default );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.vectorSize			= 4;
	uint_type.qualifier.storage		= TStorageQualifier::EvqTemporary;
	uint_type.qualifier.precision	= TPrecisionQualifier::EpqHigh;

	fcall->setLoc( loc );
	fcall->setUserDefined();
	fcall->setName( "dbg_GetCurrentTime(" );
	fcall->setType( TType{uint_type} );

	TIntermBinary*		assign_time	= new TIntermBinary{ TOperator::EOpAssign };
	assign_time->setLoc( loc );
	assign_time->setType( TType{uint_type} );
	assign_time->setLeft( dst );
	assign_time->setRight( fcall );

	dbgInfo.RequestFunc( fcall->getName() );

	return assign_time;
}

/*
=================================================
	ProcessFunctionDefinition
----
	begin function profiling
=================================================
*/
ND_ static bool  ProcessFunctionDefinition (TIntermAggregate* node, DebugInfo &dbgInfo)
{
	if ( node->getSequence().size() < 2 )
		return false;	// empty function

	CHECK_ERR( node->getSequence()[0]->getAsOperator()->getOp() == TOperator::EOpParameters );
	CHECK_ERR( node->getSequence()[1]->getAsOperator()->getOp() == TOperator::EOpSequence );

	TIntermAggregate*	body = node->getSequence()[1]->getAsAggregate();

	TIntermSymbol*		start_time_node	= dbgInfo.CreateStartTimeSymbolNode();
	start_time_node->setLoc( node->getLoc() );

	TIntermBinary*		assign_time	= AssignClock( start_time_node, body->getLoc(), dbgInfo );
	CHECK_ERR( assign_time != null );

	body->getSequence().insert( body->getSequence().begin(), assign_time );

	CHECK_ERR( dbgInfo.PushStartTime( start_time_node ));
	return true;
}

/*
=================================================
	ProcessFunctionDefinition2
----
	end function profiling
=================================================
*/
ND_ static bool  ProcessFunctionDefinition2 (TIntermAggregate* node, DebugInfo &dbgInfo)
{
	CHECK_ERR( node->getSequence().size() >= 2 );
	CHECK_ERR( node->getSequence()[0]->getAsOperator()->getOp() == TOperator::EOpParameters );
	CHECK_ERR( node->getSequence()[1]->getAsOperator()->getOp() == TOperator::EOpSequence );

	TIntermAggregate*	body		= node->getSequence()[1]->getAsAggregate();
	TIntermNode*		last_node	= body->getSequence().back();

	if ( auto* branch = last_node->getAsBranchNode(); branch and branch->getFlowOp() == TOperator::EOpReturn )
		return true;

	TIntermSymbol*		start_time	= dbgInfo.GetStartTime();
	CHECK_ERR( start_time != null );

	body->getSequence().push_back( CreateAddTimeToTrace2( start_time, dbgInfo ));

	CHECK_ERR( dbgInfo.PopStartTime() );
	return true;
}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	InsertFunctionProfiler
=================================================
*/
	bool  ShaderTrace::InsertFunctionProfiler (TIntermediate &intermediate, uint descSetIndex, bool shaderSubgroupClock, bool shaderDeviceClock)
	{
		CHECK_ERR( shaderSubgroupClock or shaderDeviceClock );
		CHECK_ERR( intermediate.getNumEntryPoints() == 1 );

		if ( shaderSubgroupClock )
			intermediate.addRequestedExtension( "GL_ARB_shader_clock" );

		if ( shaderDeviceClock )
			intermediate.addRequestedExtension( "GL_EXT_shader_realtime_clock" );

		DebugInfo		dbg_info{ intermediate, OUT _exprLocations, _fileMap, shaderSubgroupClock, shaderDeviceClock };

		TIntermNode*	root = intermediate.getTreeRoot();
		CHECK_ERR( root != null );

		_initialPosition = uint(usize( HashOf( this ) + HashVal{ descSetIndex+1 }));
		ASSERT( _initialPosition != 0 );
		_initialPosition |= InitialPositionMask;

		CreateShaderDebugStorage( descSetIndex, dbg_info, OUT _posOffset, OUT _dataOffset );

		dbg_info.Enter( root );
		{
			CHECK_ERR( RecursiveProcessNode( root, dbg_info ));

			CreateShaderBuiltinSymbols( root, dbg_info );

			CHECK_ERR( CreateDebugTraceFunctions( root, _initialPosition, dbg_info ));
		}
		dbg_info.Leave( root );

		CHECK_ERR( dbg_info.UpdateSymbolIDs() );
		CHECK_ERR( dbg_info.PostProcess( OUT _varNames ));

		CHECK_ERR( ValidateInterm( intermediate ));
		return true;
	}

} // AE::PipelineCompiler
