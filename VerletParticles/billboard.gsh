
layout(points)           in;
layout(triangle_strip)   out;
layout(max_vertices = 4) out;

uniform mat4  u_MVP;
uniform vec3  u_Right;
uniform vec3  u_Up;
uniform float u_BillboardSize;

void simpleBillboard()
{
    vec3 pos     = gl_in[0].gl_Position.xyz;
    vec3 up      = u_Up*u_BillboardSize;
    vec3 right   = u_Right*u_BillboardSize;

    pos         -= right;
    gl_Position  = u_MVP*vec4(pos, 1.0);
    EmitVertex();

    pos         += up;
    gl_Position  = u_MVP*vec4(pos, 1.0);
    EmitVertex();

    pos         -= up;
    pos         += right;
    gl_Position  = u_MVP*vec4(pos, 1.0);
    EmitVertex();

    pos         += up;
    gl_Position  = u_MVP*vec4(pos, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    simpleBillboard();
}
