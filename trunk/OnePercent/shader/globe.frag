#define MAX_LIGHTS 1

varying vec3 V;
varying vec3 E;
varying vec3 lightVec[MAX_LIGHTS];

uniform sampler2D colormap;
uniform sampler2D specmap;
uniform sampler2D nightmap;
uniform sampler2D citymap;
uniform sampler2D normalmap;
uniform sampler2D reliefmap;

void main (void) 
{ 
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 colormap_color = texture2D(colormap, gl_TexCoord[0].st);
	vec4 specmap_color = texture2D(specmap, gl_TexCoord[0].st);
	vec4 reliefmap_color = texture2D(reliefmap, gl_TexCoord[0].st);
	vec3 normal = 2.0 * texture2D (normalmap, gl_TexCoord[0].st).rgb - 1.0;
	vec4 nightmap_color = texture2D(nightmap, gl_TexCoord[0].st) + texture2D(citymap, gl_TexCoord[0].st);
   
   	normal = normalize(normal);

	//vec3 E = normalize(-V);

	for (int i=0;i<MAX_LIGHTS;i++)
	{
		vec3 R = normalize(-reflect(lightVec[i], normal));

		float ndotl, edge;

		//calculate Ambient Term: 
		vec4 Iamb = gl_FrontLightProduct[i].ambient;

		//calculate Diffuse Term:
		ndotl = dot(lightVec[i], normal);

		ndotl = clamp(ndotl, -1.0, 1.0);
		edge = (clamp(ndotl * 3.0, -1.0, 1.0) + 1.0) * 0.5;

		float ocean_depth = clamp(1.0 - specmap_color.r + reliefmap_color.r + 0.7, 0.0, 1.0);

		vec4 Idiff = ocean_depth * colormap_color * edge  +  ocean_depth * nightmap_color * (1.0 - edge);

		// calculate Specular Term:
		vec4 Ispec = gl_FrontLightProduct[i].specular 
			* pow(max(dot(R, -E),0.0), 0.3*gl_FrontMaterial.shininess) * (specmap_color * 0.7 + vec4(0.3, 0.3, 0.3, 0.0));

		Ispec = clamp(Ispec, 0.0, 1.0); 

		finalColor += Iamb + Idiff + Ispec;
	}
   
	gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission;
}