// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Canvas/Canvas.h"
#include "graphics_hl/Resources/RasterFont.h"
#include "graphics_hl/Resources/FormattedText.h"

#ifndef AE_ENABLE_UTF8PROC
# error AE_ENABLE_UTF8PROC required
#endif

namespace AE::Graphics
{

/*
=================================================
	_Alloc
=================================================
*/
	bool  Canvas::_Alloc () __NE___
	{
		CHECK_ERR( _buffers.size() < _buffers.capacity() );

		BufferRange	range;
		range.posCapacity		= _PositionVBufSize;
		range.attribsCapacity	= _AttribsVBufSize;
		range.indexCapacity		= _IndexBufSize;

		auto&			res_mngr = GraphicsScheduler().GetResourceManager();
		VertexStream	vstream;

		CHECK_ERR( res_mngr.GetStagingManager().AllocVStream( _frameId, range.BufferSize(), OUT vstream ));

		range.ptr		= vstream.mappedPtr;
		range.handle	= res_mngr.GetResource( vstream.id )->Handle();
		range.offset	= vstream.offset;

		_buffers.push_back( range );
		return true;
	}

/*
=================================================
	_AllocDrawCall
=================================================
*/
	bool  Canvas::_AllocDrawCall (const uint instanceCount, const uint vertCount, const Bytes32u posSize, const Bytes32u attrSize, const Bytes32u idxDataSize) __NE___
	{
		const Bytes32u	pos_data_size	= posSize * vertCount;
		const Bytes32u	attr_data_size	= attrSize * vertCount;

		// allocate buffer
		if_unlikely( _buffers.empty() or not _buffers.back().HasSpace( pos_data_size, attr_data_size, idxDataSize ))
		{
			CHECK_ERR( _Alloc() );
		}

		const uint range_idx = uint(_buffers.size() - 1);

		// create new draw call
		if_unlikely( _drawCalls.empty() or not _drawCalls.back().Equal( instanceCount, range_idx ))
		{
			CHECK_ERR( _drawCalls.size() < _drawCalls.capacity() );

			auto&		buf				= _buffers[range_idx];
			const uint	pos_vtx_count	= uint( (buf.posSize + posSize - 1) / posSize);
			const uint	attr_vtx_count	= uint( (buf.attribsSize + attrSize - 1) / attrSize);
			const uint	vtx_count		= Max( pos_vtx_count, attr_vtx_count );
			const uint	idx_count		= uint(buf.indexSize / sizeof(BatchIndex_t));

			buf.posSize		= vtx_count * posSize;
			buf.attribsSize	= vtx_count * attrSize;

			ASSERT( vtx_count + vertCount <= MaxValue<BatchIndex_t>() );

			auto&		dc	 = _drawCalls.emplace_back();
			dc.vertexOffset	 = vtx_count;
			dc.firstIndex	 = idx_count;
			dc.instanceCount = instanceCount;
			dc.rangeIdx		 = range_idx;

			ASSERT( dc.instanceCount == instanceCount );
			ASSERT( dc.rangeIdx		 == range_idx );
		}

		return true;
	}

/*
=================================================
	NextFrame
=================================================
*/
	void  Canvas::NextFrame (FrameUID frameId) __NE___
	{
		CHECK_MSG( _drawCalls.empty(), "rendering is in progress" );

		ASSERT( _topology == Default );
		DEBUG_ONLY( _topology = Default );

		_frameId = frameId;
		_buffers.clear();
		_drawCalls.clear();
	}


/*
=================================================
	DrawText
=================================================
*/
	void  Canvas::DrawText (StringView text, const RasterFont &font, const FontParams &params, const RectF &regionInVP) __NE___
	{
		return DrawText( U8StringView{ Cast<CharUtf8>(text.data()), text.size() }, font, params, regionInVP );
	}

