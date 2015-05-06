varying vec3 pixel_nor;

void main(void)
{
    pixel_nor=gl_Normal;
    gl_Position=gl_Color;
}