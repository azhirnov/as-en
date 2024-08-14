// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TraceRecording.cpp.h"

namespace AE::PipelineCompiler
{
namespace
{

ND_ static bool  RecursiveProcessAggregateNode (TIntermAggregate* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProcessBranchNode (TIntermBranch* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProcessSwitchNode (TIntermSwitch* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProcessSelectionNode (TIntermSelection* node, DebugInfo &dbgInfo);
ND_ static bool  ProcessSymbolNode (TIntermSymbol* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProcessUnaryNode (TIntermUnary* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProcessBinaryNode (TIntermBinary* node, DebugInfo &dbgInfo);
ND_ static bool  RecursiveProccessLoop (TIntermLoop* node, DebugInfo &dbgInfo);

/*
=================================================
	RecursiveProcessNode
=================================================
*/
ND_ static bool  RecursiveProcessNode (TIntermNode* node, DebugInfo &dbgInfo)
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
		CHECK_ERR( RecursiveProcessUnaryNode( unary, dbgInfo ));
		return true;
	}

	if ( auto* binary = node->getAsBinaryNode() )
	{
		CHECK_ERR( RecursiveProcessBinaryNode( binary, dbgInfo ));
		return true;
	}

	if ( auto* op = node->getAsOperator() )
	{
		return false;
	}

	if ( auto* branch = node->getAsBranchNode() )
	{
		CHECK_ERR( RecursiveProcessBranchNode( branch, dbgInfo ));
		return true;
	}

	if ( auto* sw = node->getAsSwitchNode() )
	{
		CHECK_ERR( RecursiveProcessSwitchNode( sw, dbgInfo ));
		return true;
	}

	if ( auto* cunion = node->getAsConstantUnion() )
	{
		dbgInfo.AddLocation( node->getLoc() );
		return true;
	}

	if ( auto* selection = node->getAsSelectionNode() )
	{
		CHECK_ERR( RecursiveProcessSelectionNode( selection, dbgInfo ));
		return true;
	}

	if ( auto* method = node->getAsMethodNode() )
	{
		return true;
	}

	if ( auto* symbol = node->getAsSymbolNode() )
	{
		dbgInfo.AddLocation( node->getLoc() );
		CHECK_ERR( ProcessSymbolNode( symbol, dbgInfo ));
		return true;
	}

	if ( auto* typed = node->getAsTyped() )
	{
		dbgInfo.AddLocation( node->getLoc() );
		return true;
	}

	if ( auto* loop = node->getAsLoopNode() )
	{
		CHECK_ERR( RecursiveProccessLoop( loop, dbgInfo ));
		return true;
	}

	return false;
}

/*
=================================================
	InsertGlobalVariablesAndBuffers
=================================================
*/
ND_ static bool  InsertGlobalVariablesAndBuffers (TIntermAggregate* linkerObjs, TIntermAggregate* globalVars, uint initialPosition, DebugInfo &dbgInfo)
{
	// "bool dbg_EnableRecording"
	TPublicType		type;	type.init( Default );
	type.basicType			= TBasicType::EbtBool;
	type.qualifier.storage	= TStorageQualifier::EvqGlobal;

	TIntermSymbol*			is_enable_recording = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "dbg_EnableRecording", TType{type} };
	dbgInfo.CacheSymbolNode( is_enable_recording );
	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), is_enable_recording );

	// "dbg_EnableRecording = ..."
	TIntermBinary*			init_enable_recording = new TIntermBinary{ TOperator::EOpAssign };
	type.qualifier.storage = TStorageQualifier::EvqTemporary;
	init_enable_recording->setType( TType{type} );
	init_enable_recording->setLeft( is_enable_recording );
	globalVars->getSequence().insert( globalVars->getSequence().begin(), init_enable_recording );

	// "dbg_EnableRecording = <check invocation>"
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

			init_enable_recording->setRight( const_false );
			break;
		}

		case EShLangFragment :
		{
			auto*	op = CreateFragmentShaderIsDebugInvocation( dbgInfo );
			CHECK_ERR( op != null );
			init_enable_recording->setRight( op );
			break;
		}

