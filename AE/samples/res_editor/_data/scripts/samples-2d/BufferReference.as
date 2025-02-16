// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Build binary tree using buffer reference extension.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PASS1
#	define PASS2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		const uint	st_size		= 48 * 128;
		RC<Buffer>	storage		= Buffer( st_size );
		RC<Buffer>	cbuffer		= Buffer();

		cbuffer.ULong(	"root",		storage.DeviceAddress() );	cbuffer.AddReference( storage );
		cbuffer.ULong(	"maxSize",	st_size );
		cbuffer.Uint(	"count",	0 );


		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "PASS1" );
			pass.ArgInOut(	"un_CBuffer",	cbuffer );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "PASS2" );
			pass.ArgIn(		"un_CBuffer",	cbuffer );
			pass.ArgOut(	"un_OutImage",	rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE

	// forward declaration
	layout(buffer_reference) buffer Node;

	// binary tree node
	layout(std430, buffer_reference) buffer Node
	{
		float4	rect;
		float4	color;
		Node	left;
		Node	right;
	};

	const uint	Node_size	= uint(ulong(Node(ulong(0)) + 1));	// 16*2 + 8*2;
	const uint	MaxDepth	= 16;

#endif
//-----------------------------------------------------------------------------
#ifdef PASS1
	#include "Hash.glsl"
	#include "Geometry.glsl"

	void  Main ()
	{
		if ( un_CBuffer.count == 0 )
		{
			// initialize first node
			un_CBuffer.count = 1;

			Node	root = Node(un_CBuffer.root);

			root.rect	= float4( 0.f, 0.f, 1.f, 1.f );
			root.color	= float4( 0.f, 0.f, 0.f, 1.f );
			root.left	= Node(ulong(0));
			root.right	= Node(ulong(0));
		}
		else
		if ( un_CBuffer.count < (un_CBuffer.maxSize / Node_size) )
		{
			// allocate new node
			Node		root	= Node(un_CBuffer.root);
			const uint	pos		= un_CBuffer.count++;
			Node		node	= root[pos];

			const uint	seed	= MHash11( pos ^ un_PerPass.frame) % 10000;
			float2		axis	= float2( 1.f );

			// find empty node
			for (uint i = 0, p = pos; i < MaxDepth; ++i)
			{
				const uint	idx		= p & 1;
				Node		next	= idx == 0 ? root.left : root.right;

				if ( ulong(next) == 0 )
				{
					if ( idx == 0 )	{ root.left  = node;  axis = float2(0.f, 1.f); }
					else			{ root.right = node;  axis = float2(1.f, 0.f); }
					break;
				}
				else
					root = next;

				p >>= 1;
			}

			float2	size	= Rect_Size( root.rect );
			float2	center	= Rect_Center( root.rect ) + ToSNorm( axis * DHash11( float(seed) * 0.018247f )) * (0.25f * size);
					size	= Max( size, float2(0.001f) ) * 0.4f;

			node.rect	= float4( center, center + size );
			node.color	= DHash41( float(seed) * 0.02745f + 1.5442f );
			node.left	= Node(ulong(0));
			node.right	= Node(ulong(0));
		}
		else
		{
			// restart
			un_CBuffer.count = 0;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS2
	#include "InvocationID.glsl"
	#include "Geometry.glsl"

	void  Main ()
	{
		Node	node	= Node(un_CBuffer.root);
		float2	uv		= GetGlobalCoordUNorm().xy;
		float4	col		= float4(0.f);

		// search in binary tree
		if ( IsInsideRect( uv, node.rect ))
		{
			for (uint i = 0; i < MaxDepth; ++i)
			{
				col = node.color;

				if ( ulong(node.left) != 0 and IsInsideRect( uv, node.left.rect ))
					node = node.left;
				else
				if ( ulong(node.right) != 0 and IsInsideRect( uv, node.right.rect ))
					node = node.right;
				else
					break;
			}
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
