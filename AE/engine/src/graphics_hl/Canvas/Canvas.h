// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no

	Canvas does not allocate memory, it uses vstream from 'StagingBufferManager'.
	VStream allocation granularity defined in 'Canvas::_MaxVertsPerBatch' and 'Canvas::_IndexBufSize'.
	You should reuse canvas for small draw commands in single thread.
*/

#pragma once

#include "graphics_hl/Canvas/SurfaceDimensions.h"
#include "graphics_hl/Canvas/Primitives.h"

namespace AE::Graphics
{
	class RasterFont;
	class PrecalculatedFormattedText;


	//
	// Canvas
	//

	class Canvas final
	{
	// types
	public:
		struct FontParams
		{
			float	heightInPx;
			float	spacing;
			RGBA8u	color;
			ushort	bold;			// unorm, for SDF only
			bool	italic		: 1;
			bool	underline	: 1;
			bool	strikeout	: 1;

			FontParams ()					__NE___ :
				heightInPx{10.0f}, spacing{1.1f},
				bold{0}, italic{false}, underline{false}, strikeout{false}
			{}

			FontParams&  Bold (bool value)	__NE___ { bold = value ? FloatToUNormShort(0.5f) : FloatToUNormShort(0.2f);  return *this; }
		};


	private:
		using NativeBuffer_t = IResourceManager::NativeBuffer_t;

		struct DrawCall
		{
			uint	vertexOffset;
			uint	firstIndex;
			uint	indexCount;
			uint	instanceCount	: 28;
			uint	rangeIdx		: 4;

			ND_ bool  Equal (uint instCnt, uint range) C_NE___ {
				return (instanceCount == instCnt) and (rangeIdx == range);
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

		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			RmDevicePtr		devicePtr;
		  #endif


		// methods
			ND_ Bytes32u	_PositionOffset ()						C_NE___	{ return 0_b; }
			ND_ Bytes32u	_AttribsOffset ()						C_NE___	{ return posCapacity; }
			ND_ Bytes32u	_IndicesOffset ()						C_NE___	{ return posCapacity + attribsCapacity; }

			ND_ Bytes		PositionOffset ()						C_NE___	{ return offset + _PositionOffset(); }
			ND_ Bytes		AttribsOffset ()						C_NE___	{ return offset + _AttribsOffset(); }
			ND_ Bytes		IndicesOffset ()						C_NE___	{ return offset + _IndicesOffset(); }
			ND_ Bytes		BufferSize ()							C_NE___	{ return posCapacity + attribsCapacity + indexCapacity; }

			ND_ void*		Positions ()							__NE___	{ return ptr + _PositionOffset(); }
			ND_ void*		Attribs ()								__NE___	{ return ptr + _AttribsOffset(); }
			ND_ auto*		Indices ()								__NE___	{ return Cast<BatchIndex_t>( ptr + _IndicesOffset() ); }

			ND_ void*		CurrPositions ()						__NE___	{ return Positions() + posSize; }
			ND_ void*		CurrAttribs ()							__NE___	{ return Attribs() + attribsSize; }
			ND_ auto*		CurrIndices ()							__NE___	{ return Indices() + indexSize; }

			ND_ bool  HasSpace (Bytes pos, Bytes attr, Bytes idx)	C_NE___
			{
				return	(posSize     + pos  <= posCapacity)		and
						(attribsSize + attr <= attribsCapacity)	and
						(indexSize   + idx  <= indexCapacity);
			}
		};

		using DrawCalls_t		= FixedArray< DrawCall, 16 >;
		using Buffers_t			= FixedArray< BufferRange, 16 >;
		using VBufferCache_t	= FixedArray< Strong<BufferID>, 16 >;
		using Allocator_t		= LinearAllocator< UntypedAllocator, 8, false >;

		static constexpr uint		_MaxVertsPerBatch	= 1u << 12;
		static constexpr Bytes32u	_PositionVBufSize	{8_b * _MaxVertsPerBatch};
		static constexpr Bytes32u	_AttribsVBufSize	{16_b * _MaxVertsPerBatch};
		static constexpr Bytes32u	_IndexBufSize		{SizeOf<BatchIndex_t> * _MaxVertsPerBatch * 3};