		case EShLangCompute :
		{
			auto*	op = CreateComputeShaderIsDebugInvocation( dbgInfo );
			CHECK_ERR( op != null );
			init_enable_recording->setRight( op );
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
			init_enable_recording->setRight( op );
			break;
		}

		case EShLangCount :
		default :					RETURN_ERR( "not supported" );
	}
	switch_end


	// "bool dbg_IsEnabled"
	type.qualifier.storage	= TStorageQualifier::EvqGlobal;
	TIntermSymbol*			is_debug_enabled = new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "dbg_IsEnabled", TType{type} };
	dbgInfo.CacheSymbolNode( is_debug_enabled );
	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), is_debug_enabled );

	// "dbg_IsEnabled = false"
	TIntermBinary*			init_debug_enabled	= new TIntermBinary{ TOperator::EOpAssign };
	type.qualifier.storage = TStorageQualifier::EvqTemporary;
	init_debug_enabled->setType( TType{type} );
	init_debug_enabled->setLeft( is_debug_enabled );
	init_debug_enabled->setRight( is_enable_recording );
	globalVars->getSequence().insert( globalVars->getSequence().begin() + 1, init_debug_enabled );


	// "uint dbg_LastPosition"
	type.basicType			= TBasicType::EbtUint;
	type.qualifier.storage	= TStorageQualifier::EvqGlobal;

	TIntermSymbol*			last_pos		= new TIntermSymbol{ dbgInfo.GetUniqueSymbolID(), "dbg_LastPosition", TType{type} };
	dbgInfo.CacheSymbolNode( last_pos );
	linkerObjs->getSequence().insert( linkerObjs->getSequence().begin(), last_pos );

	// "dbg_LastPosition = ~0u"
	type.qualifier.storage = TStorageQualifier::EvqConst;
	TConstUnionArray		init_uvalue(1);	init_uvalue[0].setUConst( initialPosition );
	TIntermConstantUnion*	init_uconst		= new TIntermConstantUnion{ init_uvalue, TType{type} };
	TIntermBinary*			init_pos		= new TIntermBinary{ TOperator::EOpAssign };

	type.qualifier.storage = TStorageQualifier::EvqTemporary;
	init_pos->setType( TType{type} );
	init_pos->setLeft( last_pos );
	init_pos->setRight( init_uconst );
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
	CHECK_ERR( fnDecl->getName() == "dbg_EnableTraceRecording(b1;" );
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

	TIntermSymbol*		is_enable_recording = dbgInfo.GetCachedSymbolNode( "dbg_EnableRecording" );
	TIntermSymbol*		is_debug_enabled	= dbgInfo.GetCachedSymbolNode( "dbg_IsEnabled" );
	TIntermAggregate*	branch_body			= RecordShaderInfo( TSourceLoc{}, dbgInfo );
	CHECK_ERR( is_enable_recording and is_debug_enabled and branch_body );

	TPublicType		type;	type.init( Default );
	type.basicType			= TBasicType::EbtBool;
	type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	// "not dbg_EnableRecording"
	TIntermUnary*		condition	= new TIntermUnary{ TOperator::EOpLogicalNot };
	condition->setType( TType{type} );
	condition->setOperand( is_enable_recording );

	// "if ( not dbg_EnableRecording ) {...}"
	TIntermSelection*	selection	= new TIntermSelection{ arg, branch_body, null };
	selection->setType( TType{EbtVoid} );
	body->getSequence().push_back( selection );

	// "dbg_EnableRecording = arg"
	TIntermBinary*	assign = new TIntermBinary{ TOperator::EOpAssign };
	assign->setType( TType{type} );
	assign->setLeft( is_enable_recording );
	assign->setRight( arg );
	branch_body->getSequence().insert( branch_body->getSequence().begin(), assign );

	// "dbg_IsEnabled = arg"
	assign = new TIntermBinary{ TOperator::EOpAssign };
	assign->setType( TType{type} );
	assign->setLeft( is_debug_enabled );
	assign->setRight( arg );
	branch_body->getSequence().insert( branch_body->getSequence().begin() + 1, assign );

	return true;
}

