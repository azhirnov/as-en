// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::ResEditor
{

/*
=================================================
	LocalSize*
=================================================
*/
	void  COMPUTE_PASS::LocalSize3v (const packed_uint3 &v) __Th___
	{
		_localSize = v;

		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( _iterations.empty(), "LocalSize() must be used before Dispatch() call" );
	}

/*
=================================================
	DispatchGroups*
=================================================
*/
	void  COMPUTE_PASS::DispatchGroups3v (const packed_uint3 &groupCount) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( All( groupCount > 0u ), "'groupCount' must be > 0" );

		auto&	it = _iterations.emplace_back();
		it.count	= groupCount;
		it.isGroups	= true;
	}

	void  COMPUTE_PASS::DispatchGroupsDS  (const ScriptDynamicDimPtr &ds) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( ds and ds->Get() );

		auto&	it = _iterations.emplace_back();
		it.count	= ds->Get();
		it.isGroups	= true;

		ScriptDynamicDimPtr	ds2;
		ds2.Attach( ds->Mul3( packed_int3{_localSize} ));
		_SetDynamicDimension( ds2 );
	}

	void  COMPUTE_PASS::DispatchGroups1D (const ScriptDynamicUIntPtr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= true;
	}

	void  COMPUTE_PASS::DispatchGroups2D (const ScriptDynamicUInt2Ptr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= true;
	}

	void  COMPUTE_PASS::DispatchGroups3D (const ScriptDynamicUInt3Ptr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= true;
	}

/*
=================================================
	DispatchThreads*
=================================================
*/
	void  COMPUTE_PASS::DispatchThreads3v (const packed_uint3 &threads) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( All( uint3{threads} >= _localSize ), "'threads' must be >= LocalSize()" );

		auto&	it = _iterations.emplace_back();
		it.count	= threads;
		it.isGroups	= false;
	}

	void  COMPUTE_PASS::DispatchThreadsDS (const ScriptDynamicDimPtr &ds) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( ds and ds->Get() );

		auto&	it = _iterations.emplace_back();
		it.count	= ds->Get();
		it.isGroups	= false;

		_SetDynamicDimension( ds );
	}

	void  COMPUTE_PASS::DispatchThreads1D (const ScriptDynamicUIntPtr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= false;
	}

	void  COMPUTE_PASS::DispatchThreads2D (const ScriptDynamicUInt2Ptr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= false;
	}

	void  COMPUTE_PASS::DispatchThreads3D (const ScriptDynamicUInt3Ptr &dyn) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( dyn );

		auto&	it = _iterations.emplace_back();
		it.count	= dyn->Get();
		it.isGroups	= false;
	}

/*
=================================================
	DispatchGroupsIndirect*
=================================================
*/
	void  COMPUTE_PASS::DispatchGroupsIndirect1 (const ScriptBufferPtr &ibuf) __Th___
	{
		DispatchGroupsIndirect2( ibuf, 0 );
	}

	void  COMPUTE_PASS::DispatchGroupsIndirect2 (const ScriptBufferPtr &ibuf, ulong offset) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( ibuf );

		ibuf->AddUsage( EResourceUsage::IndirectBuffer );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectOffset	= Bytes{offset};
		it.isGroups			= true;
	}

	void  COMPUTE_PASS::DispatchGroupsIndirect3 (const ScriptBufferPtr &ibuf, const String &field) __Th___
	{
		CHECK_THROW_MSG( All( _localSize > 0u ), "LocalSize() must be > 0" );
		CHECK_THROW_MSG( ibuf );
		CHECK_THROW_MSG( not field.empty() );

		ibuf->AddUsage( EResourceUsage::IndirectBuffer );

		auto&	it			= _iterations.emplace_back();
		it.indirect			= ibuf;
		it.indirectCmdField	= field;
		it.isGroups			= true;
	}

/*
=================================================
	_OnAddArg
=================================================
*/
	void  COMPUTE_PASS::_OnAddArg (INOUT ScriptPassArgs::Argument &arg) __Th___
	{
		CHECK_THROW_MSG( _iterations.empty(), "Arg() must be used before Dispatch() call" );

		arg.state |= EResourceState::ComputeShader;
	}

/*
=================================================
	MinSubgroupSize
=================================================
*/
	void  COMPUTE_PASS::MinSubgroupSize (uint value) __Th___
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();

		CHECK_THROW_MSG( value <= fs.maxSubgroupSize,
			"Required subgroup size "s << ToString(value) << " must be <= than maxSubgroupSize " << ToString(fs.maxSubgroupSize) );

		if ( fs.minSubgroupSize == fs.maxSubgroupSize )
			return;

		_subgroupSize = uint{fs.minSubgroupSize};

		for (; _subgroupSize < value;) {
			_subgroupSize <<= 1;
		}
	}

} // AE::ResEditor
