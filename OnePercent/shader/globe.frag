varying vec3 N;
varying vec3 v;

uniform sampler2D colormap;
uniform sampler2D specmap;
uniform sampler2D nightmap;
uniform sampler2D citymap;

#define MAX_LIGHTS 1

void main (void) 
{ 
   vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);
   
   for (int i=0;i<MAX_LIGHTS;i++)
   {
      vec3 L;
	  float ndotl, edge;
	  
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
	  ndotl = dot(N,L);

      //vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(ndotl, 0.0);
      //Idiff = clamp(Idiff, 0.0, 1.0);
   
	  ndotl = clamp(ndotl, -1.0, 1.0);
	  edge = (clamp(ndotl * 3.0, -1.0, 1.0) + 1.0) * 0.5;

      vec4 Idiff = texture2D(colormap, gl_TexCoord[0].st) * edge + (texture2D(nightmap, gl_TexCoord[0].st) + texture2D(citymap, gl_TexCoord[0].st)) * (1.0 - edge);

      // calculate Specular Term:
      vec4 Ispec = gl_FrontLightProduct[i].specular 
             * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess) * texture2D(specmap, gl_TexCoord[0].st);
      Ispec = clamp(Ispec, 0.0, 1.0); 

	  finalColor += Iamb + Idiff + Ispec;
   }
   
   // write Total Color: 
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor + gl_FrontMaterial.emission;
}