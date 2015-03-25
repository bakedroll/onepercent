#define MAX_LIGHTS 1

attribute vec3 tangent;
attribute vec3 binormal;

varying vec3 lightVec[MAX_LIGHTS];
varying vec3 halfVec[MAX_LIGHTS];

void main(void)  
{ 
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	vec3 vertexPosition = vec3(gl_ModelViewMatrix *  gl_Vertex);

	vec3 t = normalize(gl_NormalMatrix * tangent);
	vec3 b = cross(N, t);

   for (int i=0;i<MAX_LIGHTS;i++)
   {
		vec3 lightDir;
		if (gl_LightSource[i].position.w == 1.0)
		{
			lightDir = normalize(gl_LightSource[i].position.xyz - vertexPosition);
		}
		else
		{
			lightDir = normalize(gl_LightSource[i].position.xyz);
		}
		
		// transform light and half angle vectors by tangent basis
		vec3 v;
		v.x = dot (lightDir, t);
		v.y = dot (lightDir, b);
		v.z = dot (lightDir, N);
		lightVec[i] = normalize (v);
	
	  
		v.x = dot (vertexPosition, t);
		v.y = dot (vertexPosition, b);
		v.z = dot (vertexPosition, N);

		vertexPosition = normalize(vertexPosition);
	
		/* Normalize the halfVector to pass it to the fragment shader */
		vec3 halfVector = normalize(vertexPosition + lightDir);
		v.x = dot (halfVector, t);
		v.y = dot (halfVector, b);
		v.z = dot (halfVector, N);

		// No need to normalize, t,b,n and halfVector are normal vectors.
		halfVec[i] = v ;
	}


   gl_Position = ftransform();
   gl_TexCoord[0] = gl_MultiTexCoord0;
}
