#version 330 core
struct Material
{
        sampler2D diffuse;
        vec3 specular;
        float shininess;
};

struct Light
{
        vec3 position;
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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform float transparency;

void main()
{       
    // Necessary variables for the halfway vector to calculate the specularity (Blinn-Phong)
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ATTENUATION 
    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // AMBIENT LIGHTING
    vec3 ambientLight = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    // DIFFUSE LIGHTING
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = (diff * vec3(texture(material.diffuse, TexCoords))) * light.diffuse;

    // SPECULAR LIGHTING
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(halfwayDir, reflectDir), 0.0), material.shininess);
    vec3 specularLight =  (spec * material.specular) * light.specular;

    ambientLight *= attenuation;
    diffuseLight *= attenuation;
    specularLight *= attenuation;

    // Object Final Color
    vec3 finalObjectColor = ambientLight + diffuseLight + specularLight;
    FragColor = vec4(finalObjectColor, transparency);
}