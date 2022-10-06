
#define PAYLOAD_LOC	0

#ifdef SH_RAY_GEN
layout(location=PAYLOAD_LOC) rayPayloadEXT vec4  payload;

void main ()
{
	const vec2 uv		 = vec2(gl_LaunchIDEXT.xy) / vec2(gl_LaunchSizeEXT.xy - 1);
	const vec3 origin	 = vec3(uv.x, 1.0f - uv.y, -1.0f);
	const vec3 direction = vec3(0.0f, 0.0f, 1.0f);

	traceRayEXT( /*topLevel*/un_RtScene, /*rayFlags*/gl_RayFlagsNoneEXT, /*cullMask*/0xFF,
				 /*sbtRecordOffset*/0, /*sbtRecordStride*/0, /*missIndex*/0,
				 /*origin*/origin, /*Tmin*/0.0f, /*direction*/direction, /*Tmax*/10.0f,
				 /*payload*/0 );

	imageStore( un_OutImage, ivec2(gl_LaunchIDEXT), payload );
}
#endif


#ifdef SH_RAY_MISS
layout(location=PAYLOAD_LOC) rayPayloadInEXT vec4  payload;

void main ()
{
	payload = vec4(0.0f);
}
#endif


#ifdef SH_RAY_CHIT
layout(location=PAYLOAD_LOC) rayPayloadInEXT vec4  payload;

hitAttributeEXT vec2  hitAttribs;

void main ()
{
	const vec3 barycentrics = vec3(1.0f - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y);
	payload = vec4(barycentrics, 1.0);
}
#endif
