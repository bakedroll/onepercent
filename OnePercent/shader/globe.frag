#define MAX_LIGHTS 1

varying vec3 lightVec[MAX_LIGHTS];
varying vec3 halfVec[MAX_LIGHTS];

uniform sampler2D colormap;
uniform sampler2D specmap;
uniform sampler2D nightmap;
uniform sampler2D citymap;
uniform sampler2D normalmap;

void main (void) 
{ 
	// lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
	vec3 normal = 2.0 * texture2D (normalmap, gl_TexCoord[0].st).rgb - 1.0;
	normal = normalize(normal);

   vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);
   
   for (int i=0;i<MAX_LIGHTS;i++)
   {
	  float ndotl, edge;

      //calculate Ambient Term: 
      vec4 Iamb = gl_FrontLightProduct[i].ambient;

      //calculate Diffuse Term:
	  ndotl = dot(lightVec[i], normal);

	  ndotl = clamp(ndotl, -1.0, 1.0);
	  edge = (clamp(ndotl * 3.0, -1.0, 1.0) + 1.0) * 0.5;

      vec4 Idiff = texture2D(colormap, gl_TexCoord[0].st) * edge + (texture2D(nightmap, gl_TexCoord[0].st) + texture2D(citymap, gl_TexCoord[0].st)) * (1.0 - edge);

      // calculate Specular Term:
      vec4 Ispec = gl_FrontLightProduct[i].specular 
             * pow(max(dot(halfVec[i], normal),0.0), 0.3*gl_FrontMaterial.shininess) * (texture2D(specmap, gl_TexCoord[0].st) * 0.5 + vec4(0.5, 0.5, 0.5, 0.0));

      Ispec = clamp(Ispec, 0.0, 1.0); 

	  finalColor += Iamb + Idiff + Ispec;
   }
   
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission;
}