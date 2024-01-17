#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct PointLight {
    vec4 position; // ignore w
    vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;

    vec4 ambientLightColor; // w is intensity
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

vec3 calcPointLight(PointLight light, vec3 surfaceNormal, vec3 viewDirection);

void main() {
    vec3 result = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 camPosWorld = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(camPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.pointLights[i];
        result += calcPointLight(light, surfaceNormal, viewDirection);
    }

    outColor = vec4(result * fragColor, 1.0);
}

vec3 calcPointLight(PointLight light, vec3 surfaceNormal, vec3 viewDirection) {
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared

    directionToLight = normalize(directionToLight);

    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    vec3 diffuse = intensity * cosAngIncidence;


    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinn = dot(surfaceNormal, halfAngle);
    blinn = clamp(blinn, 0, 1);
    blinn = pow(blinn, 32.0);   //Higher value -> sharper highlight
    vec3 specular = intensity * blinn;


    return diffuse + specular;
}