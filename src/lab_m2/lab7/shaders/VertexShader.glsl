#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
// TODO (student): Add the new attributes neccessary for skinning
layout(location = 3) in ivec4 v_bone_ids;
layout(location = 4) in vec4 v_bone_weights;


const int MAX_BONES = 200;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
// TODO (student): Declare a new uniform variable array, which will
// receive the bone transformations

uniform int boneCount;
uniform mat4 bones[MAX_BONES];

// Output
out vec2 texture_coord;
// TODO (student): Send the normal to the fragment shader
out vec3 normal;


void main()
{
    // TODO (student): Compute the final bone transformation
    mat4 boneTransform =
        v_bone_weights.x * bones[v_bone_ids.x] +
        v_bone_weights.y * bones[v_bone_ids.y] +
        v_bone_weights.z * bones[v_bone_ids.z] +
        v_bone_weights.w * bones[v_bone_ids.w];

    vec4 skinnedPosition = boneTransform * vec4(v_position, 1.0);
    vec3 skinnedNormal = mat3(boneTransform) * v_normal;


    texture_coord = v_texture_coord;
    // TODO (student): Compute the normal
    texture_coord = v_texture_coord;
    normal = normalize(mat3(Model) * skinnedNormal);

    // TODO (student): Apply the bone transformation on the vertex position
    gl_Position = Projection * View * Model * skinnedPosition;
}
