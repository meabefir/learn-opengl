#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform float fogStr;
uniform vec3 fogColor;

uniform vec3 viewPos;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
};

uniform SpotLight spotLight;

uniform float time;

float shininess = 64.f;

vec3 point_light_influence(PointLight light);
vec3 spot_light_influence(SpotLight light);
float fog_influence();

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{    
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //return;
    // if (length(FragPos - viewPos) < 6.f) discard;

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // if (depth < .1f) discard;

    vec3 color = vec3(texture(texture_diffuse1, TexCoords));

    /*
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        color += point_light_influence(pointLights[i]);
    }
    color += spot_light_influence(spotLight);
    */

    // color = mix(color, fogColor, fog_influence());

    // gl_FragCoord stuff
    // float scale = 0.5f;
    // color += vec3(gl_FragCoord.x / 800.f, gl_FragCoord.y / 600.f, abs(sin(time))) * scale;

    FragColor = vec4(color, texture(texture_diffuse1, TexCoords).a);
}

float fog_influence() {
    float near = 1.f;
    float far = 30.f;
    float dist_to_camera = distance(FragPos, viewPos);
    float influence = min(1.f, dist_to_camera / (far - near));

    return influence * fogStr;
}

vec3 point_light_influence(PointLight light) {
    // this also needs some tbn matrix multiplication, dont know how to do it yet
    // vec3 normal = normalize(texture(texture_normal1, TexCoords).xyz * 2.0f - 1.0f);

    vec3 normal = normalize(Normal);
    vec3 lightDir   = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));

    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 lookVector = normalize(viewPos - FragPos);
    vec3 lightDirReflected = reflect(-lightDir, normal);
  
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 spot_light_influence(SpotLight light) {
    vec3 normal = normalize(Normal);
    vec3 lookingAt = normalize(-light.direction);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);

    float f = dot(lookingAt, lightDir);
    float influence = (f - light.outerCutOff) / (light.cutOff - light.outerCutOff);
    influence = max(0.0, min(influence, 1.0));

    vec3 lookVector = normalize(viewPos - FragPos);
    vec3 lightDirReflected = reflect(-lightDir, normal);
    float spec = pow(dot(lightDirReflected, lookVector), shininess);

    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    diffuse *= influence;
    specular *= influence;

    return diffuse + specular;
}