#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 16) out;

in vec4 world_pos[];
in vec3 normal[];
out vec4 vert_col;

uniform vec3 light_dir;
uniform mat4 view;
uniform mat4 proj;

#define EXTRUDE_LENGTH 50
//#define USE_VISUALIZATION

void main()
{
    vec3 face_normal = (normal[0] + normal[1] + normal[2]) / 3; // average
    float angle_to_light = dot(face_normal, light_dir);

    if(angle_to_light < 0) {
        // start cap
        vert_col = vec4(0.0, 0.0, 0.0, 1.0);
        gl_Position = proj * view * world_pos[0];
        EmitVertex();
        gl_Position = proj * view * world_pos[1];
        EmitVertex();
        gl_Position = proj * view * world_pos[2];
        EmitVertex();
        EndPrimitive();

        // end cap
        vec3 ec0 = world_pos[0].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);
        vec3 ec1 = world_pos[1].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);
        vec3 ec2 = world_pos[2].xyz + (vec3(EXTRUDE_LENGTH) * -light_dir);

        vert_col = vec4(0.0, 0.0, 0.0, 1.0);
        gl_Position = proj * view * vec4(ec0, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(ec1, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(ec2, 1.0);
        EmitVertex();
        EndPrimitive();

        // volume sides
        vert_col = vec4(1.0, 0.0, 0.0, 1.0);
        gl_Position = proj * view * vec4(world_pos[0]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec0, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(world_pos[1]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec1, 1.0);
        EmitVertex();
        EndPrimitive();

        vert_col = vec4(1.0, 0.0, 0.0, 1.0);
        gl_Position = proj * view * vec4(world_pos[1]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec1, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(world_pos[2]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec2, 1.0);
        EmitVertex();
        EndPrimitive();

        vert_col = vec4(1.0, 0.0, 0.0, 1.0);
        gl_Position = proj * view * vec4(world_pos[2]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec2, 1.0);
        EmitVertex();
        gl_Position = proj * view * vec4(world_pos[0]);
        EmitVertex();
        gl_Position = proj * view * vec4(ec0, 1.0);
        EmitVertex();
        EndPrimitive();
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
