#version 460

const vec2 OFFSETS[6] = vec2[](
vec2(-1.0, -1.0),
vec2(-1.0, 1.0),
vec2(1.0, -1.0),
vec2(1.0, -1.0),
vec2(-1.0, 1.0),
vec2(1.0, 1.0)
);

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(location = 0) out vec2 fragOffset;

layout(push_constant) uniform Push {
    vec4 position;
    vec4 color;
    float radius;
} push;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;

    vec4 ambientLightColor; // w is intensity
    PointLight pointLights[10];
    int numLights;
} ubo;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];

    vec4 lightInCamSpace = ubo.view * vec4(push.position.xyz, 1.0);
    vec4 positionInCamSpace = lightInCamSpace + push.radius * vec4(fragOffset, 0.0f, 0.0f);
    gl_Position = ubo.projection * positionInCamSpace;

}