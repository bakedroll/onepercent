#define MAX_LIGHTS 1

varying vec3 V;
varying vec3 E;
varying vec3 lightVec[MAX_LIGHTS];

uniform sampler2D colormap;
uniform sampler2D speccitiescloudsmap;
uniform sampler2D nightmap;
uniform sampler2D normalmap;
uniform sampler2D countriesmap;

void main (void) 
{ 
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 speccitiescloudsmap_color = texture2D(speccitiescloudsmap, gl_TexCoord[0].st);

	vec3 normal = 2.0 * texture2D(normalmap, gl_TexCoord[0].st).rgb - 1.0;
	//vec4 colormap_color = clamp(texture2D(colormap, gl_TexCoord[0].st) + boundaries_color, 0.0, 1.0);
	//vec4 nightmap_color = clamp(texture2D(nightmap, gl_TexCoord[0].st) + boundaries_color, 0.0, 1.0) + specreliefcitiesboundariesmap_color.b;
	vec4 colormap_color = clamp(texture2D(colormap, gl_TexCoord[0].st), 0.0, 1.0);
	vec4 nightmap_color = clamp(texture2D(nightmap, gl_TexCoord[0].st), 0.0, 1.0) + speccitiescloudsmap_color.g;

	normal = normalize(normal);

	for (int i=0;i<MAX_LIGHTS;i++)
	{
		vec3 R = normalize(-reflect(lightVec[i], normal));

		float ndotl, edge;

		// AMBIENT
		vec4 Iamb = gl_FrontLightProduct[i].ambient;

		// DIFFUSE
		ndotl = dot(lightVec[i], normal);

		ndotl = clamp(ndotl, -1.0, 1.0);
		edge = clamp(ndotl * 3.0, -1.0, 1.0) * 0.5 + 0.5;

		// vec4 Idiff = colormap_color * edge  + nightmap_color * (1.0 - edge);
		vec4 Idiff = colormap_color * edge  +  nightmap_color * (1.0 - edge); //+ boundaries_color;

		// SPECULAR
		vec4 Ispec = gl_FrontLightProduct[i].specular 
			* pow(max(dot(R, -E), 0.0), 0.3 * gl_FrontMaterial.shininess)
			* (speccitiescloudsmap_color.r * 0.8 + 0.2);
			//* specmap_color;

		Ispec = clamp(Ispec, 0.0, 1.0); 

		finalColor += Iamb + Idiff + Ispec;
	}
   

	gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission;
}