/*
=================================================
	CreatePauseBody
=================================================
*/
ND_ static bool  CreatePauseBody (TIntermAggregate* fnDecl, DebugInfo &dbgInfo)
{
	CHECK_ERR( fnDecl->getName() == "dbg_PauseTraceRecording(b1;" );
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
	CHECK_ERR( is_debug_enabled != null );

	TPublicType		type;	type.init( Default );
	type.basicType			= TBasicType::EbtBool;
	type.qualifier.storage	= TStorageQualifier::EvqTemporary;

	// "not arg"
	TIntermUnary*		not_arg	= new TIntermUnary{ TOperator::EOpLogicalNot };
	not_arg->setType( TType{type} );
	not_arg->setOperand( arg );

	// "dbg_IsEnabled = not arg"
	TIntermBinary*	assign = new TIntermBinary{ TOperator::EOpAssign };
	assign->setType( TType{type} );
	assign->setLeft( is_debug_enabled );
	assign->setRight( not_arg );
	body->getSequence().push_back( assign );

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
		if ( aggr2->getName().rfind( "dbg_EnableTraceRecording(", 0 ) == 0 )
		{
			CHECK_ERR( CreateEnableIfBody( INOUT aggr2, dbgInfo ));
		}
		else
		if ( aggr2->getName().rfind( "dbg_PauseTraceRecording(", 0 ) == 0 )
		{
			CHECK_ERR( CreatePauseBody( INOUT aggr2, dbgInfo ));
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
			RETURN_ERR( "unknown function" );
	}
	return true;
}

/*
=================================================
	CreateAppendToTrace2
----
	'sourceLoc' - location index returned by DebugInfo::GetSourceLocation or ::GetCustomSourceLocation.
	also see 'CreateAppendToTraceBody2()'
=================================================
*/
ND_ static TIntermAggregate*  CreateAppendToTrace2 (uint sourceLoc, DebugInfo &dbgInfo)
{
	TIntermAggregate*	fcall = new TIntermAggregate( TOperator::EOpFunctionCall );

	fcall->setUserDefined();
	fcall->setName( TString{"dbg_AppendToTrace(u1;"} );
	fcall->setType( TType{ TBasicType::EbtVoid, TStorageQualifier::EvqGlobal });
	fcall->getQualifierList().push_back( TStorageQualifier::EvqConstReadOnly );

	TPublicType		uint_type;		uint_type.init( Default );
	uint_type.basicType				= TBasicType::EbtUint;
	uint_type.qualifier.storage		= TStorageQualifier::EvqConst;

	TConstUnionArray		loc_value(1);	loc_value[0].setUConst( sourceLoc );
	TIntermConstantUnion*	loc_const		= new TIntermConstantUnion{ loc_value, TType{uint_type} };

	loc_const->setLoc( Default );
	fcall->getSequence().push_back( loc_const );

	dbgInfo.RequestFunc( fcall->getName() );

	return fcall;
}

/*
=================================================
	LogTraceRayPayload
=================================================
*/
static bool  LogTraceRayPayload (TIntermAggregate* dstSeq, TIntermAggregate* fn, DebugInfo &dbgInfo)
{
	CHECK_ERR( fn->getSequence().size() == 11 );

	auto*	payload_arg = fn->getSequence()[10]->getAsConstantUnion();
	CHECK_ERR( payload_arg != null );
	CHECK_ERR( payload_arg->getConstArray().size() == 1 );
	CHECK_ERR( payload_arg->getConstArray()[0].getType() == TBasicType::EbtInt );

	const uint	payload_idx = payload_arg->getConstArray()[0].getIConst();

	// find payload variable
	CHECK_ERR( not dbgInfo.GetCallStack().empty() );

	TIntermAggregate*	root		= dbgInfo.GetCallStack().front().node->getAsAggregate();
	TIntermAggregate*	linker_objs	= null;
	CHECK_ERR( root != null );

	for (auto* node : root->getSequence()) {
		if ( auto* aggr = node->getAsAggregate(); aggr != null and aggr->getOp() == TOperator::EOpLinkerObjects ) {
			linker_objs = aggr;
			break;
		}
	}
	CHECK_ERR( linker_objs != null );

	for (auto* node : linker_objs->getSequence())
	{
		if ( auto* symb = node->getAsSymbolNode();
			symb != null													and
			symb->getQualifier().storage == TStorageQualifier::EvqPayload	and
			symb->getQualifier().hasLocation()								and
			symb->getQualifier().layoutLocation == payload_idx )
		{
			const uint	loc_id = dbgInfo.GetCustomSourceLocation( symb, payload_arg->getLoc() );

			if ( auto* fncall = CreateAppendToTrace( symb, loc_id, dbgInfo ))
				dstSeq->getSequence().push_back( fncall );

			return true;
		}
	}
	RETURN_ERR( "can't find payload symbol" );
}

