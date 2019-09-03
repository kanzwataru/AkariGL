#version 330 core
in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D col_buffer;
uniform sampler2D depth_buffer;
uniform sampler2D stencil_buffer;

void main()
{
    out_color = texture(col_buffer, tex_coord);
}
