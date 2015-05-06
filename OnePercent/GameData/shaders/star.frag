varying float size;

void main (void) 
{ 
	float sized2 = size / 2.0;

	vec2 coord_c = gl_TexCoord[0].st - vec2(0.5, 0.5);

	float s2 = coord_c.s * coord_c.s;
	float t2 = coord_c.t * coord_c.t;

	float ht2 = t2 * 12.0;
	float vs2 = s2 * 12.0;

	float maxVal = sqrt(2.0 * sized2 * sized2);

	float distance_c = clamp(1.0 - (sqrt(s2 + t2) / maxVal), 0.0, 1.0);
	float distance_h = clamp(1.0 - (sqrt(s2 + ht2) / maxVal), 0.0, 1.0);
	float distance_v = clamp(1.0 - (sqrt(vs2 + t2) / maxVal), 0.0, 1.0);

	distance_c = pow(distance_c, 3.0);
	//distance_c = sqrt(1.0 - (distance_c * distance_c - 2.0 * distance_c + 1.0));
	distance_h = pow(distance_h, 2.0);
	distance_v = pow(distance_v, 2.0);

	vec3 color_c = vec3(0.6, 0.6, 0.6) * size;
	vec3 color_vh = vec3(0.4, 0.6 * size, 0.6 * size) * size;

	vec3 result = color_c * distance_c + color_vh * distance_h + color_vh * distance_v;
	result *= 0.8;

	gl_FragColor = vec4(result, 1.0);
}