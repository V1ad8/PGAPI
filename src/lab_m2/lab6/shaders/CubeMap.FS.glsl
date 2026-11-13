#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform int type;

uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;


vec3 myReflect()
{
    vec3 incident = normalize(world_position - camera_position);
    vec3 reflection_direction = reflect(incident, normalize(world_normal));
    return texture(texture_cubemap, reflection_direction).rgb;
}


vec3 myRefract(float refractive_index)
{
    vec3 incident = normalize(world_position - camera_position);
    vec3 refraction_direction = refract(incident, normalize(world_normal), 1.0 / refractive_index);
    return texture(texture_cubemap, refraction_direction).rgb;
}


void main()
{

    if (type == 0)
    {
        out_color = vec4(myReflect(), 0);
    }
    else
    {
        out_color = vec4(myRefract(1.33), 0);
    }
}
