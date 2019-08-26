#version 330 core
in vec3 normal;
out vec4 out_color;

uniform vec3 light_dir;
uniform vec3 color;

void main() {
    vec3 ambient = vec3(0.1f, 0.05f, 0.15f);
    float lambert = clamp(dot(normalize(light_dir), normal), 0.0f, 1.0f);

    out_color = vec4((color * lambert) + ambient, 1.0f);
}
