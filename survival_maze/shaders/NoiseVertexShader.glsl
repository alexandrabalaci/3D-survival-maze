#version 330

layout(location = 0) in vec3 v_position;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float AnimationTime;
uniform vec3 object_color;

// output values to fragment shader
out vec3 color;
out vec3 position;

void main()
{
	position = v_position;
	color = object_color;

	float rand_pos = fract(sin(dot(v_position, vec3(10.321, 81.103, 13))) *9376.19837);

	float time = AnimationTime;
	time = abs(sin(time));

	vec3 pos = v_position * (1 + time * rand_pos * rand_pos * rand_pos);
	gl_Position	= Projection * View * Model*vec4(pos,1.0);
}