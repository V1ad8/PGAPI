#version 330

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
layout(triangle_strip, max_vertices = 256) out;

// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
uniform int no_of_instances;
// TODO(student): Declare any other uniforms here
uniform int mode;

// Input
in int instance[2];


vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z *sin(u);
    float z = point.x * sin(u) + point.z *cos(u);
    return vec3(x, point.y, z);
}


vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}


// This function computes B(t) with 4 control points. For a visual example,
// see [1]. For an interactive Javascript example with the exact points in
// this code, see [2].
//
// [1] https://www.desmos.com/calculator/cahqdxeshd
// [2] https://jsfiddle.net/6yuv9htf/
vec3 bezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    return  p0 * pow((1 - t), 3) +
            p1 * 3 * t * pow((1 - t), 2) +
            p2 * 3 * pow(t, 2) * (1 - t) +
            p3 * pow(t, 3);
}

vec3 bezier(float t)
{
    return bezier(t, control_p0, control_p1, control_p2, control_p3);
}


// TODO(student): If you want to take things a step further, try drawing a
// Hermite spline. Hint: you can repurpose two of the control points. For a
// visual example, see [1]. For an interactive Javascript example with the
// exact points in this code, see [2].
// 
// Unlike the Javascript function, you MUST NOT call the Bezier function.
// There is another way to draw a Hermite spline, all you need is to find
// the formula.
//
// [1] https://www.desmos.com/calculator/5knm5tkr8m
// [2] https://jsfiddle.net/6yuv9htf/
vec3 hermite(float t, vec3 p0, vec3 p1, vec3 v0, vec3 v1)
{
    return bezier(
        t,
        p0,
        p0 + v0 / 4.0,
        p1 - v1 / 4.0,
        p1
    );
}

vec3 hermite(float t)
{
    return hermite(t, control_p0, control_p3, control_p1 - control_p0, control_p2 - control_p3);
}

void rotate()
{
    float instance_step = no_of_instances > 1 ? max_rotate / float(no_of_instances) : 0.0;
    float instance_offset = float(instance[0]) * instance_step;

    // TODO(student): Rather than emitting vertices for the control
    // points, you must emit vertices that approximate the curve itself.
    for (int i = 0; i < no_of_generated_points; i++)
    {
        float u = float(i) / float(no_of_generated_points - 1);
        vec3 point = bezier(u);

        gl_Position = Projection * View * vec4(rotateY(point, instance_offset), 1.0);
        EmitVertex();

        gl_Position = Projection * View * vec4(rotateY(point, instance_offset + instance_step), 1.0);
        EmitVertex();
    }

    EndPrimitive();
}

void translate()
{
    float instance_step = no_of_instances > 1 ? max_translate / float(no_of_instances) : 0.0;
    float instance_offset = float(instance[0]) * instance_step;

    // TODO(student): Rather than emitting vertices for the control
    // points, you must emit vertices that approximate the curve itself.
    for (int i = 0; i < no_of_generated_points; i++)
    {
        float u = float(i) / float(no_of_generated_points - 1);
        vec3 point = bezier(u);

        gl_Position = Projection * View * vec4(translateX(point, instance_offset), 1.0);
        EmitVertex();

        gl_Position = Projection * View * vec4(translateX(point, instance_offset + instance_step), 1.0);
        EmitVertex();
    }

    EndPrimitive();
}

void main()
{
    const int SURFACE_TYPE_ROTATION     = 0;
    const int SURFACE_TYPE_TRANSLATION  = 1;

    if (mode == SURFACE_TYPE_ROTATION)
        rotate();
    else if (mode == SURFACE_TYPE_TRANSLATION)
        translate();
}
