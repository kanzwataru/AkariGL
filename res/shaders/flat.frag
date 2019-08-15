#version 330 core
in vec3 normal;
out vec4 out_color;

uniform vec3 light_dir;

void main() {
    vec3 color = vec3(1.0f, 0.5f, 0.3f);
    vec3 ambient = vec3(0.1f, 0.0f, 0.12f);
    float lambert = clamp(dot(normalize(light_dir), normal), 0.0f, 1.0f);

    out_color = vec4((color * lambert) + ambient, 1.0f);
}
