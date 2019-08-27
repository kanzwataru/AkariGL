#version 330 core
layout (triangles_adjacency) in; // six
layout (triangle_strip, max_vertices = 18) out;

in vec3 pos[]; // six

uniform vec3 light_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

float epsilon = 0.0001;

void emit_quad(vec3 start, vec3 end, mat4 mvp)
{
    vec3 light_dir = normalize(start - light_pos);
    gl_Position = mvp * vec4((start + light_dir * epsilon), 1.0);
    EmitVertex();

    gl_Position = mvp * vec4(light_dir, 0.0);
    EmitVertex();

    light_dir = normalize(end - light_dir);
    gl_Position = mvp * vec4((end + light_dir * epsilon), 1.0);
    EmitVertex();

    gl_Position = mvp * vec4(light_dir, 0.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    mat4 mvp = proj * view * model;

    vec3 WorldPos[6];
    for(int i = 0; i < 6; ++i) {
        WorldPos[i] = (model * vec4(pos[i], 1)).xyz;
    }

    vec3 e1 = WorldPos[2] - WorldPos[0];
    vec3 e2 = WorldPos[4] - WorldPos[0];
    vec3 e3 = WorldPos[1] - WorldPos[0];
    vec3 e4 = WorldPos[3] - WorldPos[2];
    vec3 e5 = WorldPos[4] - WorldPos[2];
    vec3 e6 = WorldPos[5] - WorldPos[0];

    vec3 normal = cross(e1,e2);
    vec3 light_dir = light_pos - WorldPos[0];
    if (dot(normal, light_dir) > 0) {
        normal = cross(e3,e1);

        if (dot(normal, light_dir) <= 0) {
            vec3 StartVertex = WorldPos[0];
            vec3 EndVertex = WorldPos[2];
            emit_quad(StartVertex, EndVertex, mvp);
        }

        normal = cross(e4,e5);
        light_dir = light_pos - WorldPos[2];

        if (dot(normal, light_dir) <= 0) {
            vec3 StartVertex = WorldPos[2];
            vec3 EndVertex = WorldPos[4];
            emit_quad(StartVertex, EndVertex, mvp);
        }

        normal = cross(e2,e6);
        light_dir = light_pos - WorldPos[4];

        if (dot(normal, light_dir) <= 0) {
            vec3 StartVertex = WorldPos[4];
            vec3 EndVertex = WorldPos[0];
            emit_quad(StartVertex, EndVertex, mvp);
        }

        // render the front cap
        light_dir = (normalize(pos[0] - light_pos));
        gl_Position = mvp * vec4((pos[0] + light_dir * epsilon), 1.0);
        EmitVertex();

        light_dir = (normalize(pos[2] - light_pos));
        gl_Position = mvp * vec4((pos[2] + light_dir * epsilon), 1.0);
        EmitVertex();

        light_dir = (normalize(pos[4] - light_pos));
        gl_Position = mvp * vec4((pos[4] + light_dir * epsilon), 1.0);
        EmitVertex();
        EndPrimitive();

        // render the back cap
        light_dir = pos[0] - light_pos;
        gl_Position = mvp * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = pos[4] - light_pos;
        gl_Position = mvp * vec4(light_dir, 0.0);
        EmitVertex();

        light_dir = pos[2] - light_pos;
        gl_Position = mvp * vec4(light_dir, 0.0);
        EmitVertex();
    }

}
