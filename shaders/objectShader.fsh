#version 420 core

#define MAX_DIRECTIONAL_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_emission0;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cosInnerCutoff;
    float cosOuterCutoff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform int numDirectionalLights;
uniform int numSpotLights;
uniform int numPointLights;

uniform Material material;

uniform vec3 viewPos;
uniform float time;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float cosTheta = dot(lightDir, normalize(-light.direction));
    float cosEpsilon = light.cosInnerCutoff - light.cosOuterCutoff;
    float intensity = smoothstep(0.0, 1.0, (cosTheta - light.cosOuterCutoff) / cosEpsilon);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    //return normalize(-light.direction);
    //return ((cosTheta > 0.5) ? cosTheta : 0.0) * vec3(1.0);
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main() {

    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    // return;

    vec3 result = vec3(0.0);
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // DIRECTIONAL LIGHT
    for (int i = 0; i < min(numDirectionalLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += CalculateDirectionalLight(directionalLights[i], normal, viewDir);
    }

    // POINT LIGHTS
    for (int i = 0; i < min(numPointLights, MAX_POINT_LIGHTS); i++) {
        result += CalculatePointLight(pointLights[i], normal, viewDir);
    }

    // SPOT LIGHTS
    for (int i = 0; i < min(numSpotLights, MAX_SPOT_LIGHTS); i++) {
        result += CalculateSpotLight(spotLights[i], normal, viewDir);
    }

    // emission
    result += texture(material.texture_emission0, TexCoords).rgb;// - vec2(0.0, time / 2.0)).rgb;

    // total
    FragColor = vec4(result, texture(material.texture_diffuse0, TexCoords).a);
} 