		using FontPosition_t	= VB_Position_f2;
		using FontAttribs_t		= VB_UVs2_SCs1_Col8;


	// variables
	private:
		DrawCalls_t			_drawCalls;
		Buffers_t			_buffers;

		FrameUID			_frameId;

		SurfaceDimensions	_surfDim;

		DEBUG_ONLY(
			EPrimitive		_topology	= Default;
		)


	// methods
	public:
		Canvas ()																										__NE___	{}
		~Canvas ()																										__NE___	{}

		void  SetDimensions (const SurfaceDimensions &dim)																__NE___	{ _surfDim = dim; }
		void  SetDimensions (const uint2 &surfaceSizeInPix, float mmPerPixel)											__NE___	{ _surfDim.SetDimensions( surfaceSizeInPix, mmPerPixel ); }
		void  SetDimensions (const App::IOutputSurface::RenderTarget &rt)												__NE___	{ _surfDim.SetDimensions( rt ); }

		ND_ SurfaceDimensions const&  Dimensions ()																		C_NE___	{ return _surfDim; }


		void  NextFrame (FrameUID frameId)																				__NE___;

		template <typename Ctx>
		void  Flush (Ctx &ctx, EPrimitive topology = Default)															__Th___;

		ND_ bool  IsEmpty ()																							C_NE___	{ return _drawCalls.empty(); }


	// High level //
		void  DrawText (StringView text, const RasterFont &font, const FontParams &params, const RectF &regionInVP)		__NE___;
		void  DrawText (U8StringView text, const RasterFont &font, const FontParams &params, const RectF &regionInVP)	__NE___;

		void  DrawText (const PrecalculatedFormattedText &text, const RasterFont &font, const RectF &regionInVP)		__NE___;


	// Low level //
		template <typename PrimitiveType>
		void  Draw (const PrimitiveType &primitive)																		__NE___;

		template <typename PrimitiveType>
		void  DrawInstanced (const PrimitiveType &primitive, uint instanceCount)										__NE___;


	private:
		template <typename PrimitiveType>	void  _BreakStrip    (const PrimitiveType &primitive, uint indexCount, uint vertexCount)__NE___;
		template <typename PrimitiveType>	void  _ContinueStrip (const PrimitiveType &primitive, uint indexCount, uint vertexCount)__NE___;

		ND_ bool  _AllocDrawCall (uint instanceCount, uint vertCount, Bytes32u posSize, Bytes32u attrSize, Bytes32u idxDataSize)	__NE___;
		ND_ bool  _Alloc ()																											__NE___;
	};



/*
=================================================
	Flush
=================================================
*/
	template <typename Ctx>
	void  Canvas::Flush (Ctx &ctx, EPrimitive topology) __Th___
	{
		StaticAssert( IsBaseOf< IDrawContext, Ctx >);

		if_unlikely( _drawCalls.empty() )
			return;

		DEBUG_ONLY(
			if ( topology != Default )
				ASSERT( topology == _topology );	// topology in pipeline must be same as in primitives

			_topology = Default;
		)
		Unused( topology );

		uint			last_range_idx = UMax;
		NativeBuffer_t	vbuffers [2];
		Bytes			offsets  [2];

		for (auto& dc : _drawCalls)
		{
			if_unlikely( last_range_idx != dc.rangeIdx )
			{
				last_range_idx = dc.rangeIdx;

				const auto&	range = _buffers[ dc.rangeIdx ];

				vbuffers[0] = range.handle;
				vbuffers[1] = range.handle;
				offsets[0]	= range.PositionOffset();
				offsets[1]  = range.AttribsOffset();

				ctx.BindVertexBuffers( 0, vbuffers, offsets );
				ctx.BindIndexBuffer( range.handle, range.IndicesOffset(), IndexDesc<BatchIndex_t>::value );
			}

			DrawIndexedCmd	cmd;
			cmd.firstIndex		= dc.firstIndex;
			cmd.indexCount		= dc.indexCount;
			cmd.instanceCount	= dc.instanceCount;

			ctx.DrawIndexed( cmd );
		}
		_drawCalls.clear();

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		for (auto& buf : _buffers) {
			ctx.CopyHostToDev( buf.devicePtr, buf.ptr, buf.offset, buf.BufferSize() );
		}
	  #endif
	}

/*
=================================================
	Draw
=================================================
*/
	template <typename PrimitiveType>
	void  Canvas::Draw (const PrimitiveType &primitive) __NE___
	{
		DrawInstanced( primitive, 1 );
	}

