#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float height;

out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    float scale = 1 - (v_position.y * height);

    vec3 new_position = v_position;
    new_position.x *= scale;
    new_position.z *= scale;

    
    texcoord = vec2(
        v_texture_coord.x * scale,
        v_position.y * height
    );

    world_position = (Model * vec4(v_position,1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

    gl_Position = Projection * View * Model * vec4(new_position, 1.0);
}
