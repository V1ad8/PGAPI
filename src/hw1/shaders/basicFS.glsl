#version 330

in vec2 texcoord;
uniform sampler2D texture;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(texture, texcoord);

    if (out_color.a <= 0.5)
        discard;
}
