// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/GraphicsImpl.h"

#include "graphics_hl/Canvas/SurfaceDimensions.h"
#include "graphics_hl/Canvas/Primitives.h"


namespace AE::Graphics
{

	//
	// Canvas
	//

	class Canvas final : public SurfaceDimensions
	{
	// types
	private:
		using NativeBuffer_t = IResourceManager::NativeBuffer_t;

		struct DrawCall
		{
			uint	vertexOffset;
			uint	firstIndex;
			uint	indexCount;
			uint	instanceCount	: 28;
			uint	rangeIdx		: 4;

			ND_ bool  Equal (uint instCnt, uint range) const
			{
				return (instanceCount == instCnt) | (rangeIdx == range);
			}
		};
		
		struct BufferRange
		{
		// variables
			void *			ptr;

			Bytes32u		posSize;
			Bytes32u		attribsSize;
			Bytes32u		indexSize;
			
			Bytes32u		posCapacity;
			Bytes32u		attribsCapacity;
			Bytes32u		indexCapacity;
			
			Bytes			offset;
			NativeBuffer_t	handle;
			

		// methods
			ND_ Bytes32u	_PositionOffset ()	const	{ return 0_b; }
			ND_ Bytes32u	_AttribsOffset ()	const	{ return posCapacity; }
			ND_ Bytes32u	_IndicesOffset ()	const	{ return posCapacity + attribsCapacity; }

			ND_ Bytes		PositionOffset ()	const	{ return offset + _PositionOffset(); }
			ND_ Bytes		AttribsOffset ()	const	{ return offset + _AttribsOffset(); }
			ND_ Bytes		IndicesOffset ()	const	{ return offset + _IndicesOffset(); }
			ND_ Bytes		BufferSize ()		const	{ return posCapacity + attribsCapacity + indexCapacity; }
			
			ND_ void*		Positions ()				{ return ptr + _PositionOffset(); }
			ND_ void*		Attribs ()					{ return ptr + _AttribsOffset(); }
			ND_ auto*		Indices ()					{ return Cast<BatchIndex_t>( ptr + _IndicesOffset() ); }
			
			ND_ void*		CurrPositions ()			{ return Positions() + posSize; }
			ND_ void*		CurrAttribs ()				{ return Attribs() + attribsSize; }
			ND_ auto*		CurrIndices ()				{ return Indices() + indexSize; }

			ND_ bool  HasSpace (Bytes pos, Bytes attr, Bytes idx) const
			{
				return	(posSize     + pos  <= posCapacity)		|
						(attribsSize + attr <= attribsCapacity)	|
						(indexSize   + idx  <= indexCapacity);
			}
		};

		using DrawCalls_t		= FixedArray< DrawCall, 16 >;
		using Buffers_t			= FixedArray< BufferRange, 16 >;
		using VBufferCache_t	= FixedArray< Strong<BufferID>, 16 >;
		using Allocator_t		= LinearAllocator< UntypedAlignedAllocator, 8, false >;

		static constexpr uint		_MaxVertsPerBatch	= 1u << 12;
		static constexpr Bytes32u	_PositionVBufSize	{8_b * _MaxVertsPerBatch};
		static constexpr Bytes32u	_AttribsVBufSize	{16_b * _MaxVertsPerBatch};
		static constexpr Bytes32u	_IndexBufSize		{SizeOf<BatchIndex_t> * _MaxVertsPerBatch * 3};


	// variables
	private:
		DrawCalls_t		_drawCalls;
		Buffers_t		_buffers;
		
		FrameUID		_frameId;

		DEBUG_ONLY(
			EPrimitive	_topology	= Default;
		)


	// methods
	public:
		Canvas () {}
		~Canvas () {}

		void  NextFrame (FrameUID frameId);

		template <typename Ctx>
		void  Flush (Ctx &ctx, EPrimitive topology = Default);



	// High level //
		//void  DrawText (const VectorFont::PrecalculatedText &text, const FontPtr &font, const RectF &regionInPx);


	// Low level //
		template <typename PrimitiveType>
		void  Draw (const PrimitiveType &primitive);
		
		template <typename PrimitiveType>
		void  DrawInstanced (const PrimitiveType &primitive, uint instanceCount);


	private:
		template <typename PrimitiveType>	void  _BreakStrip    (const PrimitiveType &primitive, uint indexCount, uint vertexCount);
		template <typename PrimitiveType>	void  _ContinueStrip (const PrimitiveType &primitive, uint indexCount, uint vertexCount);

		ND_ bool  _AllocDrawCall (uint instanceCount, uint vertCount, Bytes32u posSize, Bytes32u attrSize, Bytes32u idxDataSize);
		ND_ bool  _Alloc ();
	};


}	// AE::Graphics
