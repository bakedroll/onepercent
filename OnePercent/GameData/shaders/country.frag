uniform sampler2D distancemap;

uniform float alpha;
uniform vec3 color;

void main (void) 
{
  float distance = texture2D(distancemap, gl_TexCoord[0].st).r;
  float intensity = distance * 0.5 + 0.5;
  
  gl_FragColor = vec4(color * intensity, alpha);


  //float val = clamp(snoise(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0);
  //float val = clamp(cnoise(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0);
  
  ////float val = clamp(cellular(gl_TexCoord[1].xyz * 100.0), 0.0, 1.0).x;

  //gl_FragColor = vec4(vec3(val), 1.0);

  ////gl_FragColor = vec4(val);
}
