#version 330 core

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float     shininess;
};
  
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;  

uniform float time;

void main()
{    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    // vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    float margin = .1;
    bool is_margin = false;
    if (TexCoords.x < margin || TexCoords.x > 1 - margin || TexCoords.y < margin || TexCoords.y > 1 - margin)
        is_margin = true;

    vec3 emission = is_margin ? vec3(0) : vec3(texture(material.emission, TexCoords + vec2(0.0, time * .5f)));
    emission = abs(sin(time)) * emission;

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation; 

    vec3 result = ambient + diffuse + specular; // + emission;
    FragColor = vec4(result, 1.0);
}
