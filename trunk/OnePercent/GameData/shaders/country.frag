uniform sampler2D distancemap;

uniform int bHoverEnabled;
uniform float alpha;
uniform vec3 color;
uniform float hoverIntensity;

void main (void) 
{
  float distance;
  distance = texture2D(distancemap, gl_TexCoord[0].st).r;
  
  if (bHoverEnabled == 1)
  {
    float intensity;
    intensity = smoothstep(0.7, 0.95, distance) * 0.7 * hoverIntensity;
    
    gl_FragColor = vec4(color + vec3(0.9, 0.9, 1.0) * intensity, max(alpha, intensity));
  }
  else
  {
    float intensity;
    intensity = distance * 0.5 + 0.5;
    
    gl_FragColor = vec4(color * intensity, alpha);
  }


  float val = clamp(snoise(gl_TexCoord[1].str * 30.0), 0.0, 1.0);
  //float val = cellular(gl_TexCoord[1].str * 30.0).x;
  gl_FragColor = vec4(vec3(val), 1.0);
}
