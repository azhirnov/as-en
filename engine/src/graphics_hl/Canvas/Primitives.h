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
							 OUT void *positionPtr, OUT void *attributePtr) const
		{
			// front face CCW
			if constexpr( Strip ){
				idx[0] = 0 + firstIdx;
				idx[1] = 1 + firstIdx;
				idx[2] = 2 + firstIdx;
				idx[3] = 3 + firstIdx;
			}else{
				idx[0] = 0 + firstIdx;
				idx[1] = 1 + firstIdx;
				idx[2] = 2 + firstIdx;
				idx[3] = 2 + firstIdx;
				idx[4] = 1 + firstIdx;
				idx[5] = 3 + firstIdx;
			}

			auto*	pos = Cast<PosType>( positionPtr );
			pos[0] = PosType{ float2{ position.left,  position.top	 }};
			pos[1] = PosType{ float2{ position.left,  position.bottom}};
			pos[2] = PosType{ float2{ position.right, position.top	 }};
			pos[3] = PosType{ float2{ position.right, position.bottom}};
		
			auto*	attr = Cast<AttribType>( attributePtr );
			attr[0] = AttribType{ float2{texcoord.left,  texcoord.top   },	color };
			attr[1] = AttribType{ float2{texcoord.left,  texcoord.bottom},	color };
			attr[2] = AttribType{ float2{texcoord.right, texcoord.top   },	color };
			attr[3] = AttribType{ float2{texcoord.right, texcoord.bottom},	color };
		}
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
							 OUT void *positionPtr, OUT void *attributePtr) const
		{
			// indices (front face CCW)
			{
				// left top
				idx[0] = 0 + firstIdx;		idx[1] = 6 + firstIdx;		idx[2] = 4 + firstIdx;
				idx[3] = 4 + firstIdx;		idx[4] = 6 + firstIdx;		idx[5] = 5 + firstIdx;

				// center top
				idx[6] = 4 + firstIdx;		idx[7]  = 5 + firstIdx;		idx[8]  = 7 + firstIdx;
				idx[9] = 7 + firstIdx;		idx[10] = 5 + firstIdx;		idx[11] = 8 + firstIdx;

				// right top
				idx[12] = 7 + firstIdx;		idx[13] = 8 + firstIdx;		idx[14] = 2 + firstIdx;
				idx[15] = 2 + firstIdx;		idx[16] = 8 + firstIdx;		idx[17] = 9 + firstIdx;
			
				// left center
				idx[18] = 6 + firstIdx;		idx[19] = 12 + firstIdx;	idx[20] =  5 + firstIdx;
				idx[21] = 5 + firstIdx;		idx[22] = 12 + firstIdx;	idx[23] = 10 + firstIdx;
			
				// center
				idx[24] = 5 + firstIdx;		idx[25] = 10 + firstIdx;	idx[26] = 8 + firstIdx;
				idx[27] = 8 + firstIdx;		idx[28] = 10 + firstIdx;	idx[29] = 13 + firstIdx;
			
				// right center
				idx[30] = 8 + firstIdx;		idx[31] = 13 + firstIdx;	idx[32] =  9 + firstIdx;
				idx[33] = 9 + firstIdx;		idx[34] = 13 + firstIdx;	idx[35] = 15 + firstIdx;
			
				// left bottom
				idx[36] = 12 + firstIdx;	idx[37] = 1 + firstIdx;		idx[38] = 10 + firstIdx;
				idx[39] = 10 + firstIdx;	idx[40] = 1 + firstIdx;		idx[41] = 11 + firstIdx;
			
				// center bottom
				idx[42] = 10 + firstIdx;	idx[43] = 11 + firstIdx;	idx[44] = 13 + firstIdx;
				idx[45] = 13 + firstIdx;	idx[46] = 11 + firstIdx;	idx[47] = 14 + firstIdx;
			
				// right bottom
				idx[48] = 13 + firstIdx;	idx[49] = 14 + firstIdx;	idx[50] = 15 + firstIdx;
				idx[51] = 15 + firstIdx;	idx[52] = 14 + firstIdx;	idx[53] =  3 + firstIdx;
			}
		
			auto*		pos		= Cast<PosType>( positionPtr );
			auto*		attr	= Cast<AttribType>( attributePtr );

			const float	posx[]	= { position.left, position.left + posOffsets.left,  position.right  - posOffsets.right,   position.right  };
			const float	posy[]	= { position.top,  position.top  + posOffsets.top,   position.bottom - posOffsets.bottom,  position.bottom };
			const float tcx[]	= { texcoord.left, texcoord.left + texcOffsets.left, texcoord.right  - texcOffsets.right,  texcoord.right  };
			const float tcy[]	= { texcoord.top,  texcoord.top  + texcOffsets.top,  texcoord.bottom - texcOffsets.bottom, texcoord.bottom };

			// perimeter (0,1,2,3)
			{
				pos[0] = PosType{ float2{posx[0], posy[0]} };
				pos[1] = PosType{ float2{posx[0], posy[3]} };
				pos[2] = PosType{ float2{posx[3], posy[0]} };
				pos[3] = PosType{ float2{posx[3], posy[3]} };

				attr[0] = AttribType{ float2{tcx[0], tcy[0]}, color };
				attr[1] = AttribType{ float2{tcx[0], tcy[3]}, color };
				attr[2] = AttribType{ float2{tcx[3], tcy[0]}, color };
				attr[3] = AttribType{ float2{tcx[3], tcy[3]}, color };
			}

			// left top (4,5,6)
			{
				pos[4] = PosType{ float2{posx[1], posy[0]} };
				pos[5] = PosType{ float2{posx[1], posy[1]} };
				pos[6] = PosType{ float2{posx[0], posy[1]} };
				
				attr[4] = AttribType{ float2{tcx[1], tcy[0]}, color };
				attr[5] = AttribType{ float2{tcx[1], tcy[1]}, color };
				attr[6] = AttribType{ float2{tcx[0], tcy[1]}, color };
			}
			
			// right top (7,8,9)
			{
				pos[7] = PosType{ float2{posx[2], posy[0]} };
				pos[8] = PosType{ float2{posx[2], posy[1]} };
				pos[9] = PosType{ float2{posx[3], posy[1]} };
				
				attr[7] = AttribType{ float2{tcx[2], tcy[0]}, color };
				attr[8] = AttribType{ float2{tcx[2], tcy[1]}, color };
				attr[9] = AttribType{ float2{tcx[3], tcy[1]}, color };
			}
			
			// left bottom (10,11,12)
			{
				pos[10] = PosType{ float2{posx[1], posy[2]} };
				pos[11] = PosType{ float2{posx[1], posy[3]} };
				pos[12] = PosType{ float2{posx[0], posy[2]} };
				
				attr[10] = AttribType{ float2{tcx[1], tcy[2]}, color };
				attr[11] = AttribType{ float2{tcx[1], tcy[3]}, color };
				attr[12] = AttribType{ float2{tcx[0], tcy[2]}, color };
			}
			
			// right bottom (13,14,15)
			{
				pos[13] = PosType{ float2{posx[2], posy[2]} };
				pos[14] = PosType{ float2{posx[2], posy[3]} };
				pos[15] = PosType{ float2{posx[3], posy[2]} };
				
				attr[13] = AttribType{ float2{tcx[2], tcy[2]}, color };
				attr[14] = AttribType{ float2{tcx[2], tcy[3]}, color };
				attr[15] = AttribType{ float2{tcx[3], tcy[2]}, color };
			}
		}
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
		
		void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
			 	   OUT void *positionPtr, OUT void *attributePtr) const
		{
			// indices (front face CCW)
			{
				for (uint i = 0; i < segments-1; ++i)
				{
					*(idx++) = BatchIndex_t(i)   + firstIdx;
					*(idx++) = BatchIndex_t(i+1) + firstIdx;
				}

				*(idx++) = BatchIndex_t(segments-1) + firstIdx;
				*(idx++) = firstIdx;
			}

			const float2	center	= position.Center();
			const float2	scale	= position.Size() * 0.5f;

			auto*			pos		= Cast<PosType>( positionPtr );
			auto*			attr	= Cast<AttribType>( attributePtr );

			for (uint i = 0; i < segments; ++i)
			{
				float	a = 2.0f * float(Pi) * float(i) / segments;
				float2	p = center + scale * float2{cos(a), sin(a)};
				
				pos[i]  = PosType{ p };
				attr[i] = AttribType{ float2{}, color };
			}
		}
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
		
		void  Get (OUT BatchIndex_t *idx, BatchIndex_t firstIdx,
			 	   OUT void *positionPtr, OUT void *attributePtr) const
		{
			// indices (front face CCW)
			{
				for (uint i = 1; i < segments; ++i)
				{
					*(idx++) = firstIdx;
					*(idx++) = BatchIndex_t(i+1) + firstIdx;
					*(idx++) = BatchIndex_t(i) + firstIdx;
				}
			
				*(idx++) = firstIdx;
				*(idx++) = 1 + firstIdx;
				*(idx++) = BatchIndex_t(segments) + firstIdx;
			}
		
			const float2	center		= position.Center();
			const float2	scale		= position.Size() * 0.5f;
			const float2	tc_bias		= texcoord.Center();
			const float2	tc_scale	= texcoord.Size();

			auto*			pos			= Cast<PosType>( positionPtr );
			auto*			attr		= Cast<AttribType>( attributePtr );
			
			pos[0]  = PosType{ center };
			attr[0] = AttribType{ tc_bias, color };

			for (uint i = 0; i < segments;)
			{
				float	angle	= 2.0f * float(Pi) * float(i) / segments;
				float2	factor	= float2{cos(angle), sin(angle)};
				float2	p		= center + scale * factor;
				float2	texc	= tc_bias + tc_scale * factor;
				
				++i;
				pos[i]  = PosType{ p };
				attr[i] = AttribType{ texc, color };
			}
		}
	};

	using Rectangle2D		= RectangleBatch< VB_Position_f2, VB_UVf2_Col8, /*Strip*/false >;
	using Rectangle2DStrip	= RectangleBatch< VB_Position_f2, VB_UVf2_Col8, /*Strip*/true >;
	using NinePatch2D		= NinePatchBatch< VB_Position_f2, VB_UVf2_Col8 >;
	using Circle2D			= CircleBatch< VB_Position_f2, VB_UVf2_Col8, /*Fill*/false >;
	using FilledCircle2D	= CircleBatch< VB_Position_f2, VB_UVf2_Col8, /*Fill*/true >;


} // AE::Graphics
