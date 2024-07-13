#version 430
layout (location=0) uniform float par[6];
#define iTime par[0]
uniform sampler2D tex;

#define PI 3.14159265359
#define HALFPI 1.5707963268
float atan2(float y, float x) {
	if(x>0.)return atan(y/x);
	if(x==0.)if(y>0.)return HALFPI;else return -HALFPI;
	if(y<0.)return atan(y/x)-PI;return atan(y/x)+PI;
}
mat2 rot2(float a)
{
    return mat2(cos(a),-sin(a),sin(a),cos(a));
}

out vec4 c;
in vec2 v;
void main()
{
	vec2 uv = mix(vec2(par[1], par[3]), vec2(par[2], par[4]), v/2+.5);
	vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
	vec2 _uv = ((uv-.5) * rot2(iTime/3.)) + .5;
	float a = mod(atan2(_uv.y - .5, _uv.x - .5), HALFPI/3);
	col = mix(vec3(130.,136.,255.)/255., vec3(252.,255.,132.)/255., step(a, HALFPI/6));
	if (iTime > .350) {
		col += clamp(mix(.3, -2., mod((iTime - .350), .700) / .700), 0., .3);
	}
	if (par[5] > .5) {
		col = col * .7 + texture2D(tex, uv).xyz;
	}
	c = vec4(pow(col, vec3(.5545)), 1.); // pow for gamma correction because all the cool kids do it
}
