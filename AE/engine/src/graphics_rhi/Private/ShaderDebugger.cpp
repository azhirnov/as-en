// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_rhi/Public/ShaderDebugger.h"
#include "graphics_rhi/GraphicsImpl.h"

namespace AE::Graphics
{

/*
=================================================
	IsValid
=================================================
*/
	bool  ShaderDebugger::Result::IsValid () C_NE___
	{
		return	_ds			!= Default	and
				_deviceBuf	!= Default	and
				_hostBuf	!= Default	and
				_ppln		!= null		and
				_fn			!= null;
	}

/*
=================================================
	destructor
=================================================
*/
	ShaderDebugger::~ShaderDebugger () __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		{
			auto	buffers = _buffers.WriteLock();

			for (auto& buf : *buffers) {
				res_mngr.ImmediatelyReleaseResources( buf.dbgTraceBuffer, buf.readbackBuffer );
			}
			buffers->clear();
		}
		{
			auto	ds_arr = _dsArray.WriteLock();

			for (auto& ds : *ds_arr) {
				DEV_CHECK( res_mngr.ImmediatelyRelease2( INOUT ds ));
			}
			ds_arr->clear();
		}
	}

/*
=================================================
	_InitDS
=================================================
*/
	bool  ShaderDebugger::_InitDS (const Result &info) const
	{
		DescriptorUpdater	updater;
		CHECK_ERR( updater.Set( info._ds, EDescUpdateMode::Partialy ));

		updater.BindBuffer( UniformName{"dbg_ShaderTrace"}, info._deviceBuf, info._offset, info._size );
		CHECK_ERR( updater.Flush() );

		return true;
	}

