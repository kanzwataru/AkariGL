#version 330 core
in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D col_buffer;
uniform sampler2D depth_buffer;

const float offset = 1.0 / 1280.0;

float get_depth(vec2 coords)
{
    float depth = texture(depth_buffer, coords).r;
    //depth = depth / 4;
    depth = pow(depth, 80);

    depth = pow(depth, 2.2);

    return depth;
}

void main()
{
    const vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
    );

    const float kernel[9] = float[](
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    );

    float samples[9];
    for(int i = 0; i < 9; i++) {
        samples[i] = get_depth(tex_coord + offsets[i]);
    }

    float laplacian = 0;
    for(int i = 0; i < 9; i++) {
        laplacian += samples[i] * kernel[i];
    }

    float line = step(pow(laplacian * 1, 2.0), 0.1f);

    vec4 scene_col = texture(col_buffer, tex_coord);

    //float result = laplacian;
    //float result = get_depth(tex_coord);
    vec4 col_result = mix(scene_col * 0.1, scene_col, line);

    //out_color = vec4(result, result, result, 1.0);
    out_color = col_result;
    //out_color = texture(col_buffer, tex_coord);
}

