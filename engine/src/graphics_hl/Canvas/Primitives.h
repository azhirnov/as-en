// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Radians.h"
#include "graphics/Public/RenderStateEnums.h"

namespace AE::Graphics
{
#	include "graphics_hl/Canvas/VertexTypes.h"
	
	using BatchIndex_t = ushort;


	//
	// Rectangle Batch
	//
	template <typename PosType, typename AttribType, bool Strip>
	struct RectangleBatch
	{
	// types
		using Position_t	= PosType;
		using Attribs_t		= AttribType;

	// variables
		RectF		position;
		RectF		texcoord;
		RGBA8u		color;

	// methods
		constexpr RectangleBatch () {}
		constexpr RectangleBatch (const RectF &pos, RGBA8u color) : RectangleBatch{pos, RectF{0.f, 0.f, 1.f, 1.f}, color} {}
		constexpr RectangleBatch (const RectF &pos, const RectF &texc, RGBA8u color = HtmlColor::White) :	position{pos}, texcoord{texc}, color{color} {}
		
		ND_ static constexpr EPrimitive	Topology ()				{ return Strip ? EPrimitive::TriangleStrip : EPrimitive::TriangleList; }
		ND_ constexpr uint				IndexCount ()	const	{ return Strip ? 4 : 6; }
		ND_ constexpr uint				VertexCount ()	const	{ return 4; }

		constexpr void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
							 OUT void *positionPtr, OUT void *attributePtr) const;
	};
	

	
	//
	// Nine Patch Batch
	//
	template <typename PosType, typename AttribType>
	struct NinePatchBatch
	{
	// types
		using Position_t	= PosType;
		using Attribs_t		= AttribType;

	// variables
		RectF		position;		// center + border
		RectF		posOffsets;		// border
		RectF		texcoord;		// center + border
		RectF		texcOffsets;	// border
		RGBA8u		color;

	// methods
		constexpr NinePatchBatch () {}
		constexpr NinePatchBatch (const RectF &pos, const RectF &posOffset, const RectF &texc, const RectF &texcOffset, RGBA8u color = HtmlColor::White) :
			position{pos}, posOffsets{posOffset}, texcoord{texc}, texcOffsets{texcOffset}, color{color}
		{
			ASSERT( posOffsets.left		>= 0.0f and
					posOffsets.right	>= 0.0f	and
					posOffsets.top		>= 0.0f	and
					posOffsets.bottom	>= 0.0f );
			ASSERT( texcOffsets.left	>= 0.0f and
					texcOffsets.right	>= 0.0f	and
					texcOffsets.top		>= 0.0f	and
					texcOffsets.bottom	>= 0.0f );
		}
		
		ND_ static constexpr EPrimitive	Topology ()				{ return EPrimitive::TriangleList; }
		ND_ constexpr uint				IndexCount ()	const	{ return 54; }
		ND_ constexpr uint				VertexCount ()	const	{ return 16; }
		
		constexpr void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
							 OUT void *positionPtr, OUT void *attributePtr) const;
	};



	//
	// Circle Batch
	//
	template <typename PosType, typename AttribType, bool Fill>
	struct CircleBatch
	{
	// types
		using Position_t	= PosType;
		using Attribs_t		= AttribType;

	// variables
		RectF		position;
		RGBA8u		color;
		uint		segments	= 0;

	// methods
		constexpr CircleBatch () {}
		constexpr CircleBatch (uint segments, const RectF &pos, RGBA8u color) : position{pos}, color{color}, segments{segments} {}

		ND_ static constexpr EPrimitive	Topology ()				{ return EPrimitive::LineList; }
		ND_ constexpr uint				IndexCount ()	const	{ return segments * 2; }
		ND_ constexpr uint				VertexCount ()	const	{ return segments; }
		
		constexpr void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
							 OUT void *positionPtr, OUT void *attributePtr) const;
	};
	


	//
	// Circle Batch
	//
	template <typename PosType, typename AttribType>
	struct CircleBatch< PosType, AttribType, true >
	{
	// types
		using Position_t	= PosType;
		using Attribs_t		= AttribType;

	// variables
		RectF		position;
		RectF		texcoord;
		RGBA8u		color;
		uint		segments	= 0;

	// methods
		constexpr CircleBatch () {}
		constexpr CircleBatch (uint segments, const RectF &pos, RGBA8u color) : CircleBatch{segments, pos, RectF{}, color} {}

		constexpr CircleBatch (uint segments, const RectF &pos, const RectF &texc, RGBA8u color) :
			position{pos}, texcoord{texc}, color{color}, segments{segments}
		{
			ASSERT( segments >= 4 );
		}

		ND_ static constexpr EPrimitive	Topology ()				{ return EPrimitive::TriangleList; }
		ND_ constexpr uint				IndexCount ()	const	{ return segments * 3; }
		ND_ constexpr uint				VertexCount ()	const	{ return segments + 1; }
		
		constexpr void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
							 OUT void *positionPtr, OUT void *attributePtr) const;
	};

	using Rectangle2D		= RectangleBatch< VB_Position_f2, VB_UVf2_Col, /*Strip*/false >;
	using Rectangle2DStrip	= RectangleBatch< VB_Position_f2, VB_UVf2_Col, /*Strip*/true >;
	using NinePatch2D		= NinePatchBatch< VB_Position_f2, VB_UVf2_Col >;
	using Circle2D			= CircleBatch< VB_Position_f2, VB_UVf2_Col, /*Fill*/false >;
	using FilledCircle2D	= CircleBatch< VB_Position_f2, VB_UVf2_Col, /*Fill*/true >;


}	// AE::Graphics
