#version 330 core
layout (location = 0) in vec3 in_position;
out vec4 vert_col;

uniform mat4 model;

void main() {
   gl_Position = model * vec4(in_position, 1.0);
   vert_col = vec4(in_position, 1.0) * 0.5 + 0.5;
};