/*
=================================================
	ProcessFunctionCall
----
	log out/inout parameters and returned value
=================================================
*/
static void  ProcessFunctionCall (TIntermOperator* node, DebugInfo &dbgInfo)
{
	bool	is_builtin		= IsBuiltinFunction( node->getOp() );
	bool	is_user_defined	= (node->getOp() == TOperator::EOpFunctionCall);
	bool	is_debug		= IsDebugFunction( node );

	if ( not (is_builtin or is_user_defined) or is_debug )
		return;

	/*bool	has_output = false;
	for (auto& qual : node->getQualifierList()) {
		if ( qual == TStorageQualifier::EvqOut or qual == TStorageQualifier::EvqInOut )
			has_output = true;
	}*/

	// record returned value
	if ( node->getType().getBasicType() == TBasicType::EbtVoid or
		 node->getType().isScalarOrVec1() or node->getType().isVector() or node->getType().isMatrix() )
	{
		// this is location at the end of the function call
		TSourceLoc	loc = node->getLoc();

		// try to find location at the begining of the function call
		if ( dbgInfo.GetCallStack().size() > 1 and
			 node->getType().getBasicType() != TBasicType::EbtVoid )
		{
			TIntermNode*	temp = (dbgInfo.GetCallStack().end()-2)->node;

			if ( temp->getLoc() != TSourceLoc{} and temp->getLoc() < loc )
				loc = temp->getLoc();

			// pattern: "value = FunctionCall(...)"
			// returned value will be recorded later
			if ( TIntermOperator* op = temp->getAsOperator(); op and op->getOp() == TOperator::EOpAssign )
				return;

		#if HIGH_DETAIL_TRACE
			// pattern: "return FunctionCall(...)"
			// returned value will be recorded later
			if ( TIntermBranch* branch = temp->getAsBranchNode(); branch and branch->getFlowOp() == TOperator::EOpReturn )
				return;
		#endif
		}

		const uint	loc_id = dbgInfo.GetSourceLocation( node, loc, true );

		if ( node->getType().getBasicType() == TBasicType::EbtVoid )
		{
			TIntermAggregate*	temp = new TIntermAggregate{ TOperator::EOpSequence };
			temp->getSequence().push_back( node );
			temp->getSequence().push_back( CreateAppendToTrace2( loc_id, dbgInfo ));

			// special case for 'traceRays()' to log payload
			if ( TIntermAggregate* fn = node->getAsAggregate();  fn != null and fn->getOp() == TOperator::EOpTraceKHR )
			{
				LogTraceRayPayload( temp, fn, dbgInfo );
			}

			dbgInfo.InjectNode( temp );
		}
		else
			dbgInfo.InjectNode( CreateAppendToTrace( node, loc_id, dbgInfo ));

	}
}

