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


vec3 gHitPosition = vec3(0);
float cube(vec3 p)
{
	p.xz *= rot2(iTime);
	p.yz *= rot2(iTime * .3);

	// opTwist from iq
	const float k = cos(iTime) * .07;
	float c = cos(k*p.y);
	float s = sin(k*p.y);
	mat2 m = mat2(c,-s,s,c);
	p = vec3(m*p.xz,p.y);

	p = abs(p) - vec3(23.);
	float x = length(max(p,0.0)) + min(max(p.x,max(p.y,p.z)),0.0);
	return x + sin(1.*p.x)*sin(1.*p.y)*sin(1.*p.z);
	return x;
}

float map(vec3 p)
{
	return cube(p);
}

vec3 norm(vec3 p, float dist_to_p)
{
	vec2 e=vec2(.00035,-.00035);
	return normalize(e.xyy*map(p+e.xyy).x+e.yyx*map(p+e.yyx).x+e.yxy*map(p+e.yxy).x+e.xxx*map(p+e.xxx).x);
}

// x=hit y=dist_to_p z=dist_to_ro
vec3 march(vec3 ro, vec3 rd, int maxSteps)
{
	vec3 r = vec3(0);
	for (int i = 0; i < maxSteps && r.z < 350.; i++){
		gHitPosition = ro + rd * r.z;
		float dist = map(gHitPosition);
		if (dist < .00001) {
			r.x = float(i)/float(maxSteps);
			r.y = dist;
			break;
		}
		r.z += dist * .5;
	}
	return r;
}

float calcAO(vec3 pos, vec3 nor )
{
	float o = 0., s = 1.;
	for(int i=0; i<5; i++) {
		float h = 0.001 + 0.15*float(i)/4.0;
		float d = map( pos + h*nor ).x;
		o += (h-d)*s;
		s *= 0.95;
	}
	return clamp(1.-1.5*o, 0.,1.);
}

float softshadow(vec3 ro, vec3 rd)
{
	float res = 1.0;
	float ph = 9e9;
	for(float dist = 0.01; dist < 40.; ) {
		float h = map(ro + rd*dist).x;
		if (h<0.001) {
			return 0.0;
		}
		float y = h*h/(2.0*ph);
		float d = sqrt(h*h-y*y);
		res = min(res, 5.*d/max(0.0,dist-y));
		ph = h;
		dist += h;
	}
	return res;
}

vec3 colorHit(vec3 result, vec3 rd)
{
	vec3 shade = vec3(.6);

	vec3 normal = norm(gHitPosition, result.y);
	vec3 material = shade * .3;
	return shade * dot(normal, rd);

	// key light
	vec3 ligfrom = vec3(-4., -10., -2.);
	vec3 lig = normalize(ligfrom - gHitPosition);
	vec3 hal = normalize(lig-rd);
	float dif = clamp(dot(normal, lig), 0.0, 1.0) * softshadow(gHitPosition, lig);

	float spe = pow(clamp(dot(normal, hal), 0.0, 1.0 ),16.0)* dif *
	(0.04 + 0.96*pow(clamp(1.0+dot(hal,rd),0.0,1.0), 5.0));

	vec3 col = material * 4.0*dif*vec3(1.00,0.70,0.5);
	col += 12.0*spe*vec3(1.00,0.70,0.5);
	return col;
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


	vec3 ro = vec3(-3, -8, -3.2) * 10.;
	vec3 at = vec3(0, 0, 0);
        vec3	cf = normalize(at-ro),
		cl = normalize(cross(cf,vec3(0,0,-1)));
	mat3 rdbase = mat3(cl,normalize(cross(cl,cf)),cf);
	vec3 rd = rdbase*normalize(vec3((uv-.5)*2.,1.));
	vec3 result = march(ro, rd, 200);
	if (result.x > 0.) { // hit
		col = colorHit(result, rd);
	}

	c = vec4(pow(col, vec3(.5545)), 1.); // pow for gamma correction because all the cool kids do it
}
