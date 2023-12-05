#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float prop;
uniform sampler2D ourTexture;
uniform vec3 otherColor;
uniform int colorSwitch;
void main()
{
if(colorSwitch==1)
{
FragColor =mix(texture(ourTexture, TexCoord), vec4(ourColor,1), prop) ;
}
if(colorSwitch==2)
{
FragColor =mix(texture(ourTexture, TexCoord), vec4(otherColor,1), prop) ;
}
if(colorSwitch==3)
{
FragColor =mix(vec4(otherColor,1), vec4(ourColor,1), prop) ;
}
}
