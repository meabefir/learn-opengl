#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 cameraPos;
uniform sampler2D texture1;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(mix(texture(skybox, R).rgb, vec3(texture(texture1, TexCoords)),0.2), 1.0);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = texture(texture1, TexCoords);
}