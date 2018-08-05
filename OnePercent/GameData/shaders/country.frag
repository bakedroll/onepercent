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
    intensity = smoothstep(0.7, 0.95, distance) * hoverIntensity;
    
    gl_FragColor = vec4(color * (1.0 - intensity) + vec3(0.9, 0.9, 1.0) * intensity, max(alpha, intensity));
  }
  else
  {
    float intensity;
    intensity = distance * 0.5 + 0.5;
    
    gl_FragColor = vec4(color * intensity, alpha);
  }


  //float val = clamp(snoise(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0);
  //float val = clamp(cnoise(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0);
  
  ////float val = clamp(cellular(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0).x;

  //gl_FragColor = vec4(vec3(val), 1.0);

  ////gl_FragColor = vec4(val);
}