	template <typename PrimitiveType>
	void  Canvas::DrawInstanced (const PrimitiveType &primitive, uint instanceCount) __NE___
	{
		DEBUG_ONLY(
			if ( _topology != Default )
				ASSERT( _topology == PrimitiveType::Topology() )
			else
				_topology = PrimitiveType::Topology();
		)
		ASSERT( instanceCount >= 1 );

		const uint		idx_count	= primitive.IndexCount();
		const uint		vert_count	= primitive.VertexCount();
		const Bytes32u	pos_size	= SizeOf< typename PrimitiveType::Position_t >;
		const Bytes32u	attr_size	= SizeOf< typename PrimitiveType::Attribs_t  >;
		const Bytes32u	idx_size	= SizeOf< BatchIndex_t > * idx_count;

		// add primitive to draw call
		if constexpr( PrimitiveType::Topology() == EPrimitive::LineStrip	or
					  PrimitiveType::Topology() == EPrimitive::TriangleStrip )
		{
			CHECK_ERRV( _AllocDrawCall( instanceCount, vert_count, pos_size, attr_size, idx_size + SizeOf<BatchIndex_t>*2 ));

			_BreakStrip( primitive, idx_count, vert_count );
		}
		else
		{
			CHECK_ERRV( _AllocDrawCall( instanceCount, vert_count, pos_size, attr_size, idx_size ));

			_ContinueStrip( primitive, idx_count, vert_count );
		}

		// update buffer size
		{
			auto&	buf		= _buffers[ _drawCalls.back().rangeIdx ];
			buf.posSize		+= pos_size  * vert_count;
			buf.attribsSize	+= attr_size * vert_count;
			buf.indexSize	+= idx_size;
		}
	}

/*
=================================================
	_BreakStrip
=================================================
*/
	template <typename PrimitiveType>
	void  Canvas::_BreakStrip (const PrimitiveType &primitive, const uint indexCount, const uint vertexCount) __NE___
	{
		auto&		dc		= _drawCalls.back();
		auto&		buf		= _buffers[ dc.rangeIdx ];
		auto*		indices	= buf.CurrIndices();
		const uint	off		= dc.indexCount ? 2 : 0;
		NonNull( buf.ptr );

		primitive.Get( OUT indices + off, BatchIndex_t(dc.vertexOffset), OUT buf.CurrPositions(), OUT buf.CurrAttribs(), _surfDim );

		if_likely( off )
		{
			indices[0]	= indices[-1];
			indices[1]	= indices[2];
		}

		buf.indexSize	+= off * SizeOf<BatchIndex_t>;
		dc.indexCount	+= indexCount + off;
		dc.vertexOffset	+= vertexCount;
	}

/*
=================================================
	_ContinueStrip
=================================================
*/
	template <typename PrimitiveType>
	void  Canvas::_ContinueStrip (const PrimitiveType &primitive, const uint indexCount, const uint vertexCount) __NE___
	{
		auto&	dc	= _drawCalls.back();
		auto&	buf	= _buffers[ dc.rangeIdx ];
		NonNull( buf.ptr );

		primitive.Get( OUT buf.CurrIndices(), BatchIndex_t(dc.vertexOffset), OUT buf.CurrPositions(), OUT buf.CurrAttribs(), _surfDim );

		dc.indexCount	+= indexCount;
		dc.vertexOffset	+= vertexCount;
	}


} // AE::Graphics
