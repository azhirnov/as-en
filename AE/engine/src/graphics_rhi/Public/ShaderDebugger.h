// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe:  yes

	Exceptions only in fatal error:
		- transfer context exceptions (failed to allocate space for command).
		- Array allocation exceptions.
*/

#pragma once

#include "graphics_rhi/Public/CommandBuffer.h"

namespace AE::Graphics
{
	using AE::Threading::RWSpinLock;
	using AE::Threading::Synchronized;


	//
	// Shader Debugger
	//

	class ShaderDebugger final
	{
	// types
	public:
		enum class ELogFormat : uint
		{
			Unknown,
			Text,			// as plane text with part of source code
			VS_Console,		// compatible with VS output, allow navigation to code by click, format: 'file (line): ...'
			FileURL,		// click to file path will open shader source file, format: 'file:///file (line)'
			VSCode,			// click to file path will open shader source file in specified line, format: 'file:///file#line'
			_Count
		};

		using ParseTraceFn_t = bool (*) (const void* ppln, const void* ptr, Bytes maxSize, ELogFormat, OUT Array<String> &result) __NE___;

		struct Result
		{
			friend class ShaderDebugger;

		// variables
		private:
			DescriptorSetID		_ds;
			ushort				_dsIndex	= UMax;
			EResourceState		_state		= Default;
			BufferID			_deviceBuf;
			BufferID			_hostBuf;
			Bytes				_offset;
			Bytes				_size;
			const void *		_ppln		= null;
			ParseTraceFn_t		_fn			= null;

		// methods
		public:
			ND_ BufferID			Buffer ()	C_NE___	{ return _deviceBuf; }
			ND_ Bytes				Offset ()	C_NE___	{ return _offset; }
			ND_ Bytes				Size ()		C_NE___	{ return _size; }
			ND_ DescriptorSetID		DescSet ()	C_NE___	{ return _ds; }
			ND_ DescSetBinding		DSIndex ()	C_NE___	{ return DescSetBinding{ _dsIndex }; }

			ND_ bool				IsValid ()	C_NE___;
			ND_ explicit operator	bool ()		C_NE___	{ return IsValid(); }
		};

	private:
		struct Buffer
		{
			Strong<BufferID>	dbgTraceBuffer;
			Strong<BufferID>	readbackBuffer;
			Bytes				capacity;
			Bytes				size;
			FrameUID			lastUsage;
		};

		enum class EShaderTraceFormat
		{
			Trace,
			Asserts,
		};

		static constexpr Bytes	_SingleBufferSize	{4_MiB};
		static constexpr Bytes	_AssertsBufferSize	{1_KiB};
		static constexpr Bytes	_AllocBlockSize		{16_MiB};
		static constexpr Bytes	_OffsetAlign		{16_b};
		static constexpr Bytes	_TraceHeaderSize	{16_b};

		static constexpr DescriptorSetName	_DbgShaderTrace {"ShaderTrace"};

		using DSArray_t = Array< Strong<DescriptorSetID> >;


	// variables
	private:
		Synchronized< RWSpinLock, Array< Buffer >>	_buffers;
		Synchronized< RWSpinLock, Array< Result >>	_pending;
		Synchronized< RWSpinLock, DSArray_t >		_dsArray;

		AtomicRC< IGfxMemAllocator >				_gfxAlloc;

		const Bytes									_blockSize;


	// methods
	public:
		explicit ShaderDebugger (Bytes blockSize = _AllocBlockSize)																__NE___ : _blockSize{blockSize} {}
		~ShaderDebugger ()																										__NE___;

		template <typename PplnID>
		ND_ bool  AllocForCompute (OUT Result &result, ITransferContext &ctx, PplnID ppln, const uint3 &globalID,
								   DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)				__Th___;

		template <typename PplnID>
		ND_ bool  AllocForCompute (OUT Result &result, ITransferContext &ctx, PplnID ppln,
								   DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)				__Th___;

		ND_ bool  AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, const uint3 &launchID,
									  DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;

		ND_ bool  AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln,
									  DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;

		template <typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln,
									const uint2 &fragCoord_or_vertexIdInstanceId,
									DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;

		template <typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln,
									DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;

		template <typename PplnID>
		ND_ bool  AllocForAsserts (OUT Result &result, ITransferContext &ctx, PplnID ppln,
									DescriptorSetName::Ref dsName = _DbgShaderTrace, Bytes size = _AssertsBufferSize)			__Th___;


