#version 330 core
in vec4 vert_col;
in vec3 pixel_normal;

out vec4 out_color;

uniform vec3 light_dir;

void main() {
    if(gl_FrontFacing) {
        out_color = vec4(0.8, 0.8, 0.8, 1.0) * vert_col;
    } else {
        out_color = vec4(0.2, 0.2, 0.2, 1.0) * vert_col;
    }

    if(dot(light_dir, pixel_normal) > 0) {
        discard;
    }
}
