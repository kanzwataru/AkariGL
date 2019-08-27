#version 330 core
in vec4 vert_col;

out vec4 out_color;

void main() {
    out_color = vec4(0.6, 0.4, 0.5, 1.0) * vert_col;
}
