#version 330
#extension GL_EXT_geometry_shader4 : enable

layout (std140) uniform Matrices {
    mat4 m_pvm;
    mat4 m_viewModel;
    mat3 m_normal;
};

out vData
{
    vec3 normal;
} vertex;

void main()
{
    vertex.normal = normalize(gl_NormalMatrix * gl_Normal);
    //gl_Position = gl_ModelViewMatrix * gl_Vertex; // ftransform();
	gl_Position = ftransform();
	//gl_Position = gl_Vertex;
}