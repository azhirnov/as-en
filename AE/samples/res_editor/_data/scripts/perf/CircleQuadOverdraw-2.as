// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	const uint	vcount = 12;


	void  Topology0 (array<float2> &vertices, array<uint> &indices)
	{
		vertices.resize( 4 );
		vertices[0] = float2(-1.0, -1.0);
		vertices[1] = float2( 1.0, -1.0);
		vertices[2] = float2(-1.0,  1.0);
		vertices[3] = float2( 1.0,  1.0);

		indices.resize( 6 );
		indices[0] = 0;		indices[1] = 1;		indices[2] = 2;
		indices[3] = 2;		indices[4] = 1;		indices[5] = 3;
	}


	void  Topology1 (array<float2> &vertices, array<uint> &indices)
	{
		vertices.resize( vcount+1 );
		vertices[0] = float2(0.f);

		const float	a = 2.f * Pi() / float(vcount);
		for (uint i = 0; i < vcount; ++i)
		{
			float	s	= Sin( a * i );
			float	c	= Cos( a * i );
			vertices[i+1] = float2( s, c );
		}

		indices.resize( vcount*3 );
		for (uint i = 1; i < vcount; ++i)
		{
			indices[i*3+0] = 0;
			indices[i*3+1] = i+1;
			indices[i*3+2] = i;
		}

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = vcount;
	}


	void  Topology2 (array<float2> &vertices, array<uint> &indices)
	{
		vertices.resize( vcount );

		const float	a = 2.f * Pi() / float(vcount);
		for (uint i = 0; i < vcount; ++i)
		{
			float	s	= Sin( a * i );
			float	c	= Cos( a * i );
			vertices[i] = float2( c, s );
		}

		indices.resize( (vcount-2)*3 );

		indices[0] = 0;		indices[1] = 1;		indices[2] = vcount-1;

		for (uint i = 1; i < vcount-4;)
		{
			uint j = 1 + i/2;

			indices[i*3+0] = vcount-j;
			indices[i*3+1] = j;
			indices[i*3+2] = j+1;
			++i;

			indices[i*3+0] = vcount-j;
			indices[i*3+1] = j+1;
			indices[i*3+2] = vcount-1-j;
			++i;
		}
		{
			uint	i = (vcount-3) * 3;
			uint	v = vcount / 2;
			indices[i+0] = v;		indices[i+1] = v+1;		indices[i+2] = v-1;
		}
	}


	void  Topology3 (array<float2> &vertices, array<uint> &indices)
	{
		vertices.resize( vcount );

		const float	a = 2.f * Pi() / float(vcount);
		for (uint i = 0; i < vcount; ++i)
		{
			float	s	= Sin( a * i );
			float	c	= Cos( a * i );
			vertices[i] = float2( c, s );
		}

		indices.resize( 3 + vcount/2*3 + vcount/4*3 );
		{
			uint	v = vcount/3;
			indices[0] = 0;		indices[1] = v;		indices[2] = v*2;
		}
		for (uint i = 0; i < vcount/2; ++i)
		{
			uint	j = (i+1)*3;
			uint	v = i*2;

			indices[j+0] = v;
			indices[j+1] = v+1;
			indices[j+2] = v+2 >= vcount ? 0 : v+2;
		}
		for (uint i = 0; i < vcount/4; ++i)
		{
			uint	j = (i+1+vcount/2)*3;
			uint	v = i*4;

			indices[j+0] = v;
			indices[j+1] = v+2;
			indices[j+2] = v+4 >= vcount ? 0 : v+4;
		}
	}


	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		array<RC<Scene>>	scenes;
		RC<DynamicUInt>		shape		= DynamicUInt();
		RC<DynamicUInt>		po2_count	= DynamicUInt();
		RC<DynamicUInt>		draw_count	= DynamicUInt();
		RC<DynamicUInt>		inst_count	= po2_count.Exp2().Mul( 4*4 );

		for (uint i = 0; i < 4; ++i)
		{
			RC<Scene>				scene		= Scene();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				vbuf		= Buffer();
			array<float2>			vertices;
			array<uint>				indices;

			switch ( i ) {
				case 0 :	Topology0( vertices, indices );		break;
				case 1 :	Topology1( vertices, indices );		break;
				case 2 :	Topology2( vertices, indices );		break;
				case 3 :	Topology3( vertices, indices );		break;
			}

			vbuf.FloatArray( "vertices",	vertices );
			vbuf.UIntArray(  "indices",		indices );
			vbuf.LayoutName( "VBuffer" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( vbuf, "indices" );
			cmd.InstanceCount( inst_count );
			geometry.Draw( cmd );

			geometry.ArgIn( "un_VBuffer",	vbuf );

			scene.Add( geometry );

			scenes.push_back( scene );
		}

		Slider( shape,		"Shape",	0,	scenes.size()-1 );
		Slider( po2_count,	"Count",	4,	18 );
		Slider( draw_count,	"Repeat",	1,	10 );

		// render
		for (uint i = 0; i < scenes.size(); ++i)
		{
			RC<SceneGraphicsPass>	pass = scenes[i].AddGraphicsPass( "v"+i );
			pass.AddPipeline( "perf/CircleQuadOverdraw-2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/CircleQuadOverdraw-2.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Constant( "iMaxInstancePow2", po2_count );
			pass.EnableIfEqual( shape, i );
			pass.Repeat( draw_count );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
