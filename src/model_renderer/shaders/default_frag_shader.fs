#version 140

in vec3 vertColor;
out vec4 outColor;

void main()
{
  outColor = vec4(vertColor, 0.0);
}
