#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float height;
uniform float frequency;
uniform float phase;
uniform float time;

out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    vec3 new_position = v_position;

    float t = v_position.y * height + height / 2;
    float wave = sin(time * frequency + t * .5 + phase);
    float offset = wave * t;
    new_position.x += offset;

    texcoord = vec2(
        v_texture_coord.x,
        v_texture_coord.y * height
    );

    world_position = (Model * vec4(v_position,1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

    gl_Position = Projection * View * Model * vec4(new_position, 1.0);
}
