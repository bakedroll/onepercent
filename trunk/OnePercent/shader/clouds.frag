varying vec3 N;
varying vec3 v;

uniform sampler2D colormap;

#define MAX_LIGHTS 1

void main (void) 
{ 
	vec4 colormap_color = texture2D(colormap, gl_TexCoord[0].st);

	vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);

	for (int i=0; i<MAX_LIGHTS; i++)
	{
		vec3 L;
	  
		if (gl_LightSource[i].position.w == 1.0)
		{
			L = normalize(gl_LightSource[i].position.xyz - v);
		}
		else
		{
			L = normalize(gl_LightSource[i].position.xyz);
		}

		vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0) 
		vec3 R = normalize(-reflect(L,N));
   
		//calculate Ambient Term: 
		vec4 Iamb = gl_FrontLightProduct[i].ambient;

		//calculate Diffuse Term: 
		vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(dot(N,L), 0.0) * colormap_color;
		Idiff = clamp(Idiff, 0.0, 1.0);

		// calculate Specular Term:
		vec4 Ispec = gl_FrontLightProduct[i].specular 
			* pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
		Ispec = clamp(Ispec, 0.0, 1.0);

		finalColor += Iamb + Idiff + Ispec;
	}
   
	// write Total Color:
	vec4 total = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission + gl_FrontMaterial.ambient;
	total.a = colormap_color.r * 0.7;

	gl_FragColor = total;
}