// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exceptions only in fatal error:
		- transfer context exceptions (failed to allocate space for command).
		- Array allocation exceptions.
*/

#pragma once

#include "graphics/Public/GraphicsImpl.h"

namespace AE::Graphics
{

	//
	// Shader Debugger
	//

	class ShaderDebugger final
	{
	// types
	public:
		using ParseTraceFn_t = bool (*) (const void* ppln, const void *ptr, Bytes maxSize, OUT Array<String> &result);

		struct Result
		{
			friend class ShaderDebugger;

		// variables
		private:
			DescriptorSetID		_ds;
			ushort				_dsIndex	= UMax;
			EResourceState		_state		= Default;
			BufferID			_device;
			BufferID			_host;
			Bytes				_offset;
			Bytes				_size;
			const void *		_ppln		= null;
			ParseTraceFn_t		_fn			= null;
			
		// methods
		public:
			ND_ BufferID			Buffer ()	C_NE___	{ return _device; }
			ND_ Bytes				Offset ()	C_NE___	{ return _offset; }
			ND_ Bytes				Size ()		C_NE___	{ return _size; }
			ND_ DescriptorSetID		DescSet ()	C_NE___	{ return _ds; }
			ND_ DescSetBinding		DSIndex ()	C_NE___	{ return DescSetBinding{ _dsIndex }; }

			ND_ bool				IsValid ()	C_NE___;
		};

	private:
		struct Buffer
		{
			Strong<BufferID>	dbgTraceBuffer;
			Strong<BufferID>	readbackBuffer;
			Bytes				capacity;
			Bytes				size;
		};

		static constexpr Bytes	_SingleBufferSize	{1_Mb};
		static constexpr Bytes	_OffsetAlign		{16_b};
		static constexpr Bytes	_TraceHeaderSize	{16_b};

		static constexpr DescriptorSetName	_DbgShaderTrace {"ShaderTrace"};

		using DSArray_t = Array< Strong<DescriptorSetID> >;


	// variables
	private:
		Array< Buffer >		_buffers;
		Array< Result >		_pending;
		DSArray_t			_dsArray;

		GfxMemAllocatorPtr	_gfxAlloc;

		const Bytes			_blockSize;
		
		DRC_ONLY(
			RWDataRaceCheck	_drCheck;
		)


	// methods
	public:
		explicit ShaderDebugger (Bytes blockSize = 8_Mb) : _blockSize{blockSize} {}
		~ShaderDebugger ();

