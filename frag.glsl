#version 430
layout (location=0) uniform float par[5];
#define iTime par[0]

out vec4 c;
in vec2 v;
void main()
{
	vec2 uv = mix(vec2(par[1], par[3]), vec2(par[2], par[4]), v/2+.5);
	vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
	c = vec4(pow(col, vec3(.5545)), 1.); // pow for gamma correction because all the cool kids do it
}
