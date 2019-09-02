#version 330 core
in vec3 normal;
out vec4 out_color;

uniform vec3 light_dir;
uniform vec3 color;
uniform vec3 ambient;

void main() {
    float lambert = clamp(dot(normalize(light_dir), normal), 0.0f, 1.0f);

    out_color = vec4(mix(color, ambient, step(lambert, 0.3)), 0);
}