		ND_ Promise<Array<String>>  Read (ITransferContext &ctx, const Result &request, ELogFormat format = Default)			__Th___;
		ND_ Promise<Array<String>>  ReadAll (ITransferContext &ctx, ELogFormat format = Default)								__Th___;

		ND_ bool  HasPendingRequests ()																							__NE___	{ return not _pending->empty(); }

			void  Reset ()																										__Th___;


	private:
		ND_ bool  _GetComputePipeline (ComputePipelineID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);
		ND_ bool  _GetRayTracingPipeline (RayTracingPipelineID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (GraphicsPipelineID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (MeshPipelineID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (TilePipelineID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);

		template <typename PplnID>
		ND_ bool  _GetPipeline (PplnID, DescriptorSetName::Ref, EShaderTraceFormat, OUT Result &result);

			void  _FillBuffer (const Result &result, ITransferContext &ctx, Bytes headerSize, const void* headerData) const;

		ND_ bool  _AllocStorage (Bytes size, INOUT Result &result);
		ND_ bool  _InitDS (const Result &info) const;

		ND_ static InlinePromise<Array<String>>  _Parse (Promise<ArrayView<ubyte>>, CoSafe<const void*> ppln, ParseTraceFn_t fn, ELogFormat format);

		ND_ static Promise<Array<String>>  _Merge (Array<Promise<Array<String>>> tasks);
	};



/*
=================================================
	AllocForCompute
=================================================
*/
	template <typename PplnID>
	bool  ShaderDebugger::AllocForCompute (OUT Result &result, ITransferContext &ctx, PplnID ppln, const uint3 &globalID, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		if constexpr( IsSame< PplnID, ComputePipelineID >)
		{
			if_unlikely( not _GetComputePipeline( ppln, dsName, EShaderTraceFormat::Trace, OUT result ))
				return false;
		}
		else
		if constexpr( IsSame< PplnID, MeshPipelineID >)
		{
			if_unlikely( not _GetGraphicsPipeline( ppln, dsName, EShaderTraceFormat::Trace, OUT result ))
				return false;
		}
		else
		{
			return false;
		}

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		const uint	data[4] = { globalID.x, globalID.y, globalID.z, 0 };
		StaticAssert( _TraceHeaderSize == sizeof(data) );

		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}

	template <typename PplnID>
	bool  ShaderDebugger::AllocForCompute (OUT Result &result, ITransferContext &ctx, PplnID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		return AllocForCompute( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
	}

/*
=================================================
	AllocForGraphics
=================================================
*/
	template <typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln, const uint2 &fragCoord_or_vertexIdInstanceId, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		if_unlikely( not _GetGraphicsPipeline( ppln, dsName, EShaderTraceFormat::Trace, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		const uint	data[4] = { fragCoord_or_vertexIdInstanceId.x, fragCoord_or_vertexIdInstanceId.y, 0, 0 };
		StaticAssert( _TraceHeaderSize == sizeof(data) );

		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}

	template <typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		return AllocForGraphics( OUT result, ctx, ppln, uint2{~0u}, dsName, size );
	}

/*
=================================================
	AllocForAsserts
=================================================
*/
	template <typename PplnID>
	bool  ShaderDebugger::AllocForAsserts (OUT Result &result, ITransferContext &ctx, PplnID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
	{
		if constexpr( IsSame< PplnID, ComputePipelineID >)
		{
			if_unlikely( not _GetComputePipeline( ppln, dsName, EShaderTraceFormat::Asserts, OUT result ))
				return false;
		}
		else
		if constexpr( IsSame< PplnID, GraphicsPipelineID > or IsSame< PplnID, MeshPipelineID >)
		{
			if_unlikely( not _GetGraphicsPipeline( ppln, dsName, EShaderTraceFormat::Asserts, OUT result ))
				return false;
		}
		else
		if constexpr( IsSame< PplnID, RayTracingPipelineID >)
		{
			if_unlikely( not _GetRayTracingPipeline( ppln, dsName, EShaderTraceFormat::Asserts, OUT result ))
				return false;
		}
		else
		{
			return false;
		}

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		ctx.FillBuffer( result._deviceBuf, result._offset, result._size, 0 );
		ctx.ResourceBarrier( result._deviceBuf, EResourceState::ClearDst, result._state );
		ctx.CommitBarriers();

		return true;
	}

} // AE::Graphics
