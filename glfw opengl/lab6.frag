#version 330 core
out vec4 FragColor;
  
in vec3 outColor;

void main()
{
    FragColor = vec4(outColor, 1.0);

    /*
    switch(mode) {
        case 0:
            FragColor = vec4(mix(outColor, vec3(1.0, 0.0, 0.0), 0.5), 1.f);
            break;
        case 1:
            FragColor = mix(vec4(vec3(texture(tex, TexCoords)), 1.f), vec4(vec3(texture(tex2, TexCoords)), 1.f), 0.5);
            break;
    }
    */
}