/*
=================================================
	_AllocStorage
=================================================
*/
	bool  ShaderDebugger::_AllocStorage (const Bytes storageSize, INOUT Result &result)
	{
		NonNull( result._ppln );
		NonNull( result._fn );
		ASSERT( result._ds and result._dsIndex != UMax );
		ASSERT( result._state != Default );

		CHECK_ERR( storageSize > _TraceHeaderSize );

		auto&		rts			= GraphicsScheduler();
		const auto	frame_id	= rts.GetFrameId();

		// find in existing
		bool	found = false;
		{
			auto	buffers = _buffers.WriteLock();

			for (auto& buf : *buffers)
			{
				if ( frame_id.Diff( buf.lastUsage ) > slong(frame_id.MaxFrames()+2) )
				{
					buf.size = 0_b;
				}

				const Bytes  offset = AlignUp( buf.size, _OffsetAlign );

				if ( offset + storageSize <= buf.capacity )
				{
					result._deviceBuf	= buf.dbgTraceBuffer;
					result._hostBuf		= buf.readbackBuffer;
					result._offset		= offset;
					result._size		= storageSize;

					buf.size			= offset + storageSize;
					buf.lastUsage		= frame_id;

					found = true;
					break;
				}
			}
		}
		if ( found )
		{
			_pending->push_back( result );
			return _InitDS( result );
		}

		auto&	res_mngr = rts.GetResourceManager();

		// create allocator
		GfxMemAllocatorPtr	gfx_alloc = _gfxAlloc.load();
		if_unlikely( gfx_alloc == null )
		{
			GfxMemAllocatorPtr	new_alloc = res_mngr.CreateLinearGfxMemAllocator( _blockSize * 4 );
			if ( _gfxAlloc.CAS_Loop( INOUT gfx_alloc, new_alloc ))
				gfx_alloc = new_alloc;
		}

		// create new buffer
		{
			Buffer		buf;
			EMemoryType	host_mem_type = EMemoryType::HostCachedCoherent;
			if ( not res_mngr.IsSupported( host_mem_type ))
			{
				host_mem_type = EMemoryType::HostCached;
				CHECK_ERR( res_mngr.IsSupported( host_mem_type ));
			}

			auto&		fs	= res_mngr.GetFeatureSet();
			EBufferOpt	opt	= Default;

			if ( fs.vertexPipelineStoresAndAtomics == FeatureSet::EFeature::RequireTrue )
				opt |= EBufferOpt::VertexPplnStore;

			if ( fs.fragmentStoresAndAtomics == FeatureSet::EFeature::RequireTrue )
				opt |= EBufferOpt::FragmentPplnStore;

			buf.dbgTraceBuffer = res_mngr.CreateBuffer( BufferDesc{	_blockSize,
															EBufferUsage::Transfer | EBufferUsage::Storage,
															opt,
															Default,
															EMemoryType::DeviceLocal }, "debug storage buffer", gfx_alloc );
			buf.readbackBuffer = res_mngr.CreateBuffer( BufferDesc{	_blockSize,
															EBufferUsage::TransferDst,
															Default,
															Default,
															host_mem_type }, "readback dbg buffer", gfx_alloc );
			CHECK_ERR( buf.dbgTraceBuffer and buf.readbackBuffer );

			result._deviceBuf	= buf.dbgTraceBuffer;
			result._hostBuf		= buf.readbackBuffer;
			result._offset		= 0_b;
			result._size		= storageSize;

			buf.capacity		= _blockSize;
			buf.size			= storageSize;
			buf.lastUsage		= frame_id;

			auto	pending	= _pending.WriteNoLock();
			auto	buffers	= _buffers.WriteNoLock();
			EXLOCK( pending, buffers );

			pending->push_back( result );
			buffers->push_back( RVRef(buf) );
		}
		return _InitDS( result );
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
namespace {
	template <typename PplnType>
	static bool  ParseShaderTrace (const void* ppln, const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) __NE___
	{
	#if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
		return Cast<PplnType>( ppln )->ParseShaderTrace( ptr, maxSize, format, OUT result );
	#else
		Unused( ppln, ptr, maxSize, format, result );
		return false;
	#endif
	}
}
/*
=================================================
	_Get***Pipeline
=================================================
*/
	bool  ShaderDebugger::_GetComputePipeline (ComputePipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		result = Default;
		result._state = EResourceState::ShaderStorage_RW | EResourceState::ComputeShader;

		return _GetPipeline( ppln, dsName, OUT result );
	}

	bool  ShaderDebugger::_GetRayTracingPipeline (RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		result = Default;
		result._state = EResourceState::ShaderStorage_RW | EResourceState::RayTracingShaders;

		return _GetPipeline( ppln, dsName, OUT result );
	}

	bool  ShaderDebugger::_GetGraphicsPipeline (GraphicsPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		result = Default;
		result._state = EResourceState::ShaderStorage_RW | EResourceState::AllGraphicsShaders;

		return _GetPipeline( ppln, dsName, OUT result );
	}

	bool  ShaderDebugger::_GetGraphicsPipeline (MeshPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		result = Default;
		result._state = EResourceState::ShaderStorage_RW | EResourceState::AllGraphicsShaders;

		return _GetPipeline( ppln, dsName, OUT result );
	}

	bool  ShaderDebugger::_GetGraphicsPipeline (TilePipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		result = Default;
		result._state = EResourceState::ShaderStorage_RW | EResourceState::TileShader;

		return _GetPipeline( ppln, dsName, OUT result );
	}

	template <typename PplnID>
	bool  ShaderDebugger::_GetPipeline (PplnID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		if ( auto* res = res_mngr.GetResource( ppln ))
		{
			result._ppln	= res;
			result._fn		= &ParseShaderTrace< RemoveAllQualifiers< decltype(*res) >>;

			auto [ds, idx]	 = res_mngr.CreateDescriptorSet( ppln, dsName );
			CHECK_ERR( ds );

			result._ds		= ds;
			result._dsIndex	= CheckCast{ idx.vkIndex };

			_dsArray->push_back( RVRef(ds) );  // throw
			return true;
		}
		return false;
	}

/*
=================================================
	_Parse
=================================================
*/
	InlinePromise<Array<String>>  ShaderDebugger::_Parse (Promise<ArrayView<ubyte>> readback, CoSafe<const void*> ppln, ParseTraceFn_t fn, ELogFormat format)
	{
		using namespace AE::Threading;

		ArrayView<ubyte>	view = co_await readback;
		Array<String>		result;

		Unused( fn( ppln, view.data(), ArraySizeOf(view), format, OUT result ));
		co_return result;
	}

/*
=================================================
	_Merge
=================================================
*/
	Promise<Array<String>>  ShaderDebugger::_Merge (Array< Promise< Array<String> >> tasks)
	{
		return	[] (Array< Promise< Array<String> >> promises) -> InlinePromise< Array<String> >
				{
					auto res = co_await promises;
					Array<String> result;
					for (auto& arr : res)
					{
						for (auto& s : arr)
							result.push_back( RVRef(s) );
					}
					co_return RVRef(result);
				}
				( RVRef(tasks) );
	}

/*
=================================================
	Reset
=================================================
*/
	void  ShaderDebugger::Reset ()
	{
		auto	buffers		= _buffers.WriteNoLock();
		auto	ds_arr		= _dsArray.WriteNoLock();
		auto	pending		= _pending.WriteNoLock();

		EXLOCK( buffers, ds_arr, pending );

		ASSERT( pending->empty() );
		pending->clear();

		for (auto& buf : *buffers)
		{
			buf.size		= 0_b;
			buf.lastUsage	= Default;
		}

		if ( not ds_arr->empty() )
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();

			for (auto& ds : *ds_arr) {
				res_mngr.DelayedRelease( ds );
			}
			ds_arr->clear();
		}
	}

