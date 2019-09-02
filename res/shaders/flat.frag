#version 330 core
in vec3 normal;
out vec4 out_color;

uniform vec3 ambient;
uniform vec3 color;

void main() {
    //out_color = vec4(mix(ambient, color, 0.1), 1.0);
    out_color = vec4(ambient, 1.0);
}
