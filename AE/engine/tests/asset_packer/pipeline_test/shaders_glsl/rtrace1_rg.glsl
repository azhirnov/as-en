
layout(location=0) rayPayloadEXT vec4  payload;

void Main ()
{
	const vec2 uv			= vec2(gl_LaunchIDEXT.xy + 0.5) / vec2(gl_LaunchSizeEXT.xy);
	const vec3 origin		= vec3(uv.x, 1.0 - uv.y, -1.0);
	const vec3 direction	= vec3(0.0, 0.0, 1.0);

	payload = vec4(0.0);
	traceRayEXT( un_TLAS,				// acceleration structure
				 gl_RayFlagsNoneEXT,	// rayFlags
				 0xFF,					// cullMask
				 0,						// sbtRecordOffset
				 0,						// sbtRecordStride
				 0,						// missIndex
				 origin,				// ray origin
				 0.01,					// ray min range
				 direction,				// ray direction
				 10.0,					// ray max range
				 0 );					// payload location

	imageStore( un_OutImage, ivec2(gl_LaunchIDEXT), payload );
}