/*
=================================================
	AllocForRayTracing
=================================================
*/
	bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, const uint3 &launchID, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( not _GetRayTracingPipeline( ppln, dsName, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		const uint	data[4] = { launchID.x, launchID.y, launchID.z, 0 };
		StaticAssert( _TraceHeaderSize == sizeof(data) );

		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}

	bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		return AllocForRayTracing( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
	}

/*
=================================================
	_FillBuffer
=================================================
*/
	void  ShaderDebugger::_FillBuffer (const Result &result, ITransferContext &ctx, Bytes headerSize, const void* headerData) const
	{
		ctx.FillBuffer( result._deviceBuf, result._offset + headerSize, result._size - headerSize, 0 );
		ctx.UpdateBuffer( result._deviceBuf, result._offset, headerSize, headerData );

		ctx.BufferBarrier( result._deviceBuf, EResourceState::CopyDst,  result._state );
		ctx.BufferBarrier( result._deviceBuf, EResourceState::ClearDst, result._state );
		ctx.CommitBarriers();
	}

/*
=================================================
	Read
=================================================
*/
	Promise<Array<String>>  ShaderDebugger::Read (ITransferContext &ctx, const Result &request, ELogFormat format) __Th___
	{
		DRC_EXLOCK( _drCheck );

		BufferCopy	range;
		range.srcOffset	= request._offset;
		range.dstOffset	= request._offset;
		range.size		= request._size;

		ctx.BufferBarrier( request._deviceBuf, request._state, EResourceState::CopySrc );
		ctx.CommitBarriers();

		ctx.CopyBuffer( request._deviceBuf, request._hostBuf, {range} );

		ctx.BufferBarrier( request._hostBuf, EResourceState::CopyDst, EResourceState::Host_Read );
		ctx.CommitBarriers();

		return ctx.ReadHostBuffer( request._hostBuf, request._offset, request._size )
					.Then( CoSafe<const void*>{request._ppln}, request._fn, format, _Parse );
	}

/*
=================================================
	ReadAll
=================================================
*/
	Promise<Array<String>>  ShaderDebugger::ReadAll (ITransferContext &ctx, ELogFormat format) __Th___
	{
		DRC_EXLOCK( _drCheck );

		auto	pending = _pending.WriteLock();

		if ( pending->empty() )
			return Default;

		for (auto& res : *pending) {
			ctx.BufferBarrier( res._deviceBuf, res._state, EResourceState::CopySrc );
		}
		ctx.CommitBarriers();

		for (auto& res : *pending)
		{
			BufferCopy	range;
			range.srcOffset	= res._offset;
			range.dstOffset	= res._offset;
			range.size		= res._size;

			ctx.CopyBuffer( res._deviceBuf, res._hostBuf, {range} );
		}

		ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
		ctx.CommitBarriers();

		Array< Promise< Array<String> >>	temp;
		temp.reserve( 32 );

		for (auto& res : *pending)
		{
			temp.push_back( ctx.ReadHostBuffer( res._hostBuf, res._offset, res._size )
								.Then( CoSafe<const void*>{res._ppln}, res._fn, format, _Parse ));

			if_unlikely( temp.size() >= 32 )
			{
				auto	task = _Merge( RVRef(temp) );
				temp.clear();
				temp.reserve( 32 );
				temp.push_back( task );
			}
		}
		pending->clear();

		Promise<Array<String>>	result;
		if ( temp.size() == 1 )
			result = RVRef( temp[0] );
		else
			result = _Merge( RVRef(temp) );

		GraphicsScheduler().AddNextCycleEndDeps( AsyncTask{result} );	// TODO: is it needed?
		return result;
	}


} // AE::Graphics
