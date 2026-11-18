#version 330

#define MAX_LIGHTS 30

in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

struct light_t {
	int spotlight;
	vec3 position;
	vec3 color;
	vec3 direction;
	float cut_off_angle;
    float intensity;
    int cookie;
};

uniform sampler2D texture;
uniform vec3 eye_position;

uniform float material_ke;
uniform float material_ka;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 color;
uniform light_t lights[MAX_LIGHTS];
uniform int num_lights;

uniform sampler2D cookie_0;
uniform sampler2D cookie_1;
uniform sampler2D cookie_2;
uniform sampler2D cookie_3;
uniform sampler2D cookie_4;


layout(location = 0) out vec4 out_color;

void main()
{
    // ------ CONSTANTS -------------
    const float kc = 1;
    const float kl = 0.09;
    const float kq = 0.032;
    float attenuation_factor = 0;
    const float EPS = 1e-5f;
    // ------------------------------

    vec3 emmissive_light = vec3(material_ke) * color;
    vec3 ambient_light = vec3(material_ka);
    vec3 diffuse_light = vec3(0);
    vec3 specular_light = vec3(0);

    vec3 V = normalize(eye_position - world_position);
    vec3 N = world_normal;

    for (int i = 0; i < num_lights; i++) {
        light_t light = lights[i];

        vec3 L = normalize(light.position - world_position);
        vec3 H = normalize(L + V);

        float spot_light = dot(-L, light.direction);
        float spot_light_limit = cos(light.cut_off_angle);
        if (light.spotlight == 1) {
            if (spot_light > spot_light_limit)
            {
                float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
                attenuation_factor = pow(linear_att, 2);
            } else {
                continue;
            }

            vec3 z_base = normalize(light.direction);
            vec3 a;

            if (abs(dot(z_base, vec3(0, 1, 0))) < 1.0f - EPS)
                a = vec3(0, 1, 0);
            else
                a = vec3(1, 0, 0);

            vec3 x_base = normalize(cross(a, z_base));
            vec3 y_base = cross(z_base, x_base);

            vec3 v = normalize(light.position - world_position);

            float x = dot(v, x_base);
            float y = dot(v, y_base);
            float z = dot(v, z_base);

            vec2 p = vec2(x, y) / z;
            vec2 n = p / tan(light.cut_off_angle);
            vec2 t = n + vec2(1);
            t /= 2.0;

            if (light.cookie >= 0 && light.cookie <=4 ) {
                vec4 c;

                if (light.cookie == 0)
                    c = texture2D(cookie_0, t);
                else if (light.cookie == 1)
                    c = texture2D(cookie_1, t);
                else if (light.cookie == 2)
                    c = texture2D(cookie_2, t);
                else if (light.cookie == 3)
                    c = texture2D(cookie_3, t);
                else
                    c = texture2D(cookie_4, t);

                if (c.a <= 0.5)
                    discard;

                diffuse_light += material_kd * attenuation_factor * light.intensity * c.rgb * max(dot(N,L), 0);

                if (dot(N,L) > 0) {
                    vec3 add = material_ks * attenuation_factor * light.intensity * c.rgb * abs(pow(max(dot(N, H), 0), material_shininess));
                    if (add.r > 0 || add.g > 0 || add.b > 0)
                            specular_light += add;
                }

                continue;
            }

            diffuse_light += material_kd * attenuation_factor * light.intensity * light.color * max(dot(N,L), 0);

            if (dot(N,L) > 0) {
                vec3 add = material_ks * attenuation_factor * light.intensity * light.color * abs(pow(max(dot(N, H), 0), material_shininess));
                if (add.r > 0 || add.g > 0 || add.b > 0)
                        specular_light += add;
            }

            continue;
        }

        float distance = length(light.position - world_position);
        attenuation_factor = 1.0 / (kc + kl * distance + kq * distance * distance);

        diffuse_light += material_kd * attenuation_factor * light.intensity * light.color * max(dot(N,L), 0);

        if (dot(N,L) > 0) {
            vec3 add = material_ks * attenuation_factor * light.intensity * light.color * abs(pow(max(dot(N, H), 0), material_shininess));
            if (add.r > 0 || add.g > 0 || add.b > 0)
                    specular_light += add;
        }
    }

    vec3 light_sum = emmissive_light + ambient_light + diffuse_light + specular_light;

    vec4 texture_color = texture2D(texture, texcoord);
    texture_color.rgb *= light_sum;

    float distance = length(eye_position - world_position);
    float density = 0.1;

    float fog_factor = 1.0 - exp(-density * density * distance * distance);
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    vec4 fog_color = vec4(0.765, 0.839, 0.827, 1.0);

    out_color = mix(texture_color, fog_color, fog_factor);

    if (out_color.a <= 0.5)
        discard;
}
