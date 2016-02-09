#define MAX_LIGHTS 1

varying vec3 V;
varying vec3 E;
varying vec3 lightVec[MAX_LIGHTS];

uniform sampler2D colormap;
uniform sampler2D specreliefcitiesboundariesmap;
uniform sampler2D nightmap;
uniform sampler2D normalmap;
uniform sampler2D countriesmap;

uniform int selected_country_id;

void main (void) 
{ 
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 specreliefcitiesboundariesmap_color = texture2D(specreliefcitiesboundariesmap, gl_TexCoord[0].st);
	float ocean_depth = clamp(1.0 - specreliefcitiesboundariesmap_color.r + specreliefcitiesboundariesmap_color.g + 0.7, 0.0, 1.0);
	//vec4 boundaries_color = vec4(specreliefcitiesboundariesmap_color.a);

	vec3 normal = 2.0 * texture2D (normalmap, gl_TexCoord[0].st).rgb - 1.0;
	//vec4 colormap_color = clamp(texture2D(colormap, gl_TexCoord[0].st) + boundaries_color, 0.0, 1.0);
	//vec4 nightmap_color = clamp(texture2D(nightmap, gl_TexCoord[0].st) + boundaries_color, 0.0, 1.0) + specreliefcitiesboundariesmap_color.b;
	vec4 colormap_color = clamp(texture2D(colormap, gl_TexCoord[0].st), 0.0, 1.0);
	vec4 nightmap_color = clamp(texture2D(nightmap, gl_TexCoord[0].st), 0.0, 1.0) + specreliefcitiesboundariesmap_color.b;


	// red overlay for selected country (smoothed)
	vec4 country_selected_color;
	if (selected_country_id == 255)
	{
		country_selected_color = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		country_selected_color = vec4(0.0, 0.0, 0.0, 0.0);

		for (int y = -2; y <= 2; y++)
		{
			for (int x = -2; x <= 2; x++)
			{
				vec2 coords = gl_TexCoord[0].st + vec2(x / 4096.0, y / 4096.0);

				if (int(texture2D(countriesmap, coords).r * 255.0) == selected_country_id)
				{
					country_selected_color += vec4(1.0, 0.5, 0.5, 1.0);
				}
				else
				{
					country_selected_color += vec4(1.0, 1.0, 1.0, 1.0);
				}
			}
		}
		country_selected_color /= 25.0;
	}


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

		// vec4 Idiff = ocean_depth * colormap_color * edge  +  ocean_depth * nightmap_color * (1.0 - edge) + boundaries_color;
		vec4 Idiff = ocean_depth * country_selected_color * (colormap_color * edge  +  nightmap_color * (1.0 - edge)); //+ boundaries_color;

		// SPECULAR
		vec4 Ispec = gl_FrontLightProduct[i].specular 
			* pow(max(dot(R, -E), 0.0), 0.3 * gl_FrontMaterial.shininess)
			* (specreliefcitiesboundariesmap_color.r * 0.8 + 0.2);
			//* specmap_color;

		Ispec = clamp(Ispec, 0.0, 1.0); 

		finalColor += Iamb + Idiff + Ispec;
	}
   

	gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission;
}