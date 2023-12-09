#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}



// #version 330 core
// struct Material {
//     sampler2D diffuse;
//     float shininess;
// }; 

// struct Light {
//     vec3 position;
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// in vec3 FragPos;  
// in vec3 Normal;  
// in vec2 TexCoords;

// out vec4 FragColor;

// uniform vec3 viewPos;
// uniform Material material;
// uniform Light light;

// void main()
// {
//     // Ambient
//     vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

//     // Diffuse 
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(light.position - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
//     // Specular
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * texture(material.diffuse, TexCoords).rgb;    

//     vec3 result = ambient + diffuse + specular;
//     FragColor = vec4(result, 1.0);
// }
