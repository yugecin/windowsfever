unsigned long int next = 1;

/*rand & srand copied from "the c (ansi c) programming language second edition"*/
#define RAND_MAX 32768
int rand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int) (next/65536) % 32768;
}
void srand(unsigned int seed)
{
	next = seed;
}

int randn(int maxexcl)
{
	if (maxexcl <= 1) {
		return 0;
	}
	return rand() / (RAND_MAX / maxexcl + 1);
}

float eq_out_quad(float t) // fast
{
	return -1.0f * t * (t - 2.0f);
}

float eq_out_cubic(float t) // faster
{
	t = t - 1.0f;
	return t = t * t * t + 1.0f;
}

float eq_out_quart(float t) // fastest
{
	t = t - 1.0f;
	return t = -1.0f * (t * t * t * t - 1.0f);
}

float eq_out_quint(float t) // fasterest
{
	t = t - 1.0f;
	return t * t * t * t * t + 1.0f;
}

float eq_in_quad(float t)
{
	return t * t;
}

float eq_in_quart(float t)
{
	return t * t * t * t;
}

float costable[] = { // :)
	1.0f,
	0.9999f,
	0.9998f,
	0.9996f,
	0.9993f,
	0.9990f,
	0.9986f,
	0.9981f,
	0.9975f,
	0.9969f,
	0.9961f,
	0.9953f,
	0.9945f,
	0.9935f,
	0.9925f,
	0.9914f,
	0.9902f,
	0.9890f,
	0.9876f,
	0.9862f,
	0.9848f,
	0.9832f,
	0.9816f,
	0.9799f,
	0.9781f,
	0.9762f,
	0.9743f,
	0.9723f,
	0.9702f,
	0.9681f,
	0.9659f,
	0.9636f,
	0.9612f,
	0.9588f,
	0.9563f,
	0.9537f,
	0.9510f,
	0.9483f,
	0.9455f,
	0.9426f,
	0.9396f,
	0.9366f,
	0.9335f,
	0.9304f,
	0.9271f,
	0.9238f,
	0.9205f,
	0.9170f,
	0.9135f,
	0.9099f,
	0.9063f,
	0.9025f,
	0.8987f,
	0.8949f,
	0.8910f,
	0.8870f,
	0.8829f,
	0.8788f,
	0.8746f,
	0.8703f,
	0.8660f,
	0.8616f,
	0.8571f,
	0.8526f,
	0.8480f,
	0.8433f,
	0.8386f,
	0.8338f,
	0.8290f,
	0.8241f,
	0.8191f,
	0.8141f,
	0.8090f,
	0.8038f,
	0.7986f,
	0.7933f,
	0.7880f,
	0.7826f,
	0.7771f,
	0.7716f,
	0.7660f,
	0.7604f,
	0.7547f,
	0.7489f,
	0.7431f,
	0.7372f,
	0.7313f,
	0.7253f,
	0.7193f,
	0.7132f,
	0.7071f,
	0.7009f,
	0.6946f,
	0.6883f,
	0.6819f,
	0.6755f,
	0.6691f,
	0.6626f,
	0.6560f,
	0.6494f,
	0.6427f,
	0.6360f,
	0.6293f,
	0.6225f,
	0.6156f,
	0.6087f,
	0.6018f,
	0.5948f,
	0.5877f,
	0.5807f,
	0.5735f,
	0.5664f,
	0.5591f,
	0.5519f,
	0.5446f,
	0.5372f,
	0.5299f,
	0.5224f,
	0.5150f,
	0.5075f,
	0.5000f,
	0.4924f,
	0.4848f,
	0.4771f,
	0.4694f,
	0.4617f,
	0.4539f,
	0.4461f,
	0.4383f,
	0.4305f,
	0.4226f,
	0.4146f,
	0.4067f,
	0.3987f,
	0.3907f,
	0.3826f,
	0.3746f,
	0.3665f,
	0.3583f,
	0.3502f,
	0.3420f,
	0.3338f,
	0.3255f,
	0.3173f,
	0.3090f,
	0.3007f,
	0.2923f,
	0.2840f,
	0.2756f,
	0.2672f,
	0.2588f,
	0.2503f,
	0.2419f,
	0.2334f,
	0.2249f,
	0.2164f,
	0.2079f,
	0.1993f,
	0.1908f,
	0.1822f,
	0.1736f,
	0.1650f,
	0.1564f,
	0.1478f,
	0.1391f,
	0.1305f,
	0.1218f,
	0.1132f,
	0.1045f,
	0.0958f,
	0.0871f,
	0.0784f,
	0.0697f,
	0.0610f,
	0.0523f,
	0.0436f,
	0.0348f,
	0.0261f,
	0.0174f,
	0.0087f,
	0.0f,
};

float mcos(int deg)
{
	int minus = 1;
	// yeah
	while (deg > 360) deg -= 360;
	while (deg < 0) deg += 360;
	if (deg > 180) deg = 360 - deg;
	if (deg > 90) { deg = 180 - deg; minus = -1; }
	return costable[deg] * minus; // could lerp, but too tired
}
