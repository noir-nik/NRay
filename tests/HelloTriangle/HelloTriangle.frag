#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
	vec3 color = pow(fragColor, vec3(1.0 / 2.2));
	// vec3 color = fragColor;
    outColor = vec4(color, 1.0);
}