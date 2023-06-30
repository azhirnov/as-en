
layout(location=0) rayPayloadInEXT vec4  payload;

hitAttributeEXT vec2  hitAttribs;

void Main ()
{
    const vec3 barycentrics = vec3(1.0 - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y);
    if ( barycentrics.y > barycentrics.x )
        ignoreIntersectionEXT;
    else
        payload += vec4(barycentrics, 1.0) / 3.0;
}
