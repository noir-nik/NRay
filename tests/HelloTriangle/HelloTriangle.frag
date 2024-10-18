#version 460
#extension GL_ARB_separate_shader_objects : enable

// layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    // outColor = vec4(fragColor, 1.0);
	if(gl_FrontFacing)
		outColor = vec4(0.1686, 0.102, 0.5451, 1.0);
	else
		outColor = vec4(0.5451, 0.102, 0.1686, 1.0);
    // outColor = vec4(0.1686, 0.102, 0.5451, 1.0);
}