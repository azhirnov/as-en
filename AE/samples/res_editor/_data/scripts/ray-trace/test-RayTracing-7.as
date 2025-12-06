// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_RAY_GEN
#	include <glsl.h>
#	define CALL_0
#	define CALL_1
#	define CALL_2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );


		// render loop
		{
			RC<RayTracingPass>		pass = RayTracingPass();
			pass.ArgOut( "un_OutImage",		rt );
			pass.Slider( "iCallable",	0,	4 );
			pass.Dispatch( rt.Dimension() );

			pass.MaxCallableRecursion( 1 );

			// setup SBT
			pass.RayGen( RTShader("") );
			pass.Callable( CallableIndex(0), RTShader("", "CALL_0") );
			pass.Callable( CallableIndex(1), RTShader("", "CALL_1") );
			pass.Callable( CallableIndex(2), RTShader("", "CALL_2") );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------

#define CALL_LOC	0

//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "Math.glsl"

	layout(location=CALL_LOC) gl::CallableData float4  payload;

	void Main ()
	{
		payload = float4(0.0);

		gl.ExecuteCallable( iCallable % 3, CALL_LOC );

		gl.image.Store( un_OutImage, int2(gl.LaunchID), payload );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CALL_0
	layout(location=CALL_LOC) gl::CallableDataIn float4  payload;

	void Main ()
	{
		payload += float4(1.0, 0.0, 0.0, 1.0);

		if ( iCallable == 3 )
			gl.ExecuteCallable( 1, CALL_LOC );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CALL_1
	layout(location=CALL_LOC) gl::CallableDataIn float4  payload;

	void Main ()
	{
		payload += float4(0.0, 1.0, 0.0, 1.0);

		if ( iCallable == 4 )
			gl.ExecuteCallable( 2, CALL_LOC );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CALL_2
	layout(location=CALL_LOC) gl::CallableDataIn float4  payload;

	void Main ()
	{
		payload += float4(0.0, 0.0, 1.0, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
