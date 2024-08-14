// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'


/*
=================================================
	HelperInvocationCount
=================================================
*/
ND_ uint  HelperInvocationCountPerQuad ()
{
	uint	helper = 0;
	#ifdef AE_demote_to_helper_invocation
		helper = gl.IsHelperInvocation() ? 1 : 0;
	#else
		helper = gl.HelperInvocation ? 1 : 0;
	#endif
	return	gl.quadGroup.Broadcast( helper, 0 ) +
			gl.quadGroup.Broadcast( helper, 1 ) +
			gl.quadGroup.Broadcast( helper, 2 ) +
			gl.quadGroup.Broadcast( helper, 3 );
}


ND_ uint  HelperInvocationCountPerWarp ()
{
	uint	helper = 0;
	#ifdef AE_demote_to_helper_invocation
		helper = gl.IsHelperInvocation() ? 1 : 0;
	#else
		helper = gl.HelperInvocation ? 1 : 0;
	#endif
	return gl.subgroup.InclusiveAdd( helper );
}
