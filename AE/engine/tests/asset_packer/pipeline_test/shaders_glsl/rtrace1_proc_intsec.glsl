
hitAttributeEXT vec3  out_hitAttribs;

void Main ()
{
    const float radius = 0.5;
    const vec3  center = vec3(0.25, 0.5, 2.0); // must match with AABB center

    // ray sphere intersection
    vec3  oc = gl_WorldRayOriginEXT - center;
    float a  = dot(gl_WorldRayDirectionEXT, gl_WorldRayDirectionEXT);
    float b  = 2.0 * dot(oc, gl_WorldRayDirectionEXT);
    float c  = dot(oc, oc) - radius * radius;
    float d  = b * b - 4 * a * c;

    if ( d >= 0 )
    {
        float hitT = (-b - sqrt(d)) / (2.0 * a);
        out_hitAttribs = vec3(0.5);
        reportIntersectionEXT(hitT, 3);
    }
}