	void  Canvas::DrawText (U8StringView text, const RasterFont &font, const FontParams &params, const RectF &regionInVP) __NE___
	{
		ASSERT( params.heightInPx > 0.0f );
		ASSERT( regionInVP.IsValid() );

		DEBUG_ONLY(
			if ( _topology != Default )
				ASSERT( _topology == EPrimitive::TriangleList )
			else
				_topology = EPrimitive::TriangleList;
		)

		const RectF		region_px		= _surfDim.ViewportToPixels( regionInVP );
		const float		line_h_px		= params.heightInPx * params.spacing;
		float2			line_px			{ region_px.left, region_px.top + line_h_px };
		const uint		font_h_px		= font.ValidateHeight( params.heightInPx );
		const float		font_scale_px	= params.heightInPx / float(font_h_px);
		const float2	px_to_vp		= _surfDim.GetPixelsToViewport();
		const usize		max_chars		= Utf8CharCount( text );

		// allocate space
		CHECK_ERRV( _AllocDrawCall( 1, uint(max_chars * 4), SizeOf<FontPosition_t>, SizeOf<FontAttribs_t>, SizeOf<BatchIndex_t> * uint(max_chars * 6) ));

		auto&	dc			= _drawCalls.back();
		auto&	buf			= _buffers[ dc.rangeIdx ];
		uint	vert_count	= 0;
		uint	idx_count	= 0;

		// draw
		for (usize num_chars = 0, idx = 0; idx < text.length();)
		{
			const CharUtf32	c = Utf8Decode( text, INOUT idx );

			if_unlikely( c == '\n' )
			{
				line_px.x  = region_px.left;
				line_px.y += line_h_px;
				continue;
			}

			auto*	glyph = font.GetGlyph( c, font_h_px );
			if_unlikely( glyph == null )
				continue;

			ASSERT( num_chars < max_chars );
			++num_chars;

			const float  width_px = glyph->advance * font_scale_px;	// pixels

			// in viewport space
			const float  pos_x1 = (line_px.x + glyph->offset.left	* font_scale_px) * px_to_vp.x - 1.0f;
			const float  pos_x2 = (line_px.x + glyph->offset.right	* font_scale_px) * px_to_vp.x - 1.0f;
			const float  pos_y1 = (line_px.y + glyph->offset.top	* font_scale_px) * px_to_vp.y - 1.0f;
			const float  pos_y2 = (line_px.y + glyph->offset.bottom	* font_scale_px) * px_to_vp.y - 1.0f;

			line_px.x += width_px;

			// if completely outside
			if_unlikely( (pos_x1 > regionInVP.right) or (pos_y1 > regionInVP.bottom) )
				continue;

			if_unlikely( not glyph->HasImage() )
				continue;

			// add vertices & indices
			{
				auto*		indices		= buf.CurrIndices() + idx_count;
				const auto	first_idx	= BatchIndex_t(dc.vertexOffset + vert_count);
				ASSERT( buf.ptr != null );

				indices[0] = 0 + first_idx;
				indices[1] = 1 + first_idx;
				indices[2] = 2 + first_idx;
				indices[3] = 2 + first_idx;
				indices[4] = 1 + first_idx;
				indices[5] = 3 + first_idx;

				auto*	pos = Cast<FontPosition_t>( buf.CurrPositions() ) + vert_count;
				pos[0] = FontPosition_t{ float2{ pos_x1, pos_y1 }};
				pos[1] = FontPosition_t{ float2{ pos_x1, pos_y2 }};
				pos[2] = FontPosition_t{ float2{ pos_x2, pos_y1 }};
				pos[3] = FontPosition_t{ float2{ pos_x2, pos_y2 }};

				auto*	attr = Cast<FontAttribs_t>( buf.CurrAttribs() ) + vert_count;
				attr[0] = FontAttribs_t{ packed_ushort2{glyph->texcoord.left,  glyph->texcoord.top   },	params.bold, params.color };
				attr[1] = FontAttribs_t{ packed_ushort2{glyph->texcoord.left,  glyph->texcoord.bottom},	params.bold, params.color };
				attr[2] = FontAttribs_t{ packed_ushort2{glyph->texcoord.right, glyph->texcoord.top   },	params.bold, params.color };
				attr[3] = FontAttribs_t{ packed_ushort2{glyph->texcoord.right, glyph->texcoord.bottom},	params.bold, params.color };

				idx_count  += 6;
				vert_count += 4;
			}
		}

		ASSERT( vert_count <= uint(max_chars * 4) );
		ASSERT( idx_count  <= uint(max_chars * 6) );

		dc.indexCount	+= idx_count;
		dc.vertexOffset	+= vert_count;

		buf.posSize		+= SizeOf<FontPosition_t>	* vert_count;
		buf.attribsSize	+= SizeOf<FontAttribs_t>	* vert_count;
		buf.indexSize	+= SizeOf<BatchIndex_t>		* idx_count;
	}

/*
=================================================
	DrawText
=================================================
*/
	void  Canvas::DrawText (const PrecalculatedFormattedText &text, const RasterFont &font, const RectF &regionInVP) __NE___
	{
		ASSERT( regionInVP.IsValid() );

		DEBUG_ONLY(
			if ( _topology != Default )
				ASSERT( _topology == EPrimitive::TriangleList )
			else
				_topology = EPrimitive::TriangleList;
		)

		const RectF		region_px	= _surfDim.ViewportToPixels( regionInVP );
		float2			line_px		{ region_px.left, region_px.top + text.LineHeights().front() };
		const float2	px_to_vp	= _surfDim.GetPixelsToViewport();
		const uint		max_chars	= text.Text().GetMaxChars();
		const float		region_w_px	= region_px.Width();	// pixels
		uint			line_idx	= 1;

		// allocate space
		CHECK_ERRV( _AllocDrawCall( 1, (max_chars * 4), SizeOf<FontPosition_t>, SizeOf<FontAttribs_t>, SizeOf<BatchIndex_t> * (max_chars * 6) ));

		uint	vert_count	= 0;
		uint	idx_count	= 0;
		uint	num_chars	= 0;

		const auto	AddGlyph = [this, &idx_count, &vert_count] (float pos_x1, float pos_x2, float pos_y1, float pos_y2,
																const Rectangle<ushort> &texcoord, ushort txt_scale, const RGBA8u &color)
		{{
			auto&		dc			= _drawCalls.back();
			auto&		buf			= _buffers[ dc.rangeIdx ];
			auto*		indices		= buf.CurrIndices() + idx_count;
			const auto	first_idx	= BatchIndex_t(dc.vertexOffset + vert_count);
			ASSERT( buf.ptr != null );

			indices[0] = 0 + first_idx;
			indices[1] = 1 + first_idx;
			indices[2] = 2 + first_idx;
			indices[3] = 2 + first_idx;
			indices[4] = 1 + first_idx;
			indices[5] = 3 + first_idx;

			auto*	pos = Cast<FontPosition_t>( buf.CurrPositions() ) + vert_count;
			pos[0] = FontPosition_t{ float2{ pos_x1, pos_y1 }};
			pos[1] = FontPosition_t{ float2{ pos_x1, pos_y2 }};
			pos[2] = FontPosition_t{ float2{ pos_x2, pos_y1 }};
			pos[3] = FontPosition_t{ float2{ pos_x2, pos_y2 }};

			auto*	attr = Cast<FontAttribs_t>( buf.CurrAttribs() ) + vert_count;
			attr[0] = FontAttribs_t{ packed_ushort2{texcoord.left,  texcoord.top   },	txt_scale, color };
			attr[1] = FontAttribs_t{ packed_ushort2{texcoord.left,  texcoord.bottom},	txt_scale, color };
			attr[2] = FontAttribs_t{ packed_ushort2{texcoord.right, texcoord.top   },	txt_scale, color };
			attr[3] = FontAttribs_t{ packed_ushort2{texcoord.right, texcoord.bottom},	txt_scale, color };

			idx_count  += 6;
			vert_count += 4;
		}};

		// draw text chunks
		for (auto* chunk = text.Text().GetFirst(); chunk; chunk = chunk->next)
		{
			const uint		font_h_px		= font.ValidateHeight( chunk->Height() );
			const float		font_scale_px	= chunk->Height() / float(font_h_px);
			const RGBA8u	color			= chunk->color;
			const float2	line_start		= line_px;

			for (usize idx = 0; idx < chunk->length;)
			{
				const CharUtf32	c = Utf8Decode( chunk->Text(), INOUT idx );

				if_unlikely( c == '\n' )
				{
					line_px.x  = region_px.left;
					line_px.y += text.LineHeights()[ line_idx++ ];
					continue;
				}

				auto*	glyph = font.GetGlyph( c, font_h_px );
				if_unlikely( glyph == null )
					continue;

				ASSERT( num_chars < max_chars );
				++num_chars;

				const float  width_px = glyph->advance * font_scale_px;	// pixels

				if_unlikely( text.IsWordWrap() and (line_px.x + width_px > region_w_px) )
				{
					line_px.x  = region_px.left;
					line_px.y += text.LineHeights()[ line_idx++ ];
				}

				// in viewport space
				const float  pos_x1 = (line_px.x + glyph->offset.left	* font_scale_px) * px_to_vp.x - 1.0f;
				const float  pos_x2 = (line_px.x + glyph->offset.right	* font_scale_px) * px_to_vp.x - 1.0f;
				const float  pos_y1 = (line_px.y + glyph->offset.top	* font_scale_px) * px_to_vp.y - 1.0f;
				const float  pos_y2 = (line_px.y + glyph->offset.bottom	* font_scale_px) * px_to_vp.y - 1.0f;

				line_px.x += width_px;

				if_unlikely( not glyph->HasImage() )
					continue;

				AddGlyph( pos_x1, pos_x2, pos_y1, pos_y2, glyph->texcoord, 0, color );
			}

			if_unlikely( chunk->underline )
			{
				// in viewport space
				const float  pos_x1 = line_start.x		* px_to_vp.x - 1.0f;
				const float  pos_x2 = line_px.x			* px_to_vp.x - 1.0f;
				const float  pos_y1 = (line_px.y - 1.f)	* px_to_vp.y - 1.0f;
				const float  pos_y2 = (line_px.y + 1.f)	* px_to_vp.y - 1.0f;

				AddGlyph( pos_x1, pos_x2, pos_y1, pos_y2, Default, 0, color );
			}

			if_unlikely( chunk->strikeout )
			{
				// in viewport space
				const float  pos_x1 = line_start.x		* px_to_vp.x - 1.0f;
				const float  pos_x2 = line_px.x			* px_to_vp.x - 1.0f;
				const float  pos_y1 = (line_px.y - 1.f)	* px_to_vp.y - 1.0f;	// TODO
				const float  pos_y2 = (line_px.y + 1.f)	* px_to_vp.y - 1.0f;

				AddGlyph( pos_x1, pos_x2, pos_y1, pos_y2, Default, 0, color );
			}
		}

		ASSERT( vert_count <= (max_chars * 4) );
		ASSERT( idx_count  <= (max_chars * 6) );

		auto&	dc		= _drawCalls.back();
		dc.indexCount	+= idx_count;
		dc.vertexOffset	+= vert_count;

		auto&	buf		= _buffers[ dc.rangeIdx ];
		buf.posSize		+= SizeOf<FontPosition_t>   * vert_count;
		buf.attribsSize	+= SizeOf<FontAttribs_t>    * vert_count;
		buf.indexSize	+= SizeOf<BatchIndex_t>		* idx_count;
	}


} // AE::Graphics
