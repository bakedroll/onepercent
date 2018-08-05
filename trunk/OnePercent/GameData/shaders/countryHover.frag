uniform sampler2D distancemap;

uniform float alpha;
uniform float hoverIntensity;

void main (void) 
{
  float distance = texture2D(distancemap, gl_TexCoord[0].st).r;
  float intensity = smoothstep(0.85, 1.0, distance) * hoverIntensity;
  
  gl_FragColor = vec4(vec3(0.9, 0.9, 1.0), intensity * 0.8);
}
