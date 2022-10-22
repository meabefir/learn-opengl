/*
#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blend_amount;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), blend_amount);
}
*/

#version 330 core
out vec4 FragColor;
  
in vec3 outColor;

void main()
{
     // FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), blend_amount);
    // FragColor = vec4(1.f, 1.f, 0.f, 1.f);

    FragColor = vec4(outColor, 1.f);
}