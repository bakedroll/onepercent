varying vec3 N;
varying vec3 V;
varying vec3 T;
varying vec3 B;

attribute vec3 tangent;
attribute vec3 binormal;

void main(void)  
{ 
	N = normalize(gl_NormalMatrix * gl_Normal);
	V = vec3(gl_ModelViewMatrix *  gl_Vertex);

	T = normalize(gl_NormalMatrix * tangent);
	B = cross(N, T);

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