/*
=================================================
	RecursiveProcessAggregateNode
=================================================
*/
ND_ static bool  RecursiveProcessAggregateNode (TIntermAggregate* aggr, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( aggr );

	for (auto& node : aggr->getSequence())
	{
		CHECK_ERR( RecursiveProcessNode( node, dbgInfo ));

		if ( auto* inj = dbgInfo.GetInjection() )
			node = inj;
	}

	ProcessFunctionCall( aggr, dbgInfo );

	if ( aggr->getOp() == TOperator::EOpFunction and
		 aggr->getSequence().size() == 2 )
	{
		TIntermAggregate*	parameters	= aggr->getSequence()[0]->getAsAggregate();
		TIntermAggregate*	body		= aggr->getSequence()[1]->getAsAggregate();
		CHECK_ERR( parameters and body );

		auto		begin_iter	= body->getSequence().begin();
		TSourceLoc	loc			= aggr->getLoc();
		loc.column = 0;	// TODO

		for (auto& arg : parameters->getSequence())
		{
			TIntermSymbol*		symb	= arg->getAsSymbolNode();
			TStorageQualifier	qual	= symb->getQualifier().storage;

			if ( qual == TStorageQualifier::EvqConstReadOnly or
				 qual == TStorageQualifier::EvqIn			 or
				 qual == TStorageQualifier::EvqInOut )
			{
				if ( auto* fncall = CreateAppendToTrace( symb, dbgInfo.GetSourceLocation( symb, loc ), dbgInfo ))
					begin_iter = body->getSequence().insert( begin_iter, fncall );
			}
		}
	}

	const auto	loc = dbgInfo.GetCurrentLocation();
	dbgInfo.Leave( aggr );

	if ( aggr->getOp() >= TOperator::EOpNegative	 or
		 aggr->getOp() == TOperator::EOpFunctionCall or
		 aggr->getOp() == TOperator::EOpParameters	 )
	{
		// propagate source location to root
		dbgInfo.AddLocation( loc );
	}
	return true;
}

