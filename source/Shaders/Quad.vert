#version 450

const vec3 QUAD[6] = {
    vec3(-1.0, -1.0,  0.0),
    vec3(-1.0,  1.0,  0.0),
    vec3( 1.0, -1.0,  0.0),
    vec3(-1.0,  1.0,  0.0),
    vec3( 1.0,  1.0,  0.0),
    vec3( 1.0, -1.0,  0.0)
};

layout(location = 3) out vec2 fragUV;

void main() {
    vec3 fragPos = QUAD[gl_VertexIndex];
    fragUV = (fragPos*0.5 + 0.5).xy;
    gl_Position = vec4(fragPos, 1.0);
}