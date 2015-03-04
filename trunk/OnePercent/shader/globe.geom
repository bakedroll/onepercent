#version 330
#extension GL_EXT_geometry_shader4 : enable

layout (points) in;
layout (triangle_strip, max_vertices=32) out;

layout (std140) uniform Matrices
{
    mat4 projModelViewMatrix;
    mat3 normalMatrix;
};

in vData
{
    vec3 normal;
    vec4 color;
} vertices[];

out fData
{
    vec3 normal;
} frag;    

void main()
{
	frag.normal = vec3(0.0, 0.0, -1.0);

	for (int i=0; i<gl_PositionIn.length(); i++)
	{
		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(1.0, -1.0, 0.0, 1.0));
		EmitVertex();

		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(-1.0, -1.0, 0.0, 1.0));
		EmitVertex();

		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(1.0, 1.0, 0.0, 1.0));
		EmitVertex();

		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(-1.0, 1.0, 0.0, 1.0));
		EmitVertex();

		EndPrimitive();
	}
}