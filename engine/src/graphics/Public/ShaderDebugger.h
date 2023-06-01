// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exceptions only in fatal error:
		- transfer context exceptions (failed to allocate space for command).
		- Array allocation exceptions.
*/

#pragma once

#include "threading/Primitives/SpinLock.h"
#include "threading/Primitives/Synchronized.h"

#include "graphics/Public/CommandBuffer.h"

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
			VS_Console,		// compatible with VS outpit, allow navigation to code by click
			VS,				// click to file path will open shader source file
			VSCode,			// click to file path will open shader source file in specified line
			_Count
		};

		using ParseTraceFn_t = bool (*) (const void* ppln, const void *ptr, Bytes maxSize, ELogFormat, OUT Array<String> &result);

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
		};

		static constexpr Bytes	_SingleBufferSize	{1_Mb};
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
		
		DRC_ONLY( RWDataRaceCheck					_drCheck;)


	// methods
	public:
		explicit ShaderDebugger (Bytes blockSize = 8_Mb) : _blockSize{blockSize} {}
		~ShaderDebugger ();

		ND_ bool  AllocForCompute (OUT Result &result, ITransferContext &ctx, ComputePipelineID ppln, const uint3 &globalID,
								   const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;

		ND_ bool  AllocForCompute (OUT Result &result, ITransferContext &ctx, ComputePipelineID ppln,
								   const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;
		
		ND_ bool  AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, const uint3 &launchID,
									  const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;
		
		ND_ bool  AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln,
									  const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)		__Th___;

		template <typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln, const uint2 &fragCoord,
									const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;
		
		template <typename PplnID>
		ND_ bool  AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln,
									const DescriptorSetName &dsName = _DbgShaderTrace, Bytes size = _SingleBufferSize)			__Th___;


		ND_ Promise<Array<String>>  Read (ITransferContext &ctx, const Result &request, ELogFormat format = Default)			__Th___;
		ND_ Promise<Array<String>>  ReadAll (ITransferContext &ctx, ELogFormat format = Default)								__Th___;

		ND_ bool  HasPendingRequests ()																							__NE___	{ return not _pending->empty(); }

			void  Reset ()																										__Th___;


	private:
		ND_ bool  _GetComputePipeline (ComputePipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetRayTracingPipeline (RayTracingPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (GraphicsPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (MeshPipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);
		ND_ bool  _GetGraphicsPipeline (TilePipelineID ppln, const DescriptorSetName &dsName, OUT Result &result);

		template <typename PplnID>
		ND_ bool  _GetPipeline (PplnID ppln, const DescriptorSetName &dsName, OUT Result &result);
		
			void  _FillBuffer (const Result &result, ITransferContext &ctx, Bytes headerSize, const void* headerData) const;

		ND_ bool  _AllocStorage (Bytes size, INOUT Result &result);
		ND_ bool  _InitDS (const Result &info) const;

		ND_ static Array<String>  _Parse (ArrayView<ubyte> view, const void* ppln, ParseTraceFn_t fn, ELogFormat format);

		ND_ static Promise<Array<String>>  _Merge (Array<Promise<Array<String>>> tasks);
	};
	
	

/*
=================================================
	AllocForGraphics
=================================================
*/
	template <typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln, const uint2 &fragCoord, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( not _GetGraphicsPipeline( ppln, dsName, OUT result ))
			return false;

		if_unlikely( not _AllocStorage( size, OUT result ))
			return false;

		const uint	data[4] = { fragCoord.x, fragCoord.y, 0, 0 };
		STATIC_ASSERT( _TraceHeaderSize == sizeof(data) );

		_FillBuffer( result, ctx, Sizeof(data), data );
		return true;
	}
	
	template <typename PplnID>
	bool  ShaderDebugger::AllocForGraphics (OUT Result &result, ITransferContext &ctx, PplnID ppln, const DescriptorSetName &dsName, Bytes size) __Th___
	{
		return AllocForGraphics( OUT result, ctx, ppln, uint2{~0u}, dsName, size );
	}


} // AE::Graphics
