#version 460

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

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

const float PI = 3.1415926538;


void main() {
    float dist = sqrt(dot(fragOffset, fragOffset));
    if(dist > 1){
        discard;
    }

    float cosDist = 0.2f * (cos(dist * PI) + 1.0);
    outColor = vec4(push.color.xyz + cosDist, cosDist);
}