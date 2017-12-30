uniform sampler2D distancemap;

void main (void) 
{
  float distance;
  float alpha;

  distance = texture2D(distancemap, gl_TexCoord[0].st).r;
  alpha = smoothstep(0.7, 0.95, distance);
  
	gl_FragColor = vec4(vec3(0.9, 0.9, 1.0), alpha * 0.7);
}