		template <typename TransferCtx>
		ND_ bool  AllocForCompute (OUT Result &result, TransferCtx &ctx, ComputePipelineID ppln, const uint3 &globalID,
								   const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;

		template <typename TransferCtx>
		ND_ bool  AllocForCompute (OUT Result &result, TransferCtx &ctx, ComputePipelineID ppln,
								   const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;
		
		template <typename TransferCtx>
		ND_ bool  AllocForRayTracing (OUT Result &result, TransferCtx &ctx, RayTracingPipelineID ppln, const uint3 &launchID,
									  const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)	__Th___;
		
		template <typename TransferCtx>
		ND_ bool  AllocForRayTracing (OUT Result &result, TransferCtx &ctx, RayTracingPipelineID ppln,
									  const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)	__Th___;

		template <typename TransferCtx, typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, TransferCtx &ctx, PplnID ppln, const uint2 &fragCoord,
									const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;
		
		template <typename TransferCtx, typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, TransferCtx &ctx, PplnID ppln,
									const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;


		template <typename TransferCtx>
		ND_ Promise<Array<String>>  Read (TransferCtx &ctx, const Result &result)											__Th___;
		
		template <typename TransferCtx>
		ND_ Promise<Array<String>>  ReadAll (TransferCtx &ctx)																__Th___;

			void  Reset ();


	private:
		ND_ bool  _GetComputePipeline (ComputePipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetRayTracingPipeline (RayTracingPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (GraphicsPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (MeshPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (TilePipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);

		template <typename PplnID>
		ND_ bool  _GetPipeline (PplnID ppln, const DescriptorSetName &dsName, OUT Result &result);
		
		template <typename TransferCtx>
			void  _FillBuffer (const Result &result, TransferCtx &ctx, Bytes headerSize, const void* headerData) const;

		ND_ bool  _AllocStorage (Bytes size, INOUT Result &result);
		ND_ bool  _InitDS (const Result &info) const;

		ND_ static Array<String>  _Parse (ArrayView<ubyte> view, const void* ppln, ParseTraceFn_t fn);

		ND_ static Promise<Array<String>>  _Merge (Array<Promise<Array<String>>> tasks);
	};
	
	
/*
=================================================
	AllocForCompute
=================================================
*/
	template <typename TransferCtx>
	bool  ShaderDebugger::AllocForCompute (OUT Result &result, TransferCtx &ctx, ComputePipelineID ppln, const uint3 &globalID, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsBaseOf< ITransferContext, TransferCtx >);

		if_unlikely( not _GetComputePipeline( ppln, dsName, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;
		
		const uint	data[4] = { globalID.x, globalID.y, globalID.z, 0 };
		STATIC_ASSERT( _TraceHeaderSize == sizeof(data) );
		
		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}
	
	template <typename TransferCtx>
	bool  ShaderDebugger::AllocForCompute (OUT Result &result, TransferCtx &ctx, ComputePipelineID ppln, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		return AllocForCompute( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
	}

/*
=================================================
	AllocForRayTracing
=================================================
*/
	template <typename TransferCtx>
	bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, TransferCtx &ctx, RayTracingPipelineID ppln, const uint3 &launchID, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsBaseOf< ITransferContext, TransferCtx >);

		if_unlikely( not _GetRayTracingPipeline( ppln, dsName, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;
		
		const uint	data[4] = { launchID.x, launchID.y, launchID.z, 0 };
		STATIC_ASSERT( _TraceHeaderSize == sizeof(data) );
		
		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}
	
	template <typename TransferCtx>
	bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, TransferCtx &ctx, RayTracingPipelineID ppln, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		return AllocForRayTracing( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
	}

/*
=================================================
	AllocForGraphics
=================================================
*/
	template <typename TransferCtx, typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, TransferCtx &ctx, PplnID ppln, const uint2 &fragCoord, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsBaseOf< ITransferContext, TransferCtx >);

		if_unlikely( not _GetGraphicsPipeline( ppln, dsName, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		const uint	data[4] = { fragCoord.x, fragCoord.y, 0, 0 };
		STATIC_ASSERT( _TraceHeaderSize == sizeof(data) );

		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}
	
	template <typename TransferCtx, typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, TransferCtx &ctx, PplnID ppln, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		return AllocForGraphics( OUT result, ctx, ppln, uint2{~0u}, dsName, size );
	}
	
/*
=================================================
	_FillBuffer
=================================================
*/
	template <typename TransferCtx>
	void  ShaderDebugger::_FillBuffer (const Result &result, TransferCtx &ctx, Bytes headerSize, const void* headerData) const
	{
		ctx.FillBuffer( result._device, result._offset + headerSize, result._size - headerSize, 0 );
		ctx.UpdateBuffer( result._device, result._offset, headerSize, headerData );

		ctx.BufferBarrier( result._device, EResourceState::CopySrc, result._state );
		ctx.CommitBarriers();
	}

/*
=================================================
	Read
=================================================
*/
	template <typename TransferCtx>
	Promise<Array<String>>  ShaderDebugger::Read (TransferCtx &ctx, const Result &result) __Th___
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsBaseOf< ITransferContext, TransferCtx >);

		BufferCopy	range;
		range.srcOffset	= result._offset;
		range.dstOffset	= result._offset;
		range.size		= result._size;

		ctx.BufferBarrier( result._device, result._state, EResourceState::CopySrc );
		ctx.CommitBarriers();

		ctx.CopyBuffer( result._device, result._host, {range} );
		
		ctx.BufferBarrier( result._host, EResourceState::CopyDst, EResourceState::Host_Read );
		ctx.CommitBarriers();

		return ctx.ReadHostBuffer( result._host, result._offset, result._size )
				.Then( [ppln = result._ppln, fn = result._fn] (const ArrayView<ubyte> &view)
						{
							return _Parse( view, ppln, fn );
						});
	}
	
/*
=================================================
	ReadAll
=================================================
*/
	template <typename TransferCtx>
	Promise<Array<String>>  ShaderDebugger::ReadAll (TransferCtx &ctx) __Th___
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsBaseOf< ITransferContext, TransferCtx >);

		if ( _pending.empty() )
			return Default;

		for (auto& res : _pending) {
			ctx.BufferBarrier( res._device, res._state, EResourceState::CopySrc );
		}
		ctx.CommitBarriers();

		for (auto& res : _pending)
		{
			BufferCopy	range;
			range.srcOffset	= res._offset;
			range.dstOffset	= res._offset;
			range.size		= res._size;

			ctx.CopyBuffer( res._device, res._host, {range} );
		}

		ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
		ctx.CommitBarriers();

		Array< Promise< Array<String> >>	temp;
		temp.reserve( 32 );
		
		for (auto& res : _pending)
		{
			temp.push_back(
				ctx.ReadHostBuffer( res._host, res._offset, res._size )
					.Then( [ppln = res._ppln, fn = res._fn] (const ArrayView<ubyte> &view)
							{
								return _Parse( view, ppln, fn );
							}));

			if_unlikely( temp.size() >= 32 )
			{
				auto	task = _Merge( RVRef(temp) );
				temp.clear();
				temp.reserve( 32 );
				temp.push_back( task );
			}
		}
		_pending.clear();
		
		if ( temp.size() == 1 )
			return temp[0];

		return _Merge( RVRef(temp) );
	}


} // AE::Graphics
