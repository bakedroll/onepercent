
void main(void)
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, gl_TexCoord[0].x);//smoothstep(0.0, 1.0, gl_TexCoord[0].x));
}
