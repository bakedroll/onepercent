void main(void)
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, gl_TexCoord[0].x);
	//gl_FragColor = vec4(1.0, 1.0, 1.0, smoothstep(0.0, 0.5, gl_TexCoord[0].x)));
}
