varying vec3 pixel_nor;
varying vec4 pixel_pos;
varying vec4 pixel_scr;

varying vec3 p_r;
uniform vec3 planet_r;

void main(void)
{
    p_r.x=1.0/sqrt(planet_r.x);
    p_r.y=1.0/sqrt(planet_r.y);
    p_r.z=1.0/sqrt(planet_r.z);
    pixel_nor=gl_Normal;
    pixel_pos=gl_Vertex;
    pixel_scr=gl_Color;
    gl_Position=gl_Color;
}