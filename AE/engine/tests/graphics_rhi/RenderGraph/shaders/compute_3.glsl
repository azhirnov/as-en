// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

//layout(set=0, binding=0, rgba8) writeonly uniform image2D  un_OutImage;

void dbg_Assert (uint b) {}

#define ASSERT( _expr_ )	\
	if ( !(_expr_) ){		\
		dbg_Assert(0);		\
	}

vec4 Hash (vec2 uv)
{
	vec4 p = fract( uv.xyyx * vec4(9233.4654, 4736.323, 7.356, 843.435) );
	p += dot( p, p.wzxy + 423.876 );
	return fract( (p.yzxx * p.xzwy) + p.zyzw );
}

void Main ()
{
	vec2 uv		= vec2(gl_GlobalInvocationID.xy) / vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
	vec4 color	= Hash( uv );

	ASSERT( !isnan( color.r ));
	ASSERT( color.g >= 0.0 );
	ASSERT( color.b <= 1.0 );
	ASSERT( color.a < 0.5 );
	ASSERT( false );

	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );
}
