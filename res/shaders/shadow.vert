#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec4 world_pos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    world_pos = model * vec4(in_position, 1.0);
    gl_Position = proj * view * world_pos;
    normal = mat3(transpose(inverse(model))) * in_normal;
}

/*
out vec3 out_pos;
out vec4 vert_col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 light_dir;

void main() {
    vec4 world_pos = model * vec4(in_position, 1.0);
    vec3 normal = mat3(transpose(inverse(model))) * in_normal;

    float lambert = dot(normalize(light_dir), normalize(normal));
    float facing = step(lambert, 0);

    vert_col = (1 - facing) * vec4(1,1,1,1);

    if(lambert < 0) {
        gl_Position = proj * view * vec4(world_pos.xyz + (vec3(1,1,1) * -light_dir), 1.0);
    }
    else {
        gl_Position = proj * view * world_pos;
    }
};
*/