/*
=================================================
	RecursiveProcessUnaryNode
=================================================
*/
ND_ static bool  RecursiveProcessUnaryNode (TIntermUnary* unary, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( unary );

	CHECK_ERR( RecursiveProcessNode( unary->getOperand(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		unary->setOperand( inj );

	switch ( unary->getOp() )
	{
		case TOperator::EOpPreIncrement :
		case TOperator::EOpPostIncrement :	// TODO: log as result+1
		case TOperator::EOpPreDecrement :
		case TOperator::EOpPostDecrement :	// TODO: log as result-1
		{
			dbgInfo.InjectNode( CreateAppendToTrace( unary, dbgInfo.GetSourceLocation( unary->getOperand(), unary->getOperand()->getLoc() ), dbgInfo ));
			break;
		}
		default :
			ProcessFunctionCall( unary, dbgInfo );
			break;
	}

	const auto	loc = dbgInfo.GetCurrentLocation();
	dbgInfo.Leave( unary );

	// propagate source location to root
	dbgInfo.AddLocation( loc );
	return true;
}

/*
=================================================
	RecursiveProcessBinaryNode
=================================================
*/
ND_ static bool  RecursiveProcessBinaryNode (TIntermBinary* binary, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( binary );

	CHECK_ERR( RecursiveProcessNode( binary->getLeft(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		binary->setLeft( inj );

	CHECK_ERR( RecursiveProcessNode( binary->getRight(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		binary->setRight( inj );

	switch ( binary->getOp() )
	{
		case TOperator::EOpAssign :
		case TOperator::EOpAddAssign :
		case TOperator::EOpSubAssign :
		case TOperator::EOpMulAssign :
		case TOperator::EOpDivAssign :
		case TOperator::EOpModAssign :
		case TOperator::EOpAndAssign :
		case TOperator::EOpInclusiveOrAssign :
		case TOperator::EOpExclusiveOrAssign :
		case TOperator::EOpLeftShiftAssign :
		case TOperator::EOpRightShiftAssign :
		{
			dbgInfo.InjectNode( CreateAppendToTrace( binary, dbgInfo.GetSourceLocation( binary->getLeft(), binary->getLeft()->getLoc() ), dbgInfo ));
			break;
		}
		default :
			ProcessFunctionCall( binary, dbgInfo );
			break;
	}

	const auto	loc = dbgInfo.GetCurrentLocation();
	dbgInfo.Leave( binary );

	// propagate source location to root
	dbgInfo.AddLocation( loc );
	return true;
}

/*
=================================================
	RecursiveProcessBranchNode
=================================================
*/
ND_ static bool  RecursiveProcessBranchNode (TIntermBranch* branch, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( branch );

	const TSourceLoc	loc = branch->getLoc();

	CHECK_ERR( RecursiveProcessNode( branch->getExpression(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
	{
		new(branch) TIntermBranch{ branch->getFlowOp(), inj };
		branch->setLoc( loc );
	}

	#if HIGH_DETAIL_TRACE
	else
	if ( branch->getFlowOp() == TOperator::EOpCase or
		 branch->getFlowOp() == TOperator::EOpDefault )
	{}
	else
	if ( auto* expr = branch->getExpression(); expr and not expr->getAsConstantUnion() )
	{
		if ( auto* fncall = CreateAppendToTrace( expr, dbgInfo.GetSourceLocation( expr, branch->getLoc() ), dbgInfo ))
		{
			new(branch) TIntermBranch{ branch->getFlowOp(), fncall };
			branch->setLoc( loc );
		}
	}
	else
	{
		if ( auto* fncall = CreateAppendToTrace2( dbgInfo.GetSourceLocation( branch, branch->getLoc() ), dbgInfo ))
		{
			TIntermAggregate*	temp = new TIntermAggregate{ TOperator::EOpSequence };
			dbgInfo.InjectNode( temp );

			temp->getSequence().push_back( fncall );
			temp->getSequence().push_back( branch );
		}
	}
	#endif

	dbgInfo.Leave( branch );
	return true;
}

/*
=================================================
	ReplaceIntermSwitch
=================================================
*/
static void  ReplaceIntermSwitch (INOUT TIntermSwitch* sw, TIntermTyped* cond, TIntermAggregate* b)
{
	const bool			is_flatten		= sw->getFlatten();
	const bool			dont_flatten	= sw->getDontFlatten();
	const TSourceLoc	loc				= sw->getLoc();

	new(sw) TIntermSwitch{ cond, b };

	sw->setLoc( loc );

	if ( is_flatten )	sw->setFlatten();
	if ( dont_flatten )	sw->setDontFlatten();
}

/*
=================================================
	RecursiveProcessSwitchNode
=================================================
*/
ND_ static bool  RecursiveProcessSwitchNode (TIntermSwitch* sw, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( sw );

	CHECK_ERR( RecursiveProcessNode( sw->getCondition(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermSwitch( INOUT sw, inj, sw->getBody() );

	#if HIGH_DETAIL_TRACE
	else
	if ( not sw->getCondition()->getAsConstantUnion() and sw->getCondition()->getAsTyped() )
	{
		auto*	cond	= sw->getCondition()->getAsTyped();
		auto*	fncall	= CreateAppendToTrace( cond, dbgInfo.GetSourceLocation( cond, cond->getLoc() ), dbgInfo );

		if ( fncall )
			ReplaceIntermSwitch( INOUT sw, fncall, sw->getBody() );
	}
	#endif

	CHECK_ERR( RecursiveProcessNode( sw->getBody(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermSwitch( INOUT sw, sw->getCondition()->getAsTyped(), inj->getAsAggregate() );

	dbgInfo.Leave( sw );
	return true;
}

/*
=================================================
	ReplaceIntermSelection
=================================================
*/
static void  ReplaceIntermSelection (INOUT TIntermSelection *selection, TIntermTyped* cond, TIntermNode* trueB, TIntermNode* falseB)
{
	const bool			is_flatten			= selection->getFlatten();
	const bool			dont_flatten		= selection->getDontFlatten();
	const bool			is_short_circuit	= selection->getShortCircuit();
	const TSourceLoc	loc					= selection->getLoc();
	TType				type;				type.shallowCopy( selection->getType() );

	new(selection) TIntermSelection{ cond, trueB, falseB, type };

	selection->setLoc( loc );

	if ( is_flatten )			selection->setFlatten();
	if ( dont_flatten )			selection->setDontFlatten();
	if ( not is_short_circuit )	selection->setNoShortCircuit();
}

/*
=================================================
	RecursiveProcessSelectionNode
=================================================
*/
ND_ static bool  RecursiveProcessSelectionNode (TIntermSelection* selection, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( selection );

	CHECK_ERR( RecursiveProcessNode( selection->getCondition(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermSelection( INOUT selection, inj, selection->getTrueBlock(), selection->getFalseBlock() );

	#if HIGH_DETAIL_TRACE
	else
	if ( not selection->getCondition()->getAsConstantUnion() )
	{
		auto*	cond	= selection->getCondition();
		auto*	fncall	= CreateAppendToTrace( cond, dbgInfo.GetSourceLocation( cond, cond->getLoc() ), dbgInfo );

		if ( fncall )
			ReplaceIntermSelection( INOUT selection, fncall, selection->getTrueBlock(), selection->getFalseBlock() );
	}
	#endif

	CHECK_ERR( RecursiveProcessNode( selection->getTrueBlock(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermSelection( INOUT selection, selection->getCondition(), inj, selection->getFalseBlock() );

	CHECK_ERR( RecursiveProcessNode( selection->getFalseBlock(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermSelection( INOUT selection, selection->getCondition(), selection->getTrueBlock(), inj );

	dbgInfo.Leave( selection );
	return true;
}

/*
=================================================
	ReplaceIntermLoop
=================================================
*/
static void  ReplaceIntermLoop (INOUT TIntermLoop* loop, TIntermNode* aBody, TIntermTyped* aTest, TIntermTyped* aTerminal)
{
	const bool			test_first	= loop->testFirst();
	const bool			is_unroll	= loop->getUnroll();
	const bool			dont_unroll	= loop->getDontUnroll();
	const int			dependency	= loop->getLoopDependency();
	const TSourceLoc	loc			= loop->getLoc();

	new(loop) TIntermLoop{ aBody, aTest, aTerminal, test_first };

	if ( is_unroll )		loop->setUnroll();
	if ( dont_unroll )		loop->setDontUnroll();

	loop->setLoopDependency( dependency );
	loop->setLoc( loc );
}

/*
=================================================
	RecursiveProccessLoop
=================================================
*/
ND_ static bool  RecursiveProccessLoop (TIntermLoop* loop, DebugInfo &dbgInfo)
{
	dbgInfo.Enter( loop );

	CHECK_ERR( RecursiveProcessNode( loop->getBody(), dbgInfo ));

	if ( auto* inj = dbgInfo.GetInjection() )
		ReplaceIntermLoop( INOUT loop, inj, loop->getTest(), loop->getTerminal() );

	if ( loop->getTerminal() )
	{
		CHECK_ERR( RecursiveProcessNode( loop->getTerminal(), dbgInfo ));

		if ( auto* inj = dbgInfo.GetInjection() )
			ReplaceIntermLoop( INOUT loop, loop->getBody(), loop->getTest(), inj );
	}

	if ( loop->getTest() )
	{
		CHECK_ERR( RecursiveProcessNode( loop->getTest(), dbgInfo ));

		if ( auto* inj = dbgInfo.GetInjection() )
			ReplaceIntermLoop( INOUT loop, loop->getBody(), inj, loop->getTerminal() );

		#if HIGH_DETAIL_TRACE
		else
		{
			auto*	test	= loop->getTest();
			auto*	fncall	= CreateAppendToTrace( test, dbgInfo.GetSourceLocation( test, test->getLoc() ), dbgInfo );

			if ( fncall )
				ReplaceIntermLoop( INOUT loop, loop->getBody(), fncall, loop->getTerminal() );
		}
		#endif
	}

	dbgInfo.Leave( loop );
	return true;
}


} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	InsertTraceRecording
=================================================
*/
	bool  ShaderTrace::InsertTraceRecording (TIntermediate &intermediate, uint descSetIndex)
	{
		CHECK_ERR( intermediate.getNumEntryPoints() == 1 );

		DebugInfo		dbg_info{ intermediate, OUT _exprLocations, _fileMap };

		TIntermNode*	root = intermediate.getTreeRoot();
		CHECK_ERR( root != null );

		_initialPosition = uint(usize( HashOf( this ) + HashVal{ descSetIndex+1 }));
		ASSERT( _initialPosition != 0 );
		_initialPosition |= InitialPositionMask;

		CreateShaderDebugStorage( descSetIndex, dbg_info, OUT _posOffset, OUT _dataOffset );

		dbg_info.Enter( root );
		{
			CHECK_ERR( RecursiveProcessNode( root, dbg_info ));
			CHECK( not dbg_info.GetInjection() );

			CreateShaderBuiltinSymbols( root, dbg_info );

			CHECK_ERR( CreateDebugTraceFunctions( root, _initialPosition, dbg_info ));
		}
		dbg_info.Leave( root );
		dbg_info.PostProcess( OUT _varNames );

		CHECK_ERR( ValidateInterm( intermediate ));
		return true;
	}

} // AE::PipelineCompiler
