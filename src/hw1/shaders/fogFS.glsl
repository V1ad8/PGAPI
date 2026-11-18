#version 330

in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

uniform sampler2D texture;
uniform vec3 eye_position;

layout(location = 0) out vec4 out_color;

void main()
{
    float distance = length(eye_position - world_position);
    float fog_factor = clamp(distance / 10, 0, 1);
    vec4 fog_color = vec4(0.765, 0.839, 0.827, 1);

    vec4 texture_color = texture(texture, texcoord);
    out_color = vec4(mix(texture_color, fog_color, fog_factor));

    if (out_color.a <= 0.5)
        discard;
}
