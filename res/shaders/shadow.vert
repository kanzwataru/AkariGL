#verision 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 out_pos;

void main(void)
{
    out_pos = in_position;
}
