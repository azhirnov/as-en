// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

Random	_rnd;

ND_ float	Rnd ()	{ return _rnd.Uniform( 0.f, 1.f ); }
ND_ float3  Rnd3 ()	{ return float3(Rnd(), Rnd(), Rnd()); }


ND_ array<float2x4>  GenColoredSpheresDrawTasks ()
{
	int3			ipos	 (0);
	const int3		grid_dim (8);
	array<float2x4>	draw_tasks;

	for (ipos.z = 0; ipos.z < grid_dim.z; ++ipos.z)
	for (ipos.y = 0; ipos.y < grid_dim.y; ++ipos.y)
	for (ipos.x = 0; ipos.x < grid_dim.x; ++ipos.x)
	{
		int		idx		= VecToLinear( ipos, grid_dim );
		float	scale1	= 0.2f;
		float	scale2	= 2.5f;
		float3	pos		= (float3(ipos - grid_dim / 2) * scale2 + ToSNorm(Rnd3())) * scale1;
		float	size	= Remap( 0.f, 1.f, 0.25f, 1.f, Rnd() ) * scale1;				// sphere size
		float4	color	= float4(Rainbow( float(idx) / Area(grid_dim) ));	color.w = 0.5;

		draw_tasks.push_back( float2x4( float4(pos, size), color ));
	}
	return draw_tasks;
}
