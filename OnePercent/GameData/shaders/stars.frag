#define pi 3.141592653589793238462643383279

uniform sampler2D colormap_0x0;
uniform sampler2D colormap_1x0;

varying vec3 pixel_nor;

void main(void)
{
	vec2 polar;

	polar.x = (atan(pixel_nor.y, pixel_nor.x) + pi) / (2.0 * pi);
	float xyLen = sqrt(pow(pixel_nor.y, 2.0) + pow(pixel_nor.x, 2.0));
	polar.y = (atan(pixel_nor.z, xyLen) + pi / 2.0) / pi;

	vec4 color;

	if (polar.x > 0.5)
	{
		color = texture2D(colormap_1x0, vec2((polar.x - 0.5) * 2.0, polar.y));
	}
	else
	{
		color = texture2D(colormap_0x0, vec2(polar.x * 2.0, polar.y));
	}

	color *= 0.3;
	color.a = 1.0;
	gl_FragColor = color;
}