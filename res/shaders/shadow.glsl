#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec4 world_pos[];
in vec3 normal[];
out vec4 vert_col;

uniform vec3 light_dir;
uniform mat4 view;
uniform mat4 proj;

#define EXTRUDE_LENGTH 30
//#define USE_VISUALIZATION

void emit_quadface(vec4 top_a, vec4 top_b, vec3 btm_a, vec3 btm_b)
{
    gl_Position = proj * view * vec4(top_b);
    EmitVertex();
    gl_Position = proj * view * vec4(btm_a, 1.0);
    EmitVertex();
    gl_Position = proj * view * vec4(top_a);
    EmitVertex();
    gl_Position = proj * view * vec4(btm_b, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    vec3 face_normal = (normal[0] + normal[1] + normal[2]) / 3; // average
    float angle_to_light = dot(face_normal, light_dir);

    if(angle_to_light < 0) {
        // start cap
        vert_col = vec4(0.0, 0.0, 1.0, 1.0);
        gl_Position = proj * view * world_pos[2];
        EmitVertex();
        gl_Position = proj * view * world_pos[1];
        EmitVertex();
        gl_Position = proj * view * world_pos[0];
        EmitVertex();
        EndPrimitive();

        // end cap
        vec3 ec0 = world_pos[0].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);
        vec3 ec1 = world_pos[1].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);
        vec3 ec2 = world_pos[2].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);

        vert_col = vec4(0.0, 1.0, 0.0, 1.0);
        gl_Position = proj * view * vec4(ec0, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(ec1, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(ec2, 1.0);
        EmitVertex();
        EndPrimitive();

        // volume sides
        vert_col = vec4(1.0, 0.0, 0.0, 1.0);
        emit_quadface(world_pos[1], world_pos[2], ec2, ec1);
        emit_quadface(world_pos[2], world_pos[0], ec0, ec2);
        emit_quadface(world_pos[0], world_pos[1], ec1, ec0);
    }
#ifdef USE_VISUALIZATION
    else {
        // for visualization only
        vert_col = vec4(1.0, 1.0, 1.0, 1.0);
        gl_Position = proj * view * world_pos[0];
        EmitVertex();
        gl_Position = proj * view * world_pos[1];
        EmitVertex();
        gl_Position = proj * view * world_pos[2];
        EmitVertex();
        EndPrimitive();
    }
#endif
}
