#define MAX_LIGHTS 1

varying vec3 V;
varying vec3 E;
varying vec3 lightVec[MAX_LIGHTS];

attribute vec3 tangent;
attribute vec3 binormal;

void main(void)  
{ 
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	vec3 T = normalize(gl_NormalMatrix * tangent);
	vec3 B = normalize(gl_NormalMatrix * binormal);

	V = vec3(gl_ModelViewMatrix * gl_Vertex);

	for (int i=0; i<MAX_LIGHTS; i++)
	{
		vec3 lightDir;
		if (gl_LightSource[i].position.w == 1.0)
		{
			lightDir = normalize(gl_LightSource[i].position.xyz - V);
		}
		else
		{
			lightDir = normalize(gl_LightSource[i].position.xyz);
		}

		vec3 v;
		v.x = dot(lightDir, T);
		v.y = dot(lightDir, B);
		v.z = dot(lightDir, N);
		lightVec[i] = normalize(v);

		v.x = dot(V, T);
		v.y = dot(V, B);
		v.z = dot(V, N);
		E = normalize(v);
	}

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
